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

;;
;;  Define synthesis methods
;;
(defSynthType OGI_word
  
  (set! OGI_unitselFAIL nil)
  
  ;; select units and set features of "dbUnit" relation
  (OGI_set_dbUnit_feats utt (CSLU_word_unitsel utt))
  
  ;; if can't do it, revert to diphones
  (if OGI_unitselFAIL
      (begin 
       
	;;; switch to FALLBACK VOICE
       (fallback_voice_cmd)
       
       ;; select units again using diphones
       (OGI_set_dbUnit_feats utt (CSLU_di_unitsel utt))
       
       ;; synthesize
       (cond 
	((string-equal (OGIdbase.get_data_type) "resLPC")
	 (OGIresLPC.synth utt))
	((string-equal (OGIdbase.get_data_type) "sine")
	 (OGIsine.synth utt))
	(t 
	 (error "unknown OGIdbase data type")))
       
	;;; switch BACK to WORD VOICE
       (reactivate_voice_cmd)
       )
    (begin        ;;; else 
     ;; synthesize
     (cond 
      ((string-equal (OGIdbase.get_data_type) "resLPC")
       (OGIresLPC.synth utt))
      (t 
       (error "unknown OGIdbase data type")))
     )
    )
  )

; loop thru segments, create dbUnit stream, lookup diphone
(define (OGI_word_unitsel utt)
  "(OGI_word_unitsel utt)
Simple unit selection for isolated word synthesis: 
constructs name from Word names and selects first unit
with same name."  	   
  ;; UNIT SELECTION 
  (set! SelectedUnits nil)
  (mapcar
   (lambda (wrd)
     ;; create the name of the desired unit 
     (set! n (item.feat wrd "name"))
     
     ;; look up in dbase
     (set! indx (OGIdbase.unitsel_name n))
     
     ;; look for alternates if original not found
     (if (equal? indx nil)
	 (begin
	   (set! OGI_unitselFAIL t)
	   (set! indx 0))  ;;; dummy
	 )

     ;; lookup info structure for this unit
     (set! sel (list (OGIdbase.lookup_info indx)))
     
     ;; append info to list
     (set! SelectedUnits (append SelectedUnits sel))
     )
   (utt.relation.items utt "Word"))

  (set! SIL (car (car (cdr (assoc 'silences (PhoneSet.description))))))
  (mapcar
   (lambda (wrd)
     ;;; insert a 1ms pause between the words (in Segment relation)
     (set! firstseg (item.relation (item.daughter1 (item.relation.daughter1 wrd "SylStructure")) "Segment"))

     (if (not (string-equal SIL (item.name (item.prev firstseg))))
	 (begin
	  (set! pau_item_desc (list SIL (list (list "end" (- (item.feat firstseg "end") 0.001)))))
	  (item.insert firstseg  pau_item_desc 'before)
	  )))
   (utt.relation.items utt "Word"))
  
  SelectedUnits)


(provide 'ogi_wordsynth)
