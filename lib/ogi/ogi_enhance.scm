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

;; ogi_enhance

(define (ogisable_apply_enhance utt)
  "ogisable_apply_enhance utt \n\
       Apply enhancement parameters to utt -- stored on tokens in enhance_info feature"
  (let ((TfeatL) (TfeatR) (Tgain) (Tstretch))
    
    (set! Lseg (car (utt.relation.items utt "Segment")))

    (mapcar 
     (lambda (tok) 
       (if (not (item.parent tok)) 
	   (begin
	     (set! do_it (item.feat tok "ogisable_enhance"))
	     (if (not (equal? do_it 0))
		 (begin
		   (set! enhinfo (item.feat tok "enhance_info"))
		   (set! params (read-from-string enhinfo))
		   
		   ;;; get feature specs
		   (set! TfeatL   (cadr (assoc 'TfeatL   params)))
		   (set! TfeatR   (cadr (assoc 'TfeatR   params)))		   
		   (set! Tgain    (cadr (assoc 'Tgain    params)))
		   (set! Tstretch (cadr (assoc 'Tstretch params)))

		    ;;; search thru Segments assoc with this token
		   (mapcar
		    (lambda (word)
		      (mapcar
		       (lambda (syl)
			 (mapcar
			  (lambda (Rseg)
			    ;;; look for match of feature spec
			    (if (and (featmatches Lseg TfeatL) (featmatches Rseg TfeatR))
				(begin
			       
 		                ;;; attach features to the Segments
				(item.set_feat Lseg "enh_Tgain" Tgain)
				(item.set_feat Lseg "enh_Tstretch" Tstretch)
				(item.set_feat Lseg "enh_orig_dur" (item.feat Lseg "segment_duration"))
				(item.set_feat Rseg "enh_Tgain" Tgain)
				(item.set_feat Rseg "enh_Tstretch" Tstretch)
				(item.set_feat Rseg "enh_orig_dur" (item.feat Rseg "segment_duration"))

				; controls what % of segment is affected by stretch/gain
				(set! Lbkpt (get_breakpoint Lseg))
				(set! Rbkpt (get_breakpoint Rseg))
				(item.set_feat Lseg "enh_Lbkpt" Lbkpt)
				(item.set_feat Rseg "enh_Rbkpt" Rbkpt)


                                ;;; apply duration modifications
				(set_dur utt Lseg (+ (* (- 1.0 Lbkpt) Tstretch (item.feat Lseg "segment_duration"))
						 (* Lbkpt (item.feat Lseg "segment_duration"))))
				(set_dur utt Rseg (+ (* Rbkpt Tstretch (item.feat Rseg "segment_duration"))
						 (* (- 1.0 Rbkpt) (item.feat Rseg "segment_duration"))))

				; set gain targets
				(set! end (item.feat Lseg "end"))
				(set! K (/ (- 1 Lbkpt) (* Lbkpt Tstretch)))
				(OGIadd_gain_targets utt (list 
				   (list 'begin (- end (* (/ 1 (+ 1 K)) (item.feat Lseg "segment_duration"))))
				   (list 'end end)
				   '(begin_val 1.0)
				   (list 'end_val Tgain)
				   '(method "sin")
				   '(samp 0.005)))

				(set! K (/ (- 1 Rbkpt) (* Rbkpt Tstretch)))
				(OGIadd_gain_targets utt (list 
				   (list 'begin end)
				   (list 'end (+ end (* (/ 1 (+ 1 K)) (item.feat Rseg "segment_duration"))))
				   (list 'begin_val Tgain)
				   '(end_val 1.0)
				   '(method "cos")
				   '(samp 0.005)))
				))
			    (set! Lseg Rseg)
			    )
			  (item.relation.daughters syl "SylStructure")))
		       (item.relation.daughters word "SylStructure")))
		    (item.daughters tok))
		   


    )))))
(utt.relation.items utt 'Token))
    
    )
(set! utt0 utt)
  utt)


;;; load so OGISABLE sees it
(set! after_analysis_hooks 
    (append (list ogisable_apply_enhance) after_analysis_hooks))

(define (get_breakpoint seg )
  "proportion of this segment in 'transition region' that should be stretched/compressed"
  
  (if (string-equal (item.feat seg "ph_vc") "+")
      (set! f 0.10)
      (cond ((string-equal (item.feat seg "ph_ctype") "s")
	   (set! f 0.3))
	  ((string-equal (item.feat seg "ph_ctype") "n")
	   (set! f 0.3))
	  (t
	   (set! f 0.4))))
  f)

(define (set_dur utt seg dur)
  "modify duration, and reset all following Segment and Target items"
  (set! seg (item.relation seg "Segment"))
  (set! diffdur (- dur (item.feat seg "segment_duration")))

  (if (item.prev seg)
      (set! end (item.feat (item.prev seg) "end"))
      (set! end 0.0))

  ;;; move F0 targets
  (set! f0targs (utt.relation.leafs utt "Target"))

  (while (> (length f0targs) 0)
    (set! f0 (car f0targs))
    (if (> (item.feat f0 "pos") end)
	(item.set_feat f0 "pos" (+ (item.feat f0 "pos") diffdur)))
    (set! f0targs (cdr f0targs))
    )

  ;; move Segments
  (item.set_feat seg "end" (+ end dur))
  (set! seg (item.next seg))  
  (while seg
    (item.set_feat seg "end" (+ (item.feat seg "end") diffdur))
    (set! seg (item.next seg))
    )
  t)

(define (featmatches seg lfeats)
  "return t if all feats in lfeats are matched by seg"  
  (let ((matchOK))
    (set! matchOK t)
    (mapcar 
     (lambda (entry)
       (if 
	   (not (string-equal (item.feat seg (car entry)) (format nil "%s" (car (cdr entry)))))
	   (set! matchOK nil))
       )
     lfeats)
    matchOK))

(provide 'ogi_enhance)


