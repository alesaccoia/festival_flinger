(define (function_words_addenda)
"(function_words_addenda)
Redefine phonetic transcription of a number of function words to remove stress. Ideally, one would want to keep the stress but avoid these words being emphasized."
  (lex.add.entry '("a" dt (((&) 0))))
  (lex.add.entry '("about" nil (((&) 0)((b aU t) 0))))
  (lex.add.entry '("above" nil (((&) 0)((b ^ v) 0))))
  (lex.add.entry '("after" nil (((@ f) 0)((t 3r) 0))))
  (lex.add.entry '("ahead" nil (((&) 0)((h E d) 0))))
  (lex.add.entry '("although" nil (((> l) 0)((D oU) 0))))
  (lex.add.entry '("always" nil (((> l) 0)((w ei z) 0))))
  (lex.add.entry '("am" nil (((@ m) 0))))
  (lex.add.entry '("among" nil (((& m) 0)((^ N) 0))))
  (lex.add.entry '("an" nil (((@ n) 0))))
  (lex.add.entry '("and" nil (((@ n d) 0))))
  (lex.add.entry '("any" nil (((E) 0)((n i:) 0))))
  (lex.add.entry '("are" nil (((A 9r) 0))))
  (lex.add.entry '("around" nil (((3r) 0)((aU n d) 0))))
  (lex.add.entry '("as" nil (((@ z) 0))))
  (lex.add.entry '("at" nil (((@ t) 0))))
  (lex.add.entry '("be" nil (((b i:) 0))))
  (lex.add.entry '("because" nil (((b I) 0)((k > z) 0))))
  (lex.add.entry '("before" nil (((b I) 0)((f oU 9r) 0))))
  (lex.add.entry '("below" nil (((b I) 0)((l oU) 0))))
  (lex.add.entry '("between" nil (((b i:) 0)((t w i: n) 0))))
  (lex.add.entry '("beyond" nil (((b i:) 0)((A n d) 0))))
  (lex.add.entry '("both" nil (((b oU T) 0))))
  (lex.add.entry '("but" nil (((b ^ t) 0))))
  (lex.add.entry '("by" nil (((b aI) 0))))
  (lex.add.entry '("could" nil (((k U d) 0))))
  (lex.add.entry '("did" nil (((d I d) 0))))
  (lex.add.entry '("do" nil (((d u) 0))))
  (lex.add.entry '("does" nil (((d ^ z) 0))))
  (lex.add.entry '("down" nil (((d aU n) 0))))
  (lex.add.entry '("during" nil (((d U 9r) 0)((I N) 0))))
  (lex.add.entry '("either" nil (((i:) 0)((D 3r) 0))))
  (lex.add.entry '("for" nil (((f oU 9r) 0))))
  (lex.add.entry '("from" nil (((f 9r ^ m) 0))))
  (lex.add.entry '("had" nil (((h @ d) 0))))
  (lex.add.entry '("has" nil (((h @ z) 0))))
  (lex.add.entry '("have" nil (((h @ v) 0))))
  (lex.add.entry '("he" nil (((h i:) 0))))
  (lex.add.entry '("her" nil (((h 3r) 0))))
  (lex.add.entry '("him" nil (((h I m) 0))))
  (lex.add.entry '("his" nil (((h I z) 0))))
  (lex.add.entry '("how" nil (((h aU) 0))))
  (lex.add.entry '("i" nil (((aI) 0))))
  (lex.add.entry '("if" nil (((I f) 0))))
  (lex.add.entry '("in" nil (((I n) 0))))
  (lex.add.entry '("into" nil (((I n) 0)((t u) 0))))
  (lex.add.entry '("is" nil (((I z) 0))))
  (lex.add.entry '("it" nil (((I t) 0))))
  (lex.add.entry '("its" nil (((I t s) 0))))
  (lex.add.entry '("just" nil (((dZ ^ s t) 0))))
  (lex.add.entry '("me" nil (((m i:) 0))))
  (lex.add.entry '("my" nil (((m aI) 0))))
  (lex.add.entry '("near" nil (((n I 9r) 0))))
  (lex.add.entry '("never" nil (((n E) 0)((v 3r) 0))))
  (lex.add.entry '("not" nil (((n A t) 0))))
  (lex.add.entry '("of" nil (((^ v) 0))))
  (lex.add.entry '("on" nil (((A n) 0))))
  (lex.add.entry '("onto" nil (((A n) 0)((t u) 0))))
  (lex.add.entry '("or" nil (((oU 9r) 0))))
  (lex.add.entry '("our" nil (((aU) 0)((3r) 0))))
  (lex.add.entry '("out" nil (((aU t) 0))))
  (lex.add.entry '("over" nil (((oU) 0)((v 3r) 0))))
  (lex.add.entry '("rather" nil (((9r @) 0)((D 3r) 0))))
  (lex.add.entry '("shall" nil (((S & l) 0))))
  (lex.add.entry '("she" nil (((S i:) 0))))
  (lex.add.entry '("so" nil (((s oU) 0))))
  (lex.add.entry '("some" nil (((s ^ m) 0))))
  (lex.add.entry '("such" nil (((s ^ tS) 0))))
  (lex.add.entry '("than" nil (((D @ n) 0))))
  (lex.add.entry '("that" nil (((D @ t) 0))))
  (lex.add.entry '("the" det (((D &) 0))))
  (lex.add.entry '("there" nil (((D E 9r) 0))))
  (lex.add.entry '("their" nil (((D E 9r) 0))))
  (lex.add.entry '("them" nil (((D E m) 0))))
  (lex.add.entry '("they" nil (((D ei) 0))))
  (lex.add.entry '("through" nil (((T 9r u) 0))))
  (lex.add.entry '("to" nil (((t u) 0))))
  (lex.add.entry '("under" nil (((^ n) 0)((d 3r) 0))))
  (lex.add.entry '("up" nil (((^ p) 0))))
  (lex.add.entry '("us" nil (((^ s) 0))))
  (lex.add.entry '("was" nil (((w A z) 0))))
  (lex.add.entry '("we" nil (((w i:) 0))))
  (lex.add.entry '("were" nil (((w 3r) 0))))
  (lex.add.entry '("what" nil (((w ^ t) 0))))
  (lex.add.entry '("where" nil (((w E 9r) 0))))
  (lex.add.entry '("which" nil (((w I tS) 0))))
  (lex.add.entry '("who" nil (((h u) 0))))
  (lex.add.entry '("why" nil (((w aI) 0))))
  (lex.add.entry '("will" nil (((w I l) 0))))
  (lex.add.entry '("with" nil (((w I D) 0))))
  (lex.add.entry '("you" nil (((j u) 0))))
  (lex.add.entry '("your" nil (((j oU 9r) 0))))
)

(define (segdur_addenda)
  (lex.add.entry '("Styron" nil (((s t aI) 1) ((9r & n) 0))))
  (lex.add.entry '("Ho-Ja" nil (((h >) 1) ((dZ A) 0))))
  (lex.add.entry '("Seikaly" nil (((s ei) 0) ((k ^) 1) ((l i:) 0))))
  (lex.add.entry '("Bechera" nil (((b E) 1) ((tS E) 0) ((9r &) 0))))
  (lex.add.entry '("Houdge" nil (((h oU dZ) 1))))
  (lex.add.entry '("jian" nil (((z j A n) 1))))
)

(define (worldbet_addenda)
"(worldbet_addenda)
Add a whole host of various entries to the current lexicon with
worldbet phones."
  (lex.add.entry 
   '("Tedline" nil (((t E d) 2) ((l i: n) 1))))
  (lex.add.entry 
   '("read" riid (((9r i: d) 1))))
  (lex.add.entry 
   '("read" red (((9r E d) 1))))
  (lex.add.entry
   '("a" n (((ei) 1))))
  (lex.add.entry
   '("ae" n (((ei) 1))))
  (lex.add.entry
   '("a" dt (((&) 0))))
  (lex.add.entry
   '("'s" pos (((& z) 0))))
  (lex.add.entry
   '("'s" n (((& z) 0))))
  (lex.add.entry
   '("the" det (((D &) 0))))
  (lex.add.entry 
   '("voices" n (((v >i) 1) ((s E z) 0))))
  (lex.add.entry 
   '("caught" v (((k > t) 1))))
  (lex.add.entry
   '("UN" n ((( j u ) 0) ((E n) 1))))
  (lex.add.entry
   '("parametrically" j (((p E ) 2) ((9r &) 0) ((m E) 1) ((tS 9r I k) 0) ((l i:) 0))))
  (lex.add.entry
   '("graduate" j (((g 9r @) 1) ((dZ u) 0) ((E t) 0))))
  (lex.add.entry
   '("graduate" n (((g 9r @) 1) ((dZ u) 0) ((E t) 0))))
  (lex.add.entry
   '("graduate" v (((g 9r @) 1) ((dZ u) 0) ((ei t) 0))))
  (lex.add.entry
   '("articulators" n (((A 9r) 0) ((t I k) 1) ((j &) 0) ((l ei ) 2) ((t 3r z) 0))))
  (lex.add.entry
   '("Institute" n (((I n) 1) ((s t I) 0) ((t u t) 0))))
  (lex.add.entry
   '("Hosom" n (((h oU) 1) ((s ^ m) 0))))
  (lex.add.entry
   '("Macon" n (((m ei) 1) ((k ^ n) 0))))
  (lex.add.entry
   '("edges" n (((E dZ) 1) ((I z) 0))))
  (lex.add.entry
   '("Festival" n (((f E s) 1) ((t I) 0) ((v & l) 0))))
  (lex.add.entry
   '("University" n ((( j u ) 2) ((n ^) 0) ((v 3r) 1) ((s ^) 0) ((t i:) 0))))
;  (lex.add.entry 
;   '( "awb" n ((( ei ) 1) ((d ^) 1) ((b & l) 0) ((y u) 0) ((b i:) 1))))
  (lex.add.entry 
   '( "pault" n ((( p > l) 2) ((t i:) 1))))
  (lex.add.entry 
   '( "Taylor" n ((( t ei) 1) ((l 3r) 0))))
  (lex.add.entry 
   '( "cstr" n ((( s i: ) 1) (( E s ) 1) (( t i: ) 1) (( A 9r) 1)) ))
  (lex.add.entry 
   '( "edinburgh" n ((( E d ) 1) (( I n ) 0) ((b 9r &) 0))))
  (lex.add.entry 
   '( "tts" n ((( t i: ) 1) (( t i: ) 1) (( E s ) 1))))
  (lex.add.entry 
   '( "emacs" n ((( i: ) 1) (( m @ k s) 0))))
  (lex.add.entry 
   '( "mbrola" n (((E m) 1) ((b 9r oU ) 1) ((l & ) 0))))
  (lex.add.entry 
   '( "psola" n ((( p I ) 0) (( s oU  ) 1) (( l & ) 0 ))))
  (lex.add.entry 
   '( "tobi" n ((( t oU ) 1) (( b i: ) 0))))
  (lex.add.entry 
   '( "ulaw" n ((( m j u ) 1) (( l > ) 0))))
  (lex.add.entry 
   '( "Los" n ((( l > s) 1))))
  (lex.add.entry 
   '( "Angeles" n (((@ n) 1) ((dZ &) 0) ((l I s) 0))))
  (lex.add.entry 
   '( "Jr" n (((dZ u n) 1) ((j 3r) 0)) ((pos "K6%" "OA%"))))
  (lex.add.entry
   '("diphone" n (((d aI) 1) ((f oU n) 0))))
  (lex.add.entry
   '("diphones" n (((d aI) 1) ((f oU n s) 0))))
  (lex.add.entry 
   '("reuter" n (((9r >i) 1) ((t 3r) 0))))
  (lex.add.entry 
   '("reuters" n (((9r >i) 1) ((t 3r z) 0))))
  (lex.add.entry 
   '("alices" n (((@ l) 1) ((I s) 0) ((E z) 0))))
  (lex.add.entry
   '("cdrom" n (((s i:) 1) ((d i:) 1) ((9r A m) 1))))
  (lex.add.entry
   '("cdroms" n (((s i:) 1) ((d i:) 1) ((9r A m z) 1))))
  (lex.add.entry
   '("sony" n (((s oU) 1) ((n i:) 1))))
  (lex.add.entry
   '("ogi" n (((oU) 1) ((dZ i:) 1) ((aI) 1))))
  (lex.add.entry
   '("doin" v (((d u) 1) ((I n) 0))))
  (lex.add.entry
   '("prototyping" n (((p 9r oU) 1) ((t &) 0) ((t aI) 0) ((p I N) 0))))
  (lex.add.entry 
   '("ae" n (((ei) 1))))  ;; used in spelling 'a'
  (lex.add.entry 
   '("*" n (((@ s) 1) ((t &) 0) ((9r I s k) 0)) ((pos "K6%"))))
  (lex.add.entry 
   '("%" n (((p 3r) 0) ((s E n t) 1)) ((pos "K9%"))))
  (lex.add.entry 
   '("&" n (((@ n d) 1))))
  (lex.add.entry 
   '("$" n (((d A) 1) ((l 3r) 0))))
  (lex.add.entry 
   '("#" n (((h @ S) 1))))
  (lex.add.entry 
   '("@" n (((@ t) 1))))
  (lex.add.entry 
   '("+" n (((p l ^ s) 0)) ((pos "K7%" "OA%" "T-%"))))
  (lex.add.entry 
   '("^" n (((k E 9r) 1) ((E t) 0)) ((pos "K6$"))))
  (lex.add.entry 
   '("~" n (((t I l) 1) ((d &) 0)) ((pos "K6$"))))
  (lex.add.entry 
   '("=" n (((i:) 1) ((k w & l) 0))))
  (lex.add.entry 
   '("/" n (((s l @ S) 1))))
  (lex.add.entry 
   '("\\" n (((b @ k) 1) ((s l @ S) 1))))
  (lex.add.entry 
   '("_" n (((^ n) 1) ((d 3r) 0) ((s k > 9r) 1))))
  (lex.add.entry 
   '("|" n (((v 3r) 1) ((t I k) 0) ((& l) 0) ((b A 9r) 1))))
  (lex.add.entry 
   '(">" n ((( g 9r ei) 1) ((t 3r) 0) ((T @ n) 1))))
  (lex.add.entry 
   '("<" n ((( l E s) 1) ((T @ n) 1))))
  (lex.add.entry 
   '("[" n ((( l E f t) 1) ((b 9r @ k) 1) ((I t) 0))))
  (lex.add.entry 
   '("]" n ((( 9r aI t) 1) ((b 9r @ k) 1) ((I t) 0))))
  (lex.add.entry 
   '(" " n (((s p ei s) 1))))
  (lex.add.entry 
   '("\t" n (((t @ b) 1))))
  (lex.add.entry 
   '("\n" n (((n j u) 1) ((l aI n) 1))))
)
