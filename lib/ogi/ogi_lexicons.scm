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

(require 'pos)

(set! OGI_USE_TRAINED_LTS nil)

(define (setup_ogi_lex)
"(setup_ogi_lex)
Define and setup the OGI compilation of the MOBY and CMU lexicons.  This is derived 
from the public domain version of the Moby (TM) Pronunciator II lexicon and CMU dict."
 (if (not (member_string "ogi" (lex.list)))
     (begin  (lex.create "ogi")
	     (lex.set.compile.file (string-append lexdir "ogi_lex.out"))
	     (lex.set.phoneset "worldbet")

	     (if OGI_USE_TRAINED_LTS
		 (lex.set.lts.method 'ogi_lts_function)
		 (begin
		   (lex.set.lts.method 'lts_rules)
		   (lex.set.lts.ruleset 'lts_worldbet)
		   ))

	     (lex.set.pos.map english_pos_map_wp39_to_wp20)
	     (worldbet_addenda)
	     )))

(define (ogi_lts_function word feats)
  "(ogi_lts_function word feats)
Function called for OGILEX when word is not found in lexicon.  Uses
LTS rules trained from the original lexicon, and lexical stress
prediction rules."
  (require 'lts)
  (if OGI_USE_TRAINED_LTS
      (if (not (boundp 'ogilex_lts_rules))
	  (load (path-append lexdir "ogilex_lts_rules.scm")))
      )
  (let ((dcword (downcase word)) (syls) (phones))
    (if (string-matches dcword "[a-z]*")
	(begin
	  (if OGI_USE_TRAINED_LTS
	      (set! phones (lts_predict dcword ogilex_lts_rules))
	      (set! phones (lts_predict dcword lts_worldbet))
	      )
	  (set! syls (lex.syllabify.phstress phones))
	  )
	(set! syls nil))
    (list word nil syls)))


(provide 'ogi_lexicons)

