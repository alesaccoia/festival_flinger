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
;;;
;;;  Various tokenizing functions for Mexican Spanish

(define (mx_spanish_token_to_words token name)
"(mx_spanish_token_to_words TOKEN NAME)
Returns a list of words for NAME from TOKEN in UTT.  This allows the
user to customize various non-local, multi-word, context dependent
translations of tokens into words.  If this function is unset 
only the builtin translation rules are used, if this is set the
builtin rules are not used unless explicitly called.
[see Token to word rules]"


;; Useful for devining funky characters
;; (set! FD (fopen "bob" "wb"))
;; (format FD "%s" name)
;; (fclose FD)


;; catch abbreviations

  (cond
  ((is_abbr name abbr)
    (explode_abbr name abbr)) 

;; rudimentary time 12 hour clock


((string-matches name "1:00")
 '("una"))  

((string-matches name "1[0-2]\\:00")  
   (mx_spanish_token_to_words token (string-before name ":"))
)

((string-matches name "[2-9]\\:00")  
   (mx_spanish_token_to_words token (string-before name ":"))
)

((string-matches name "1[0-2]\\:[0-5][0-9]")
 (append  
   (mx_spanish_token_to_words token (string-before name ":"))
   (list "y")
   (mx_spanish_token_to_words token (string-after name ":"))
 )
)

((string-matches name "[1-9]\\:[0-5][0-9]")
 (append  
   (mx_spanish_token_to_words token (string-before name ":"))
   (list "y")
   (mx_spanish_token_to_words token (string-after name ":"))
 )
)

((string-matches name "\\-?[0-9,]+[∫∞]")
  (append (mx_spanish_token_to_words token (string-before name "∫")) 
	    (mx_spanish_token_to_words token (string-before name "∞")) '("grados"))) 
((string-matches name "\\-?[0-9,]+[∫∞][Cc]")
  (append (mx_spanish_token_to_words token (string-before name "∫")) 
	    (mx_spanish_token_to_words token (string-before name "∞")) '("gradosCelsius")))

;; Percentages

((string-matches name "\\-?[0-9,]+[%]")
  (append (mx_spanish_token_to_words token (string-before name "%")) '("porciento")))

;; money

((string-matches name "\\$1")
    (list "un" "peso"))

((string-matches name "\\$1\\.00")
    (list "un" "peso" "y" "zero" "centavos"))

((string-matches name "\\$1\\.01")
    (list "un" "peso" "con" "un" "centavo"))

((string-matches name "\\$1\\.[0-9][0-9]")
    (append 
      (list "un" "peso" "y") 
      (mx_spanish_token_to_words token (string-after name "."))
	(list "centavos")) )

((string-matches name "\\$[0-9,]+\\.01")
    (append (mx_spanish_token_to_words token 
              (string-before (string-remove (string-after name "$") ",") ".")) 
           '("pesos" "con" "un" "centavo")))

((string-matches name "\\$[0-9,]+\\.[0-9][0-9]")
    (append (mx_spanish_token_to_words token 
              (string-before (string-remove (string-after name "$") ",") ".")) 
           '("pesos" "y")
            (mx_spanish_token_to_words token (string-after name "."))
           '("centavos")))

((string-matches name "\\$[0-9,]+")
    (append (mx_spanish_token_to_words token (string-remove (string-after name "$") ",")) 
           '("pesos")))


;; catch numbers and decimal points

  ((string-matches name ".*\\..*" )
    (append (mx_spanish_token_to_words token (string-before name "."))
	      '("punto")
            (mx_spanish_token_to_words token (string-after name "."))))

  
  ((string-matches name "[0-9,]+" )
    (spanish_number (string-remove name ",")))

 ((string-matches name "\\-[0-9,]+" )
   (append '("menos")
            (spanish_number (string-after (string-remove name ",") "-"))))

;; catch words and funky accents, tildes, etc.
 
  ((string-matches name ".*[Uu]\\:.*" )
    (list name))
 
  ((string-matches name ".*[âﬂçî–¬Õ‘ıíõ±“].*" )
    (list name))   

  ((string-matches name ".*[AEIOUaeiou]'.*" )
    (list name))   

  ((string-matches name ".*[Nn]\\~.*" )
    (list name))

  ((string-matches name ".*[ÒÌÈ·Û˙—…¸‹].*" )
    (list name))
 
  ((string-matches name "[AEIOUaeioubcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ]+" ) 
    (list name))

;; Funky punctuation

 ((string-matches name ".*[ø°].*" )
    (append (mx_spanish_token_to_words token (string-after name "°")) 
            (mx_spanish_token_to_words token (string-after name "ø"))  ))

  (t
    (list ""))))

;; punctuation

(defvar token.punctuation "\"'`.,:;!?(){}[]"
  "token.punctuation
A string of characters which are to be treated as punctuation when
tokenizing text.  Punctuation symbols will be removed from the text
of the token and made available through the \"punctuation\" feature.
[see Tokenizing]")
(defvar token.whitespace " \t\n\r"
  "token.whitespace
A string of characters which are to be treated as whitespace when
tokenizing text.  Whitespace is treated as a separator and removed
from the text of a token and made available through the \"whitespace\"
feature.  [see Tokenizing]")


;; Used in tokenizing money

(define (string-remove string1 string2)
"(string-remove STRING1 STRING2)
Remove all instances of STRING2 in STRING1 and return the result."
    (begin
       (while (and (not (equal? string1 string2))
                        (or (not (equal? (string-before string1 string2) ""))
                            (not (equal? (string-after  string1 string2) ""))
                        )
               )
              (let (before after)
                  (set! before (string-before string1 string2))
                  (set! after  (string-after string1 string2))
    			(set! string1 (string-append before after))
              )
        )
	  (if (equal? string1 string2) "" string1)
    )
)


;; Tokenize numbers

(define (spanish_number name)
"(spanish_number name)
Convert a string of digits into a list of words saying the number."
  (spanish_number_from_digits (symbolexplode name)))

(define (just_zeros digits)
"(just_zeros digits)
If this only contains 0s then we just do something different."
 (cond
  ((not digits) t)
  ((string-equal "0" (car digits))
   (just_zeros (cdr digits)))
  (t nil)))

(define (spanish_number_from_digits digits)
"(spanish_number_from_digits digits)
Takes a list of digits and converts it to a list of words
saying the number."
  (let ((l (length digits)))
   (cond
    ((equal? l 0)
     (list "cero"))
    ((string-equal (car digits) "0")
     (spanish_number_from_digits (cdr digits)))
    ((equal? l 1)  ;; single digit
     (cond 
      ((string-equal (car digits) "0") (list "cero"))
      ((string-equal (car digits) "1") (list "uno"))
      ((string-equal (car digits) "2") (list "do's"))
      ((string-equal (car digits) "3") (list "tres"))
      ((string-equal (car digits) "4") (list "cuatro"))
      ((string-equal (car digits) "5") (list "cinco"))
      ((string-equal (car digits) "6") (list "sei's"))
      ((string-equal (car digits) "7") (list "siete"))
      ((string-equal (car digits) "8") (list "ocho"))
      ((string-equal (car digits) "9") (list "nueve"))
      ;; fill in the rest
      (t (list "etc"))))
    ((equal? l 2)  ;; less than 100
     (if (string-equal (car digits) "1")  ;; < 20
	 (cond
	  ((string-equal (car (cdr digits)) "0") (list "diez"))
	  ((string-equal (car (cdr digits)) "1") (list "once"))
	  ((string-equal (car (cdr digits)) "2") (list "doce"))
	  ((string-equal (car (cdr digits)) "3") (list "trece"))
	  ((string-equal (car (cdr digits)) "4") (list "catorce"))
	  ((string-equal (car (cdr digits)) "5") (list "quince"))
	  ((string-equal (car (cdr digits)) "6") (list "dieciseis"))
	  ((string-equal (car (cdr digits)) "7") (list "diecisiete"))
	  ((string-equal (car (cdr digits)) "8") (list "dieciocho"))
	  ((string-equal (car (cdr digits)) "9") (list "diecinueve"))
	  ;; ...
	  (t (list "umpteen")))   ;; shouldn't get here
	 (cond ;; > 20 < 100
	  ((string-equal (car digits) "2") 
        (if (not (just_zeros (cdr digits)))
 	    (cons "veinti" (spanish_number_from_digits (cdr digits)))
 	    (list "veinte" )))
	  ((string-equal (car digits) "3") 
        (if (not (just_zeros (cdr digits)))
 	    (cons "treintai" (spanish_number_from_digits (cdr digits)))
 	    (list "treinta" )))
	  ((string-equal (car digits) "4") 
        (if (not (just_zeros (cdr digits)))
 	    (cons "cuarentai" (spanish_number_from_digits (cdr digits)))
 	    (list "cuarenta" )))
	  ((string-equal (car digits) "5") 
        (if (not (just_zeros (cdr digits)))
 	    (cons "cincuentai" (spanish_number_from_digits (cdr digits)))
 	    (list "cincuenta" )))
	  ((string-equal (car digits) "6") 
        (if (not (just_zeros (cdr digits)))
 	    (cons "sesentai" (spanish_number_from_digits (cdr digits)))
 	    (list "sesenta" )))
	  ((string-equal (car digits) "7") 
        (if (not (just_zeros (cdr digits)))
 	    (cons "setentai" (spanish_number_from_digits (cdr digits)))
 	    (list "setenta" )))
	  ((string-equal (car digits) "8") 
        (if (not (just_zeros (cdr digits)))
 	    (cons "ochentai" (spanish_number_from_digits (cdr digits)))
 	    (list "ochenta" )))
	  ((string-equal (car digits) "9") 
        (if (not (just_zeros (cdr digits)))
 	    (cons "noventai" (spanish_number_from_digits (cdr digits)))
 	    (list "noventa" )))
	  (t
	   (cons "umpty" (spanish_number_from_digits (cdr digits)))))))

    ((equal? l 3) ;; in the hundreds
      (cond  ; from 100 to 999
	  ((string-equal (car digits) "1") 
          (if (not (just_zeros (cdr digits)))
            (cons "ciento" (spanish_number_from_digits (cdr digits)))
 	      (list "cien" )))
	  ((string-equal (car digits) "2") 
          (if (not (just_zeros (cdr digits)))
            (cons "dosientos" (spanish_number_from_digits (cdr digits)))
 	      (list "dosientos" )))
	  ((string-equal (car digits) "3") 
          (if (not (just_zeros (cdr digits)))
            (cons "tresientos" (spanish_number_from_digits (cdr digits)))
 	      (list "tresientos" )))
	  ((string-equal (car digits) "4") 
          (if (not (just_zeros (cdr digits)))
            (cons "cuatrosientos" (spanish_number_from_digits (cdr digits)))
 	      (list "cuatrosientos" )))
	  ((string-equal (car digits) "5") 
          (if (not (just_zeros (cdr digits)))
            (cons "quinientos" (spanish_number_from_digits (cdr digits)))
 	      (list "quinientos" )))
	  ((string-equal (car digits) "6") 
          (if (not (just_zeros (cdr digits)))
            (cons "seisientos" (spanish_number_from_digits (cdr digits)))
 	      (list "seisientos" )))
	  ((string-equal (car digits) "7") 
          (if (not (just_zeros (cdr digits)))
            (cons "setesientos" (spanish_number_from_digits (cdr digits)))
 	      (list "setesientos" )))
	  ((string-equal (car digits) "8") 
          (if (not (just_zeros (cdr digits)))
            (cons "ochosientos" (spanish_number_from_digits (cdr digits)))
 	      (list "ochosientos" )))
	  ((string-equal (car digits) "9") 
          (if (not (just_zeros (cdr digits)))
            (cons "novesientos" (spanish_number_from_digits (cdr digits)))
 	      (list "novesientos" )))))


    ((< l 7)
     (set! thousands (reverse (cdr (cdr (cdr (reverse digits))))))
     (let ((sub_thousands 
	    (list 
	     (car (cdr (cdr (reverse digits))))
	     (car (cdr (reverse digits)))
	     (car (reverse digits)))))

      (append
      (if (not (and (eq (length thousands) 1 ) 
                    (string-equal "1" (car thousands)))) 
	(spanish_number_from_digits thousands))
	(list "mil")
	(if (not (just_zeros sub_thousands))
	  (spanish_number_from_digits sub_thousands)
	  nil))))

    ((< l 10)
     (set! millions (reverse (cdr (cdr (cdr (cdr (cdr (cdr (reverse digits)))))))))
     (set! sub_millions 
	    (list 
	     (car (cdr (cdr (cdr (cdr (cdr (reverse digits)))))))
	     (car (cdr (cdr (cdr (cdr (reverse digits))))))
	     (car (cdr (cdr (cdr (reverse digits)))))	     
           (car (cdr (cdr (reverse digits))))
	     (car (cdr (reverse digits)))
	     (car (reverse digits))))

      (append
      (if (and (eq (length millions) 1 )(string-equal "1" (car millions)))
        (list "unmillo'n")
        (append (spanish_number_from_digits millions) 
                (list "millones")))
	(if (not (just_zeros sub_millions))
	  (spanish_number_from_digits sub_millions)
	  nil)))


((< l 19)
     (set! billions (reverse (cdr (cdr (cdr (cdr (cdr (cdr (cdr (cdr (cdr (reverse digits))))))))))))
     (set! sub_billions 
	    (list 
	     (car (cdr (cdr (cdr (cdr (cdr (cdr (cdr (cdr (reverse digits))))))))))
           (car (cdr (cdr (cdr (cdr (cdr (cdr (cdr (reverse digits)))))))))
           (car (cdr (cdr (cdr (cdr (cdr (cdr (reverse digits))))))))
	     (car (cdr (cdr (cdr (cdr (cdr (reverse digits)))))))
	     (car (cdr (cdr (cdr (cdr (reverse digits))))))
	     (car (cdr (cdr (cdr (reverse digits)))))	     
           (car (cdr (cdr (reverse digits))))
	     (car (cdr (reverse digits)))
	     (car (reverse digits))))
      (append
      (if (and (eq (length billions) 1 )(string-equal "1" (car billions)))
        (list "unbillo'n")
        (append (spanish_number_from_digits billions) 
                (list "billones")))
	(if (not (just_zeros sub_billions))
	  (spanish_number_from_digits sub_billions)
	  nil)))

    (t
     (list "nu'meromuygrande")))))

(define (is_abbr name ablist)
  (if (eq nil ablist) nil
    (if (string-matches name ( caar ablist ))
      t
      (is_abbr name (cdr ablist)))))

(define (explode_abbr name ablist)
  (if (eq nil ablist) (list "abreviatura")
    (if (string-matches name ( caar ablist ))
      (cdar ablist)
      (explode_abbr name (cdr ablist)))))

(set! abbr '(	
	("Dr"			"doctor")
	("Dra"		"doctora")
      ("AA"			"autores")
      ("Abr"		"abril")
      ("[Aa]cept"       "aceptacio'n")
      ("[Aa]cr" 		"acreedor")
      ("[Aa]\\.[Cc]"	"antesdecristo")
      ("[Aa]dj"         "adjetivo")
      ("[Aa]\\.[Mm]"    "aeme")
	("[Aa][Mm]"       "aeme")
	("[Pp][Mm]"       "peeme")
	("[Aa]dm"		"administrador")
      ("[Aa]dmon"		"administracio'n")
	("[Aa]dmÛn"		"administracio'n")
	("[Aa]dmo'n"	"administracio'n")
 	("[Aa]erop"		"aeropuerto")
	("afmo"		"afecti'simo")
	("Ago"		"agosto")
	("[Aa]gr"		"agricultura")
	("Ags"		"aguascalientes")
	("[Aa]lt"		"altura")
	("Am[eÈ]r"		"ame'rica")
	("amer"		"americano")
	("[Aa]pdo"		"apartado")
	("ant"		"antiguo")
	("Antr[Ûo]p"	"antropo'logo")
	("[Aa]pend"		"ape'ndice")
	("[Aa]prox"		"aproximadamente")
      ("[∫∞][Cc]"		"gradoscelsius")
	("[∫∞]"		"grados")
      ("[Pp]\\.[Mm]"	"peeme")
	("[Ss]r"		"sen~or") 
	("[Ss]res"		"sen~ores") 
	("[Ss]ra"		"sen~ora")
	("[Ss]rta"		"sen~orita")
	("[Ss]rita"		"sen~orita") 
	("[Cc]d"		"ciudad") 
	("[Ee]do"		"estado") 
	("[Pp]\\.[Dd]"	"postdata") 
	("[Uu]d"		"usted") 
	("[Uu]ds"		"ustedes") 
	("[Aa]tte"		"atentamente") 
	("atta"		"atenta")
	("[Aa]ux"		"auxiliar")  
	("Av"			"avenida")
	("C"			"ciudadano") 
	("Cap"		"capitan")
	("Coop"		"cooperativa") 
	("ej"			"ejemplo") 
	("p\\.e"		"porejemplo") 
	("[Cc][iÌ]a"	"compan~ça")
	("[Cc]\\.[Pp]"	"codigopostal")
	("C\\.V"		"capitalvariable")
	("[Dd][Ff]"		"distritofederal") 
	("[Dd]\\.[Ff]"	"distritofederal")
	("D"			"don") 
	("Da"			"don~a")
	("[Dd]\\.[Cc]"	"despuesdecristo")
	("Depto"		"departamento")
   	("Dir"		"director")
	("Distr"		"distrito")
	("distrib"		"distributivo")
	("Depto"		"departamento")
	("D\\.N"		"defensanacional")
	("[Dd]octo"		"documento")
	("Edic"		"edicio'n")
	("Edit"		"editorial")
	("E.M"		"estadomayor")
	("Esc"		"escuela")
	("etc"		"etce'tera")
	("E\\.U\\.A"	"estadosunidosdeame'rica")
	("Excia"		"excelencia")
	("export"		"exportacio'n")
	("fam"		"familia")
	("fem"		"femenino")
	("F[Ìi]s"		"fi'sica")
	("f[Ìi]s"		"fi'sico")
	("fot"		"fotografi'a")
	("frec"		"frecuente")
	("[Ff]und"		"fundacio'n")
	("Gob"		"gobierno")
	("Gobr"		"gobernador")
	("[Gg]ral"		"general")
	("gralte"		"generalmente")
	("Gte"		"gerente")
	("hab"		"habitante")
	("habs"		"habitantes")
	("Hist"		"historia")
	("hist"		"histo'rico")
	("Ing"		"ingeniero")
	("Inst"		"instituto")
	("kg"			"kilogramo")
	("km"			"kilo'metro")
	("Lic"		"licenciado")
	("long"		"longitud")
	("masc"		"masculino")
	("m·x"		"ma'ximo")
	("MÈx"		"me'xico")
	("mex"		"mexicano")
	("Mex"		"mexicana")
	("M\\.N"		"monedanacional")
	("Mtro"		"maestro")
	("Ntra"		"nuestra")
	("Ntro"		"nuestro")
	("Nal"		"nacional")
	("Nos"		"nosotros")
	("N\\.S"		"nuestrosen~or")
	("N˙m"		"nu'mero")
	("[Oo]bserv"	"observacio'n")
	("[Pp]ral"		"principal")
	("[Pp]rales"	"principales")
	("[Pp]ralm"		"principalmente")
	("Pres"		"presidencia")
	("pres"		"presente")
	("Proc"		"procurador")
	("Profr"		"profesor")
	("Profra"		"profesora")
	("pr[oÛ]x"		"pro'ximo")
	("Psic"		"psicolcslu'a")
	("psic"		"psicolo'gico")
	("Psiq"		"psiquiatri'a")
	("psiq"		"psiquiatra")
	("Pte"		"presidente")
	("Pue"		"puebla")
	("pza"		"pieza")
	("QuÌm"		"quçmica")
	("quÌm"		"quçmico")
	("Rbo"		"recibo")
	("Rep"		"repu'blica")
	("R\\.I\\.P"	"requiescatinpace")
	("Renac"		"renacimiento")
	("Rev"		"revolucio'n")
	("Romant"		"romanticismo")
	("S\\.A"		"sociedadano'nima")
	("SA"			"sociedadano'nima")
	("S\\.C\\.L"	"sociedadcooperativalimitada")
	("Sen"		"senador")
	("seud"		"seudo'nimo")
	("sgte"		"siguiente")
	("[Ss]ist"		"sistema")
	("Coop"		"cooperativa")
	("S\\.N"		"servicionacional")
	("S\\.M\\.N"	"serviciomilitarnacional")
	("s/n"		"sinnu'mero")
	("soc"		"sociedad")
	("SrÌa"		"secretari'a")
	("Sria"		"secretaria")
	("Srio"		"secretario")
	("Sta"		"santa")
	("Sto"		"santo")
	("[Ss]uc"		"sucursal")
	("[Tt]el"		"tele'fono")
	("[Tt]emp"		"temperatura")
	("[Tt]errit"	"territorio")
	("[Tt]ext"		"textil")
	("[Tt]Ìt"		"ti'tulo")
	("ton"		"toneladas")
	("tons"		"toneladas")
	("[Tt]rad"		"traduccio'n")
	("Tte"		"teniente")
	("TV"			"televisio'n")
	("Univ"		"universidad")
	("Vda"		"viuda")
	("Vdo"		"viudo")
	("Vicepres"		"vicepresidente")
	("Vo"			"visto")
	("Bo"			"bueno")
	("Vo\\.Bo"		"vistobueno")
	("vol"		"volumen")
	("vols"		"volu'menes")
	("vulg"		"vulgar")
	("yac"		"yacimiento")
	("Zool"		"zoolcslu'a")
	("zool"		"zoolo'gico")
	("Ene"		"enero")
	("Feb"		"febrero")
	("Mar"		"marzo")
	("May"		"mayo")
	("Jun"		"junio")
	("Jul"		"julio")
	("Sep"		"septiembre")
	("Oct"		"octubre")
	("Nov"		"noviembre")
	("Dic"		"diciembre")
	("AC"			"asociacio'ncivil")
	("ALALC"		"asociacio'nlatinoamericanadelibrecomercio")
	("ALPRO"		"alianzaparaelprogreso")
	("AMA"		"asociacio'nmexicanaautomovili'stica")
	("BBC"		"bebese")
	("CBETIS"		"sebetis")
	("CCH"		"colegiodecienciasyhumanidades")
	("CFE"		"comisio'nfederaldeelectricidad")
	("CFI"		"corporacio'nfinancierainternacional")
	("CGT"		"confederacio'ngeneraldeltrabajo")
	("CNC"		"confederacio'nnacionalcampesina")
	("CNOP"		"confederacio'nnacionaldeorganizacionespopulares")
	("CNP"		"consejonacionaldelapublicidad")
	("CNT"		"confederacio'nnacionaldeltrabajo")
	("CNTE"		"consejonacionalte'cnicodelaeducacio'n")
	("CONACYT"		"conasit")
	("CONCAMIN"		"concami'n")
	("CTIS"		"centrotecnolo'gicoindustrialydeservicios")
	("CTM"		"confederacio'ndetrabajadoresdeme'xico")
	("CU"			"ciudaduniversitaria")
	("DDF"		"departamentodeldistritofederal")
	("EUA"		"estadosunidosdeame'rica")
	("FBI"		"efebeii'")
	("FOVISSSTE"	"foviste")
	("GE"			"gobernadordeestado")
	("IBM"		"ibee'me")
	("ISSSTE"		"iste")
	("LN"			"loteri'anacional")
	("MCC"		"mercadocumu'ncentroamericano")
	("MCE"		"mercadocumu'neuropeo")
	("PCM"		"partidocomunistamexicano")
	("PDM"		"partidodemo'cratamexicano")
	("PVE"		"partidoverdeecolcslusta")
	("PRD"		"peerrede")
	("PGJDF"		"procuraduri'ageneraldejusticiadeldistritofederal")
	("PGR"		"procuraduri'ageneraldelarepu'blica")
	("RTC"		"direccio'ngeneralderadiotelevisio'ni'cinematografi'a")
	("SCFI"		"secofi")
	("SCT"		"secretari'adecomunicacionesytransportes")
	("SDN"		"secretari'adeladefensanacional")
	("SG"			"secretari'adegobernacio'n")
	("SHCP"		"secretari'adehaciendai'cre'ditopu'blico")
	("SM"			"secretari'ademarina")
	("SME"		"sistemamonetarioeuropeo")
	("SMI"		"sistemamonetariointernacional")
	("SNTE"		"sindicatonacionaldetrabajadoresdelaeducacio'n")
	("SP"			"secretari'adepesca")
	("SPP"		"secretari'adeprogramacio'nypresupuesto")
	("SRA"		"secretari'adereformaagraria")
	("SRE"		"secretari'aderelacionesexteriores")
	("SSA"		"secretari'adesalubridadi'asistencia")
	("ST"			"secretari'adeturismo")
	("STC"		"secretari'adetransportecoectivo")
	("STPS"		"secretari'adeltrabajoi'previsio'nsocial")
	("UABC"		"universidadauto'nomadebajacalifornia")
	("UACJ"		"universidadauto'nomadeciudadjua'rez")	
	("UPN"		"universidadpedago'gicanacional")
	("[Bb]"			"be") 
	("[Cc]"			"se") 
	("CH"				"che") 
	("ch"				"che") 
	("[Dd]"			"de") 
	("[Ff]"			"efe") 
	("[Gg]"			"je") 
	("[Hh]"			"ache") 
	("[Jj]"			"jota") 
	("[Kk]"			"ka") 
	("LL"				"elle") 
	("ll"				"elle") 
	("[Ll]"			"ele") 
	("[Mm]"			"eme") 
	("[Nn]"			"ene") 
	("NY"				"en~e") 
	("ny"				"en~e")
      ("n~"				"en~e")
	("Ò"				"en~e") 
	("[Pp]"			"pe") 
	("[Qq]"			"ku") 
	("RR"				"erre") 
	("rr"				"erre") 
	("[Rr]"			"ere") 
	("[Ss]"			"ese") 
	("[Tt]"			"te") 
	("[Vv]"			"be") 
	("[Ww]"			"doblebe") 
	("[Xx]"			"ekis") 
	("[Yy]"			"i") 
	("[Zz]"			"zeta")
))


(provide 'cslu_span_mx_token)
