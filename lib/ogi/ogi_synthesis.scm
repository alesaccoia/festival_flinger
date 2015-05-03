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
(defSynthType OGIdiphone
  
  ;; select units and set features of "dbUnit" relation
  ;;;; slow:   (OGI_set_dbUnit_feats utt (CSLU_di_unitsel utt))
  (OGIdbase.unitsel_diphone utt)

  ;; synthesize
  (cond 
   ((string-equal (OGIdbase.get_data_type) "resLPC")
    (OGIresLPC.synth utt))
   ((string-equal (OGIdbase.get_data_type) "sine")
    (OGIsine.synth utt))
   (t 
    (error "unknown OGIdbase data type")))

)

(defSynthType OGIdemiphone
  
  ;; select units and set features of "dbUnit" relation
  (OGI_set_dbUnit_feats utt (CSLU_demi_unitsel utt))

  ;; synthesize
  (cond 
   ((string-equal (OGIdbase.get_data_type) "resLPC")
    (OGIresLPC.synth utt))
   ((string-equal (OGIdbase.get_data_type) "sine")
    (OGIsine.synth utt))
   (t 
    (error "unknown OGIdbase data type")))
)






(define (OGIresynth src_segs targ_segs F0targs filelist)
  "(OGIresynth src_segs targ_segs F0targs filelist)
  Resynthesize an existing wave file with pitch and duration modification.
  Returns and utterance containing the wave and the above info.  src_segs
  and targ_segs contain durations, not end points.  The phone names and number
  of phones passed in don't really matter unless you want them for something later.
  More info in ogi_synthesis.scm"


   ;;
   ;;  Example:
   ;;	    (set! src_segs  '((x 0.115) (x 0.293)))
   ;;	    (set! targ_segs  '((x 0.115) (x 0.593)))
   ;;	    (set! F0targs  '((0.115 170.0) (0.220 180.0)))
   ;;	     (set! fileparams (list
   ;;			 '(wav_file "ogi_0001.res")
   ;;			 '(lsf_file "ogi_0001.lpc")
   ;;			 '(pm_file  "ogi_0001.pmv")
   ;;			 ))
   ;;
   ;;  The function will cleverly do different things depending on whether you give it 
   ;;  lsf/pm file names or not.
   ;;
   ;;  Call (OGIresLPC.init ...) or (CSLUresLPC.init++ ...) to change the parameters
   ;;  of the synthesizer -- e.g., (OGIresLPC.init++ (list '(pitch_method "natural")))
   ;;  will cause it to use the natural pitch contour.  F0targs is ignored in this case."
   ;;


  (set! utt (Utterance Segments nil))
  (Initialize utt)
  (utt.relation.create utt 'Segment)
  (utt.relation.create utt 'SrcSeg)  
  (utt.relation.create utt 'dbUnit)  
  (utt.relation.create utt 'Target)
  
  
  ;; fill Segment relation
  (let (s_end)
    (set! s_end 0)
    (mapcar
     (lambda (tseg)
       (set! segitem (utt.relation.append utt "Segment"))
       (item.set_name segitem (car tseg))
       (set! s_end (+ s_end (car (cdr tseg))))
       (item.set_feat segitem "end" s_end)
       )
     targ_segs))
  
  ;; fill SrcSeg relation
  (mapcar
   (lambda (sseg)
     (set! segitem (utt.relation.append utt "SrcSeg"))
     (item.set_name segitem (car sseg))
     (item.set_feat segitem "dur" (car (cdr sseg)))
     )
   src_segs)
  
  ;; fill (F0) Target relation
  ;;  if values < 10.0 are found, assume these are
  ;;  mod factors, not F0 values
  (mapcar
   (lambda (f0s)
     (set! f0item (utt.relation.append utt "Target"))
     (item.set_feat f0item "pos" (car f0s))
     (item.set_feat f0item "f0" (car (cdr f0s)))
     )
   F0targs)
  
  ;; call resynthesis routine on speech data
  ;;   if lpc, pm, exc --> lpc synthesis
  ;;   if pm, exc      --> NOlpc synthesis
  ;; assume (OGIresLPC.init ... ) called already
  (OGIresLPC.resynth utt fileparams)
  
  utt)

