;;;;;;;;;;;;;;;;;;;;;;;;;;;;;<--CSLU-->;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                        ;;
;;             Center for Spoken Language Understanding                   ;;
;;        Oregon Graduate Institute of Science & Technology               ;;
;;                         Portland, OR USA                               ;;
;;                        Copyright (c) 1999                              ;;
;;                                                                        ;;
;;      This module is not part of the CSTR/University of Edinburgh       ;;
;;               release of the Festival TTS system.                      ;;
;;                                                                        ;;
;;  In addition to any conditions disclaimers below, please see the file  ;;
;;  "license_cslu_tts.txt" distributed with this software for information  ;;
;;  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  ;;
;;                                                                        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;<--CSLU-->;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  Letter to sound rules for Mexican Spanish
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;   Letter to sound rules for spanish 
;;;   Got these from Alistair, which obvious are derived from 
;;;                  Rob van Gerwen, 
;;;                  University of Nijmegen
;;;                  Department of Language and Speech
;;;   But have been updated too.
;;;
;;;   Not sure about the copyright position
;;;

(lts.ruleset
;  Name of rule set
 span_mx
;  Sets used in the rules
(
  (LNS l n s )
  (AEIOU a e i o u )
  (AEO a e o )
  (EI e i )
  (BDGLMN b d g l m n )
  (C b c d f g j k l m n p q r s t v w x y z )
  (TILDE "~")
  (ACCENT "'")
  (STOP t p k b d g )
  (I i I )
  (IA ç Õ Ì)
  (O o O )
  (OA  ‘ Û )
  (E e E )
  (EA â – È … )
)
;  Rules
;; aec changed accents to follow vowels
(
 ( [ a "'" ] = a1 )
 ( [ e "'" ] = e1 )
 ( [ i "'" ] = i1 )
 ( [ o "'" ] = o1 )
 ( [ u "'" ] = u1 )

 ( [ · ] = a1 )
 ( [ ﬂ ] = a1 )
 ( [ È ] = e1 )
 ( [ â ] = e1 ) 
 ( [ Ì ] = i1 )
 ( [ ç ] = i1 )
 ( [ Û ] = o1 )
 ( [  ] = o1 )
 ( [ ˙ ] = u1 )
 ( [ î ] = u1 )

 ( [ ¬ ] = a1 )
 ( [ … ] = e1 )
 ( [ – ] = e1 )
 ( [ Õ ] = i1 )
 ( [ ‘ ] = o1 )
 ( [ ı ] = u1 )

 ( [ Ò ] = ny )
 ( [ ± ] = ny )
 ( [ — ] = ny )
 ( [ “ ] = ny )
 ( [ n y ] = ny )
 ( [ n "~" ] = ny ) 
 ( [ N "~" ] = ny ) 

 ( [ u ":" ] I = w i ) 
 ( [ u ":" ] IA = w i1 ) 
 ( [ í ] I = w i )
 ( [ õ ] I = w i )
 ( [ í ] IA = w i1 )
 ( [ õ ] IA = w i1 )
 ( [ ¸ ] IA = w i1 )
 ( [ ‹ ] IA = w i1 )
 ( [ ¸ ] I = w i )
 ( [ ‹ ] I = w i )

 ( [ u ":" ] O = w o )
 ( [ u ":" ] OA = w o1 )
 ( [ í ] O = w o )
 ( [ õ ] O = w o )
 ( [ í ] OA = w o1 )
 ( [ õ ] OA = w o1 )
 ( [ ¸ ] OA = w o1 )
 ( [ ‹ ] OA = w o1 )
 ( [ ¸ ] O = w o )
 ( [ ‹ ] O = w o )

 ( [ u ":" ] E = w e ) 
 ( [ u ":" ] EA = w e1 )
 ( [ í ] E = w e )
 ( [ õ ] E = w e )
 ( [ í ] EA = w e1 )
 ( [ õ ] EA = w e1 )
 ( [ ¸ ] EA = w e1 )
 ( [ ‹ ] EA = w e1 )
 ( [ ¸ ] E = w e )
 ( [ ‹ ] E = w e )



 ( rr [ u ] AEIOU = u )
 ( # r [ u ] AEIOU = u )

 ( [ u ] AEIOU = w )
 ( [ u ] = u )
 ( [ a ] = a )
 ( [ e ] = e )
 ( [ i ] = i )
 ( [ o ] = o )
 
 ( [ b ] = b )
 ( [ v ] = b )
 ( [ c ] EI "'" = s )
 ( [ c ] EI = s )
 ( [ c h ] = tS )
 ( [ c ] = k )
 ( [ d ] = d )
 ( [ f ] = f )
 ( [ g ] EI "'" = x )
 ( [ g ] EI = x )
 ( [ g u ] EI "'" = g )
 ( [ g u ] EI = g )
 ( [ g - ] u EI "'" =  )
 ( [ g - ] u EI =  )
 ( [ g u ] a = w )
 ( [ g ] = g )
 ( [ h u e ] = u e )
 ( [ h i e ] = i e )
 ( [ h ] =  )
 ( [ j ] = x )
 ( [ k ] = k )
;; ( [ l l ] = dZ )
 ( [ l l ] = j )
 ( [ l ] = l )
 ( [ m ] = m )
 ( [ n  g ]  = N g ) ;; comment out this line to remove N sound - aec
 ( [ n ] = n )
 ( [ p ] = p )
 ( [ q u ] = k )
 ( [ r r ] = rr )
 ( # [ r ] = rr )
 ( LNS [ r ] = rr )
 ( [ r ] = r )
 ( # [ s t ] = e s t )
 ( [ s ] = s )
 ( [ t ] = t )
 ( [ w ] = u )
 ( [ x ] = k s )
;; ( [ x ] = x )
 ( [ y ] # = i1 )

;; ( [ y ] = dZ )

 ( [ y ] = j )

 ( [ z ] = s )
))

(provide 'cslu_span_mx_lts)



