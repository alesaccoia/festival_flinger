;;;;;;;;;;;;;;;;;;;;;;;;;;;;;<--OHSU-->;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                        ;;
;;                   Oregon Health & Science University                   ;;
;;                Division of Biomedical Computer Science                 ;;
;;                Center for Spoken Language Understanding                ;;
;;                         Portland, OR USA                               ;;
;;                        Copyright (c) 2000                              ;;
;;                                                                        ;;
;;      This module is not part of the CSTR/University of Edinburgh       ;;
;;               release of the Festival TTS system.                      ;;
;;                                                                        ;;
;;  In addition to any conditions disclaimers below, please see the file  ;;
;;  "NE Copyright Materials License.txt" distributed with this software   ;;
;;  for information on usage and redistribution, and for a DISCLAIMER OF  ;;
;;  ALL WARRANTIES.                                                       ;;
;;                                                                        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;<--OHSU-->;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define (OGI_PuncPhrasify utt)
  "(OGI_PuncPhrasify utt)
Put in preliminary phrase breaks based only on the punctuation, then call festival Phrasify function"
  (mapcar
   (lambda (w)
     (if (not (null? (item.next w)))
	 (let ((npos (item.feat (item.next w) "pos")))
	   (if (or (string-equal "punc" npos)
		   (string-equal "fpunc" npos))
	       (item.set_feat (item.parent (item.relation w "Token")) "pbreak" "B")))))
  (utt.relation.items utt "Word"))
 
  (festival-phrasify utt)

  ;; Added by Taniya Mishra on June 27, 2005 
  ;; This is to ensure that if the user mistakenly adds a space before the 
  ;; final punctuation, the resulting Utterance structure is corrected to 
  ;; produce correct pitch curves.
  (if (and  (not (null (utt.relation.last utt "Phrase")))
	    (equal? (length (item.daughters (utt.relation.last utt "Phrase"))) 1)
            (string-equal (item.feat (item.daughter1 (utt.relation.last utt "Phrase")) "pos") "punc"))
		(begin
			(set! p (item.relation.prev (utt.relation.last utt "Phrase") "Phrase"))
			(item.set_feat (item.relation.parent (item.daughtern p) "Token")
				 "punc" (item.feat (item.daughter1 (utt.relation.last utt "Phrase")) "name"))
			(item.append_daughter p (item.daughter1 (utt.relation.last utt "Phrase")))
			(item.delete (utt.relation.last utt "Phrase"))
	
		)
  )


  ;; Added by Taniya Mishra on June 24, 2005 
  ;; This is to ensure that utterance ends with a big break.
  (if (and (not (null (utt.relation.last utt "Phrase")))
	(string-equal (item.feat (utt.relation.last utt "Phrase") "name") "B"))
	   (begin (item.set_feat (utt.relation.last utt "Phrase") "name" "BB")
			   (item.set_feat (utt.relation.last utt "Phrase") "pbreak" "BB")
	   )
  )
  utt)


(defvar OGI_english_phr_break_params
  (list
   ;; The name and filename off the ngram with the a priori ngram model
   ;; for predicting phrase breaks in the Phrasify module.  This model should 
   ;; predict probability distributions for B and NB given some context of 
   ;; part of  speech tags.
   (list 'pos_ngram_name 'english_break_pos_ngram)
   (list 'pos_ngram_filename
	 (path-append pbreak_ngram_dir "sec.ts20.quad.ngrambin"))
   ;; The name and filename of the ngram  containing the a posteriori ngram
   ;; for predicting phrase breaks in the Phrasify module.  This module should
   ;; predict probability distributions for B and NB given previous B and
   ;; NBs.
   (list 'break_ngram_name 'english_break_ngram)
   (list 'break_ngram_filename
	 (path-append pbreak_ngram_dir "sec.B.hept.ngrambin"))
   ;; A weighting factor for breaks in the break/non-break ngram.
   ;;   ** changed by MWM **
   (list 'gram_scale_s 0.25)
   ;; When Phrase_Method is prob_models, this tree, if set is used to 
   ;; potentially predict phrase type.  At least some prob_models only
   ;; predict B or NB, this tree may be used to change some Bs into
   ;; BBs.  If it is nil, the pbreak value predicted by prob_models
   ;; remains the same.
   (list 'phrase_type_tree english_phrase_type_tree)
   ;; A list of tags used in identifying breaks.  Typically B and NB (and
   ;; BB).  This should be the alphabet of the ngram identified in
   ;; break_ngram_name
   (list 'break_tags '(B NB))
   (list 'pos_map english_pos_map_wp39_to_wp20)
   ))



(provide 'ogi_phrase)
