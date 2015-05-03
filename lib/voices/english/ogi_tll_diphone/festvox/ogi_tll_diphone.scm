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

;;;  Set up diphone voice
;;;  OGI TLL diphone: female American English (Mike Macon), version 000821 by Vincent Pagel


;; load unit selection routine
(set! ogi_tll_diphone_dir (cdr (assoc 'ogi_tll_diphone voice-locations)))

(set! load-path (cons (path-append ogi_tll_diphone_dir "festvox") load-path))

(set! load-path (cons (path-append libdir "ogi") load-path))

(require 'ogi_configure_voice)

;; select voice
;; this defines all the modules to use when synthesizing text using the current voice
(define (voice_ogi_tll_diphone)
  "(voice_ogi_tll_diphone)
  Set up the current voice to be an American female TLL using the tll diphone set."
  
  ;; set target average pitch and variation
  (set! ogi_tll_diphone_f0_mean 210)
  (set! ogi_tll_diphone_f0_std 40)
  
  ;; set unit selection method
  (set! OGI_unitsel OGI_diphone_unitsel)
  
  ;; use the grouped file by default
  (set! ogi_grouped_or_ungrouped 'grouped)

  ;; use resLPC or sinLPC signal processing
  (set! ogi_resORsinLPC 'resLPC)

  ;; configure voice using defaults 
  (ogi_configure_voice 'ogi_tll_diphone)
  
  ;; overwrite defaults here:
  
  ;; diphone unit selection fallbacks
  (set! ogi_di_alt_L '((m= (m)) (n= (n)) (l= (l)) (h (pau)) 
           (j (i:)) (dx (t d)) (& (^))
           (> (A))  ;; she needs this one
           (k>9r (k)) (k>w (k)) (k>l (k))
           (p>9r (p)) (p>w (p)) (p>l (p))
           (t>9r (t)) (t>w (t)) (t>l (t)) (t>9r<s (t>9r t<s t)) 
           (p>9r<s (p>9r p)) (t<s (t))))

  (set! ogi_di_alt_R '((m= (m)) (n= (n)) (l= (l)) (h (pau)) 
           (j (i:)) (dx (t d)) (& (^))
           (> (A))  ;; she needs this one
           (k>9r (k)) (k>w (k)) (k>l (k))
           (p>9r (p)) (p>w (p)) (p>l (p))
           (t>9r (t)) (t>w (t)) (t>l (t)) 
           (t>9r<s (t>9r t<s t)) (p>9r<s (p>9r p))
           (t<s (t))))

  ;; phrase curve values
  (set! AccentMaxStart 65);50
  (set! AccentMaxMid 40);30
  (set! AccentMaxEnd 50);15
  (set! AccentQuestion 80)
  (set! AccentComma2 20) ;; 40
  (set! AccentComma3 10) ;; 15
  (set! AccentComma4 30) ;; 60
  (set! PhraseStart 220);130
  (set! PhraseMid 180);110
  (set! PhraseEnd 140);90

  ;; reslpc ungrouped analysis
  ;; define analysis parameters for OGIresLPC module
  (set! voicename 'ogi_tll_diphone)
  (if (string-equal ogi_grouped_or_ungrouped "grouped")
    (set! ogi_resLPC_analysis_params (list
      (list 'dbname voice_dirname)
      (list 'groupfile (path-append ogi_diphone_dir "group" (string-append voicename "_resLPC.group")))
      '(data_type "resLPC")
      '(access_mode "ondemand")
    ))
    ;; else if ungrouped
    (set! ogi_resLPC_analysis_params (list
        (list 'dbname voice_dirname)
        (list 'unitdic_file (path-append ogi_diphone_dir "ungrouped" "unittable.ms"))
        ;(list 'gain_file (path-append ogi_diphone_dir "festvox" "gain.dat"))
        '(phoneset "worldbet")  
        (list 'base_dir  (path-append ogi_diphone_dir "ungrouped/"))
        '(lpc_dir "lpc/")
        '(lpc_ext ".lsf")
        '(exc_dir "lpc/")
        '(exc_ext ".res")
        '(pm_dir "pm/")
        '(pm_ext ".pmv")
        '(data_type "resLPC")
        '(access_mode "ondemand")
        '(samp_freq 16000)
        '(sig_band 0.010) 
        '(isCompressed "Y") ;; if "Y", compress when saving group file
        '(preemph 0.96)  
    ))
  )

  ;; analysis parameters for OGIsinLPC module
  (set! ogi_sinLPC_analysis_params  
    (list
      (list 'voicename voice_dirname)
      (list 'groupfile (path-append ogi_diphone_dir "group" (string-append voicename "_sinLPC.group")))
      '(access_mode "ondemand")  ; ondemand OR load_all
      '(useCompressed 0)  ;; compress groupfile if one is created.  Reduces size by 30%, but mulaw artifacts in nasals.  
      '(doSinAnalysis 1)
    )
  )
  (if (string-equal ogi_grouped_or_ungrouped "grouped")
    (set! ogi_sinLPC_analysis_params 
      (append ogi_sinLPC_analysis_params (list '(useGrouped 1)))
    )
    ;; else if ungrouped
    (set! ogi_sinLPC_analysis_params (append ogi_sinLPC_analysis_params 
      (list
        '(useGrouped 0)
        (list 'unittable_file (path-append ogi_diphone_dir "ungrouped" "unittable.ms"))
        (list 'pmdir (path-append ogi_diphone_dir "ungrouped" "pm"))
        (list 'wavdir (path-append ogi_diphone_dir "ungrouped" "wav"))
        (list 'sinlpcdir (path-append ogi_diphone_dir "ungrouped" "sinlpc"))
        '(sinlpc_save_format "est_ascii")  ;; other option: est_binary
        '(Fs 16000)         ;; sampling frequency of database units
        '(Fvox 4000)        ;; compute sinusoidal params only in 0-Fvox frequency band
        '(vuv_filt          ;; result of Matlab  fir1(6,2*4000/22050) 
          ((-0.002650233384 0.042637343726 0.253502315847 0.413021147621 0.253502315847 0.042637343726 -0.002650233384)
           (1.0)))
        '(lpcorder 14)      ;; lpcorder (for mel warped spectrum)
        '(pre-emph .95)
        '(show_chunk_pms 0)
      )
    ))
  )

  ;; initialize signal processing module 
  (initialize_OGIsynth)
  
)

;; proclaim voice
(proclaim_voice
 'ogi_tll_diphone
 '((language english)
   (gender female)
   (dialect american)
   (description
    "This voice provides an American English female voice using a
     residual excited or sinusoidal LPC diphone synthesis module created at
     OGI.  It uses a lexicon compiled from MOBY and CMU lexicons, and
     other trained modules used by CSTR voices.")))

;; comment this out if you want changes in this file to take effect without restarting Festival
(provide 'ogi_tll_diphone)   
