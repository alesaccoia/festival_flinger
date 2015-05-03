/****************************<--OGI-->*************************************/
/*                                                                        */
/*             Center for Spoken Language Understanding                   */
/*        Oregon Graduate Institute of Science & Technology               */
/*                         Portland, OR USA                               */
/*                        Copyright (c) 2000                              */
/*                                                                        */
/*      This module is not part of the CSTR/University of Edinburgh       */
/*               release of the Festival TTS system.                      */
/*                                                                        */
/*  In addition to any conditions disclaimers below, please see the file  */
/*  "license_ogi_tts.txt" distributed with this software for information  */
/*  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  */
/*                                                                        */
/****************************<--OGI-->*************************************/
#ifndef __NUMERIC_H_
#define __NUMERIC_H_

#include <cfloat>
#include "matrix.h"
#include "stat.h"

#ifndef PI
#define PI    3.14159265358979
#endif
#define TWOPI 6.28318530717959


void	lu(const Matrix&, Matrix&, Matrix&, int&); // ludcmp(A, LU, P, d)
void	lubksb(const Matrix&, const Matrix&, const Matrix&, Matrix&); // lubksb(LU, P, b, x)

void	chol(const Matrix&, Matrix&); // choldc(A, CH)
void	cholsl(const Matrix&, const Matrix&, Matrix&); // cholsl(CH, b, x)

void	luinv(const Matrix&, const Matrix&, Matrix&); //luinv(LU, P, Ainv)
void	cholinv(const Matrix&, Matrix&); // cholinv(CH, Ainv)
void	inv(const Matrix&, Matrix&); // inv(A, Ainv) - smart function
void	invlu(const Matrix&, Matrix&);
void	invchol(const Matrix&, Matrix&); // invch(A, Ainv), where A.t()=A;

void	lusolve(const Matrix&, const Matrix&, const Matrix&, Matrix&); //luinv(LU, P, B, AinvB)
void	cholsolve(const Matrix&, const Matrix&, Matrix&); // cholinv(CH, B, AinvB)
void	solve(const Matrix&, const Matrix&, Matrix&); // solve(A, B, Ainv*B=x) - smart function
void	solvelu(const Matrix&, const Matrix&, Matrix&);
void	solvechol(const Matrix&, const Matrix&, Matrix&); // 

double	ludet(const Matrix&, int); //ludet(LU, d)
double	choldet(const Matrix&); //choldet(CH)
double	det(const Matrix&); // smart
double	detlu(const Matrix&); 
double	detchol(const Matrix&);

#endif // __NUMERIC_H_
