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

;; ogi_configure_voice.scm
;;  Set up diphone voice
;;  General set up for all voices. Voice specific parameters can be set up in
;;  the voice definition files

(require 'phrase)
(require 'pos)
(require 'tobi)
(require 'tilt)
(require 'f2bf0lr)
(require 'ogi/ogi_worldbet_phones)
(require 'ogi/ogi_lexicons)
(require 'ogi/ogi_lex_addenda)
;(require 'lex_addenda)
(require 'ogi/ogi_syllabify)
(require 'ogi/ogi_lts_syllabify)
(require 'ogi/ogi_postlex)
(require 'ogi/ogi_phrase)
(require 'ogi/ogi_lts_worldbet)
(require 'ogi/ogi_unitsel)
(require 'ogi/ogi_synthesis)

;YK 2011-07-25 Why aren't these included? Added:
(require 'ogi/ogi_effect)
;(require 'ogi/ogi_enhance) ; EST Error: Feature token not defined
(require 'ogi/ogi_languages)
(require 'ogi/ogi_token)
(require 'ogi/ogi_wordsynth)

(define (ogi_configure_voice voicename)

;  (set! voice_dirname (read-from-string (string-append voicename '_diphone)))  
  (set! voice_dirname voicename)
  (set! ogi_diphone_dir (cdr (assoc voice_dirname voice-locations)))

  (set! newint 0)
  (set! newdur 0)

  ;;; load MOBY+CMU lexicon if needed
  (setup_ogi_lex)
  ;(function_words_addenda)
  ;(lex_addenda)
  ;; reset global parameters
  (voice_reset)

  ;; Phone set
  (Parameter.set 'Language 'americanenglish)
  (Parameter.set 'PhoneSet 'worldbet)
  (PhoneSet.select 'worldbet)
  
  ;; Token to word rules
  (set! token_to_words english_token_to_words)
  
  ;; POS tagger
  (set! pos_lex_name "english_poslex")
  (set! pos_ngram_name 'english_pos_ngram)
  (set! pos_supported t)
  (set! guess_pos english_guess_pos)   ;; need this for accents
  (set! pos_map english_pos_map_wp39_to_wp20)

  ;; Lexicon selection
  (lex.select "ogi")
  
  ;; Postlexical rules (some defined in "ogi_postlex.scm")
  (set! postlex_vowel_reduce_cart_tree  OGI_postlex_vowel_reduce_cart_tree_hand) ;; must set this!
  (set! postlex_rules_hooks (list OGI_PostLex_Reduce The_Before_Vowel postlex_apos_s_check syllabify_lts secondary_stress_hack 
  plosive_aspiration flap_hack ogi_onsetcoda))
  
  ;; Phrase prediction
  (set! festival-phrasify Phrasify)
  (set! Phrasify OGI_PuncPhrasify)
;  (Parameter.set 'Phrase_Method 'prob_models)  
;  (set! phr_break_params OGI_english_phr_break_params)
  (Parameter.set 'Phrase_Method 'cart_tree)
  (set! phrase_cart_tree simple_phrase_cart_tree)
  ;; Accent and tone prediction
  (set! int_tone_cart_tree f2b_int_tone_cart_tree)
  (set! int_accent_cart_tree f2b_int_accent_cart_tree)

  ;; F0 prediction

  ;; check if OGI intonation module is present
  ;; Note: this module is currently only available to researchers at OGI

  (if (and (probe_file (path-append libdir 'ogi_foot_structure.scm))
	   (equal? newint 1))
      (begin
      (format stderr "Using OGI intonation module...\n")
	(require 'ogi_foot_structure)
	(require 'ogi_word_feat)
	(set! AccentMaxStart 65);50
	(set! AccentMaxMid 40);30
	(set! AccentMaxEnd 50);15
	(set! AccentQuestion 80)
	(set! AccentComma2 20)
	(set! AccentComma3 10)
	(set! AccentComma4 30)
	(set! PhraseStart 110);130
	(set! PhraseMid 90);110
	(set! PhraseEnd 70);90
	(defUttType Text
          (Initialize utt)
          (Text utt)
          (Token_POS utt)
          (Token utt)
          (POS utt)
          (Phrasify utt)
          (Word utt)
	  (word_feat utt)
          (Pauses utt)
          (Intonation utt)
          (PostLex utt)
          (Duration utt)
          (Foot utt)
          (foot_feat utt)
          (Int_Targets utt)
          (OGI_Intonation utt)
          (Wave_Synth utt)
          )
	
        (defUttType Tokens   ;; This is used in Sable, Tokens will be preloaded
          (Token_POS utt)    ;; when utt.synth is called
          (Token utt)
          (POS utt)
          (Phrasify utt)
          (Word utt)
	  (word_feat utt)
          (Pauses utt)
          (Intonation utt)
          (PostLex utt)
          (Duration utt)
          (Foot utt)
          (foot_feat utt)
          (Int_Targets utt)
          (OGI_Intonation utt)
          (Wave_Synth utt)
          )
        )
      )
  
  (set! f0_lr_start f2b_f0_lr_start)
  (set! f0_lr_mid f2b_f0_lr_mid)
  (set! f0_lr_end f2b_f0_lr_end)
  (Parameter.set 'Int_Method Intonation_Tree)
  (set! target_f0_mean (eval (read-from-string (string-append voicename '_f0_mean))))
  (set! target_f0_std (eval (read-from-string (string-append voicename '_f0_std))))
  (set! int_lr_params
    (list (list 'target_f0_mean target_f0_mean) ;; f0 mean of target speaker
          (list 'target_f0_std target_f0_std)     ;; f0 deviation of target speaker
  	'(model_f0_mean 170) '(model_f0_std 34)))     ;; these depend on training data f2b 
  (Parameter.set 'Int_Target_Method Int_Targets_LR)


  ;; Duration prediction

  ;; Check if OGI SOP duration module is present
  ;; Note: This model is currently only available to OGI researchers

  (if (and (probe_file (path-append libdir 'ogi_dur_sop.scm))
	   (equal? newdur 1))
      (begin
      (format stderr "Using OGI duration module...\n")
	(require 'ogi_dur_sop)
	(Parameter.set 'Duration_Stretch 1.25)
	(set! duration_sop_params nil)
	(Parameter.set 'Duration_Method Duration_OGIdur_sop)
	(set! PhoneMap.table (phone_map.table))
	)
      (begin
	(require 'ogi_kddurtreeZ_wb)
	(set! duration_cart_tree kd_duration_cart_tree_wb)
	(set! duration_ph_info kd_durs_wb)
	(Parameter.set 'Duration_Method Duration_Tree_ZScores)
        (Parameter.set 'Duration_Stretch 1.2)
	))

  
  ;; diphone unit selection fallbacks
  (set! ogi_di_alt_L '((m= (m)) (n= (n)) (l= (l)) (h (pau)) 
		       (j (i:)) (dx (t d)) (& (^))))
  (set! ogi_di_alt_R '((m= (m)) (n= (n)) (l= (l)) (h (pau)) 
		       (j (i:)) (dx (t d)) (& (^)) (l (&l.))))
  (set! ogi_di_default "pau-h")
  
  ;;
  ;;  Residual LPC synthesizer parameters - used by OGIresLPC.init
  ;;

  ;; Context-dependent smoothing 
  ;;   (feat val int) for each
  ;;   Can put any feat,val pair here, as long as feat is defined in phoneset definition.
  ;;   Smoother setup algorithm iterates through list until feat, val is matched, then uses int as 
  ;;     smoothing window length.  This could be improved.
  (set! ogi_smooth_spectra   ;;; max number of frames to use in smoothing LPC at joins
    '(spectra_smooth
      ((vc + 30)   ; vowel      10
       (ctype s 0) ; stop        0
       (ctype f 10) ; fricative   2
       (ctype a 0) ; affricate   0
       (ctype n 10) ; nasal       4
       (ctype l 10) ; lateral     4
       (ctype r 10) ; approximant 4
  )))

  (set! ogi_smooth_power   ;;; max number of frames to use in smoothing power at joins
    '(power_smooth
      ((vc + 30)   ; vowel      10
       (ctype s 0) ; stop        0
       (ctype f 10) ; fricative   2
       (ctype a 0) ; affricate   0
       (ctype n 10) ; nasal       5
       (ctype l 10) ; lateral     5
       (ctype r 10) ; approximant 5 
  )))

  (set! ogi_dump '(dump (
     (srcseg  "srcseg.phn")
     (srcexc  "srcexc")      ;; dumps .wav and joins (.phn)
     (srcpm   "srcpm.est")   ;;  (exc pmarks)
     (srcpm_as_wav "srcpm.wav")  
     (srclpc  "srclpc.est")  ;;  (LSF, E, etc.)
     (targexc "targexc.wav") ;; 
     (targseg "targseg.phn")
     (targjoin "targjoin.phn")
     (targpm  "targpm.est")  ;; 
     (targpm_as_wav "targpm.wav")  
     (targlpc "targlpc.est") ;;  (LSF, E, etc.)
     (output  "out.wav")     ;; 
     (chunks  "chunk")       ;; dumps .wav and .phn
  )))

  (set! ogi_di_vqual '(vqual_mod
    ((vt_global_warp_wave 1.0)
     ;(vt_voiced_warp_wave 0.90)
     (vt_global_warp_lsf 1.0)
     (vt_voiced_warp_lsf 1.0))))          

  (set! ogi_resLPC_syn_params  
   (list
    '(F0_default 100.0)     ;; default Fo used if can't find any Fo targets
    '(T0_UV_thresh 0.020)   ;; used to make V/UV decision (if F0 < 50Hz then UV)
    '(T0_UV_pm 0.010)       ;; used to place UV pmarks
    '(post_gain 1.0)        ;; adjust final loudness
    '(uv_gain 1)            ;; multiply unvoiced regions by this factor
   '(deemphasis 0.99)      ;; opposite of preemphasis (lowpass filter)
   '(mod_method "direct")  ;; method for realizing prosodic targets
    ;'(mod_method "none")        ;; method for realizing prosodic targets
    ;'(dur_method "natural")     ;; method for realizing prosodic targets
    ;'(pitch_method "natural")   ;; method for realizing prosodic targets
    '(smooth_cross_ph_join "Y") ;; smooth across joins at PHONE boundaries?
    '(window_type "trapezoid") ;; ola window
    ogi_smooth_spectra
    ogi_smooth_power
    ogi_di_vqual
    ;ogi_dump
  ))

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
  
   (set! ogi_sinLPC_syn_params  
    (list
    ;; general params
    '(F0_default 100.0)      ;; default Fo used if can't find any Fo targets
    '(uv_gain 1)             ;; multiply unvoiced regions by this factor
    '(T0_UV_pm 0.01)

    ;; F0 and timing modification
    ;; set to "close" to preserve timing or F0 of original units, "target" for predicted contours
    ;'(F0_method "target") 
    '(F0_method "close")
    '(do_dyn_F0_control 1) '(scale_orig_F0 1.)  ;; not used if F0_method is "target"
    '(dur_method "target")  ;; by Qi Miao to using target duration method
    
    ;; params for debuging and experimentation
    '(show_synth_progress 0)  ;; shows which step in synthesis is taking place.  SinSynth clearly slowest
    '(doSin 1) 
    '(do_synth_FFT 0) ;; much faster, but need non-order-of-2 fft order.  Current results very poor.
    '(do_sin_interp 0) '(do_double_psola 0) '(open_quotient .6)
    '(doShow 0) '(showpm 0.0) ;; dump sinwarp vectors for display, starting from first frame after showpm
    '(show_LSF_traj 0)
    '(show_E_traj 0)
    '(show_F0_traj 0)
    '(showProsGain 0)  

    ;; params for dynamic control of LSF and energy trajectories
    '(do_dyn_LSF_control 1)
    '(do_dyn_Energy_control 0)
    ;; delta weights :  a*(b+delta)^c
    '(delta_wt_a_E 10)
    '(delta_wt_a_F0 10)
    ;; weights for LSF time-derivatives
    '(delta_wt_a 600)
    (list 'delta_wt_b (* .05 3.1428))
    '(delta_wt_c 1)
    ;; weights for LSF distances
    '(delta_wt_a2 7)
    (list 'delta_wt_b2 (* .05 3.1428))
    '(delta_wt_c2 -2)

    ;; prosgain params: slopes of E, LSF, .. are attenuated depending on prosodic factors
    '(no_stress_B .8)
    '(no_stress_E .9)
    '(no_accent_B .9)  ;; if no accent but has stress
    '(no_accent_E .95)
    '(final_B .9)
    '(final_E .7)
    '(func_B .8)
    '(func_E .8)
    '(style 1)         ;; use this to attenuate dynamics in all sonorant segments
))
    ;; define analysis parameters for OGIsinLPC module
    ;; Note: The sinlpc module is currently only available to OGI researchers
    (set! ogi_sinLPC_analysis_params  
      (list
        (list 'voicename voice_dirname)
        (list 'groupfile (path-append ogi_diphone_dir "group" (string-append voicename "_diphone_sinLPC.group")))
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
          ;;'(Fvox 3500)        ;; compute sinusoidal params only in 0-Fvox frequency band
          '(Fvox 7500)        ;; compute sinusoidal params only in 0-Fvox frequency band
          '(vuv_filt          ;; result of Matlab  fir1(6,2*4000/22050) now use new data by fir1(6, 2*3500/1600) for 16k
           ;; ((-0.002650233384 0.042637343726 0.253502315847 0.413021147621 0.253502315847 0.042637343726 -0.002650233384)
           ;;((-0.00749288678367 0.02004497021714 0.25521060735909 0.46447461841488 0.25521060735909 0.02004497021714 -0.00749288678367)
           ;;for 7000hz
           ;;(( 0.0078 -0.0346 0.093 0.8676 0.093 -0.0346 0.0078)
           ;;for 7500Hz
           (( 0.0047 -0.0188 0.0476 0.933 0.0476 -0.0188 0.0047)
           (1.0)))
          ;;'(vuv_filt ((1.0) (1.0)))
          '(lpcorder 14)      ;; lpcorder (for mel warped spectrum)
          '(pre-emph .95)
          '(show_chunk_pms 0)
          
          
        )
      ))
    )
  

  ;; define synthesis method
  ;; sets unit selection method and signal processing method
  (defSynthType OGI_Synth
    (let (selectedUnits)
    ;; select units and set features of "dbUnit" relation
    (if OGI_unitsel (begin
      (set! selectedUnits (OGI_unitsel utt))
      ;(format t "%l\n" selectedUnits)
      (OGI_set_dbUnit_feats utt selectedUnits)
    ))
    ;; synthesize
    (ogi_synth_function utt)
  ))
  (Parameter.set 'Synth_Method 'OGI_Synth)

  (set! current_voice_reset ogi_voice_reset)
  (set! current-voice voice_dirname)  

)

(define (initialize_resLPC)
  (set! dbase_lookup_info OGIdbase.lookup_info)  ;; allows to switch between OGIresLPC and OGIsinLPC
  (set! dbase_unitsel_name OGIdbase.unitsel_name)
  (set! ogi_synth_function OGIresLPC.synth)
  ;; only load database once
  (if (not (member voice_dirname (OGIdbase.list)))
    (OGIdbase.init ogi_resLPC_analysis_params)  ;; will delete and re-init existing database
  )
  (OGIdbase.activate voice_dirname) 
  (OGIresLPC.init ogi_resLPC_syn_params)  
)

(define (initialize_sinLPC)
  (set! ogi_synth_function OGIsinLPC.synth)
  (set! OGIdbase.lookup_info OGIsinDbase.lookup_info) ;; allows to switch between OGIresLPC and OGIsinLPC
  (set! OGIdbase.unitsel_name OGIsinDbase.unitsel_name)
  (OGIsinDbase.init ogi_sinLPC_analysis_params)  ;; will update but not delete and re-init existing dbase
  (OGIsinDbase.activate voice_dirname)   ;; name of database to make current   
  (OGIsinLPC.init ogi_sinLPC_syn_params)
)

(define (initialize_OGIsynth)
  (if (not (boundp 'prev_dbase_lookup_info))
    (set! prev_dbase_lookup_info OGIdbase.lookup_info))
  (if (not (boundp 'prev_dbase_unitsel_name))
    (set! prev_dbase_unitsel_name OGIdbase.unitsel_name))
  (if (string-equal ogi_resORsinLPC 'resLPC)
    (initialize_resLPC)
    ;; else
    (initialize_sinLPC)
  )
)
   


(define (ogi_voice_reset)
  (set! OGIdbase.lookup_info prev_dbase_lookup_info)
  (set! OGIdbase.unitsel_name prev_dbase_unitsel_name)
  (set! Phrasify festival-phrasify)
  )

