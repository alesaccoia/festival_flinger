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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (ogi_syllabify infile outfile)
(set! inf (fopen infile "r"))
(set! outf (fopen outfile "w"))
(format outf "MNCL\n")
(while (not (equal? (set! entry (readfp inf)) (eof-val)))
       (set! word (car entry))
       (set! rest (cdr entry))
       (set! pos (car rest))
       (set! phonemes (car (cdr rest)))
       (set! res nil)
       (mapcar (lambda (letter) 
		 (set! l (list letter))
		 (set! res (append res l '(-))))
	       phonemes)
       (set! newphonemes (lts.apply res 'syl_worldbet))
       (set! phonword (rewrite_syllabified newphonemes))
       (format outf "(\"%s\" %s %l)\n" word pos phonword)
       )
(fclose inf)
(fclose outf)

)

(define (rewrite_syllabified phones)
(set! syl nil)
(set! phonword nil)
(set! stress 0)
(mapcar
 (lambda (phon)
   (set! phon (string-append phon))
   (if (string-matches phon ".*1")
       (begin
	 (set! phon (string-before phon "1"))
	 (set! stress 1))
       ;; else
       (if (string-matches phon ".*2")
	   (begin
	     (set! phon (string-before phon "2"))
	     (set! stress 2))
	   ;; else
	   (if (string-matches phon ".*0")
	       (begin
		 (set! phon (string-before phon "0"))
		 (set! stress 0))
	       ;; else
	       (set! phon phon))))
   (set! phon2 (intern phon))
   (if (not (string-equal phon "-"))
       (begin
	 (set! syl (cons phon2 syl))
	 )
       ;; else
       (begin
	 (set! syl (reverse syl))
	 (set! syl (cons syl (cons stress '())))
	 (set! phonword (cons syl phonword))
	 (set! syl nil)))
   
   )
phones)

    (set! syl (reverse syl))
    (set! syl (cons syl (cons stress '())))
    (set! phonword (cons syl phonword))
    (set! phonword (reverse phonword))
)

(define (syllabify_lts utt)
  "(syllabify_lts UTT)
Resyllabify using ogi_lts_syllabify.scm"
  (set! ss nil)
  (set! sItems nil)
  (mapcar
   (lambda (seg)
     (if (not (string-equal (item.name seg) "pau"))
	 (begin
	   (set! ss (append ss (list (intern (item.name seg)))))
	   (set! sItems (append sItems (list seg)))
	   ))
     (if (not (in_same_word seg (item.next seg)))
	 (begin 

;; EK: replaced function lex.syllabify.phstress with new procedure below

	   (if (not (equal? ss nil))
	       (begin
		 (set! newph (lts.apply ss 'syl_worldbet))
		 (set! resyll (rewrite_syllabified newph)))
	       ;; else
	       (set! resyll nil))
;		 )
;	       )

	   ;; loop through syllables in this word
	   (mapcar
	    (lambda (wordsyl)
	      
	      ;; loop through segments in this syllable
	      (set! vfoundyet nil)
	      (mapcar
	       (lambda (wordseg)
		 (set! segItem (car sItems))
		 (set! sItems (cdr sItems))
		 
		 (if (not vfoundyet)
		     (if (string-equal (item.feat segItem 'ph_vc) "+")
			   (set! vfoundyet t)
			 ))
		 (if vfoundyet
		     (item.set_feat segItem 'ogi_onsetcoda "coda")
		     (item.set_feat segItem 'ogi_onsetcoda "onset"))
		 (if (or (string-equal (item.name segItem) "pau")
			 (string-equal (item.feat segItem 'ph_vc) "+"))
		     (item.set_feat seg 'ogi_onsetcoda "0"))
		 )
	       (car wordsyl)))
	    resyll)
	   
	   
	   (set! ss nil)
	   (set! sItems nil)
	   )))
   (utt.relation.items utt 'Segment))
  utt)

(define (ogi_onsetcoda utt)
  (mapcar
   (lambda (syl)
     (set! segs (item.relation.daughters syl 'SylStructure))
     (set! vfoundyet nil)
     (mapcar
      (lambda (seg)
	(if (not vfoundyet)
	    (if (string-equal (item.feat seg 'ph_vc) "+")
		(set! vfoundyet t)
		))
	(if vfoundyet
	    (if (string-equal (item.feat seg 'ph_vc) "-")
		(item.set_feat seg 'ogi_onsetcoda "coda")
		(item.set_feat seg 'ogi_onsetcoda "0"))
	    (if (string-equal (item.feat seg 'ph_vc) "-")
		(item.set_feat seg 'ogi_onsetcoda "onset")
		(item.set_feat seg 'ogi_onsetcoda "0")))
	(car segs))
      segs)
     )
     (utt.relation.items utt 'Syllable))
 utt)

(define (secondary_stress_hack utt)
  (mapcar
   (lambda (syl)
      (if (equal? (item.feat syl "stress") 2)
	  (item.set_feat syl "stress" 0)))
   (utt.relation.items utt 'Syllable))
  utt)
