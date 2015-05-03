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

;;; Phrase breaks
;;;    use punctuation only

(set! span_mx_phrase_cart_tree
'
((R:Token.parent.punc in ("?" "." ":"))
    ((B))
    ((R:Token.parent.punc in ("'" "\"" "," ";"))
      ((B))
        ((R:Token.parent.n.name in ("y"))
        ((B))
        ((n.name is 0)  ;; end of utterance
          ((B))
          ((NB))
        )
      )
    )
  )
)

(provide 'cslu_span_mx_phrasing)
