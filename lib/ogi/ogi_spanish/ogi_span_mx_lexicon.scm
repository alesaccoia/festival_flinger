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
;;;  Lexicon for Mexican Spanish
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;; Lexicon
(lex.create "span_mx")
(lex.set.phoneset "span_mx")
(lex.set.lts.method 'function)
(lex.set.lts.ruleset 'span_lx)

(define (span_mx_lts word features)
  "(span_mx_lts WORD FEATURES)
Using letter to sound rules build a spanish pronunciation of WORD."
  (list word
	nil
	(mex.syllabify.phstress (lts.apply (downcase word) 'span_mx))))

(lex.set.lts.method 'function)

(lex.add.entry
	'("texas" nil (((t e) 1) ((x a s) 0))))
(lex.add.entry
	'("me'xico" nil (((m e) 1) ((x i) 0) ((k o) 0))))
(lex.add.entry
	'("m‰xico" nil (((m e) 1) ((x i) 0) ((k o) 0))))
(lex.add.entry
	'("mexicano" nil (((m e) 0) ((x i) 0) ((k a) 1) ((n o) 0))))
(lex.add.entry
	'("mexicana" nil (((m e) 0) ((x i) 0) ((k a) 1) ((n a) 0))))
(lex.add.entry
	'("mexicanos" nil (((m e) 0) ((x i) 0) ((k a) 1) ((n o s) 0))))
(lex.add.entry
	'("mexicanas" nil (((m e) 0) ((x i) 0) ((k a) 1) ((n a s) 0))))
(lex.add.entry
	'("mexicanismo" nil (((m e) 0) ((x i) 0) ((k a) 0) ((n i s) 1) ((m o) 0))))
(lex.add.entry
	'("xochimilco" nil (((s o) 0) ((tS i) 0) ((m i l) 1) ((k o) 0))))
(lex.add.entry
	'("atlixco" nil (((a) 0) ((t l i s) 1) ((k o) 0))))
(lex.add.entry
	'("telmex" nil (((t e l) 1) ((m e k s) 0))))
(lex.add.entry
	'("kellogs" nil (((k e) 1) ((l o g s) 0))))

(provide 'cslu_span_mx_lexicon)
