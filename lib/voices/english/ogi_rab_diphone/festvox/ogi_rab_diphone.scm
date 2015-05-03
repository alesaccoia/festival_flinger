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
;; modified from the original distribution by OGI
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;                Centre for Speech Technology Research                  ;;
;;;                     University of Edinburgh, UK                       ;;
;;;                       Copyright (c) 1996,1997                         ;;
;;;                        All Rights Reserved.                           ;;
;;;                                                                       ;;
;;;  Permission to use, copy, modify, distribute this software and its    ;;
;;;  documentation for research, educational and individual use only, is  ;;
;;;  hereby granted without fee, subject to the following conditions:     ;;
;;;   1. The code must retain the above copyright notice, this list of    ;;
;;;      conditions and the following disclaimer.                         ;;
;;;   2. Any modifications must be clearly marked as such.                ;;
;;;   3. Original authors' names are not deleted.                         ;;
;;;  This software may not be used for commercial purposes without        ;;
;;;  specific prior written permission from the authors.                  ;;
;;;                                                                       ;;
;;;  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK        ;;
;;;  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      ;;
;;;  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   ;;
;;;  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE     ;;
;;;  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    ;;
;;;  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   ;;
;;;  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          ;;
;;;  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       ;;
;;;  THIS SOFTWARE.                                                       ;;
;;;                                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  Set up ogi_rab_diphones using the OGIresLPC diphone synthesizer
;;;
;;;  Roger diphones: male RP English collected October 1996
;;;

(defvar ogi_rab_diphone_dir (cdr (assoc 'ogi_rab_diphone voice-locations))
  "ogi_rab_diphone_dir
  The default directory for the ogirab diphone database.")

(set! load-path (cons (path-append libdir "ogi") load-path))

(require 'mrpa_phones)
(require 'pos)
(require 'phrase)
(require 'tobi)
(require 'f2bf0lr)
(require 'mrpa_durs)
(require 'gswdurtreeZ)
(require 'ogi_unitsel)
(require 'ogi_synthesis)

(setup_oald_lex)

(set! ogi_grouped_or_ungrouped 'grouped)

(define (voice_ogi_rab_diphone)
"(voice_ogi_rab_diphone)
 Set up the current voice to be a British male RP (Roger) speaker using
 the ogirab diphone set."
  (voice_reset)
  (Parameter.set 'Language 'britishenglish)
  ;; Phone set
  (Parameter.set 'PhoneSet 'mrpa)
  (PhoneSet.select 'mrpa)
  ;; Tokenization rules
  (set! token_to_words english_token_to_words)
  ;; POS tagger
  (set! pos_lex_name "english_poslex")
  (set! pos_ngram_name 'english_pos_ngram)
  (set! pos_supported t)
  (set! guess_pos english_guess_pos)   ;; need this for accents
  ;; Lexicon selection
  (lex.select "oald")
  (set! postlex_rules_hooks (list postlex_apos_s_check ogirab_postlex_syllabics)) 

  ;; Phrase prediction
  (Parameter.set 'Phrase_Method 'prob_models)
  (set! phr_break_params english_phr_break_params)
  ;; Accent and tone prediction
  (set! int_tone_cart_tree f2b_int_tone_cart_tree)
  (set! int_accent_cart_tree f2b_int_accent_cart_tree)
  ;; F0 prediction
  (set! f0_lr_start f2b_f0_lr_start)
  (set! f0_lr_mid f2b_f0_lr_mid)
  (set! f0_lr_end f2b_f0_lr_end)
  (Parameter.set 'Int_Method Intonation_Tree)
  (set! int_lr_params
	'((target_f0_mean 110) (target_f0_std 14)
	  (model_f0_mean 170) (model_f0_std 34)))
  (Parameter.set 'Int_Target_Method Int_Targets_LR)
  ;; Duration prediction -- use gsw durations
  (set! duration_cart_tree gsw_duration_cart_tree)
  (set! duration_ph_info gsw_durs)
  (Parameter.set 'Duration_Method Duration_Tree_ZScores)
  (Parameter.set 'Duration_Stretch 1.00)

  ;; diphone unit selection fallbacks
  (set! ogi_di_alt_L '((e@ (ei)) (@ (i)) (ll (l)) (l (ll)) (ii (y)) (m= (m)) (n= (n)) (l= (l)) (o (oo)) (a (aa))))
  (set! ogi_di_alt_R '((ll (l)) (l (ll)) (y (i)) (ch (t)) (jh (d)) (i (y)) (d (t)) (m= (m)) (n= (n)) (l= (l))))
  (set! ogi_di_default "#-h")

  (Parameter.set 'Synth_Method 'OGIdiphone)
  (OGIdbase.activate "ogi_rab_diphone")      
  (OGIresLPC.init ogirabdi_OGI_syn_params)  
  (set! current-voice 'ogi_rab_diphone)
)


(define (ogirab_postlex_syllabics utt)
"(ogirab_postlex_syllabics utt)
Because the lexicon is somewhat random in its used of syllable l n and
m this is designed to post process the output inserting schwa before
them.  Ideally the lexicon should be fixed."
  (mapcar
   (lambda (s)
     (if (and (member_string (item.name s) '("l" "n" "m"))
	      (string-equal "coda" (item.feat s "seg_onsetcoda"))
	      (not (member_string (item.feat s "p.name") '(l r)))
	      (string-equal "-" (item.feat s "p.ph_vc")))
	 (item.relation.insert 
	  s 'SylStructure
	  (item.insert s (list "@") 'before)
	  'before)))
   (utt.relation.items utt 'Segment)))


;; Context-dependent smoothing:
;;   (feat val int) for each
;;   Can put any feat,val pair here, as long as feat is defined in phoneset definition.
;;   Smoother setup algorithm iterates through list until feat, val is matched, then uses int as 
;;     smoothing window length.  This could be improved.
(set! ogirabdi_smooth_spectra   ;;; max number of frames to use in smoothing LPC at joins
      '(spectra_smooth
        ((vc + 5)   ; vowel
         (ctype s 0) ; stop
         (ctype f 2) ; fricative
         (ctype a 0) ; affricate
         (ctype n 5) ; nasal
         (ctype l 5) ; lateral
         (ctype r 5) ; approximant
         )))

(set! ogirabdi_smooth_power   ;;; max number of frames to use in smoothing power at joins
      '(power_smooth
        ((vc + 5)   ; vowel
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
(set! ogirabdi_OGI_syn_params  
 (list
  '(F0_default 50.0)      ;; default Fo used if can't find any Fo targets
  '(T0_UV_thresh 0.020)   ;; used to make V/UV decision 
  '(T0_UV_pm 0.010)       ;; used to place UV pmarks
  '(post_gain 1.0)        ;; adjust final loudness
  '(deemphasis 0.94)      ;; opposite of preemphasis (lowpass filter)
  '(mod_method "direct")  ;; method for realizing prosodic targets
  '(data_type "resLPC")
  '(access_mode "ondemand")
  '(beta_smooth 5)        ;; smoother len for pitch mod factor in "soft" mod method
  '(window_type "trapezoid") 
  '(smooth_cross_ph_join "Y") ;; smooth across joins at phone boundaries?
  '(spectra_match_or_replace "match") ;; for join smoothing
  '(power_match_or_replace   "match") ;; for join smoothing
  ogirabdi_smooth_spectra
  ogirabdi_smooth_power
;  mwmdi_dump
  ))


(define (init_ogi_rab_diphone)
"(init_ogi_rab_diphone)
  Initialise the OGIRAB diphone database.  This sets up the 16K version
  for residual excited LPC."
(set! voicename 'ogi_rab_diphone)
(if (equal? ogi_grouped_or_ungrouped 'ungrouped)

    ;;  ungrouped
    (OGIdbase.init
     (list
      '(dbname ogi_rab_diphone)
      (list 'unitdic_file (path-append ogi_rab_diphone_dir "festvox" "rabdiph.msec"))
      (list 'gain_file (path-append ogi_rab_diphone_dir "festvox" "rabgain.dat"))
      '(phoneset "worldbet")  
      '(base_dir (path-append ogi_rab_aiphone_dir "ungrouped"))
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
      '(dbname ogi_rab_diphone)
      (list 'groupfile (path-append ogi_rab_diphone_dir "group" (string-append voicename "_resLPC.group")))
    '(data_type "resLPC")  
    '(access_mode "ondemand")
      ))
    t)
)

(init_ogi_rab_diphone)


(proclaim_voice
 'ogi_rab_diphone
 '((language english)
   (gender male)
   (dialect british)
   (description
    "This voice provides a British RP English male voice collected at CSTR using a
     residual excited LPC diphone synthesis method developed at OGI.  It uses a 
     modified Oxford Advanced Learners' Dictionary for pronunciations.
     Prosodic phrasing is provided by a statistically trained model
     using part of speech and local distribution of breaks.  Intonation
     is provided by a CART tree predicting ToBI accents and an F0 
     contour generated from a model trained from natural speech.  The
     duration model is also trained from data using a CART tree.")
    (samplerate 16000)))

(provide 'ogi_rab_diphone)
