;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                      Copyright (c) 1998-2011                        ;;;
;;;                        All Rights Reserved.                         ;;;
;;;                                                                     ;;;
;;; Permission is hereby granted, free of charge, to use and distribute ;;;
;;; this software and its documentation without restriction, including  ;;;
;;; without limitation the rights to use, copy, modify, merge, publish, ;;;
;;; distribute, sublicense, and/or sell copies of this work, and to     ;;;
;;; permit persons to whom this work is furnished to do so, subject to  ;;;
;;; the following conditions:                                           ;;;
;;;  1. The code must retain the above copyright notice, this list of   ;;;
;;;     conditions and the following disclaimer.                        ;;;
;;;  2. Any modifications must be clearly marked as such.               ;;;
;;;  3. Original authors' names are not deleted.                        ;;;
;;;  4. The authors' names are not used to endorse or promote products  ;;;
;;;     derived from this software without specific prior written       ;;;
;;;     permission.                                                     ;;;
;;;                                                                     ;;;
;;; CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK        ;;;
;;; DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING     ;;;
;;; ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT  ;;;
;;; SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE     ;;;
;;; FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   ;;;
;;; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN  ;;;
;;; AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,         ;;;
;;; ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF      ;;;
;;; THIS SOFTWARE.                                                      ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                      ;;
;;;  A generic voice definition file for a clustergen synthesizer        ;;
;;;  Customized for: cmu_us_slt                                       ;;
;;;                                                                      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Try to find the directory where the voice is, this may be from
;;; .../festival/lib/voices/ or from the current directory
(if (assoc 'cmu_us_slt_cg voice-locations)
    (defvar cmu_us_slt::dir 
      (cdr (assoc 'cmu_us_slt_cg voice-locations)))
    (defvar cmu_us_slt::dir (string-append (pwd) "/")))

;;; Did we succeed in finding it
(if (not (probe_file (path-append cmu_us_slt::dir "festvox/")))
    (begin
     (format stderr "cmu_us_slt::clustergen: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append  cmu_us_slt::dir "festvox/"))
     (format stderr "   Either the voice isn't linked in Festival library\n")
     (format stderr "   or you are starting festival in the wrong directory\n")
     (error)))

;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append cmu_us_slt::dir "festvox/") 
		      load-path))

(require 'clustergen)  ;; runtime scheme support

;;; Voice specific parameter are defined in each of the following
;;; files
(require 'cmu_us_slt_phoneset)
(require 'cmu_us_slt_tokenizer)
(require 'cmu_us_slt_tagger)
(require 'cmu_us_slt_lexicon)
(require 'cmu_us_slt_phrasing)
(require 'cmu_us_slt_intonation)
(require 'cmu_us_slt_durdata_cg) 
(require 'cmu_us_slt_f0model)
(require 'cmu_us_slt_other)

(require 'cmu_us_slt_statenames)
;; ... and others as required

;;;
;;;  Code specific to the clustergen waveform synthesis method
;;;

;(set! cluster_synth_method 
;  (if (boundp 'mlsa_resynthesis)
;      cg_wave_synth
;      cg_wave_synth_external ))

;;; Flag to save multiple loading of db
(defvar cmu_us_slt::cg_loaded nil)
;;; When set to non-nil clunits voices *always* use their closest voice
;;; this is used when generating the prompts
(defvar cmu_us_slt::clunits_prompting_stage nil)

;;; You may wish to change this (only used in building the voice)
(set! cmu_us_slt::closest_voice 'voice_kal_diphone_us)

(set! us_phone_maps
      '(
;        (M_t t)
;        (M_dH d)
;        ...
        ))

(define (voice_kal_diphone_us_phone_maps utt)
  (mapcar
   (lambda (s) 
     (let ((m (assoc_string (item.name s) us_phone_maps)))
       (if m
           (item.set_feat s "us_diphone" (cadr m))
           (item.set_feat s "us_diphone"))))
   (utt.relation.items utt 'Segment))
  utt)

(define (voice_kal_diphone_us)
  (voice_kal_diphone)
  (set! UniSyn_module_hooks (list voice_kal_diphone_us_phone_maps ))

  'kal_diphone_us
)

;;;  These are the parameters which are needed at run time
;;;  build time parameters are added to this list from build_clunits.scm
(set! cmu_us_slt_cg::dt_params
      (list
       (list 'db_dir 
             (if (string-matches cmu_us_slt::dir ".*/")
                 cmu_us_slt::dir
                 (string-append cmu_us_slt::dir "/")))
       '(name cmu_us_slt)
       '(index_name cmu_us_slt)
       '(trees_dir "festival/trees/")
       '(clunit_name_feat lisp_cmu_us_slt::cg_name)
))

;; So as to fit nicely with existing clunit voices we check need to 
;; prepend these params if we already have some set.
(if (boundp 'cmu_us_slt::dt_params)
    (set! cmu_us_slt::dt_params
          (append 
           cmu_us_slt_cg::dt_params
           cmu_us_slt::dt_params))
    (set! cmu_us_slt::dt_params cmu_us_slt_cg::dt_params))

(define (cmu_us_slt::nextvoicing i)
  (let ((nname (item.feat i "n.name")))
    (cond
;     ((string-equal nname "pau")
;      "PAU")
     ((string-equal "+" (item.feat i "n.ph_vc"))
      "V")
     ((string-equal (item.feat i "n.ph_cvox") "+")
      "CVox")
     (t
      "UV"))))

(define (cmu_us_slt::cg_name i)
  (let ((x nil))
  (if (assoc 'cg::trajectory clustergen_mcep_trees)
      (set! x i)
      (set! x (item.relation.parent i 'mcep_link)))

  (let ((ph_clunit_name 
         (cmu_us_slt::clunit_name_real
          (item.relation
           (item.relation.parent x 'segstate)
           'Segment))))
    (cond
     ((string-equal ph_clunit_name "ignore")
      "ignore")
     (t
      (item.name i)))))
)

(define (cmu_us_slt::clunit_name_real i)
  "(cmu_us_slt::clunit_name i)
Defines the unit name for unit selection for us.  The can be modified
changes the basic classification of unit for the clustering.  By default
this we just use the phone name, but you may want to make this, phone
plus previous phone (or something else)."
  (let ((name (item.name i)))
    (cond
     ((and (not cmu_us_slt::cg_loaded)
	   (or (string-equal "h#" name) 
	       (string-equal "1" (item.feat i "ignore"))
	       (and (string-equal "pau" name)
		    (or (string-equal "pau" (item.feat i "p.name"))
			(string-equal "h#" (item.feat i "p.name")))
		    (string-equal "pau" (item.feat i "n.name")))))
      "ignore")
     ;; Comment out this if you want a more interesting unit name
     ((null nil)
      name)

     ;; Comment out the above if you want to use these rules
     ((string-equal "+" (item.feat i "ph_vc"))
      (string-append
       name
       "_"
       (item.feat i "R:SylStructure.parent.stress")
       "_"
       (cmu_us_slt::nextvoicing i)))
     ((string-equal name "pau")
      (string-append
       name
       "_"
       (cmu_us_slt::nextvoicing i)))
     (t
      (string-append
       name
       "_"
;       (item.feat i "seg_onsetcoda")
;       "_"
       (cmu_us_slt::nextvoicing i))))))

(define (cmu_us_slt::rfs_load_models)
  (let ((c 1))
    (set! cmu_us_slt:rfs_models nil)
    (if (probe_file (format nil "%s/rf_models/mlist" cmu_us_slt::dir))
        (set! cmu_us_slt:rfs_models
              (mapcar
               (lambda (c)
                 (list
                  (load (format nil "%s/rf_models/trees_%02d/cmu_us_slt_mcep.tree" cmu_us_slt::dir c) t)
                  (track.load (format nil "%s/rf_models/trees_%02d/cmu_us_slt_mcep.params" cmu_us_slt::dir c))))
               (load (format nil "%s/rf_models/mlist" cmu_us_slt::dir) t)))
        ;; no mlist file so just load all of them
        (while (<= c cg:rfs)
               (set! cmu_us_slt:rfs_models
                     (cons
                      (list
                       (load (format nil "%s/rf_models/trees_%02d/cmu_us_slt_mcep.tree" cmu_us_slt::dir c) t)
                       (track.load (format nil "%s/rf_models/trees_%02d/cmu_us_slt_mcep.params" cmu_us_slt::dir c)))
                      cmu_us_slt:rfs_models))
               (set! c (+ 1 c))))
    cmu_us_slt:rfs_models))

(define (cmu_us_slt::rfs_load_dur_models)
  (let ((c 1) (dur_tree))
    (set! cmu_us_slt:rfs_dur_models nil)
    (if (probe_file (format nil "%s/dur_rf_models/mlist" cmu_us_slt::dir))
        (set! cmu_us_slt:rfs_dur_models
         (mapcar
          (lambda (c)
            (load (format nil "%s/dur_rf_models/dur_%02d/cmu_us_slt_durdata_cg.scm" cmu_us_slt::dir c))
            cmu_us_slt::zdur_tree)
          (load (format nil "%s/dur_rf_models/mlist" cmu_us_slt::dir) t)))
        ;; no mlist file so just load all of them
        ;; Probably not viable for multiple voices at once
        (while (<= c cg:rfs_dur)
               (load (format nil "%s/dur_rf_models/dur_%02d/cmu_us_slt_durdata_cg.scm" cmu_us_slt::dir c))
               (set! cmu_us_slt:rfs_dur_models
                     (cons
                      cmu_us_slt::zdur_tree
                      cmu_us_slt:rfs_dur_models))
               (set! c (+ 1 c))))
    cmu_us_slt:rfs_dur_models))

(define (cmu_us_slt::cg_dump_model_filenames ofile)
  "(cg_dump_model_files ofile)
Dump the names of the files that must be included in the distribution."
  (let ((ofd (fopen ofile "w")))
    (format ofd "festival/lib/voices/us/cmu_us_slt_cg/festival/trees/cmu_us_slt_f0.tree\n")
    (if cg:rfs
        (begin
          (mapcar
           (lambda (mn)
             (format ofd "festival/lib/voices/us/cmu_us_slt_cg/rf_models/trees_%02d/cmu_us_slt_mcep.tree\n" mn)
             (format ofd "festival/lib/voices/us/cmu_us_slt_cg/rf_models/trees_%02d/cmu_us_slt_mcep.params\n" mn))
           (load "rf_models/mlist" t))
          (format ofd "festival/lib/voices/us/cmu_us_slt_cg/rf_models/mlist\n")
          ))
    ;; Always include these too
    (format ofd "festival/lib/voices/us/cmu_us_slt_cg/festival/trees/cmu_us_slt_mcep.tree\n")
    (format ofd "festival/lib/voices/us/cmu_us_slt_cg/festival/trees/cmu_us_slt_mcep.params\n")

    (if cg:rfs_dur
        (begin
          (mapcar
           (lambda (mn)
             (format ofd "festival/lib/voices/us/cmu_us_slt_cg/dur_rf_models/dur_%02d/cmu_us_slt_durdata_cg.scm\n" mn))
           (load "dur_rf_models/mlist" t))
          (format ofd "festival/lib/voices/us/cmu_us_slt_cg/dur_rf_models/mlist\n")
          )
        (begin
          ;; basic dur build
          ;; will get the duration tree from festvox/
          t
          ))
    (fclose ofd))
)

(define (cmu_us_slt::cg_load)
  "(cmu_us_slt::cg_load)
Function that actual loads in the databases and selection trees.
SHould only be called once per session."
  (set! dt_params cmu_us_slt::dt_params)
  (set! clustergen_params cmu_us_slt::dt_params)
  (if cg:multimodel
      (begin
        ;; Multimodel: separately trained statics and deltas
        (set! cmu_us_slt::static_param_vectors
              (track.load
               (string-append 
                cmu_us_slt::dir "/"
                (get_param 'trees_dir dt_params "trees/")
                (get_param 'index_name dt_params "all")
                "_mcep_static.params")))
        (set! cmu_us_slt::clustergen_static_mcep_trees
              (load (string-append 
                     cmu_us_slt::dir "/"
                     (get_param 'trees_dir dt_params "trees/")
                     (get_param 'index_name dt_params "all")
                     "_mcep_static.tree") t))
        (set! cmu_us_slt::delta_param_vectors
              (track.load
               (string-append 
                cmu_us_slt::dir "/"
                (get_param 'trees_dir dt_params "trees/")
                (get_param 'index_name dt_params "all")
                "_mcep_delta.params")))
        (set! cmu_us_slt::clustergen_delta_mcep_trees
              (load (string-append 
                     cmu_us_slt::dir "/"
                     (get_param 'trees_dir dt_params "trees/")
                     (get_param 'index_name dt_params "all")
                     "_mcep_delta.tree") t))
        (set! cmu_us_slt::str_param_vectors
              (track.load
               (string-append
                cmu_us_slt::dir "/"
                (get_param 'trees_dir dt_params "trees/")
                (get_param 'index_name dt_params "all")
                "_str.params")))
        (set! cmu_us_slt::clustergen_str_mcep_trees
              (load (string-append
                     cmu_us_slt::dir "/"
                     (get_param 'trees_dir dt_params "trees/")
                     (get_param 'index_name dt_params "all")
                     "_str.tree") t))
        (if (null (assoc 'cg::trajectory cmu_us_slt::clustergen_static_mcep_trees))
            (set! cmu_us_slt::clustergen_f0_trees
                  (load (string-append 
                          cmu_us_slt::dir "/"
                          (get_param 'trees_dir dt_params "trees/")
                          (get_param 'index_name dt_params "all")
                          "_f0.tree") t)))
        )
      (begin
        ;; Single joint model 
        (set! cmu_us_slt::param_vectors
              (track.load
               (string-append 
                cmu_us_slt::dir "/"
                (get_param 'trees_dir dt_params "trees/")
                (get_param 'index_name dt_params "all")
                "_mcep.params")))
        (set! cmu_us_slt::clustergen_mcep_trees
              (load (string-append 
                      cmu_us_slt::dir "/"
                      (get_param 'trees_dir dt_params "trees/")
                      (get_param 'index_name dt_params "all")
                      "_mcep.tree") t))
        (if (null (assoc 'cg::trajectory cmu_us_slt::clustergen_mcep_trees))
            (set! cmu_us_slt::clustergen_f0_trees
                  (load (string-append 
                         cmu_us_slt::dir "/"
                         (get_param 'trees_dir dt_params "trees/")
                         (get_param 'index_name dt_params "all")
                         "_f0.tree") t)))))

  ;; Random forests
  (if (and cg:rfs (not (boundp 'cmu_us_slt:rfs_models)) )
      (cmu_us_slt::rfs_load_models))
  (if (and cg:rfs_dur (not (boundp 'cmu_us_VOICE:rfs_dur_models)))
      (cmu_us_slt::rfs_load_dur_models))

  (set! cmu_us_slt::cg_loaded t)
)

(define (cmu_us_slt::voice_reset)
  "(cmu_us_slt::voice_reset)
Reset global variables back to previous voice."
  (cmu_us_slt::reset_phoneset)
  (cmu_us_slt::reset_tokenizer)
  (cmu_us_slt::reset_tagger)
  (cmu_us_slt::reset_lexicon)
  (cmu_us_slt::reset_phrasing)
  (cmu_us_slt::reset_intonation)
  (cmu_us_slt::reset_f0model)
  (cmu_us_slt::reset_other)

  t
)

;; This function is called to setup a voice.  It will typically
;; simply call functions that are defined in other files in this directory
;; Sometime these simply set up standard Festival modules othertimes
;; these will be specific to this voice.
;; Feel free to add to this list if your language requires it

(define (voice_cmu_us_slt_cg)
  "(voice_cmu_us_slt_cg)
Define voice for us."
  ;; *always* required
  (voice_reset)

  ;; We are going to force a load of the local clustergen.scm file 
  ;; If we were more careful we could do this properly with parameters
  ;; but I doubt we'd get it right.
  (load (path-append cmu_us_slt::dir "festvox/clustergen.scm"))

  ;; Select appropriate phone set
  (cmu_us_slt::select_phoneset)

  ;; Select appropriate tokenization
  (cmu_us_slt::select_tokenizer)

  ;; For part of speech tagging
  (cmu_us_slt::select_tagger)

  (cmu_us_slt::select_lexicon)

  (cmu_us_slt::select_phrasing)

  (cmu_us_slt::select_intonation)

  ;; For CG voice there is no duration modeling at the seg level
  (Parameter.set 'Duration_Method 'Default)
  (set! duration_cart_tree_cg cmu_us_slt::zdur_tree)
  (set! duration_ph_info_cg cmu_us_slt::phone_durs)
  (Parameter.set 'Duration_Stretch 1.0)

  (cmu_us_slt::select_f0model)

  ;; Waveform synthesis model: cluster_gen
  (set! phone_to_states cmu_us_slt::phone_to_states)
  (if (not cmu_us_slt::clunits_prompting_stage)
      (begin
	(if (not cmu_us_slt::cg_loaded)
	    (cmu_us_slt::cg_load))
        (if cg:multimodel
            (begin
              (set! clustergen_param_vectors cmu_us_slt::static_param_vectors)
              (set! clustergen_mcep_trees cmu_us_slt::clustergen_static_mcep_trees)
              (set! clustergen_delta_param_vectors cmu_us_slt::delta_param_vectors)
              (set! clustergen_delta_mcep_trees cmu_us_slt::clustergen_delta_mcep_trees)
              (set! clustergen_str_param_vectors cmu_us_slt::str_param_vectors)
              (set! clustergen_str_mcep_trees cmu_us_slt::clustergen_str_mcep_trees)

              )
            (begin
              (set! clustergen_param_vectors cmu_us_slt::param_vectors)
              (set! clustergen_mcep_trees cmu_us_slt::clustergen_mcep_trees)
              ))
        (if (boundp 'cmu_us_slt::clustergen_f0_trees)
            (set! clustergen_f0_trees cmu_us_slt::clustergen_f0_trees))

        (if cg:mixed_excitation
            (set! me_filter_track 
                  (track.load 
                   (string-append cmu_us_slt::dir "/"
                                  "festvox/mef.track"))))
        (if cg:mlsa_lpf
            (set! lpf_track 
                  (track.load 
                   (string-append cmu_us_slt::dir "/"
                                  "festvox/lpf.track"))))
        (if (and cg:rfs (boundp 'cmu_us_slt:rfs_models))
            (set! cg:rfs_models cmu_us_slt:rfs_models))
        (if (and cg:rfs_dur (boundp 'cmu_us_slt:rfs_dur_models))
            (set! cg:rfs_dur_models cmu_us_slt:rfs_dur_models))

	(Parameter.set 'Synth_Method 'ClusterGen)
      ))

  ;; This is where you can modify power (and sampling rate) if desired
  (set! after_synth_hooks nil)
;  (set! after_synth_hooks
;      (list
;        (lambda (utt)
;          (utt.wave.rescale utt 2.1))))

  (set! current_voice_reset cmu_us_slt::voice_reset)

  (set! current-voice 'cmu_us_slt_cg)
)

(define (is_pau i)
  (if (phone_is_silence (item.name i))
      "1"
      "0"))

(provide 'cmu_us_slt_cg)

