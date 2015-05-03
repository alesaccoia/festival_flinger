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

;; ogi_languages.scm

(define (language_american_english)
"(language_american_english)
Set up language parameters for American English."

  (set! male1 voice_mwm5_diphone)

  (male1)
  (Parameter.set 'Language 'americanenglish)
)

(define (language_mexican_spanish)
"(language_mexican_spanish)
Set up language parameters for Mexican Spanish."
  (voice_abc_diphone)
  (Parameter.set 'Language 'mexicanspanish)
)

(define (language_german)
"(language_mexican_spanish)
Set up language parameters for Mexican Spanish."
  (voice_axk_diphone)
  (Parameter.set 'Language 'german)
)


(define (select_language language)
  (cond
   ((or (equal? language 'britishenglish)
	(equal? language 'english))  ;; we all know its the *real* English
    (language_british_english))
   ((equal? language 'americanenglish)
    (language_american_english))
   ((equal? language 'german)
    (language_german))
   ((equal? language 'scotsgaelic)
    (language_scots_gaelic))
   ((equal? language 'welsh)
    (language_welsh))
   ((equal? language 'spanish)
    (language_castillian_spanish))
   ((equal? language 'mexicanspanish)
    (language_mexican_spanish))
   ((equal? language 'klingon)
    (language_klingon))
   (t
    (print "Unsupported language, using English")
    (language_british_english))))

(defvar language_default language_british_english)

(provide 'languages)
