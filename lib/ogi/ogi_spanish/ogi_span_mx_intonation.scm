;;;;;;;;;;;;;;;;;;;;;;;;;;;;;<--CSLU-->;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                        ;;
;;             Center for Spoken Language Understanding                   ;;
;;        Oregon Graduate Institute of Science & Technology               ;;
;;                         Portland, OR USA                               ;;
;;                        Copyright (c) 1999                              ;;
;;                                                                        ;;
;;      This module is not part of the CSTR/University of Edinburgh       ;;
;;               release of the Festival TTS system.                      ;;
;;                                                                        ;;
;;  In addition to any conditions disclaimers below, please see the file  ;;
;;  "license_cslu_tts.txt" distributed with this software for information  ;;
;;  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  ;;
;;                                                                        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;<--CSLU-->;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  Intonation module for Mexican Spanish
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(set! span_mx_cart_tree
     '((stress is 1 )
          ((Accented))
      ((position_type is 1)
        ((Single))
        ((None))
      )))

;; calculate targets
(define (span_mx_targ_func utt syl)
  "(span_mx_targ_func UTT ITEM)
       Returns a list of targets for the given syllable item."

  (let ((start (item.feat syl 'syllable_start))
	(end (item.feat syl 'syllable_end)))

   (set! intonation_table normal_int_table)
   (set! base_f0 (cadr (assoc 'target_f0_mean int_lr_params)))

   (cond
   ((eq? (item.feat syl "R:SylStructure.parent.R:Phrase.first.word_start")
	  (item.feat syl "R:SylStructure.parent.R:Phrase.last.word_start"))
	   (set! intonation_table flat_int_table))

    ((string-matches (item.feat syl "R:SylStructure.parent.R:Phrase.last.R:Token.parent.punc") "\\.")
         (set! intonation_table normal_int_table))

    ((string-matches (item.feat syl "R:SylStructure.parent.R:Phrase.last.R:Token.parent.punc") "\\,")
         (set! intonation_table comma_int_table))

    ((string-matches (item.feat syl "R:SylStructure.parent.R:Phrase.last.R:Token.parent.punc") "\\!")
         (set! intonation_table exclam_int_table))

    ((string-matches (item.feat syl "R:SylStructure.parent.R:Phrase.last.R:Token.parent.punc") "\\?")
         (set! intonation_table question_int_table)))

;(set! intonation_table flat_int_table)


(cond
;; Debug option
     (nil 
     (list
       (list start  
             (* base_f0 multfactor (phrase_int_factor syl 0.01 intonation_table)))
	 (list (+ start (* (- end start) 0.25)) 
             (* base_f0 multfactor (phrase_int_factor syl 0.25 intonation_table)))
	 (list (+ start (* (- end start) 0.50)) 
             (* base_f0 multfactor (phrase_int_factor syl 0.50 intonation_table)))
	 (list (+ start (* (- end start) 0.75)) 
             (* base_f0 multfactor (phrase_int_factor syl 0.75 intonation_table)))
       (list end (* base_f0 multfactor (phrase_int_factor syl 1.0 intonation_table)))))

;; accented utterances
     ((string-matches (item.feat syl "R:Intonation.daughter1.name") "Accented")
     (list 
       (list start  
             (* base_f0 multfactor (phrase_int_factor syl 0.01 intonation_table)))
	 (list (+ start (* (- end start) 0.1)) 
             (* (+ base_f0 3) multfactor (phrase_int_factor syl 0.1 intonation_table)))
       (list (+ start (* (- end start) 0.26)) 
             (* (+ base_f0 6) multfactor (phrase_int_factor syl 0.26 intonation_table)))
       (list (+ start (* (- end start) 0.42)) 
             (* (+ base_f0 6) multfactor (phrase_int_factor syl 0.42 intonation_table)))
       (list (+ start (* (- end start) 0.67)) 
             (* (+ base_f0 2) multfactor (phrase_int_factor syl 0.67 intonation_table)))
       (list end (* base_f0 multfactor (phrase_int_factor syl 1.0 intonation_table)))))

     ((string-matches (item.feat syl "R:Intonation.daughter1.name") "None")
     (list
       (list start  
             (* base_f0 multfactor (phrase_int_factor syl 0.01 intonation_table)))
       (list end (* base_f0 multfactor (phrase_int_factor syl 1.0 intonation_table)))))

     ((string-matches (item.feat syl "R:Intonation.daughter1.name") "Single")
     (list
       (list start  
             (* base_f0 multfactor (phrase_int_factor syl 0.01 intonation_table)))
       (list (+ start (* (- end start) 0.25)) 
             (* (- base_f0 1) multfactor (phrase_int_factor syl 0.25 intonation_table)))
       (list (+ start (* (- end start) 0.5)) 
             (* (- base_f0 1) multfactor (phrase_int_factor syl 0.50 intonation_table)))
       (list (+ start (* (- end start) 0.75)) 
             (* base_f0 multfactor (phrase_int_factor syl 0.75 intonation_table)))
       (list end (* base_f0 multfactor (phrase_int_factor syl 1.0 intonation_table)))))


)))
   


(define (phrase_int_factor syl sylpos int_table)
  "(phrase_int_factor ITEM STREAMPOS)
       Returns the phrase intonation factor."

  (let 
      ((start (item.feat syl "syllable_start"))
       (end (item.feat syl "syllable_end"))
       (phrase_start (item.feat syl "R:SylStructure.parent.R:Phrase.first.word_start"))
       (phrase_end   (item.feat syl "R:SylStructure.parent.R:Phrase.last.word_end"))
       (dur)
       (ref)
       (last_pair))

       (set! dur (- phrase_end phrase_start))
    
       (set! target_pos (- (+ start (* (- end start) sylpos)) phrase_start))
       (set! ref (/ target_pos dur))
       (set! last_pair (car int_table))


  (mapcar
   (lambda (pair)
     (if (> ref (car last_pair))
	 (set! factor (interpolate last_pair pair ref ))
	 )
     (set! last_pair pair)
     )
   (cdr int_table))
  
  factor
))

(define (interpolate last_pair pair ref )
(set! m (/(- (cadr pair) (cadr last_pair)) (- (car pair) (car last_pair))))
(set! m (+ (cadr last_pair) (* m (- ref (car last_pair)))))
m
)


(set! normal_int_table
  '(	(0.00 0.98)
	(0.10 1.00)
	(0.20 1.01)
	(0.46 0.98)
	(0.65 1.01)
	(0.97 0.96)
	(0.98 0.97)
	(0.99 0.93)
	(1.00	0.82)))



(set! normal_int_table
  '(	(0.00 0.98)
	(0.10 1.02)
	(0.20 0.98)
	(0.30 1.02)
	(0.40 0.98)
	(0.50 1.01)
	(0.60 0.98)
	(0.70 1.02)
	(0.80 0.98)
	(0.90 1.01)
	(0.91 0.96)
	(0.94 0.97)
	(0.96 0.86)
	(1.00	0.82)))

(set! flat_int_table
  '(	(0.00 1.00)
	(0.50 1.00)
	(1.00	1.00)))

(set! exclam_int_table
  '(	(0.00 0.82)
	(0.10 0.86)
	(0.50 1.05)
	(0.98 0.90)
	(0.99 0.91)
	(1.00	0.80)))

(set! word_int_table
  '(	(0.00 0.90)
	(0.10 0.91)
	(0.90 0.91)
	(0.97 0.89)
	(1.00	0.83)))


(set! comma_int_table
  '(	(0.00 0.88)
	(0.10 0.97)
	(0.90 1.00)
	(0.98 1.10)
	(1.00	1.20)))

(set! question_int_table
  '(	(0.00 0.95)
	(0.10 1.03)
	(0.20 1.10)
	(0.30 1.06)
	(0.40 1.03)
	(0.50 1.00)
	(0.60 0.90)
	(0.70 0.86)
	(0.80 0.86)
	(0.90 0.88)
	(0.96 0.90)
	(0.98 1.10)
	(0.99 1.20)
	(1.00	1.50)))

(provide 'cslu_span_mx_intonation)
