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
;;;  Set up diphone voice                                                 ;;
;;;  HVS diphones: female Mexican Spanish collected June 1997 by OGI/UDLA ;;
;;;  Mexican Spanish synthesis modules by A. Barbosa of UDLA, June 1997   ;;
;;;  Further improvements by A. Cronk.                                    ;;
;;;  OGI residual LPC synthesizer by M. Macon.                            ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; for common mexican spanish modules
(set! load-path (cons (path-append libdir "ogi") load-path))
(set! load-path (cons (path-append libdir "ogi/ogi_spanish/") load-path))
(require 'ogi_unitsel)
(require 'ogi_synthesis)
(require 'ogi_span_mx_phones)
(require 'ogi_span_mx_duration)
(require 'ogi_span_mx_intonation)
(require 'ogi_span_mx_lexicon)
(require 'ogi_span_mx_lts)
(require 'ogi_span_mx_phrasing)
(require 'ogi_span_mx_syl)
(require 'ogi_span_mx_token)

;;; Use the grouped file by default
(defvar hvsdi_grouped_or_ungrouped 'grouped)

;; Location at runtime
(defvar ogi_hvs_diphone_dir (cdr (assoc 'ogi_hvs_diphone voice-locations))
  "ogi_hvs_diphone_dir
  The default directory for the hvs diphone database.")
(set! load-path (cons (path-append ogi_hvs_diphone_dir "festvox") load-path))

(define (voice_ogi_hvs_diphone)
"(voice_ogi_hvs_diphone)
 Set up the current voice to be a Mexican Spanish female HVS using
 the HVS diphone set and OGIresLPC."

  ;; reset global parameters
  (voice_reset)

  ;; Phone set
  (Parameter.set 'Language 'spanish)
  (Parameter.set 'PhoneSet 'span_mx)
  (PhoneSet.select 'span_mx)
  
  ;; token to word rules
  (Parameter.set 'Token_Method 'Token_Any) 
  (set! token_to_words mx_spanish_token_to_words)
  (set! prev_token.punctuation token.punctuation)
  (set! token.punctuation "\"`.,:;!?(){}[]" "[see Tokenizing]")

  ;; No pos prediction
  (set! pos_lex_name nil)
  (set! pos_supported nil)

  (set! lex_user_unknown_word span_mx_lts)
  (lex.select "span_mx")

  ;;; Postlexical rules
  (set! postlex_vowel_reduce_cart_tree  nil)
  (set! postlex_rules_hooks nil)

  ;; Phrasing 
  (set! phrase_cart_tree span_mx_phrase_cart_tree)
  (Parameter.set 'Phrase_Method 'cart_tree)

  ;; Accent prediction 
  (set! int_accent_cart_tree span_mx_cart_tree)
  (set! multfactor 1.68)

  ;; set up intonation method
  (Parameter.set 'Int_Method 'General)
  (Parameter.set 'Int_Target_Method Int_Targets_General)
  (set! int_lr_params
	'((target_f0_mean 124) (target_f0_std 19) ;; the standard value for target_f0_mean 124
	  (model_f0_mean 170) (model_f0_std 34))) ;; aec only target_f0_mean is used by voice
                                                ;; and then only for compatibility between
 								;; english voices and tcl glue code

  (set! int_general_params
	(list 
	 (list 'targ_func span_mx_targ_func)))

  ;; Duration prediction
  (Parameter.set 'Duration_Method 'OGI_Spanish_Duration); 
  (Parameter.set 'Duration_Stretch 1.0); 

  ;; diphone unit selection fallbacks (none)
  (set! ogi_di_alt_L '((m= (m))  ))
  (set! ogi_di_alt_R '((m= (m))  ))
  (set! ogi_di_default "pau-f")

  (Parameter.set 'Synth_Method 'OGIdiphone)
  (OGIdbase.activate "ogi_hvs_diphone")      
  (OGIresLPC.init hvsdi_OGI_syn_params)  
  (set! current_voice_reset hvs_voice_reset)
  (set! current-voice 'ogi_hvs_diphone)  
)

(define (hvs_voice_reset)
  (set! token.punctuation prev_token.punctuation)
)


;; Context-dependent smoothing:
;;   (feat val int) for each
;;   Can put any feat,val pair here, as long as feat is defined in phoneset definition.
;;   Smoother setup algorithm iterates through list until feat, val is matched, then uses int as 
;;     smoothing window length.  This could be improved.
(set! hvsdi_smooth_spectra   ;;; max number of frames to use in smoothing LPC at joins
      '(spectra_smooth
        ((vc + 10)   ; vowel
         (ctype s 0) ; stop
         (ctype f 2) ; fricative
         (ctype a 0) ; affricate
         (ctype n 5) ; nasal
         (ctype l 5) ; lateral
         (ctype r 5) ; approximant
         )))

(set! hvsdi_smooth_power   ;;; max number of frames to use in smoothing power at joins
      '(power_smooth
        ((vc + 10)   ; vowel
         (ctype s 0) ; stop
         (ctype f 2) ; fricative
         (ctype a 0) ; affricate
         (ctype n 5) ; nasal
         (ctype l 5) ; lateral
         (ctype r 5) ; approximant 
         )))
;;
;;  Residual LPC synthesizer parameters - used by OGIresLPC.init
;;
(set! hvsdi_OGI_syn_params  
 (list
  '(F0_default 50.0)      ;; default Fo used if can't find any Fo targets
  '(T0_UV_thresh 0.020)   ;; used to make V/UV decision 
  '(T0_UV_pm 0.010)       ;; used to place UV pmarks
  '(post_gain 1.0)        ;; adjust final loudness
  '(deemphasis 0.94)      ;; opposite of preemphasis (lowpass filter)
  '(mod_method "direct")  ;; method for realizing prosodic targets
  '(beta_smooth 3)        ;; smoother len for pitch mod factor in "soft" mod method
  '(window_type "trapezoid") 
  '(smooth_cross_ph_join "Y") ;; smooth across joins at phone boundaries?
  '(spectra_match_or_replace "match") ;; for join smoothing
  '(power_match_or_replace   "match") ;; for join smoothing
  hvsdi_smooth_spectra
  hvsdi_smooth_power
  ))


(define (init_ogi_hvs_diphone)
"(init_ogi_hvs_diphone)
  Initialise the HVS diphone database."
  (set! voicename 'ogi_hvs_diphone)

(if (equal? hvsdi_grouped_or_ungrouped 'ungrouped)
    ;;  ungrouped
    (OGIdbase.init
     (list
      '(dbname ogi_hvs_diphone)
      (list 'unitdic_file (path-append ogi_hvs_diphone_dir "festvox" "hvsdiph.msec"))
      '(phoneset "span_mx")  
      '(base_dir "/u/macon/TTS/tts_data/hvs/")
      '(lpc_dir "lpc16/")
      '(lpc_ext ".lpc")
      '(exc_dir "lpc16/")
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

    ;;  grouped -- parameters set here override those in groupfile
    (OGIdbase.init   
     (list
      '(dbname ogi_hvs_diphone)
      (list 'groupfile (path-append ogi_hvs_diphone_dir "group" (string-append voicename "_reslpc.group")))
      '(data_type "resLPC")
      '(access_mode "ondemand")
      ))
    t)
)

(init_ogi_hvs_diphone)


(proclaim_voice
 'ogi_hvs_diphone
 '((language spanish)
   (gender female)
   (dialect mexican)
   (description
    "This voice provides a Mexican Spanish female voice using a
     residual excited LPC diphone synthesis module, both created at
     OGI.  The voice data was collected and labeled by Alejandro Barbosa
     of UDLA (Puebla, Mexico).  It uses fairly simple intonation
     and duration rules, with letter-to-sound rules for pronunciation.
     The OGIresLPC synthesizer module is a plug-in to Festival and is 
     available at http://www.cse.ogi.edu/OGI/tts.")
    (samplerate 16000)))
 
(provide 'ogi_hvs_diphone)
