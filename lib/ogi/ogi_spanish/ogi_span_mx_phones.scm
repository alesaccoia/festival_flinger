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


;;;
;;;  Mexican Spanish synthesizer by A. Barbosa, UDLA
;;;

(defPhoneSet
  span_mx
  ;;;  Phone Features
  (;; vowel or consonant
   (vc + -)  
   ;; vowel length: short long dipthong schwa
   (vlng s l d a 0)
   ;; vowel height: high mid low
   (vheight 1 2 3 -)
   ;; vowel frontness: front mid back
   (vfront 1 2 3 -)
   ;; lip rounding
   (vrnd + -)
   ;; consonant type: stop fricative affricative nasal liquid
   (ctype s f a n l 0)
   ;; place of articulation: labial alveolar palatal labio-dental
   ;;                         dental velar
   (cplace l a p b d v 0)
   ;; consonant voicing
   (cvox + -)
   )
  ;; Phone set members (features are not! set properly)
  (
   (pau  - 0 - - - 0 0 -)
   (a  + s 2 2 - 0 0 +)
   (e  + s 2 2 - 0 0 +)
   (i  + s 1 1 - 0 0 +)
   (o  + s 2 2 - 0 0 +)
   (u  + s 1 1 - 0 0 +)
   (b  - 0 - - + s l +)
   (tS - 0 - - + s l -)
   (dZ - 0 - - + s p -)
   (d  - 0 - - + s a +)
   (f  - 0 - - + f b -)
   (g  - 0 - - + f p -)
   (k  - 0 - - + f p -)
   (l  - 0 - - + l d +)
   (j  - 0 - - + l d +)
   (m  - 0 - - + n l +)
   (n  - 0 - - + n l +)
   (N  - 0 - - + n l +)
   (ny  - 0 - - + n l +)
   (p  - 0 - - + s l -)
   (r  - 0 - - + l p +)
   (rr - 0 - - + l p +)
   (s  - 0 - - + f a +)
   (t  - 0 - - + s a -)
   (w  - 0 - - + s a -)
   (x  - 0 - - + f a -)
  )
)

(PhoneSet.silences '(pau))

(provide 'cslu_span_mx_phones)
