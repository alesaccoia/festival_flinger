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


(lts.ruleset
 lts_worldbet
;;;  Sets
(
 (V a e i o u)
 (C b c d f g h j k l m n p q r s t v w x y z)
 (Cv b d v g j l m n r w z)   ;; voiced consonants
 (VF e i y )                  ;; a front vowel
 (TSRDLZNJ t s r d l z n j )
 (TCS t c s)
 (SCGZXJ s c g z x j)
 (RSD r s d)
 (SD s d)
 (OT o t)
 (SY s y)
)
;; The rule set
(
 ;; A rules
 (   [ a ]   # = & )               ;; {Anything, "A", Nothing, "AX"},
;; (   [ a h ] # = 3r )
 ( # [ a r ] o = & 9r )	           ;; {Nothing, "AR", "O", "AXr"},
;; (   [ a r ] e d # = E 9r ) 
 (   [ a r ] V = E 9r )            ;; {Anything, "AR", "#", "EHr"},
 ( C [ a s ] V = ei s )            ;; {"^", "AS", "#", "EYs"},
 (   [ a ]   w a = & )             ;; {Anything, "A", "WA", "AX"},
 (   [ a w ] = > )                 ;; {Anything, "AW", Anything, "AO"},
 ( # C * [ a n y ] = E n i: )      ;; {" :", "ANY", Anything, "EHnIY"},
 (   [ a ] C VF V + = ei )         ;; {Anything, "A", "^+#", "EY"},
 ( V + C * [ a l l y ] = & l i: )  ;; {"#:", "ALLY", Anything, "AXlIY"},
 ( # [ a l ] V + = & l )           ;; {Nothing, "AL", "#", "AXl"},
 (  [ a g a i n ] = & g E n )      ;; {Anything, "AGAIN", Anything, "AXgEHn"},
 ( V + C * [ a g ] e = I dZ )      ;; {"#:", "AG", "E", "IHj"},
 ( [ a ] C VF C * V + =  @ )       ;; {Anything, "A", "^+:#", "AE"},
;; ( h [ a ] C VF = @ )
 ( # C * [ a ] C VF # = ei )       ;; {" :", "A", "^+ ", "EY"},
 (  [ a ] C e # = ei )             ;; {Anything, "A", "^%", "EY"},
 (  [ a ] C e RSD # = ei )         ;; {Anything, "A", "^%", "EY"},
;;  (  [ a ] C e r SY # = ei )     ;; {Anything, "A", "^%", "EY"},
 (  [ a ] C e l y # = ei )         ;; {Anything, "A", "^%", "EY"},
 (  [ a ] C i n g # = ei )         ;; {Anything, "A", "^%", "EY"},
 ( # [ a r r ] = & 9r )            ;; {Nothing, "ARR", Anything, "AXr"},
 (   [ a r r ] = @ 9r )            ;; {Anything, "ARR", Anything, "AEr"},
 ( # C * [ a r ] = A 9r )          ;; {" :", "AR", Nothing, "AAr"},
 ( [ a r ] # = 3r )                ;; {Anything, "AR", Nothing, "ER"},
 ( [ a r ] = A 9r )                ;; {Anything, "AR", Nothing, "ER"},
;; ( [ a r ] V = A 9r )            ;; {Anything, "AR", Anything, "AAr"},
 ( [ a i r ] = E 9r )              ;; {Anything, "AIR", Anything, "EHr"},
 ( [ a i ] = ei )                  ;; {Anything, "AI", Anything, "EY"},
 ( [ a y ] = ei )                  ;; {Anything, "AY", Anything, "EY"},
 ( [ a u ] = > )                   ;; {Anything, "AU", Anything, "AO"},
 ( V + C * [ a l ] # = & l )       ;; {"#:", "AL", Nothing, "AXl"},
 ( V + C * [ a l s ] # = & l z )   ;; {"#:", "ALS", Nothing, "AXlz"},
 ( [ a l k ] = > k )               ;; {Anything, "ALK", Anything, "AOk"},
 ( [ a l ] C = > l )               ;; {Anything, "AL", "^", "AOl"},
 ( # C * [ a b l e ] = ei b & l )  ;; {" :", "ABLE", Anything, "EYbAXl"},
 ( [ a b l e ] = & b & l )         ;; {Anything, "ABLE", Anything, "AXbAXl"},
 ( [ a n g ] VF = ei n dZ )        ;; {Anything, "ANG", "+", "EYnj"},
 ( [ a ] = @ )                     ;; {Anything, "A", Anything, "AE"},

;; B rules
 ( # [ b e ] C V + = b I )         ;; {Nothing, "BE", "^#", "bIH"},
 ( [ b e i n g ] = b i: I N )      ;; {Anything, "BEING", Anything, "bIYIHNG"},
 ( # [ b o t h ] # = b oU T )
 ( # [ b u s ] V + = b I z )       ;; {Nothing, "BUS", "#", "bIHz"},
 ( [ b u i l ] = b I l )           ;; {Anything, "BUIL", Anything, "bIHl"},
 ( [ b ] = b )                     ;; {Anything, "B", Anything, "b"},

;; C rules
 ( # [ c h ] C = k )               ;; {Nothing, "CH", "^", "k"},
 ( C e [ c h ] = k )               ;; {"^E", "CH", Anything, "k"},
 ( i a r [ c h ] = k )             ;; ok
 ( [ c h ] # = tS )                ;; ok
 ( a r [ c h ] i n g  = tS )          
 ( a r [ c h ] e RSD = tS )          
 ( a r [ c h ] V = k )          
 ( a r [ c h ] y # = k )          
 ( [ c h ] = tS )                  ;; {Anything, "CH", Anything, "CH"},
 ( s [ c i ] V + = s aI )          ;; {" S", "CI", "#", "sAY"},
 ( [ c i ] a = S )                 ;; {Anything, "CI", "A", "SH"},
 ( [ c i ] o = S )                 ;; {Anything, "CI", "O", "SH"},
 ( [ c i ] e n = S )               ;; {Anything, "CI", "EN", "SH"},
 ( [ c ] VF = s )                  ;; {Anything, "C", "+", "s"},
 ( [ c k ] = k )                   ;; {Anything, "CK", Anything, "k"},
 ( [ c o m ] e # = k ^ m )         ;; {Anything, "COM", "%", "kAHm"},
 ( [ c o m ] e RSD # = k ^ m )     ;; {Anything, "COM", "%", "kAHm"},
 ( [ c o m ] e l y # = k ^ m )     ;; {Anything, "COM", "%", "kAHm"},
 ( [ c o m ] i n g # = k ^ m )     ;; {Anything, "COM", "%", "kAHm"},
 ( [ c ] = k )                     ;; {Anything, "C", Anything, "k"},

;; D rules
 ( V + C * [ d e d ] # = d I d )   ;; {"#:", "DED", Nothing, "dIHd"},
 ( Cv e [ d ] # = d )              ;; {".E", "D", Nothing, "d"},
 ( V + C * C  e [ d ] # = t )      ;; {"#^:E", "D", Nothing, "t"},
 ( # [ d e ] C V + = d I )         ;; {Nothing, "DE", "^#", "dIH"},
 ( # [ d o ] # = d u )
 ( # [ d o e s ] = d ^ z )         ;; {Nothing, "DOES", Anything, "dAHz"},
 ( # [ d o i n g ] = d u w I N )   ;; {Nothing, "DOING", Anything, "dUWIHNG"},
 ( # [ d o w ] = d aU )            ;; {Nothing, "DOW", Anything, "dAW"},
 ( [ d u ] a = dZ u )              ;; {Anything, "DU", "A", "jUW"},
 ( [ d ] = d )                     ;; {Anything, "D", Anything, "d"},

;; E rules
 ( V + C * [ e ] # = )             ;; {"#:", "E", Nothing, Silent},
;; ( \' C + [ e ] # = )              ;; {"'^:", "E", Nothing, Silent},
 ( # C * [ e ] # = i: )            ;; {" :", "E", Nothing, "IY"},
 ( V + [ e d ] # = d )             ;; {"#", "ED", Nothing, "d"},
 ( V + C * [ e ] d # = )           ;; {"#:", "E", "D ", Silent},
 ( [ e v ] e r = E v )             ;; {Anything, "EV", "ER", "EHv"},
 ( [ e r ] e d = 3r )              ;; ok
 ( [ e r ] V + = E 9r )            ;; {Anything, "ER", "#", "EHr"},
 ( [ e r ] # = 3r )                ;; ok
 ( [ e r ] = 3r )                  ;; {Anything, "ER", Anything, "ER"},
 ( [ e ] C e # = i: )              ;; {Anything, "E", "^%", "IY"},
 ( [ e ] C e RSD # = i: )          ;; {Anything, "E", "^%", "IY"},
 ( [ e ] C e l y # = i: )          ;; {Anything, "E", "^%", "IY"},
 ( [ e ] C i n g # = i: )          ;; {Anything, "E", "^%", "IY"},
 ( [ e r i ] V + = i: 9r i: )      ;; {Anything, "ERI", "#", "IYrIY"},
 ( [ e r i ] = E 9r I )            ;; {Anything, "ERI", Anything, "EHrIH"},
 ( V + C * [ e r ] V + = 3r )      ;; {"#:", "ER", "#", "ER"},
 ( # [ e v e n ] = i: v E n )      ;; {Nothing, "EVEN", Anything, "IYvEHn"},
 ( V + C * [ e  w ] # = u )        ;; ok  
 ( V + C * [ e ] w = )             ;; {"#:", "E", "W", Silent},
 ( n [ e w ] = j u )               ;;
 ( TSRDLZNJ [ e w ] = u )          ;; {"T", "EW", Anything, "UW"},
 ( TCS h [ e w ] = u )             ;; {"TH", "EW", Anything, "UW"},
 ( [ e w ] = j u )                 ;; {Anything, "EW", Anything, "YUw"},
 ( [ e ] o = i: )                  ;; {Anything, "E", "O", "IY"},
 ( V + C * SCGZXJ [ e s ] # = I z )  ;; {"#:S", "ES", Nothing, "IHz"},
 ( V + C * c h [ e s ] # = I z )   ;; {"#:CH", "ES", Nothing, "IHz"},
 ( V + C * s h [ e s ] # = I z )   ;; {"#:SH", "ES", Nothing, "IHz"},
 ( V + C * [ e ] s # = )           ;; {"#:", "E", "S ", Silent},
 ( V + C * [ e l y ] # = l i: )    ;; {"#:", "ELY", Nothing, "lIY"},
 ( V + C * [ e m e n t ] = m E n t ) ;; {"#:", "EMENT", Anything, "mEHnt"},
 ( [ e f u l ] = f U l )           ;; {Anything, "EFUL", Anything, "fUHl"},
 ( [ e e ] = i: )                  ;; {Anything, "EE", Anything, "IY"},
 ( [ e a r n ] = 3r n )            ;; {Anything, "EARN", Anything, "ERn"},
 ( # [ e a r ] C = 3r )            ;; {Nothing, "EAR", "^", "ER"},
 ( [ e a d ] = E d )               ;; {Anything, "EAD", Anything, "EHd"},
 ( V + C * [ e a ] # = i: & )      ;; {"#:", "EA", Nothing, "IYAX"},
 ( [ e a ] s u = E )               ;; {Anything, "EA", "SU", "EH"},
;; ( [ e a r ] C = 3r )              ;; 
;; ( [ e a r ] = i: 3r)              ;; 
 ( [ e a ] = i: )                  ;; {Anything, "EA", Anything, "IY"},
 ( [ e i g h ] = ei )              ;; {Anything, "EIGH", Anything, "EY"},
 ( [ e i ] = ei )                  ;; {Anything, "EI", Anything, "IY"},
 ( [ e y e ] = aI )                ;; {Nothing, "EYE", Anything, "AY"},
 ( [ e y ] = i: )                  ;; {Anything, "EY", Anything, "IY"},
 ( [ e u ] = j u )                 ;; {Anything, "EU", Anything, "YUw"},
 ( [ e ] = E )                     ;; {Anything, "E", Anything, "EH"},

;; F rules
 ( [ f u l ] = f U l )             ;; {Anything, "FUL", Anything, "fUHl"},
 ( [ f ] = f )                     ;; {Anything, "F", Anything, "f"},

;; G rules 
 ( [ g i v ] = g  I v )             ;; {Anything, "GIV", Anything, "gIHv"},
 ( # [ g ] i C = g  )               ;; {Nothing, "G", "I^", "g"},
 ( [ g e ] t = g  E )               ;; {Anything, "GE", "T", "gEH"},
 ( s u [ g g e s ] = dZ E s )       ;; {"SU", "GGES", Anything, "gjEHs"},
 ( [ g g ] = g  )                   ;; {Anything, "GG", Anything, "g"},
 ( # b V + [ g ] = g  )             ;; {" B#", "G", Anything, "g"},
 ( [ g ] VF = dZ )                  ;; {Anything, "G", "+", "j"},
 ( [ g r e a t ] = g 9r ei t )      ;; {Anything, "GREAT", Anything, "grEYt"},
 ( V + [ g h ] = )                  ;; {"#", "GH", Anything, Silent},
 ( [ g ] = g  )                     ;; {Anything, "G", Anything, "g"},

;; H rules
 ( # [ h a v ] = h @ v )           ;; {Nothing, "HAV", Anything, "hAEv"},
 ( # [ h e r e ] = h i: 3r )       ;; {Nothing, "HERE", Anything, "hIYr"},
 ( # [ h o u r ] = aU 3r )         ;; {Nothing, "HOUR", Anything, "AWER"},
 ( [ h o w ] = h aU )              ;; {Anything, "HOW", Anything, "hAW"},
 ( [ h ] V + = h )                 ;; {Anything, "H", "#", "h"},
 ( [ h ] = )                       ;; {Anything, "H", Anything, Silent},

;; I rules 
 ( # [ i n ] = I n )               ;; {Nothing, "IN", Anything, "IHn"},
 ( [ i n ] d = aI n )              ;; {Anything, "IN", "D", "AYn"},
 ( [ i e r ] = i: 3r )             ;; {Anything, "IER", Anything, "IYER"},
 ( V + C * r [ i e d ] = i: d )    ;; {"#:R", "IED", Anything, "IYd"},
 ( [ i e d ] # = aI d )            ;; {Anything, "IED", Nothing, "AYd"},
 ( [ i e n ] = i: E n )            ;; {Anything, "IEN", Anything, "IYEHn"},
 ( [ i e ] t = aI E )              ;; {Anything, "IE", "T", "AYEH"},
 ( # C * [ i ] e # = aI )          ;; {" :", "I", "%", "AY"},
 ( # C * [ i ] e RSD # = aI )      ;; {" :", "I", "%", "AY"},
 ( # C * [ i ] e l y # = aI )      ;; {" :", "I", "%", "AY"},
 ( # C * [ i ] i n g # = aI )      ;; {" :", "I", "%", "AY"},
 ( [ i ] e # = i: )                ;; {Anything, "I", "%", "IY"},
 ( [ i ] e RSD # = i: )            ;; {Anything, "I", "%", "IY"},
 ( [ i ] e l y # = i: )            ;; {Anything, "I", "%", "IY"},
 ( [ i ] i n g # = i: )            ;; {Anything, "I", "%", "IY"},
 ( [ i e ] = i: )                  ;; {Anything, "IE", Anything, "IY"},
 ( [ i r ] e s = aI 9r )           ;; {Anything, "IR", "#", "AYr"},
 ( [ i r ] e d = aI 9r )           ;; {Anything, "IR", "#", "AYr"},
 ( [ i r ] V + = aI 9r )           ;; {Anything, "IR", "#", "AYr"},
 ( [ i ] C e # = aI )              ;;
 ( [ i ] C e RSD # = aI )          ;;
 ( [ i ] C e l y # = aI )          ;;
 ( [ i ] C i n g # = aI )          ;;
 ( [ i ] C VF C * V + = I )        ;; 
 ( [ i z ] e # = aI z )            ;; {Anything, "IZ", "%", "AYz"},
 ( [ i z ] e RSD # = aI z )        ;; {Anything, "IZ", "%", "AYz"},
 ( [ i z ] e l y # = aI z )        ;; {Anything, "IZ", "%", "AYz"},
 ( [ i z ] i n g # = aI z )        ;; {Anything, "IZ", "%", "AYz"},
 ( [ i s ] e # = aI z )            ;; {Anything, "IS", "%", "AYz"},
 ( [ i s ] e RSD # = aI z )        ;; {Anything, "IS", "%", "AYz"},
 ( [ i s ] e l y # = aI z )        ;; {Anything, "IS", "%", "AYz"},
 ( [ i s ] i n g # = aI z )        ;; {Anything, "IS", "%", "AYz"},
 ( [ i ] d e # = aI )              ;; {Anything, "I", "D%", "AY"},
 ( [ i ] d e RSD # = aI )          ;; {Anything, "I", "D%", "AY"},
 ( [ i ] d e l y # = aI )          ;; {Anything, "I", "D%", "AY"},
 ( [ i ] d i n g # = aI )          ;; {Anything, "I", "D%", "AY"},
 ( VF C [ i ] C VF = I )             ;; {"+^", "I", "^+", "IH"},
 ( [ i ] t e # = aI )              ;; {Anything, "I", "T%", "AY"}, 
 ( [ i ] t e RSD # = aI )          ;; {Anything, "I", "T%", "AY"}, 
 ( [ i ] t e l y # = aI )          ;; {Anything, "I", "T%", "AY"}, 
 ( [ i ] t i n g # = aI )          ;; {Anything, "I", "T%", "AY"}, 
 ( V + C + [ i ] C VF = I )        ;; {"#^:", "I", "^+", "IH"},
 ( [ i ] C VF = aI )               ;; {Anything, "I", "^+", "AY"},
 ( [ i r ] = 3r )                  ;; {Anything, "IR", Anything, "ER"},
 ( [ i g h ] = aI )                ;; {Anything, "IGH", Anything, "AY"},
 ( [ i l d ] = aI l d )            ;; {Anything, "ILD", Anything, "AYld"},
 ( [ i g n ] # = aI n )            ;; {Anything, "IGN", Nothing, "AYn"},
 ( [ i g n ] C = aI n )            ;; {Anything, "IGN", "^", "AYn"},
 ( [ i g n ] e # = aI n )          ;; {Anything, "IGN", "%", "AYn"},
 ( [ i g n ] e RSD # = aI n )      ;; {Anything, "IGN", "%", "AYn"},
 ( [ i g n ] e l y # = aI n )      ;; {Anything, "IGN", "%", "AYn"},
 ( [ i g n ] i n g # = aI n )      ;; {Anything, "IGN", "%", "AYn"},
 ( [ i q u e ] = i: k )            ;; {Anything, "IQUE", Anything, "IYk"},
;; ( [ i c r ] = aI k 9r )
 ( [ i ] = I )                     ;; {Anything, "I", Anything, "IH"},

;; J rules
 ( [ j ] = dZ )                    ;; {Anything, "J", Anything, "j"},
 
;; K rules
 ( [ k ] n = )                     ;; {Nothing, "K", "N", Silent},
 ( [ k ] = k )                     ;; {Anything, "K", Anything, "k"},

;; L rules 
 ( [ l o ] c V + = l oU )          ;; {Anything, "LO", "C#", "lOW"},
 ( l [ l ] = )                     ;; {"L", "L", Anything, Silent},
 ( V + C + [ l ] e # = & l )       ;; {"#^:", "L", "%", "AXl"},
 ( V + C + [ l ] e RSD # = & l )   ;; {"#^:", "L", "%", "AXl"},
 ( V + C + [ l ] e l y # = & l )   ;; {"#^:", "L", "%", "AXl"},
 ( V + C + [ l ] i n g # = & l )   ;; {"#^:", "L", "%", "AXl"},
 ( [ l e a d ] = l i: d )          ;; {Anything, "LEAD", Anything, "lIYd"},
 ( [ l ] = l )                     ;; {Anything, "L", Anything, "l"},

;; M rules 
 ( [ m o v ] = m u v )             ;; {Anything, "MOV", Anything, "mUWv"},
 ( [ m ] = m )                     ;; {Anything, "M", Anything, "m"},

;; N rules 
 ( e [ n g ] VF = n dZ )           ;; {"E", "NG", "+", "nj"},
 ( [ n g ] r = N g )               ;; {Anything, "NG", "R", "NGg"},
 ( [ n g ] V + = N g )             ;; {Anything, "NG", "#", "NGg"},
 ( [ n g l ] e # = N g  & l )      ;; {Anything, "NGL", "%", "NGgAXl"},
 ( [ n g l ] e RSD # = N g  & l )  ;; {Anything, "NGL", "%", "NGgAXl"},
 ( [ n g l ] e l y # = N g  & l )  ;; {Anything, "NGL", "%", "NGgAXl"},
 ( [ n g l ] i n g # = N g  & l )  ;; {Anything, "NGL", "%", "NGgAXl"},
 ( [ n g ] = N )                   ;; {Anything, "NG", Anything, "NG"},
 ( [ n k ] = N k )                 ;; {Anything, "NK", Anything, "NGk"},
 ( [ n ] = n )                     ;; {Anything, "N", Anything, "n"},
 ( # [ n o w ] # = n aU )

;; O rules
 ( [ o f ] # = & v )               ;; {Anything, "OF", Nothing, "AXv"},
 ( [ o r o u g h ] = 3r oU )       ;; {Anything, "OROUGH", Anything, "EROW"},
 ( V + C * [ o r ] # = 3r )        ;; {"#:", "OR", Nothing, "ER"},
 ( V + C * [ o r s ] # = 3r z )    ;; {"#:", "ORS", Nothing, "ERz"},
 ( [ o r ] = oU 9r )               ;; {Anything, "OR", Anything, "AOr"},
 ( # [ o n e ] = w ^ n )           ;; {Nothing, "ONE", Anything, "wAHn"},
 ( [ o w ] = oU )                  ;; {Anything, "OW", Anything, "OW"},
 ( # [ o v e r ] = oU v 3r )       ;; {Nothing, "OVER", Anything, "OWvER"},
 ( # [ o v ] = ^ v )               ;; {Anything, "OV", Anything, "AHv"},
 ( [ o ] C e # = oU )              ;; {Anything, "O", "^%", "OW"},
 ( [ o ] C e RSD # = oU )          ;; {Anything, "O", "^%", "OW"},
 ( [ o ] C e l y # = oU )          ;; {Anything, "O", "^%", "OW"},
 ( [ o ] C i n g # = oU )          ;; {Anything, "O", "^%", "OW"},
 ( [ o ] C e n  = oU )             ;; {Anything, "O", "^EN", "OW"},
 ( [ o ] C i V + = oU )            ;; {Anything, "O", "^I#", "OW"},
 ( [ o l ] d = oU l )              ;; {Anything, "OL", "D", "OWl"},
 ( [ o u g h t ] = > t )           ;; {Anything, "OUGHT", Anything, "AOt"},
 ( [ o u g h ] = ^ f )             ;; {Anything, "OUGH", Anything, "AHf"},
 ( # [ o u ] = aU )                ;; {Nothing, "OU", Anything, "AW"},
 ( h [ o u ] s V + = aU )          ;; {"H", "OU", "S#", "AW"},
 ( [ o u s ] = & s )               ;; {Anything, "OUS", Anything, "AXs"},
 ( [ o u r ] = aU 9r )             ;;  {Anything, "OUR", Anything, "AOr"},
 ( [ o u l d ] = U d )             ;; {Anything, "OULD", Anything, "UHd"},
 ( C [ o u ] C l = ^ )             ;; {"^", "OU", "^L", "AH"},
 ( [ o u p ] = u )                 ;; {Anything, "OUP", Anything, "UWp"},
 ( [ o u ] = aU )                  ;; {Anything, "OU", Anything, "AW"},
 ( [ o y ] = >i )                  ;; {Anything, "OY", Anything, "OY"},
 ( [ o i n g ] = oU I N )          ;; {Anything, "OING", Anything, "OWIHNG"},
 ( [ o i ] = >i )                  ;; {Anything, "OI", Anything, "OY"},
 ( [ o o r ] = oU 9r )             ;; {Anything, "OOR", Anything, "AOr"},
 ( [ o o k ] = U k )               ;; {Anything, "OOK", Anything, "UHk"},
 ( [ o o d ] = U d )               ;; {Anything, "OOD", Anything, "UHd"},
 ( [ o o ] = u )                   ;; {Anything, "OO", Anything, "UW"},
 ( [ o ] e = oU )                  ;; {Anything, "O", "E", "OW"},
 ( [ o ] # = oU )                  ;; {Anything, "O", Nothing, "OW"},
 ( [ o a ] = oU )                  ;; {Anything, "OA", Anything, "OW"},
 ( # [ o n l y ] = oU n l i: )     ;; {Nothing, "ONLY", Anything, "OWnlIY"},
 ( # [ o n c e ] = w ^ n s )       ;; {Nothing, "ONCE", Anything, "wAHns"},
;; ( [ o n \' t ] = oU n t )         ;; {Anything, "ON'T", Anything, "OWnt"},
 ( c [ o ] n = A )                 ;; {"C", "O", "N", "AA"},
 ( [ o ] n g = > )                 ;; {Anything, "O", "NG", "AO"},
 ( C + [ o ] n = ^ )               ;; {"^:", "O", "N", "AH"},
 ( i [ o n ] = & n )               ;; {"I", "ON", Anything, "AXn"},
 ( V + C * [ o n ] # = & n )       ;; {"#:", "ON", Nothing, "AXn"},
 ( V + C [ o n ] = & n )           ;; {"#^", "ON", Anything, "AXn"},
 ( [ o ] s t # = oU )              ;; {Anything, "O", "ST ", "OW"},
 ( [ o f ] C = > f )               ;; {Anything, "OF", "^", "AOf"},
 ( [ o t h e r ] = ^ D 3r )        ;; {Anything, "OTHER", Anything, "AHDHER"},
 ( [ o s s ] # = > s )             ;; {Anything, "OSS", Nothing, "AOs"},
 ( V + C + [ o m ] = ^ m )         ;; {"#^:", "OM", Anything, "AHm"},
 ( [ o ] = A )                     ;; {Anything, "O", Anything, "AA"},

;; P rules
 ( [ p h ] = f )                   ;; {Anything, "PH", Anything, "f"},
 ( [ p e o p ] = p i: p )          ;; {Anything, "PEOP", Anything, "pIYp"},
 ( [ p o w ] = p aU )              ;; {Anything, "POW", Anything, "pAW"},
 ( [ p u t ] # = p U t )           ;; {Anything, "PUT", Nothing, "pUHt"},
 ( [ p ] = p )                     ;; {Anything, "P", Anything, "p"},

;; Q rules
 ( [ q u a r ] = k w > 9r )        ;; {Anything, "QUAR", Anything, "kwAOr"},
 ( [ q u ] = k w )                 ;; {Anything, "QU", Anything, "kw"},
 ( [ q ] = k )                     ;; {Anything, "Q", Anything, "k"},

;; R rules 
 ( # [ r e ] C V + = 9r i: )       ;; {Nothing, "RE", "^#", "rIY"},
 ( t [ r e ] # = 3r )              ;; centre ok
 ( # [ r h ] = 9r )                ;; ok
 ( [ r ] C # = 3r )                ;; ok
;; ( [ r ] # = 9r )
 ( [ r ] = 9r )                    ;; {Anything, "R", Anything, "r"},

;; S rules
 ( [ s h ] = S )                  ;; {Anything, "SH", Anything, "SH"},
 ( V + [ s i o n ] = Z & n )      ;; {"#", "SION", Anything, "ZHAXn"},
 ( [ s o m e ] = s ^ m )          ;; {Anything, "SOME", Anything, "sAHm"},
 ( V + [ s u r ] V + = Z 3r )     ;; {"#", "SUR", "#", "ZHER"},
 ( [ s u r ] V + = S 3r )         ;; {Anything, "SUR", "#", "SHER"},
 ( V + [ s u ] V + = Z u )        ;; {"#", "SU", "#", "ZHUW"},
 ( V + [ s s u ] V + = S u )      ;; {"#", "SSU", "#", "SHUW"},
 ( V + [ s e d ] # = z d )        ;; {"#", "SED", Nothing, "zd"},
 ( V + [ s ] V + = z )            ;; {"#", "S", "#", "z"},
 ( [ s a i d ] = s E d )          ;; {Anything, "SAID", Anything, "sEHd"},
 ( C [ s i o n ] = S & n )        ;; {"^", "SION", Anything, "SHAXn"},
 ( [ s ] s = )                    ;; {Anything, "S", "S", Silent},
 ( Cv [ s ] # = z )               ;; {".", "S", Nothing, "z"},
 ( V + C * Cv e [ s ] # = z )     ;; {"#:.E", "S", Nothing, "z"},
 ( V + C + V V + [ s ] # = z )    ;; {"#^:##", "S", Nothing, "z"},
 ( V + C + V + [ s ] # = s )      ;; {"#^:#", "S", Nothing, "s"},
 ( u [ s ] # = s )                ;; {"U", "S", Nothing, "s"},
 ( # C * V + [ s ] # = z )        ;; {" :#", "S", Nothing, "z"},
 ( [ s c h ] = s k )              ;; {Nothing, "SCH", Anything, "sk"},
 ( [ s ] c VF = )                 ;; {Anything, "S", "C+", Silent},
 ( V + [ s m ] = z m )            ;; {"#", "SM", Anything, "zm"},
;; ( V + [ s n ] \' = z & n )       ;; {"#", "SN", "'", "zAXn"},
 ( [ s ] = s )                    ;; {Anything, "S", Anything, "s"},

;; T rules 
 ( # [ t h e ] # = D & )          ;; probably should delete
 ( [ t o ] # = t u )              ;; {Anything, "TO", Nothing, "tUW"},
 ( [ t h a t ] # = D @ t )
 ( # [ t h i s ] # = D I s )      ;; probably delete
 ( # [ t h e y ] = D ei )         ;; {Nothing, "THEY", Anything, "DHEY"},
 ( # [ t h e r e ] = D E 9r )     ;; {Nothing, "THERE", Anything, "DHEHr"},
 ( [ t h e r ] = D E 9r )         ;; {Anything, "THER", Anything, "DHER"},
 ( [ t h e i r ] = D E 9r )       ;; {Anything, "THEIR", Anything, "DHEHr"},
 ( # [ t h a n ] # = D @ n )      ;; probably delete
 ( # [ t h e m ] # = D E m )      ;; probably delete
 ( [ t h e s e ] # = D i: z )     ;; {Anything, "THESE", Nothing, "DHIYz"},
 ( # [ t h e n ] = D E n )        ;; {Nothing, "THEN", Anything, "DHEHn"},
 ( [ t h r o u g h ] = T 9r u )   ;; {Anything, "THROUGH", Anything, "THrUW"},
 ( [ t h o s e ] = D oU z )       ;; {Anything, "THOSE", Anything, "DHOWz"},
 ( [ t h o u g h ] = D oU )       ;; {Anything, "THOUGH", Nothing, "DHOW"},
 ( [ t h u s ] = D ^ s )          ;; {Nothing, "THUS", Anything, "DHAHs"},
 ( [ t h ] = T )                  ;; {Anything, "TH", Anything, "TH"},
 ( V + C * [ t e d ] # = t I d )  ;; {"#:", "TED", Nothing, "tIHd"},
 ( s [ t i ] V + n = tS )         ;; {"S", "TI", "#N", "CH"},
 ( [ t i ] o = S )                ;; {Anything, "TI", "O", "SH"},
 ( [ t i ] a = S )                ;; {Anything, "TI", "A", "SH"},
 ( [ t i e n ] = S & n )          ;; {Anything, "TIEN", Anything, "SHAXn"},
 ( [ t u r ] V + = tS 3r )        ;; {Anything, "TUR", "#", "CHER"},
 ( [ t u ] a = tS u )             ;; {Anything, "TU", "A", "CHUW"},
 ( # [ t w o ] = t u )            ;; {Nothing, "TWO", Anything, "tUW"},
 ( [ t ] = t )                    ;; {Anything, "T", Anything, "t"},

;; U rules 
 ( # [ u n ] i = j u n )          ;;{Nothing, "UN", "I", "yUWn"},
 ( # [ u n ] = ^ n )              ;; {Nothing, "UN", Anything, "AHn"},
 ( # [ u p o n ] = & p > n )      ;; {Nothing, "UPON", Anything, "AXpAOn"},
 ( TSRDLZNJ [ u r ] V + = U 9r )    ;; {"T", "UR", "#", "UHr"},
 ( TCS h [ u r ] V + = U 9r )       ;; {"TH", "UR", "#", "UHr"},
 ( [ u r ] e SD = j u 9r )
 ( [ u r ] V + = j U 9r )         ;; {Anything, "UR", "#", "yUHr"},
 ( [ u r ] = 3r )                 ;; {Anything, "UR", Anything, "ER"},
 ( [ u ] C # = ^ )                ;; {Anything, "U", "^ ", "AH"},
 ( [ u ] C C = ^ )                ;; {Anything, "U^^", Anything, "AH"},
 ( [ u y ] = aI )                 ;; {Anything, "UY", Anything, "AY"},
 ( # g [ u ] V + = )              ;; {" G", "U", "#", Silent},
 ( g [ u ] e # = )                ;; {"G", "U", "%", Silent},
 ( g [ u ] e RSD # = )            ;; {"G", "U", "%", Silent},
 ( g [ u ] e l y # = )            ;; {"G", "U", "%", Silent},
 ( g [ u ] i n g # = )            ;; {"G", "U", "%", Silent},
 ( g [ u ] V + = w )              ;; {"G", "U", "#", "w"},
 ( V + n [ u ] = j u )            ;; {"#N", "U", Anything, "YUw"},
 ( TSRDLZNJ [ u ] = u )           ;; {"T", "U", Anything, "UW"},
 ( TCS h [ u ] = u )              ;; {"TH", "U", Anything, "UW"},
 ( [ u ] = j u )                  ;; {Anything, "U", Anything, "UW"},

;; V rules
 ( [ v i e w ] = v j u )          ;; {Anything, "VIEW", Anything, "vYUw"},
 ( [ v ] = v )                    ;; {Anything, "V", Anything, "v"},

;; W rules 
 ( # [ w e r e ] = w 3r )          ;; {Nothing, "WERE", Anything, "wER"},
 ( [ w a ] s = w A )               ;; {Anything, "WA", "S", "wAA"},
 ( [ w a ] t = w A )               ;; {Anything, "WA", "T", "wAA"},
 ( [ w e r e ] = w 3r )            ;; {Anything, "WERE", Anything, "WHEHr"},
 ( [ w h e r e ] = w E 9r )          
 ( [ w h a t ] = w A t )           ;; {Anything, "WHAT", Anything, "WHAAt"},
 ( [ w h o l ] = h oU l )          ;; {Anything, "WHOL", Anything, "hOWl"},
 ( [ w h o ] = h u )               ;; {Anything, "WHO", Anything, "hUW"},
 ( [ w h ] = w )                   ;; {Anything, "WH", Anything, "WH"},
 ( [ w a r ] = w > 9r )            ;; {Anything, "WAR", Anything, "wAOr"},
 ( [ w o r ] C = w 3r )            ;; {Anything, "WOR", "^", "wER"},
 ( [ w r ] = 9r )                  ;; {Anything, "WR", Anything, "r"},
 ( [ w ] = w )                     ;; {Anything, "W", Anything, "w"},

;; X rules
 ( [ x ] = k s )                   ;; {Anything, "X", Anything, "ks"},

;; Y rules 
 ( [ y o u n g ] = j ^ N )         ;; {Anything, "YOUNG", Anything, "yAHNG"},
 ( # [ y o u ] = j u )             ;; {Nothing, "YOU", Anything, "yUW"},
 ( # [ y e s ] = j E s )           ;; {Nothing, "YES", Anything, "yEHs"},
 ( # [ y ] = j )                   ;; {Nothing, "Y", Anything, "y"},
 ( V + C + [ y ] # = i: )          ;; {"#^:", "Y", Nothing, "IY"},
 ( V + C + [ y ] i = i: )          ;; {"#^:", "Y", "I", "IY"},
 ( # C * [ y ] # = aI )              ;; {" :", "Y", Nothing, "AY"},
 ( # C * [ y ] V + = aI )            ;; {" :", "Y", "#", "AY"},
 ( # C * [ y ] C VF C * V + = I )    ;; {" :", "Y", "^+:#", "IH"},
 ( # C * [ y ] C V + = aI )          ;; {" :", "Y", "^#", "AY"},
 ( [ y ] = I )                     ;; {Anything, "Y", Anything, "IH"},

;; Z rules 
 ( [ z ] = z )                     ;; {Anything, "Z", Anything, "z"},
))

(provide 'ogi_lts_worldbet)









