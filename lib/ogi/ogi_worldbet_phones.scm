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

;;; Features definitions: name followed by possible values

(defPhoneSet

  worldbet

   ;;;  Phone Features

  (;; vowel or consonant

   (vc + -)  

   ;; vowel length: short long diphthong schwa

   (vlng s l d a 0)

   ;; vowel height: high mid low

   (vheight 1 2 3 0)

   ;; vowel frontness: front mid back

   (vfront 1 2 3 0)

   ;; lip rounding

   (vrnd + - 0)

   ;; consonant type: stop fricative affricate nasal lateral approximant

   (ctype s f a n l r 0)

   ;; place of articulation: labial alveolar palatal labio-dental

   ;;                         dental velar glottal

   (cplace l a p b d v g 0)

   ;; consonant voicing

   (cvox + - 0)

   )

  ;; Phone set members

  (

   (i:  +   l   1   1   -   0   0   0)  ;; beet

   (I   +   s   1   1   -   0   0   0)  ;; bit

   (E   +   s   2   1   -   0   0   0)  ;; bet

   (@   +   s   3   1   -   0   0   0)  ;; bat

   (u   +   l   1   3   +   0   0   0)  ;; boot  

   (U   +   s   1   3   +   0   0   0)  ;; book

   (^   +   s   2   2   -   0   0   0)  ;; but

   (&   +   a   2   2   -   0   0   0)  ;; About

   (uh  +   a   2   2   -   0   0   0)  ;; same as & but for sable purposes
   (>   +   l   3   3   +   0   0   0)  ;; lawn 

   (A   +   l   3   3   -   0   0   0)  ;; father

   (3r  +   l   2   2   -   0   0   0)  ;; bird  ? +   l   2   4   +   0   0   0) 

   (ei  +   d   2   1   -   0   0   0)  ;; bait

   (aI  +   d   3   2   -   0   0   0)  ;; bite

   (>i  +   d   2   3   -   0   0   0)  ;; boy   ? +   d   2   3   -   0   0   0)  

   (iU  +   d   1   1   +   0   0   0)  ;; view

   (aU  +   d   3   2   +   0   0   0)  ;; how   ? +   d   3   2   +   0   0   0)  

   (oU  +   d   2   3   +   0   0   0)  ;; lone

   (p   -   0   0   0   0   s   l   -)

   (ph  -   0   0   0   0   s   l   -)
   (t   -   0   0   0   0   s   a   -)
   (th  -   0   0   0   0   s   a   -) 

   (dx   -   0   0   0   0   s   a   +) ;;; flap

   (k   -   0   0   0   0   s   v   -)
   (kh  -   0   0   0   0   s   v   -) 

   (b   -   0   0   0   0   s   l   +) 

   (d   -   0   0   0   0   s   a   +)

   (g   -   0   0   0   0   s   v   +)  

   (m   -   0   0   0   0   n   l   +)

   (n   -   0   0   0   0   n   a   +)

   (N   -   0   0   0   0   n   v   +)

   (f   -   0   0   0   0   f   b   -)

   (T   -   0   0   0   0   f   d   -)

   (s   -   0   0   0   0   f   a   -)

   (S   -   0   0   0   0   f   p   -) 

   (h   -   0   0   0   0   f   g   -)

   (v   -   0   0   0   0   f   b   +)

   (D   -   0   0   0   0   f   d   +)

   (z   -   0   0   0   0   f   a   +)

   (Z   -   0   0   0   0   f   p   +)

   (tS  -   0   0   0   0   a   p   -)  

   (dZ  -   0   0   0   0   a   p   +)

   (l   -   0   0   0   0   l   a   +)

   (l=  +   s   0   0   0   l   a   +)

   (9r  -   0   0   0   0   r   a   +)

   (j   -   0   0   0   0   r   p   +) 

   (w   -   0   0   0   0   r   l   +)

   (pau -   0   0   0   0   0   0   0)

   (? -     0   0   0   0   0   0   +)

   (gs -     0   0   0   0   0   0   +)

  )

)   





(PhoneSet.silences '(pau))



(provide 'ogi_worldbet_phones)





  



