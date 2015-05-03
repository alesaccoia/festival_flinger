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

;;   Phone duration info for Klatt rules, for Worldbet phone set
;;   ** Modified from mrpa version by MWM  23-May-97
                              ;;  MRPA                         
(set! duration_klatt_params   ;;(set! duration_klatt_params    		      
'(
(pau    100.0   100.0)        ;;(#      100.0   100.0)       
; vowels/diphthongs 
(@      230.0   80.0)         ;;(a      230.0   80.0)        
(A      240.0   100.0)        ;;(aa     240.0   100.0)       
(&      120.0   60.0)         ;;(@      120.0   60.0)        
(aI     250.0   150.0)        ;;(ai     250.0   150.0)       
(aU     260.0   100.0)        ;;(au     240.0   100.0)       
(E      150.0   70.0)         ;;(e      150.0   70.0)        
(3r     180.0   80.0)         ;;(e@     270.0   130.0)       
(ei     190.0   100.0)        ;;(ei     180.0   100.0)       
(I      135.0   40.0)         ;;(i      135.0   40.0)        
(i:     155.0   55.0)         ;;(ii     155.0   55)          
(>      240.0   100.0)        ;;(o      240.0   130.0)       
(>i     280.0   150.0)        ;;(oi     280.0   150.0)       
(oU     220.0   80.0)         ;;(ou     220.0   80.0)        
(U      160.0   60.0)         ;;(u      210.0   70.0)        
(^      140.0   60.0)         ;;(uh     160.0   60.0)        
(u      210.0   70.0)         ;;(uu     230.0   150.0)       
(iU     280.0   150.0)        ;;
; sonorant consonants
(h      80.0    20.0)         ;;(h      80.0    20.0)        
(l      80.0    40.0)         ;;(l      80.0    40.0)        
(l=    260.0   110.0)         ;;(l      80.0    40.0)        
(w      80.0    60.0)         ;;(w      80.0    60.0)        
(j      80.0    40.0)         ;;(y      80.0    40.0)        
(9r     80.0    30.0)         ;;(r      80.0    30.0)        
; nasals
(m      70.0    60.0)         ;;(m      70.0    60.0)        
(n      60.0    50.0)         ;;(n      60.0    50.0)        
(N      95.0    60.0)         ;;(ng     95.0    60.0)        
; fricatives
(f      100.0   80.0)         ;;(f      100.0   80.0)        
(D      50.0    30.0)         ;;(dh     50.0    30.0)        
(s      105.0   60.0)         ;;(s      105.0   60.0)        
(S      105.0   80.0)         ;;(sh     105.0   80.0)        
(T      90.0    60.0)         ;;(th     90.0    60.0)        
(v      60.0    40.0)         ;;(v      60.0    40.0)        
(z      75.0    40.0)         ;;(z      75.0    40.0)        
(Z      70.0    40.0)         ;;(zh     70.0    40.0)        
; plosives
(b      85.0    60.0)         ;;(b      85.0    60.0)        
(d      75.0    50.0)         ;;(d      75.0    50.0)        
(g      80.0    60.0)         ;;(g      80.0    60.0)        
(k      80.0    60.0)         ;;(k      80.0    60.0)        
(p      90.0    50.0)         ;;(p      90.0    50.0)        
(t      75.0    50.0)         ;;(t      75.0    50.0)        
; affricates
(tS     70.0    50.0)         ;;(ch     70.0    50.0)        
(dZ     70.0    50.0)         ;;(jh     70.0    50.0)        
))
                            
                                                           
(provide 'ogi_klatt_durs_wb)      
