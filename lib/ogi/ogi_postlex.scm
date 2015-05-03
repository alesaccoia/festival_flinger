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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;; must redefine Edinburgh's too to get Worldbet into list
;;
(set! OGI_postlex_vowel_reduce_table
  '((mrpa
     ((uh @) (i @) (a @) (e @) (u @) (o @) (oo @)))
;    (worldbet
;     ((^ &) (uh &)
;      ))
    (radio
     ((ah ax el en em) 
      (ih ax) 
      (ey ax) 
      (aa ax) 
      (ae ax) 
      (eh ax))))
"Mapping of vowels to their reduced form.  This in an assoc list of
phoneset name to an assoc list of full vowel to reduced form.")

;; map English phonemes to German for Festival to speak English with a german accent

(set! eng2ger_map
      '((worldbet
	 ((pau _)
	  (D d)
	  (T t)
	  (9r R)
	  (ei eI)
	  (>i OY)
	  )))
"Mapping of English worldbet phones to German equivalents for Festival to speak English with a German accent")

;;; This doesn't work yet.

(define (map_eng2ger utt)
  "(plosive_aspiration UTT)
Change to allophones of plosives"
  (mapcar
   (lambda (seg)
	   (set! newn (cdr (assoc_string (item.name seg) eng2ger_map)))
	   (cond
	    ((newn
	      (item.set_name s (car newn)))
	     (t
	      ;; as is
	      ))))
   (utt.relation.items utt 'Segment))
  utt)
;;;
;;;  Pronounce "the" with a long "E" before a word beginning with a vowel.
;;;     MWM  8-Jul-97
;;;
(define (The_Before_Vowel utt)
  "(The_Before_Vowel UTT)
Change \"the\" to \"thee\" before a word beginning with a vowel.."
  (mapcar
   (lambda (wrd)
     (if (string-matches (item.name wrd) "[Tt]he")
	  (if (string-equal (item.feat wrd "R:SylStructure.daughtern.daughtern.R:Segment.n.ph_vc") "+")
	       (item.set_name 
		(item.daughtern (item.relation.daughtern wrd "SylStructure"))
		(car (cdr (assoc (car (cdr (assoc 'name (PhoneSet.description)))) long_E_table)))
		))))
   (utt.relation.items utt "Word"))
  utt)

(set! long_E_table
      '((mrpa ii)
	(worldbet i:)
	(radio iy)))


;;
;; Hack to make "tr" clusters sound better -- this will be removed when we get aspirated t
;;
(define (t_to_ch_hack utt)
  "(t_to_ch_hack UTT)
Change syllable-initial \"t\" to \"ch\"-sound when followed by \"r\" to fake aspirated \"t\""
  (let 
      ((phoneset (car (cdr (assoc 'name (PhoneSet.description)))))
       (tsegment (car (car (cdr (assoc (car (cdr (assoc 'name (PhoneSet.description)))) t_to_ch_table))))))
    (mapcar
     (lambda (seg)
       (if (string-equal (item.feat seg 'name)  tsegment)
	   (if (string-equal (item.feat seg 'syl_initial ) "1")
	       (if (member_string (item.feat seg 'n.name) 
				 (car (cdr (assoc phoneset t_follow_table))))
		   (item.set_name seg (find_t_subs tsegment))))))
   (utt.relation.items utt 'Segment))
  utt))

(define (find_t_subs tseg)
"(find_t_subs tseg)
Find t hack substitute for the phoneset."
   (let ((subst (car (cdr (assoc (car (cdr (assoc 'name (PhoneSet.description))))
				   t_to_ch_table)))))
     (car (cdr subst))))

(set! t_to_ch_table
  '((mrpa (t ch))
    (worldbet (t tS))
    (radio (t ch))))

(set! t_follow_table  
  '((mrpa (r))
    (worldbet (9r))
    (radio (t))))

(defvar OGI_postlex_vowel_reduce_cart_tree_hand
  '((stress is 0)
    ((1))
    ((0)))
"OGI_postlex_vowel_reduce_cart_tree_hand
A hand-written CART tree for vowel reduction.")


;;;
;;;  Vowel reduction
;;;
(define (PostLex_Reduce utt)
  "(PostLex_Reduce UTT)
Apply reduction to utterance."
  (mapcar
   (lambda (syl)
     (let ((do_reduce (car (last (wagon syl postlex_vowel_reduce_cart_tree)))))
;     (let ((do_reduce (car (last (wagon syl OGI_postlex_vowel_reduce_cart_tree_hand)))))
       (if (and (string-equal do_reduce "1") 
			     (not (string-equal 
					      (item.feat syl "R:SylStructure.parent.R:Token.parent.no_postlex") "1")))
	   (vowel_reduce syl))))
   (utt.relation.items utt 'Syllable))
  utt)

(define (vowel_reduce syl)
"(vowel_reduce syl)
Reduce the vowel in syl to that defined in a reduction assoc list."
   (let ((vowel))
;     (print (string-append (item.name vowel) " ==> "
;	    (find_reduced_vowel (item.name vowel))))
		 (set! vowel (find_vowel syl))
     (if vowel (item.set_name
      vowel
      (find_reduced_vowel (intern (item.name vowel)))))))

(define (find_vowel syl)
"(find_vowel syl)
Returns the (first) daughter of syl (i.e., a Segment) that is a vowel."
  (let ((v))  ;; default value
    (mapcar
     (lambda (s) (begin 
     ;(format t "%s %s\n" (item.name s) (item.feat s 'ph_vc))
			 (if (string-equal (item.feat s 'ph_vc) "+")
	   (set! v s))))
     (item.relation.daughters syl 'SylStructure))
		 v))

(define (find_reduced_vowel vowel)
"(find_reduced_vowel vowel)
Find the reduced vowel corresponding to vowel.  If there isn't
one return vowel as is."
   (let ((reduces (car (cdr (assoc (car (cdr (assoc 'name (PhoneSet.description))))
				   OGI_postlex_vowel_reduce_table)))))
     (if (assoc vowel reduces)
	 (car (cdr (assoc vowel reduces)))
	 vowel)))

(define (OGI_PostLex_Reduce utt)
  "(OGI_PostLex_Reduce UTT)
Apply reduction to utterance."
  (mapcar
   (lambda (syl)
     (let ((do_reduce (car (last (wagon syl OGI_postlex_vowel_reduce_cart_tree_hand)))))
	   (vowel_reduce syl)))
   (utt.relation.items utt 'Syllable))
  utt)

;;
;; plosive aspiration
;;
(define (plosive_aspiration utt)
  "(plosive_aspiration UTT)
Change to allophones of plosives"
  (mapcar
   (lambda (seg)
     (cond
      ((string-matches (item.feat seg 'name)  "[ptk]") 
       (if (and (string-equal (item.feat seg 'ogi_onsetcoda ) "onset")
		(equal? (item.feat seg 'n.ph_vc) "+")
		    (not (and (string-equal (item.feat seg 'R:SylStructure.p.name) "s")
			      (string-equal (item.feat seg 'R:SylStructure.p.ogi_onsetcoda) "onset")))
		    (equal? (item.feat seg 'R:SylStructure.parent.stress) 1))
	   (item.set_feat seg 'allophone_name (string-append (item.feat seg 'name) "h")))
       ;; if it is followed by a liquid it's also aspirated
       (if (and (string-equal (item.feat seg 'ogi_onsetcoda ) "onset")
		(or (string-equal (item.feat seg 'R:SylStructure.n.name) "j")
		    (string-equal (item.feat seg 'R:SylStructure.n.name) "l"))
		(not (and (string-equal (item.feat seg 'R:SylStructure.p.name) "s")
			  (string-equal (item.feat seg 'R:SylStructure.p.ogi_onsetcoda) "onset")))
		(equal? (item.feat seg 'R:SylStructure.parent.stress) 1))
	   (item.set_feat seg 'allophone_name (string-append (item.feat seg 'name) "h"))))
      
      ))
   (utt.relation.items utt 'Segment))
  utt)

;;
;; flaps
;;
(define (flap_hack utt)
  "(flap_hack UTT)
Change t to dx when before an unstressed vowel"
  (mapcar
   (lambda (seg)
     (if  (string-equal (item.feat seg 'name)  "t")	 
	 (begin 
	   (if (and 
		;; prev is stressed vowel in same word
		(and 
		 (string-equal (item.feat (item.prev seg) 'ph_vc) "+")
		 (equal? (item.feat (item.prev seg) 'R:SylStructure.parent.stress) 1)
		 (in_same_word seg (item.prev seg))
		 )
		
		;; next is unstressed vowel in same word
		(and 
		 (string-equal (item.feat (item.next seg) 'ph_vc) "+")
		 (equal? (item.feat (item.next seg) 'R:SylStructure.parent.stress) 0)
		 (in_same_word seg (item.next seg))
		 )
		
		;; and not -/en/ like mitten, button, ...		
		(not 
		 (string-equal (item.feat seg 'nn.ph_ctype) "n")))
		
		(begin 
		  (item.set_feat seg 'allophone_name "dx"))
	       )))
     (if (string-equal (item.feat seg 'name) "dx")
	 (item.set_feat seg 'allophone_name "dx")))


   (utt.relation.items utt 'Segment))
  utt)

 
(define (delete_glottal_stop utt)
	(mapcar
		(lambda (seg)
			;; if current segment is "?"
			(if (string-equal (item.feat seg 'name) "?")
				(if (or (string-equal (item.feat (item.prev seg) 'ph_vc) "-")
						(eq? (item.feat seg "R:SylStructure.parent.parent.emphasis") 0))
						(item.delete seg)	
				)
			)		
		)
	(utt.relation.items utt 'Segment)
	)
	utt)
	

(define (in_same_word seg1 seg2)
  "(in_same_word SEG1 SEG2)
Return TRUE if these segments are in same word, NIL else."
  (if (and seg1 seg2)
      (if (equal?
	   (item.feat seg1 'R:SylStructure.parent.parent.id)
	   (item.feat seg2 'R:SylStructure.parent.parent.id))
	  t
	  nil)
      nil)
  )

(define (clean_up_token_string utt) 
  
)

(provide 'ogi_postlex)


