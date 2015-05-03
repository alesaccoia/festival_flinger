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

;; ogi_effect.scm

;;   call (OGIeffect.init PARAMS) -- PARAMS can be effect_small_room, effect_large_room, 
;;                                   effect_hall, effect_echo, ....
;;                                   or any of your own design


(set! effect_small_room
      (list
      '(pre_gain 0.7)   
      '(post_gain 1.3)   
      '(reverb 
        ((111 0.04 0.3)
	 (140 0.02 0.3)
	 (170 0.02 0.3)
	 (240 0.04 0.3)
	 (264 0.02 0.3)
	 (270 0.02 0.3)
	 (340 0.04 0.3)
	 (353 0.04 0.3)
	 (364 0.04 0.3)
	 (500 0.03 0.3)
	 (620 0.03 0.3)
	 (740 0.03 0.3)
	 (870 0.03 0.3)
	 (900 0.03 0.3)
	 (1020 0.04 0.3)
	 (1040 0.03 0.3)
	 (1070 0.02 0.3)
	 (1400 0.01 0.3)
	 (2400 0.03 0.3)
	 (2530 0.04 0.3)
	 (3400 0.03 0.3)
	 (3530 0.02 0.3)
	 (4400 0.03 0.3)
	 (4530 0.02 0.3)
	 (4700 0.01 0.3)))
      '(echo
	((dtime 800)
	 (feedback 0.05)))
      '(effect_order (echo reverb))))



(set! effect_large_room
      (list
      '(pre_gain 0.6)   
      '(post_gain 1.3)   
      '(reverb 
        ((111 0.06 0.2)
	 (140 0.02 0.2)
	 (170 0.02 0.2)
	 (240 0.06 0.2)
	 (264 0.02 0.2)
	 (270 0.02 0.2)
	 (340 0.06 0.2)
	 (353 0.06 0.2)
	 (364 0.06 0.2)
	 (500 0.03 0.2)
	 (620 0.03 0.2)
	 (740 0.03 0.2)
	 (870 0.03 0.2)
	 (900 0.03 0.2)
	 (1520 0.06 0.2)
	 (2060 0.03 0.2)
	 (2070 0.02 0.2)
	 (1400 0.01 0.2)
	 (2400 0.03 0.2)
	 (2530 0.06 0.2)
	 (3400 0.03 0.2)
	 (4530 0.02 0.2)
	 (4400 0.03 0.2)
	 (5530 0.02 0.2)
	 (5400 0.03 0.2)
	 (6530 0.02 0.2)
	 (6400 0.03 0.2)
	 (7530 0.02 0.2)
	 (7700 0.01 0.2)))
      '(echo
	((dtime 2600)
	 (feedback 0.1)))
      '(effect_order (reverb echo))))



(set! effect_hall
      (list
      '(pre_gain 0.7)   
      '(post_gain 1.3)   
      '(reverb 
        ((111 0.01 0.3)
	 (140 0.01 0.3)
	 (170 0.01 0.3)
	 (240 0.01 0.3)
	 (264 0.02 0.3)
	 (270 0.02 0.3)
	 (340 0.01 0.3)
	 (353 0.01 0.3)
	 (364 0.01 0.3)
	 (1000 0.03 0.3)
	 (1020 0.03 0.3)
	 (1040 0.03 0.3)
	 (2070 0.03 0.3)
	 (2400 0.01 0.3)
	 (3400 0.03 0.3)
	 (3530 0.02 0.3)
	 (4400 0.03 0.3)
	 (4530 0.02 0.3)
	 (5400 0.03 0.3)
	 (6530 0.02 0.3)
	 (6700 0.01 0.3)))
      '(echo
	((dtime 3000)
	 (feedback 0.1)))
      '(slapback  
        ((dtime 10)
	 (feedback 0.0)))
      '(effect_order (slapback echo reverb))))


(set! effect_echo
      (list
      '(pre_gain 0.9)   
      '(post_gain 1.0)   
      '(reverb 
        ((111 0.01 0.2)
	 (140 0.01 0.2)
	 (170 0.01 0.2)
	 (200 0.01 0.2)))
      '(echo
	((dtime 4000)
	 (feedback 0.2)))
      '(effect_order (reverb echo))))

(provide 'ogi_effect)
