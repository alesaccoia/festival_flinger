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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  Duration module for Mexican Spanish
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Avg durations from current 22 languages database

(set! span_mx_phone_data
'(
   (pau 0.100)
   (a 0.090)
   (e 0.079)
   (i 0.080)
   (o 0.083)
   (u 0.064)
   (b 0.076)
   (tS 0.120)
   (dZ 0.076)
   (d 0.069)
   (f 0.105)
   (g 0.061)
   (k 0.099)
   (l 0.058)
   (j 0.042)
   (m 0.073)
   (n 0.065)
   (N 0.086)
   (ny 0.110)
   (p 0.092)
   (r 0.098)
   (rr 0.151)
   (s 0.095)
   (t 0.098)
   (w 0.063)
   (x 0.097)
))

;; not used as far as I can tell....  MWM
(set! span_mx_dur_tree
 '((R:SylStructure.parent.p.syl_break > 1 ) ;; clause initial
    ((R:SylStructure.parent.stress is 1)
     ((0.6))
     ((0.6)))
    ((R:SylStructure.parent.syl_break > 1)   ;; clause final
     ((R:SylStructure.parent.stress is 1)
      ((0.6))
      ((0.6)))
     ((R:SylStructure.parent.stress is 1)
      ((0.6))
      ((0.6))))))


;;;
;;;  A spanish duration module that allows specification, mostly in
;;;  lisp.

(define (CSLU_Spanish_Duration utt)
"(Spanish_Durs utt)
Apply syllable and segment rule to predict duration for spanish."
  (Spanish_Durs_Syllable utt)  
  (Spanish_Durs_Segment utt)
  (Assign_Durations utt)
  utt)

(define (Spanish_Durs_Syllable utt)
"(Spanish_Durs_Syllable utt)
Set the feature syl_dur for each syllable based on stress etc."
  (mapcar
   (lambda (syl)
     (let ((factor 2.26))
	 ;; Set factor for stressed syllabes
       (if (string-equal "1" (item.feat syl "stress"))
	   (set! factor (* 1.18 factor)))

	 ;; Set factor for single words
       (if (string-equal "1" (item.feat syl "R:SylStructure.parent.R:Word.word_numsyls"))
	   (set! factor (* 1.2 factor)))

       ;; compress the factor as we get more segments in the syllable
       (set! factor (* factor (fixed_dur (parse-number (item.feat syl "syl_numphones")))))

       ;; Increase factor at the end of a phrase (MWM)
       (if (< (item.feat syl "syl_out") 2)
	   (set! factor (* 1.1 factor)))
       (if (< (item.feat syl "syl_out") 1)
	   (set! factor (* 1.2 factor)))

       ;; put factor on syllable to access later
       (item.set_feat syl "syl_dur" factor)))
   (utt.relation.items utt "Syllable")))

(define (Spanish_Durs_Segment utt)
"(Spanish_Durs_Segment utt)
Set the feature seg_dur on each seg with a factor to change the 
average segmental duration."
  (let ((token_factor))
  (mapcar
   (lambda (seg)
     (if (string-equal (item.feat seg "name") "pau")
	     (set! factor 1.0)
       ;; else
    	 (set! factor (parse-number (item.feat seg "R:SylStructure.parent.syl_dur"))))
     (set! token_factor (item.feat seg "R:SylStructure.parent.parent.R:Token.parent.dur_stretch"))
     (set! token_factor (parse-number token_factor))
     (if (and (number? token_factor) (> token_factor 0)) 
       (set! factor (* factor token_factor)))
     (item.set_feat seg "seg_dur" (* factor (parse-number (car (cdr (assoc (intern (item.name seg)) span_mx_phone_data))))))
     )

   (utt.relation.items utt "Segment"))))

(define (Assign_Durations utt)
  "(Assign_Durations utt)
Get the factor from the feature seg_dur onto the segment itself."
  (let ((tot 0.0))
  (mapcar
   (lambda (seg)
     (set! tot (+ tot (* (parse-number (item.feat seg "seg_dur")) (Parameter.get 'Duration_Stretch))))
     (item.set_feat seg "end" tot)
     )
   (utt.relation.items utt "Segment"))))

(define (fixed_dur s)
  ( cond
    (( eq s 1 ) 0.56)
    (( eq s 2 ) 0.41)
    (( eq s 3 ) 0.34)
    (( eq s 4 ) 0.33)
    (( eq s 5 ) 0.27)
    (( eq s 6 ) 0.25)
    (t 0.25)))

(provide 'cslu_span_mx_duration)
