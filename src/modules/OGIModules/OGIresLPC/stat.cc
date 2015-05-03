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
#include "stat.h"

void normpdf(const Matrix &X, const Matrix &M, const Matrix &S, Matrix &PX) {
	int n=X.Nrows(), N=X.Ncols();
#ifdef RESIZE
	PX.resize(N);
#endif
	ASSURE(M.issize(n),"normpdf() M incorrect size");
	ASSURE(S.issize(n,n),"normpdf() S incorrect size");
	ASSURE(PX.issize(N),"normpdf() return Matrix PX incorrect size");

	static Matrix Sinv, CH, XM;
	Sinv.resize(n,n); CH.resize(n,n); XM.resize(n);

	chol(S, CH);

	//Determinant
	double Sdet=choldet(CH);
	//Inverse
	cholinv(CH,Sinv);

	//divide by this
	double p=pow(TWOPI,n/2.0) * pow(Sdet,0.5);

	double D;
	for(int j=0;j<N;j++) {
		//XM=(x(:,j)-M)
		for(int i=0;i<n;i++)
			XM(i)=X(i,j)-M(i); 

		D=0;
		//Inverse and Multiply, sum diagonal elements
		for(int r=0;r<n;r++) {
			for(int c=0;c<n;c++)
				D+=Sinv(r,c)*XM(r)*XM(c);
		}
		PX(j)=exp(D*(-.5))/p;
	}
}

void mean(const Matrix &X, Matrix &R, int dim) {
	ASSURE((dim==0) || (dim==1),"mean(), dim must be zero or one");

	sum(X,R,dim);
	if(dim==1)
		R/=X.Ncols();
	else
		R/=X.Nrows();
}
