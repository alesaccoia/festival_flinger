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
;;;  Syllabification and accent prediction module for Mexican Spanish
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(set! A_Z 	'(a a1 e e1 i i1 o o1 u u1 b tS dZ d f g h j k l L m n N ny p q r rr s t w x ) )
(set! CONS 	'( b tS dZ d f g h j k l L m n N ny p q r rr s t w x ) )

(set! SET1	'( i u o b tS dZ d f g h j k l L m n N ny p q r rr s t w x ) )
(set! SET2	'( i u o b tS dZ d f g h j k l L m n N ny p q r rr s t w x ) )
(set! SET3	'( i u a b tS dZ d f g h j k l L m n N ny p q r rr s t w x ) )

(set! A_Z3 	'(b tS dZ d f g h j k l L m n N ny p q r rr s t x ) )


(set! VOWELS	'(a e i o u a1 e1 i1 o1 u1 w) )
(set! VOWELS1	'(a e i o u w) )
(set! STVOWELS	'(a1 e1 i1 o1 u1) )

(define(next_syl l lset)
  (set! tail l )
  (cond
    (( equal? l nil) nil)  
  
    ((not (member (car l) lset )) nil)

    ((member (car l) '(w))
      (append (cons (car l) nil) (next_syl (cdr l) A_Z3)))

    ((member (car l) '(i u))
      (append (cons (car l) nil) (next_syl (cdr l) A_Z)))

    ((member (car l) '(a a1))
      (append (cons (car l) nil) (next_syl (cdr l) SET1)))

    ((member (car l) '(e e1))
      (append (cons (car l) nil) (next_syl (cdr l) SET2)))

    ((member (car l) '(o o1))
      (append (cons (car l) nil) (next_syl (cdr l) SET3)))

    ((member (car l) '(i1 u1))
      (append (cons (car l) nil) (next_syl (cdr l) CONS)))

    ((member (car l) STVOWELS)
      (append (cons (car l) nil) (next_syl (cdr l) A_Z)))

    ((member (car l) VOWELS1)
      (append (cons (car l) nil) (next_syl (cdr l) A_Z1)))
      
    ((member (car l) '(c) )
      (append (cons (car l) nil) (next_syl (cdr l) '())))              
 
     ((member (car l) '(l) )
      (append (cons (car l) nil) (next_syl (cdr l) '(b  d f g k l p t))))    
      
    ((member (car l) '(m) )
      (append (cons (car l) nil) (next_syl (cdr l) '(m))))     
      
    ((member (car l) '(n) )
      (append (cons (car l) nil) (next_syl (cdr l) '(n))))               
               
    ((member (car l) '(r) )
      (append (cons (car l) nil) (next_syl (cdr l) '(b  d f g k p r t))))
        
    ((and (member (car l) '(s) ) (equal? syl_last 's))
      (append (cons (car l) nil) (next_syl (cdr l) '(b d k n s))))    
        
    ((member (car l) '(s) )
      (append (cons (car l) nil) (next_syl (cdr l) '(s))))       
          
    ((member (car l) '(t) )
      (append (cons (car l) nil) (next_syl (cdr l) '(t))))       
                     
    ((member (car l) '(z) )
      (append (cons (car l) nil) (next_syl (cdr l) '(z))))        
                       
    ((member (car l) '(b tS d f g j k J p q x ) )
      (append (cons (car l) nil) (next_syl (cdr l) '(b tS d f g j k J p q x ))))       

    (t (append (cons (car l) nil) (next_syl (cdr l) (cons (car l) nil) )))       
  )
)

(define(syl l )
  (set! syl_last (car l))
  (if
    (equal? l nil)  nil
    (append (cons (next_syl l (cons (car l ))) nil) (syl tail) )
))

(define (solve_CV l )
  (cond
    (( equal? l nil) nil)
    
    ((not (member (caar l) VOWELS ))
     ;; (cons (reverse(append (car l) (cadr l))) (solve_CV (cddr l)) ))
       (cons (append (car l) (cadr l)) (solve_CV (cddr l)) ))
   
    (t
     ;; (cons (reverse (car l)) (solve_CV (cdr l)) ))
      (cons (car l) (solve_CV (cdr l)) ))
  )
)

(define (is_accented_syl s)
  (cond
    ((equal? s nil) nil)
    
    ((member (car s) '(a1 e1 i1 o1 u1)) t)
    
    (t (is_accented_syl (cdr s)))
  )
)

(define (is_accented l)
  (cond
    ((equal? l nil) nil)    
    
    ((is_accented_syl (car l)) t)
    
    (t (is_accented (cdr l)))
  )
)

(define (normal s)
  (cond
    ((equal? s nil) nil)
    
     ((equal? 'a1 (car s)) (append '(a) (normal (cdr s))))
     ((equal? (car s) 'e1) (cons 'e (normal (cdr s))))
     ((equal? (car s) 'i1) (cons 'i (normal (cdr s))))
     ((equal? (car s) 'o1) (cons 'o (normal (cdr s))))
     ((equal? (car s) 'u1) (cons 'u (normal (cdr s))))
     (t (cons (car s) (normal (cdr s)))
    )
  )
)

(define (build_accented l)
  (cond
    ((equal? l nil) nil)    
    
    ((is_accented_syl (car l)) 
      (cons (list (reverse (normal (car l))) 1) (build_accented (cdr l)) )
    )
    (t
      (cons (list (reverse (car l)) 0) (build_accented (cdr l)) )
    )
  ) 
)

(define (set_stress l pos cp)
  (cond
    ((equal? l nil) nil)    
    
    ((equal? pos cp) 
      (cons (list (reverse (car l)) 1) (set_stress (cdr l) pos (+ cp 1)) )
    )
    (t
      (cons (list (reverse (car l)) 0) (set_stress (cdr l) pos (+ cp 1)) )
    )
  ) 
)


(define (solve_stress l )
  (cond
    ((is_accented l)
     (build_accented l))

    ((eq (length l) 1 )			;es una monos'ilaba
      (set_stress l 1 1))

    ((member (caar l) '(a e i o u n s))  ;es una palabra grave
      (set_stress l 2 1))
    
    (t
      (set_stress l 1 1))			;es una palabra aguda
  )
)

(define (mex.syllabify.phstress word)
  (set! drow (reverse word))
  (set! bob (reverse(solve_stress (solve_CV (syl drow )))))
;;  (print bob)
   bob
)

(provide 'span_mx_syl)
