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
 syl_worldbet
;; Sets
( 
 (V  i:0 I0 E0 @0 u0 U0 ^0 &0 >0 A0 3r0 ei0 aI0 >i0 aU0 oU0 iU0 i:1 I1 E1 @1 u1 U1 ^1 >1 A1 3r1 ei1 aI1 >i1 aU1 oU1 iU1 i:2 I2 E2 @2 u2 U2 ^2 >2 A2 3r2 ei2 aI2 >i2 aU2 oU2 iU2 i: I E @ u U ^ > A 3r ei aI >i aU oU iU &)
 (C l 9r m n N p t k ph th kh b d g f T S h v D z Z tS dZ j w s)

 (V1 i:1 I1 E1 @1 u1 U1 ^1 >1 A1 3r1 ei1 aI1 >i1 aU1 oU1 iU1)
 (V0 &0 i:0 I0 E0 @0 u0 U0 ^0 >0 A0 3r0 ei0 aI0 >i0 aU0 oU0  iU0 & i: I E @ u U ^ > A 3r ei aI >i aU oU iU)
 (V2 i:2 I2 E2 @2 u2 U2 ^2 >2 A2 3r2 ei2 aI2 >i2 aU2 oU2 iU2)
 (V1and2 i:1 I1 E1 @1 u1 U1 ^1 >1 A1 3r1 ei1 aI1 >i1 aU1 oU1 iU1 i:2 I2 E2 @2 u2 U2 ^2 >2 A2 3r2 ei2 aI2 >i2 aU2 oU2 iU2)
 (V0and2 &0 i:0 I0 E0 @0 u0 U0 ^0 >0 A0 3r0 ei0 aI0 >i0 aU0 oU0 iU0 & i: I E @ u U ^ > A 3r ei aI >i aU oU i:2 I2 E2 @2 u2 U2 ^2 >2 A2 3r2 ei2 aI2 >i2 aU2 oU2 iU2)
(VafterKW oU0 ei0 >i0 E0 I0 ^0 &0 i:0 A0 oU ei >i E I ^ & i: A)
 (CnoT l 9r m n N p k b d g f T S h v D z Z tS dZ j w s)
 (CnoR l m n N p t k b d g f T S h v D z Z tS dZ j w s)
 (CnoRJ l m n N p t k b d g f T S h v D z Z tS dZ w s)
(CnoLIQ m n N p t k b d g f T S h v D z Z tS dZ s w)
(NAS m n)
(FRIQ f v)
(UPLOS p t k ph th kh)
(PLOS p t k ph th kh b d g )
(PLOSnoTD p k ph kh b g)
(CONSbeforeT p k f v s n m N l 9r S tS )
(CONSbeforeD b z v m n l 9r dZ)
(CONSbeforeZ b d g v D m n N l 9r t k)
(CONSbeforeP m s l 9r)
(CONSafterS p t k m n w l 9r)
(LRN l 9r n)
(CONSbeforeK N s l 9r)
(CONSbeforeTH p t k l 9r n f s d)
)
;; The rule set
(

; initial onsets
( # C [ - ] C = )
( # C - C [ - ] C = )
( # C - C - C [ - ] C = )

; word-final codas

( C [ - ] C - # = )
( C [ - ] C - C - # = )
( V [ - ] C - # = )
( V [ - ] C - C - # = )
( V [ - ] C - C - C - # = )

; typically the consonant before a vowel is part of the onset
( C [ - ] V = )

; word-initial morphemes
( # d - I [ - s - ] = s - ) ; dis-
( # d - I0 [ - s - ] = s - ) 
;( d - I [ - s - ] C = s - ) ; dis-
;( d - I0 [ - s - ] C = s - ) 
( # d - I2 [ - s - ] = s - ) 
( # m - I [ - s - ] = s - ) ; word-initial mis-
( # m - I0 [ - s - ] = s - )
( # E0 - k [ - s - ] = s - )
( # E1 - k [ - s - ] = s - )
( # E2 - k [ - s - ] = s - )

( # s - E1 - l [ - ] f  =  )
( # s - E1 - l [ - ] f [ - ] = - )

( # & - n [ - ] V = - )
( # ^ - n [ - ] V = - )

( # & - n [ - ] C = - )
( # ^ - n [ - ] C = - )

( # & - n - C [ - ] = )
( # ^ - n - C [ - ] = )

( [ - s - f - ] i:2 - 9r - # = - s f )
( [ - s - f - ] I2 - 9r - # = - s f )
( [ - s - f - ] i:1 - 9r - # = - s f )


; word-final morphemes

( V1 [ - s - ] UPLOS - l - i:0 - # = s )
( V1 [ - s - ] UPLOS - l - i: - # = s )
( C [ - ] UPLOS - l - i:0 - # = )
( C [ - ] UPLOS - l - i: - # =  )

( [ - s - ] l - i: - # = s -  ) ; word-final -ly
( [ - s - ] l - i:0 - # = s - )
( [ - b - ] l - i: - # = - b ) 
( [ - b - ] l - i:0 - # = - b )
( N [ - g - ] l - i: - # = - g )
( N [ - g - ] l - i:0 - # = - g )
( l [ - f - ] l - i: - # = f - )
( l [ - f - ] l - i:0 - # = f - )
( V [ - ] C - l - i: - # =  ) 
( V [ - ] C - l - i:0 - # =  )
( C [ - l - ] i: - # = - l ) 
( C [ - l - ] i:0 - # = - l )

( [ - s - ] m - I0 - T - # = - s )
( [ - s - ] m - I - T - # = - s )
( [ - s - ] m - I2 - T - # = - s )
( [ - s - ] m - I1 - T - # = - s )
( [ - s - ] m - I0 - T - s - # = - s )
( [ - s - ] m - I2 - T - s - # = - s )
( [ - s - ] m - I1 - T - s - # = - s )

;; french endings

( [ - b - ] w - A = - b )
( [ - b - ] w - A0 = - b )
( [ - b - ] w - A1 = - b )
( [ - b - ] w - A2 = - b )

( [ - t - ] w - A1 - # = - t )
( [ - p - ] w - A1 - # = - p )
( [ - t - ] w - A0 - # = - t )
( [ - t - ] w - A2 - # = - t )
( [ - k - ] w - A2 - # = - k )
( [ - k - ] w - A0 - # = - k )
( [ - t - ] w - A1 - 9r - # = - t )
( [ - t - ] w - A2 - 9r - # = - t )
( [ - t - ] w - A2 - z - # = - t )

( [ - b - 9r - w ] - A1 = - b 9r w )
( [ - f - 9r - w ] - A1 = - f 9r w )
( [ - k - 9r - w ] - A1 = - k 9r w )


;onsets

; clusters with j

( V [ - ] LRN - j =  )
( V - LRN [ - ] j = - )

( C [ - s - t - j - ] V = - s t j )
( C [ - s - k - j - ] V = - s k j )

( V1 [ - ] C - j - V2 = )
( V1 - C [ - ] j - V2 = - )

( V [ - ] C - j - V = - )
( V - C [ - ] j - V = )

( V1 - C [ - ] C - j - V2 = )
( V1 - C - C [ - ] j - V2 = - )

( V - C [ - ] C - j - V = - )
( C - C [ - ] j - V = )
( C - C [ - ] C - j - V = - )
( C - C - C [ - ] j - V = )

; clusters with l

( V1and2 [ - ] PLOSnoTD - l - V = )
( V [ - ] PLOSnoTD - l - V = - )
( d - s [ - ] PLOSnoTD - l - V = )
( C [ - ] PLOSnoTD - l - V = - )

( V1and2 - PLOSnoTD [ - ] l - V = - )
( V - PLOSnoTD [ - ] l - V = )
( C - PLOSnoTD [ - ] l - V = )

( V1 [ - ] f - l - V = )
( V [ - ] f - l - V = - )
( C [ - ] f - l - V = - )

( V1 - f [ - ] l - V = - )
( V - f [ - ] l - V = )
( C - f [ - ] l - V = )

( V1and2 [ - ] s  - l - V = )
( V1and2 - s [ - ] l - V = - )

( V1and2 - 9r [ - ] s - l - V0 = )
( V1and2 - 9r - s [ - ] l - V0 = - )

( C [ - ] s - l - V = - )
( C - s [ - ] l - V = )

( 3r [ - ] v - l - V = - )
( V1 - l [ - ] v - l - V = )
( C - v [ - ] l - V = )

; clusters with 9r

; if preceding vowel is stressed, put s in previous syllable

( V1and2 [ - ] s - UPLOS - 9r = )
( V1and2 - s [ - ] UPLOS - 9r = - )
( V1and2 - s - UPLOS [ - ] 9r = )

; if s preceded by unstressed vowel or consonant, put in next syllable
( V0 [ - ] s - UPLOS - 9r = - )
( V0 - s [ - ] UPLOS - 9r = )
( C [ - ] s - UPLOS - 9r = - )
( C - s [ - ] UPLOS - 9r = )

( C [ - ] S - UPLOS - 9r = - )
( C - S [ - ] UPLOS - 9r = )
( C - S - UPLOS [ - ] 9r = )

( V1 - 9r [ - ] t - 9r - V0 = )
( V1 - 9r - t [ - ] 9r - V0 = - )

( [ - ] PLOS - 9r - V = - )
( [ - ] FRIQ - 9r - V = - )
( [ - ] T - 9r - V = - )
( [ - ] w - 9r - V = - )

( - PLOS [ - ] 9r - V = )
( - FRIQ [ - ] 9r - V = )
( - T [ - ] 9r - V = )
( - w [ - ] 9r - V = )

; clusters with s followed by p,t,k

( V1and2 [ - ] s - UPLOS - V =  )
( V1and2 - s [ - ] UPLOS - V = - )

( V0 [ - ] s - UPLOS - V = - )
( V0 - s [ - ] UPLOS - V = )

( @ - n [ - s - ] UPLOS - V = s - )

( C [ - ] s - UPLOS - V = - )
( C - s [ - ] UPLOS - V = )

; other clusters

( C - S [ - ] CONSafterS - V = )

; clusters with w

( Z [ - ] w - V = )
( C - l [ - ] w - V = )
( 9r - n [ - ] w - V = - )
( C - n [ - ] w - V = )

( C [ - ] s - k - w = - )
( C - s [ - ] k - w = )

( [ - ] k - w - VafterKW = - )
( - k [ - ] w - VafterKW = )

( s [ - ] k - w = - )
( s - k [ - ] w = )

( C [ - ] k - w = - )
( C - k [ - ] w = )

( [ - ] k - w  = )
( - k [ - ] w = - )

( V [ - ] g - w - VafterKW = )
( V - g [ - ] w - VafterKW = - )

( V0 [ - ] g - w - V = - )
( V0 - g [ - ] w - V = )

( C [ - ] g - w - V = - )
( C - g [ - ] w - V = )

( [ - ] b - w - V1 = - )
(  -  b [ - ] w - V1 = )

( CnoLIQ [ - ] d - w - V = - )
( CnoLIQ - d [ - ] w - V = )

( [ - ] s - w - V1 = - )
( - s [ - ] w - V1 = )

( V1 - C [ - ] s - w - V = )
( V1 - C - s [ - ] w - V = - )

( V1 - n - d [ - ] s - w = )

( C - p [ - ] w - @1 = )
 
( [ - ] tS - w = - )
( - tS [ - ] w = )

; valid codas

( V - CONSbeforeP [ - ] p - CnoLIQ = )
( V - s - p [ - ] n = )
( V - n [ - ] p - h = )

( V - CONSbeforeK [ - ] k - CnoLIQ = )
( V - T [ - ] s - C = )


( V - l - UPLOS [ - ] s - CnoLIQ = )
( V - l - m [ - ] s - CnoLIQ = )
( V - l - UPLOS [ - ] s - 9r = )
( V - 9r - UPLOS [ - ] s - CnoLIQ = )
( V - 9r - l [ - ] s - CnoLIQ = )
( V - f - t [ - ] s - CnoLIQ = )
( V - n - t [ - ] s - CnoLIQ = )
( V - N - k [ - ] s - C = )
( V - n [ - ] k - s - V = )
( V - [ n - s - t ] - CnoR = n s t )
( V - [ m - s - t ] - CnoR = m s t )
( V - [ 9r - s - t ] - CnoR = 9r s t )
( V - k - s [ - ] t - CnoRJ =  )
( V - [ b - s - t ] - CnoRJ = b s t )
( V - [ 9r - n - s - ] w = 9r n s - )
( V - [ 9r - t - s - ] CnoT = 9r t s - )
( V - l - f [ - ] s - C = )
( V - N [ - ] s - CnoLIQ = )
( V - b [ - ] s - b = )

( V - CONSbeforeT [ - ] t - C = )
( V - 9r - n [ - ] t - C = )
( V - l - f [ - ] t - C = )
( V - 9r - k [ - ] t - C = )
( V - m - p [ - ] t - CnoLIQ = )
( V - N - k [ - ] t - CnoLIQ = )
( V - CONSbeforeTH [ - ] T - C = )
( k - s [ - ] T - C = )
( l - f [ - ] T - C = )
( V - l [ - ] b - C = )
( V - 9r [ - ] b - C = )
( V - CONSbeforeD [ - ] d - C = )
( V - 9r - CONSbeforeD [ - ] d - C = )
( V - l - CONSbeforeD [ - ] d - C = )

( V - 9r [ - ] g - C = )
( V - N [ - ] g - C = )
( V - l [ - ] g - CnoLIQ = )

( V - LRN [ - ] dZ - C = )
( V - LRN [ - ] tS - C = )
( V - 9r [ - ] S - C = )
( V - l [ - ] S - C = )
( V - b [ - ] S - C = )

( V - l [ - ] n - C = )
( V - 9r [ - ] n - C = )

( V - LRN - k [ - ] n - V = - )
( V - N - k [ - ] n - V = - )
( CnoLIQ - k [ - ] n - V = )

( V - 9r [ - ] m - C = )
( V - l [ - ] m - C = )
( CnoLIQ - k [ - ] m - V = )
( V - 9r [ - ] N - C = )
( V - 9r [ - ] l - C = )

( V - LRN [ - ] f - CnoLIQ = )
( V - m [ - ] f - CnoLIQ = )
( V - p [ - ] f - C = ) ; german i.e. hoepfner
( CnoLIQ - p [ - ] f - V = )
( V - l [ - ] f - C = )

( V - l [ - ] v - CnoLIQ = )
( V - 9r [ - ] v - CnoLIQ = )
( CnoLIQ - k [ - ] v - V = )

( V - UPLOS [ - ] s - C = )
( V - f [ - ] s - C = )
( V - LRN [ - ] s - C = )
( V - m [ - ] s - C = )

( V - CONSbeforeZ [ - ] z - C = )
( LRN - CONSbeforeZ [ - ] z - C = )
( N - d [ - ] z - V = )
( t - d [ - ] z - V = )

( [ - s - ] n - V2 = - s )
( [ - s - ] m - V2 = - s )
( [ - s - ] C - V2 = s - )


( V - C [ - ] C - V = - )
( V [ - ] C - C - V = )
( V - C [ - ] C - C = - )
( V [ - ] C - C - C = )


; delete final dash
( [ - ] # = )

;; all characters rewrite to themselves

 ( [ i:0 ] = i:0 )
 ( [ I0 ] = I0 )
 ( [ E0 ] = E0 )
 ( [ @0 ] = @0 )
 ( [ u0 ] = u0 )
 ( [ U0 ] = U0 )
 ( [ ^0 ] = ^0 )
 ( [ &0 ] = &0 )
 ( [ >0 ] = >0 )
 ( [ A0 ] = A0 )
 ( [ 3r0 ] = 3r0 )
 ( [ ei0 ] = ei0 )
 ( [ aI0 ] = aI0 )
 ( [ >i0 ] = >i0 )
 ( [ aU0 ] = aU0 )
 ( [ oU0 ] = oU0 )
 ( [ iU0 ] = iU0 )
 ( [ i:1 ] = i:1 )
 ( [ I1 ] = I1 )
 ( [ E1 ] = E1 )
 ( [ @1 ] = @1 )
 ( [ u1 ] = u1 )
 ( [ U1 ] = U1 )
 ( [ ^1 ] = ^1 )
 ( [ &1 ] = &1 )
 ( [ >1 ] = >1 )
 ( [ A1 ] = A1 )
 ( [ 3r1 ] = 3r1 )
 ( [ ei1 ] = ei1 )
 ( [ aI1 ] = aI1 )
 ( [ >i1 ] = >i1 )
 ( [ aU1 ] = aU1 )
 ( [ oU1 ] = oU1 )
 ( [ iU1 ] = iU1 )
 ( [ i:2 ] = i:2 )
 ( [ I2 ] = I2 )
 ( [ E2 ] = E2 )
 ( [ @2 ] = @2 )
 ( [ u2 ] = u2 )
 ( [ U2 ] = U2 )
 ( [ ^2 ] = ^2 )
 ( [ &2 ] = &2 )
 ( [ >2 ] = >2 )
 ( [ A2 ] = A2 )
 ( [ 3r2 ] = 3r2 )
 ( [ ei2 ] = ei2 )
 ( [ aI2 ] = aI2 )
 ( [ >i2 ] = >i2 )
 ( [ aU2 ] = aU2 )
 ( [ oU2 ] = oU2 )
 ( [ iU2 ] = iU2 )
 ( [ i: ] = i:0 )
 ( [ I ] = I0 )
 ( [ E ] = E0 )
 ( [ @ ] = @0 )
 ( [ u ] = u0 )
 ( [ U ] = U0 )
 ( [ ^ ] = ^0 )
 ( [ & ] = &0 )
 ( [ > ] = >0 )
 ( [ A ] = A0 )
 ( [ 3r ] = 3r0 )
 ( [ ei ] = ei0 )
 ( [ aI ] = aI0 )
 ( [ >i ] = >i0 )
 ( [ aU ] = aU0 )
 ( [ oU ] = oU0 )
 ( [ iU ] = iU0 )
 ( [ l ] = l )
 ( [ 9r ] = 9r )
 ( [ m ] = m )
 ( [ n ] = n )
 ( [ N ] = N )
 ( [ p ] = p )
 ( [ ph ] = ph )
 ( [ t ] = t )
 ( [ th ] = th )
 ( [ dx ] = dx )
 ( [ k ] = k )
 ( [ kh ] = kh )
 ( [ b ] = b )
 ( [ d ] = d )
 ( [ g ] = g )
 ( [ f ] = f )
 ( [ T ] = T )
 ( [ S ] = S )
 ( [ h ] = h )
 ( [ v ] = v )
 ( [ D ] = D )
 ( [ z ] = z )
 ( [ Z ] = Z )
 ( [ tS ] = tS )
 ( [ dZ ] = dZ )
 ( [ j ] = j )
 ( [ w ] = w )
 ( [ s ] = s )
 ( [ ? ] = ? )
 ( [ # ] = # )
 ( [ - ] = - )
 ))

(provide 'ogi_syl_worldbet)
