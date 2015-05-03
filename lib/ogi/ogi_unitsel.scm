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
;;   ((name    retname)  ;; should be same as one we asked for
;;    (db_indx dbindx#)
;;    (Lindx  Lindx# )
;;    (segs   (Uinfo1 Uinfo2 ...)))
;;
;;	  where each UinfoN is of the form
;;	    (segname
;;		 ((dur durval)
;;		  (featA featAval)    ;; these aren't used yet
;;		  (featB featBval))
;;

(define (item.alloname seg) 
  (let ((name (item.name seg)))
    (cond
     ((string-equal (item.feat seg "allophone_name") "dx") 
      "dx")
     ((string-equal (item.name seg) "h")
      (if (string-equal (item.feat (item.prev seg) "ph_cvox") "-")
	  "hu"
	  "hv"))
     ((string-matches (item.feat seg "allophone_name") "[ptk]h")
      (item.feat seg "allophone_name"))
     (t
     name))))

(define (triphone seg)
  (set! nseg (item.relation.next seg 'Segment))
  (set! nnseg (item.relation.next nseg 'Segment))
  (set! pseg (item.relation.prev seg 'Segment))

  (cond
   ((and (string-equal (item.feat nseg "name") "&")
	 (and (string-equal (item.feat nnseg "name") "m")
	      (string-equal (item.feat nnseg "ogi_onsetcoda") "coda")))
    (string-append (item.alloname seg) "+m"))
   ((and (string-equal (item.feat nseg "name") "&")
	 (and (string-equal (item.feat nnseg "name") "n")
	      (string-equal (item.feat nnseg "ogi_onsetcoda") "coda")))
    (string-append (item.alloname seg) "+n"))
   ((and (string-equal (item.feat nseg "ph_vc") "+")
	 (string-equal (item.feat nnseg "name") "l")
	 (string-equal (item.feat nnseg "ogi_onsetcoda") "coda"))
    (string-append (item.alloname seg) "+l"))
   ((and (string-equal (item.feat nseg "ph_vc") "+")
	 (string-equal (item.feat nnseg "name") "9r")
	 (string-equal (item.feat nnseg "ogi_onsetcoda") "coda"))
    (string-append (item.alloname seg) "+9r"))
   
   ((and (string-equal (item.feat seg "name") "&")
	 (and (string-equal (item.feat nseg "name") "m")
	      (string-equal (item.feat nseg "ogi_onsetcoda") "coda")))
    (string-append (item.alloname pseg) "+m"))
   ((and (string-equal (item.feat seg "name") "&")
	 (and (string-equal (item.feat nseg "name") "n")
	      (string-equal (item.feat nseg "ogi_onsetcoda") "coda")))
    (string-append (item.alloname pseg) "+n"))
   ((and (string-equal (item.feat seg "ph_vc") "+")
	 (string-equal (item.feat nseg "name") "l")
	 (string-equal (item.feat nseg "ogi_onsetcoda") "coda"))
    (string-append (item.alloname pseg) "+l"))
   ((and (string-equal (item.feat seg "ph_vc") "+")
	 (string-equal (item.feat nseg "name") "9r")
	 (string-equal (item.feat nseg "ogi_onsetcoda") "coda"))
    (string-append (item.alloname pseg) "+9r"))
   (t
    0))
  )


; loop thru segments, create dbUnit stream, lookup diphone


(define (OGI_di_alt L R)
  "(OGI_di_alt L R)
Find alternate fallback diphone when original not found.  Uses global variables
ogi_di_alt_L and ogi_di_alt_R which should be set in the voice definition."

  ;; create a list of potential fallback diphones:
  ;;  (itself + all listed alternates)
  (set! nlist nil)
  (mapcar 
   (lambda (Lseg)     
     (mapcar 
      (lambda (Rseg)
	(set! nlist (append nlist (list (string-append Lseg "-" Rseg))))
	)
      (append (list R) (cadr (assoc (intern R) ogi_di_alt_R)))))
   (append (list L) (cadr (assoc (intern L) ogi_di_alt_L))))
  
  ;; run through the list and take 1st one we can find
  (set! indx nil)  
  (while (equal? indx nil)
    (set! indx (OGIdbase.unitsel_name (car nlist)))
    (set! altname  (car nlist)) ;; just to print
    (set! nlist (cdr nlist))
    (if (and (equal? indx nil) (equal? nlist nil))
	;; if all else fails...
	(begin 
   (set! indx (OGIdbase.unitsel_name ogi_di_default))
	 ;; if still fails...
	 (if (equal? indx nil)
	     (set! indx 0)))))

;  (print (string-append "  alternate "  altname  " (" indx ") selected for " L "-" R ))
indx)

(define (OGI_diphone_unitsel utt)
  "Replace unit selection method defined in ogi_unitsel.scm.
   Strategy: if vowel or glide, determine syllable boundary, 
   check if special case diphone exists" 
  (let ((SelectedUnits) (seglist (utt.relation.items utt "Segment"))
	segL
	segR
	Lnm
	Rnm
	diphL
	diphR
	cand
	ogi_debug_unitsel nil)
;	ogi_debug_unitsel)
    (mapcar
     (lambda (segL)
       (set! segR (item.next segL))
        (if (equal? segR nil)
	   nil    ;; if last segment of utt, don't do anything
	   ;; else 
	   (begin
	     (if (not (string-equal (item.alloname segR) "dx"))
		      (set! Rnm (item.name segR))
		      (set! Rnm (item.alloname segR)))

	     (set! Lnm (item.alloname segL))
	     ;(format t "%s %s\n" (item.name segL) (item.alloname segL))
	     (set! diphL Lnm)
	     (set! diphR Rnm)
	     (cond

;	       )
	      ;; glottal stop
	      ((string-equal (item.name segL) "?")
	       (set! diphL "pau"))
	      )


	     (set! cand (string-append diphL "-" diphR))

	     (if (not (string-equal (item.feat segL "lisp_triphone") "0"))
		 (set! cand (string-append cand "+" (item.feat segL "lisp_triphone"))))

	     (if ogi_debug_unitsel	    
		 (format stderr " diphL-diphR= %s-%s\n" diphL diphR))
	     
	     
	     ;; look up in dbase
	     (set! indx (OGIdbase.unitsel_name cand))
	     (if ogi_debug_unitsel	    
	     (format stderr "%s\n" indx))
	     
	     (if (equal? indx nil)
		 (if ogi_debug_unitsel	  
		     (format stderr "FAILED......................... BACK TO %s %s\n" Lnm Rnm)
		     )
		 )


	     ;; look for alternates if original not found
	     (if (equal? indx nil)
		 (set! indx (OGI_di_alt Lnm Rnm)))
	     
	     ;; lookup info structure for this unit
	     (set! sel (list (OGIdbase.lookup_info indx)))
	     
	     ;; append info to list
	     (set! SelectedUnits (append SelectedUnits sel))
	     )))
     seglist)
    SelectedUnits
    )
  )



(define (OGI_demi_unitsel utt)
  "(OGI_demi_unitsel utt)
Does a demiphone selection (similar to diphone synthesis)."
  ;; Note: 1st and last /pau/ will have only 1 demiphone, not 2
  (set! SelectedUnits nil)
  (mapcar
   (lambda (seg)
     (if (equal? (item.next seg) nil)
	 nil    ;; if last seg, don't do anything
	 (begin
	   
	   ;;
	   ;; X>Y - right demiphone X with right context Y
	   ;;
	   (set! n (string-append (item.name seg) ">" (item.name (item.next seg))))
	   (set! indx (OGIdbase.unitsel_name n))
(print "still need to fix alternates mechanism here")
	   ;; look for alternates if original not found
	   ;; may need to change output of lookup_info too!
	   (if (equal? indx nil)
	       (set! indx 0))
	   
	   (set! sel (list (OGIdbase.lookup_info indx)))
	   (set! SelectedUnits (append SelectedUnits sel))
	   
	   ;;
           ;; Y<Z - left demiphone Y with left context Z
	   ;;
	   (set! n (string-append (item.name (item.next seg)) "<" (item.name seg)))
	   (set! indx (OGIdbase.unitsel_name n))


	   ;; look for alternates if original not found
	   ;; may need to change output of lookup_info too!
	   (if (equal? indx nil)
	       (set! indx 1))

	   (set! sel (list (OGIdbase.lookup_info indx)))
	   (set! SelectedUnits (append SelectedUnits sel))
	 )))
   (utt.relation.items utt "Segment"))
  SelectedUnits)




(define (OGI_set_dbUnit_feats utt SelectedUnits)
  "(OGI_set_dbUnit_feats utt SelectedUnits)
       Put necessary features into the dbUnit relation for OGI waveform synthesizer.
       SelectedUnits is a list of structures containing necessary dbUnit info."
 
   ;; create dbUnit stream
   (utt.relation.create utt "dbUnit")

   ;; Set FEATURES on the dbUnit for each SelectedUnit
   (mapcar
    (lambda (sel)
      ;; append a new dbUnit
      (set! uttUnit (utt.relation.append utt "dbUnit"))
      
      ;; set its name (for debug)
      (item.set_name uttUnit (cadr (assoc 'name sel)))

      ;; index into current dbase
      (item.set_feat uttUnit "db_indx" (cadr (assoc 'db_indx sel)))

       ;; is previous unit this unit's natural neighbor?
      (item.set_feat uttUnit "isNatNbL" 0)

      (if (item.prev uttUnit)
	  (if (equal? (parse-number (item.feat (item.prev uttUnit) "db_indx"))
		  (parse-number (cadr (assoc 'Lindx sel))))
		(item.set_feat uttUnit "isNatNbL" 1)
		(item.set_feat uttUnit "isNatNbL" 0)))

      ;; is previous unit in the same segment as begin of this one?
      (if (equal? (cadr (assoc 'Lbnd sel)) 1)
	  (item.set_feat uttUnit "isSegBndL" 1)
	  (item.set_feat uttUnit "isSegBndL" 0)
	  )

      ;; is next unit in the same segment as end of this one?
      (if (equal? (cadr (assoc 'Rbnd sel)) 1)
	  (item.set_feat uttUnit "isSegBndR" 1)
	  (item.set_feat uttUnit "isSegBndR" 0)
	  )

      ;; append daughters to the dbUnit for each segment in the unit
      (mapcar
       (lambda (uinfo)
  	   (item.append_daughter uttUnit uinfo))
       (cadr (assoc 'segs sel)))
       
       ;; indicate last unit
       (if (not (item.next uttUnit))
         (item.set_feat uttUnit "isLastUnit" 1))
    )
    SelectedUnits))


(define (OGI_dummy_name_unitselect uname)
  "This is just a dummy - real one in C++..."
(parse-number (intern (string-before (* 99 (rand)) ".")))
)


