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
;;;  OGI MWM5 diphone: Male American English (Mike Macon), version 000821 by Vincent Pagel


;; load unit selection routine
(set! ogi_mwm5_diphone_dir (cdr (assoc 'ogi_mwm5_diphone voice-locations)))
(set! load-path (cons (path-append ogi_mwm5_diphone_dir "festvox") load-path))
(set! load-path (cons (path-append libdir "ogi") load-path))

(require 'ogi_configure_voice)

;; select voice
;; this defines all the modules to use when synthesizing text using the current voice
(define (voice_ogi_mwm5_diphone)
  "(voice_ogi_mwm5_diphone)
  Set up the current voice to be an American male MWM using
  the MWM diphone set and OGIresLPC (recording 000821)."
  
  ;; set target average pitch and variation
  (set! ogi_mwm5_diphone_f0_mean 120)
  (set! ogi_mwm5_diphone_f0_std 20)
  
  ;; set unit selection method
  ;(set! OGI_unitsel ogi_mwm5_OGI_di_unitsel)
  (set! OGI_unitsel OGI_diphone_unitsel)
  
  ;; use the grouped file by default
  (set! ogi_grouped_or_ungrouped 'grouped)

  ;; use resLPC or sinLPC signal processing
  (set! ogi_resORsinLPC 'resLPC)

  ;; configure voice using defaults 
  (ogi_configure_voice 'ogi_mwm5_diphone)
  
  ;; overwrite defaults here:
  
  ;; For example, you can say 
  ;;   (Parameter.set 'Int_Method my_method)  to use another intonation method

  ;; diphone unit selection fallbacks
  (set! ogi_di_alt_L '((m= (m)) (n= (n)) (l= (l)) (h (pau)) (kh (k)) (th (t)) (ph (p)) (hv (h)) (hu (h))
		       (j (i:)) (dx (t d)) (& (^))))
  (set! ogi_di_alt_R '((m= (m)) (n= (n)) (l= (l)) (h (pau)) (kh (k)) (th (t)) (ph (p)) (hv (h)) (hu (h))
		       (j (i:)) (dx (t d)) (& (^)) (l (&l.))))
  (set! ogi_di_default "pau-h")
  
  (set! ogi_resLPC_syn_params  
   (list
    '(F0_default 100.0)     ;; default Fo used if can't find any Fo targets
    '(T0_UV_thresh 0.020)   ;; used to make V/UV decision (if F0 < 50Hz then UV)
    '(T0_UV_pm 0.010)       ;; used to place UV pmarks
    '(post_gain 1.0)        ;; adjust final loudness
    '(uv_gain 1)            ;; multiply unvoiced regions by this factor
   '(deemphasis 0.99)      ;; opposite of preemphasis (lowpass filter)
    '(mod_method "direct")  ;; method for realizing prosodic targets
;    '(mod_method "none")        ;; method for realizing prosodic targets
;    '(dur_method "natural")     ;; method for realizing prosodic targets
;    '(pitch_method "natural")   ;; method for realizing prosodic targets
    '(smooth_cross_ph_join "Y") ;; smooth across joins at PHONE boundaries?
    '(window_type "trapezoid") ;; ola window
    ogi_smooth_spectra
    ogi_smooth_power
    ogi_di_vqual
    ;ogi_dump
  ))
  
   (set! voicename 'ogi_mwm5_diphone)

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
;        (list 'gain_file (path-append ogi_diphone_dir "ungrouped" "gain.dat"))
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
        '(samp_freq 22050)
        '(sig_band 0.010) 
        '(isCompressed "Y") ;; if "Y", compress when saving group file
        '(preemph 0.96)  
    ))
  )
  
    ;; initialize signal processing module (do this *after* changing any synth or analysis params)
  (initialize_OGIsynth)

)






;; proclaim voice
(proclaim_voice
 'ogi_mwm5_diphone
 '((language english)
   (gender male)
   (dialect american)
   (description
    "This voice provides an American English male voice using a
     residual excited or sinusoidal LPC diphone synthesis module created at
     OGI.  It uses a lexicon compiled from MOBY and CMU lexicons, and
     other trained modules used by CSTR voices.")
      (samplerate 22050)))

;; comment this out if you want changes in this file to take effect without restarting Festival
(provide 'ogi_mwm5_diphone)   
