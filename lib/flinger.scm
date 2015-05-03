;;;;;;;;;;;;;;;;;;;;;;;;;;;;<--CSLU-->;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                        ;
;             Center for Spoken Language Understanding                   ;
;               Division of Biomedical Computer Science                  ;
;               Oregon Health and Science University                     ;
;                         Portland, OR USA                               ;
;                        Copyright (c) 1999 - 2009                       ;
;                                                                        ;
;      This module is not part of the CSTR/University of Edinburgh       ;
;               release of the Festival TTS system.                      ;
;                                                                        ;
;  In addition to any conditions disclaimers below, please see the file  ;
;  "license_ohsu_tts.txt" distributed with this software for information ;
;  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  ;
;                                                                        ;
;      Authors: Mike Macon, Esther Klabbers, Alexander Kain              ;
;                          and David Brown                               ;
;                                                                        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;<--CSLU-->;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(require 'ogi/ogi_kddurtreeZ_wb)

;;;  Once the utterance is built these functions synth and play it
(defvar sing_hooks (list utt.synth utt.play)
  "sing_hooks
Function or list of functions to be called during synthesis.
The function Flinger.sing (src in C++) applies this hook to the utterance.  
This typically contains the utt.synth function and utt.play.")


;;;  how to strip punctutation, expand numbers, etc..
(defvar lyrictoken_to_lyric_func builtin_lyrictoken_to_lyric)

(set! fl_phoneme_durations kd_durs_wb)

;;
;; define a Festival utterance type
;;
(defUttType Song
  (FL_Tokenize utt)
  (FL_Lyricize utt)
  (PostLex utt)
  (FL_NoteDur utt)
  (FL_NoteF0 utt)
  (Wave_Synth utt)
  (OGIeffect utt)
  )


(Flinger.init
 (list
  '(vibrato_freq 0.0)       ;; 5
  '(vibrato_max 0.00)      ;; 0.050
  '(maxbend_semi 1.0)       ;; 
  '(portamento_time_default 0.050)
  '(drift_freq1 4.7)        ;; 
  '(drift_freq2 7.1)        ;; 
  '(drift_freq3 12.7)       ;; 
  '(drift_ampl 0.003)       ;; 
  '(transpose 0)            ;; 
  '(consonant_stretch 1.0)  ;; 
  '(min_rest_dur 0.060)     ;; 
  '(phone_delimiter "|")    ;; 
  '(dump_info 0)            ;;
  ))

;;---------------------------------------------------------------


;;;;Added by DJLB 20-08-2006. Three new definitions to simplify use.

(define (Sing FILENAME)
"(Sing FILENAME)
Sing given MIDI file"
(set! sing_hooks (list utt.synth utt.play))
(Flinger.sing FILENAME nil))

(define (RecordAndSing INPUTFILE OUTPUTFILE)
"(RecordAndSing INPUTFILE OUTPUTFILE). 
Record given MIDI input file as given output WAVE file."
(set! sing_hooks (list utt.synth utt.play))
(utt.save.wave (Flinger.sing INPUTFILE nil) OUTPUTFILE 'riff))

(define (RecordSong INPUTFILE OUTPUTFILE)
"(RecordSong INPUTFILE OUTPUTFILE). 
Record given MIDI input file as given output WAVE file, and sing it."
(set! sing_hooks (list utt.synth))
(utt.save.wave (Flinger.sing INPUTFILE nil) OUTPUTFILE 'riff))


;;**************************************************************** 


