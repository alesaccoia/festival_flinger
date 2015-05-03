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
#include "numeric.h"


void lu(const Matrix &A, Matrix &LU, Matrix &P, int &d) {
	int n=A.Nrows();
#ifdef RESIZE
	LU.resize(n,n);
	P.resize(n);
#endif
	ASSURE(A.issquare(),"lu(), Matrix A must be square");
	ASSURE(LU.issize(A.Nrows(),A.Ncols()),"lu() return Matrix LU size mismatch");
	ASSURE(P.issize(A.Nrows(),1),"lu() return Matrix P size mismatch");

	static Matrix vv;
	vv.resize(A.Nrows());

	if(&A!=&LU)
		copy(A,LU);
	
	int i, imax=0, j, k;
	double big, dum, sum, temp;
	d=1;
	for(i=0;i<n;i++) {
		big=0.;
		for(j=0;j<n;j++) {
			if ((temp=fabs(LU(i,j)))>big)
				big=temp;
		}
		if (big==0.) {
			EXPECT(0,"matrix.ludcmp(), matrix is singular");
			big=DBL_MIN;
		}
		vv(i)=(1./big);
	}
	for(j=0;j<n;j++) {
		for(i=0;i<j;i++) {
			sum=LU(i,j);
			for(k=0;k<i;k++)
				sum-=LU(i,k)*LU(k,j);
			LU(i,j)=sum;
		}
		big=0.;
		for(i=j;i<n;i++) {
			sum=LU(i,j);
			for(k=0;k<j;k++)
				sum-=LU(i,k)*LU(k,j);
			LU(i,j)=sum;
			if((dum=vv(i)*fabs(sum))>=big) {
				big=dum;
				imax=i;
			}
		}
		if(j!=imax) {
			for(k=0;k<n;k++) {
				dum=LU(imax,k);
				LU(imax,k)=LU(j,k);
				LU(j,k)=dum;
			}
			d=-d;
			vv(imax)=vv(j);
		}
		P(j)=imax;
		if(LU(j,j)==0)
			LU(j,j)=DBL_EPSILON;
		if(j!=n) {
			dum=1./LU(j,j);
			for(i=j+1;i<n;i++)
				LU(i,j)*=dum;
		}
	}
}

void lubksb(const Matrix &LU, const Matrix &P, const Matrix &b, Matrix &x) {

  int n;
  n=LU.Nrows();
#ifdef RESIZE
	x.resize(n);
#endif
	ASSURE(P.issize(n),"lubksb() input Matrix P size mismatch");
	ASSURE(b.issize(n),"lubksb() input Matrix b size mismatch");
	ASSURE(x.issize(n),"lubksb() return Matrix x size mismatch");
	
	if(&b!=&x)
		copy(b,x);

	int i, ii=0, j, ip;
	double sum;
	for(i=0;i<LU.Nrows();i++) {
		ip=(int)P(i);
		sum=x(ip);
		x(ip)=x(i);
		if(ii) {
			for(j=ii-1;j<=i-1;j++)
				sum-=LU(i,j)*x(j);
		}
		else {
			if(sum)
				ii=i+1;
		}
		x(i)=sum;
	}
	for(i=LU.Nrows()-1;i>=0;i--) {
		sum=x(i);
		for(j=i+1;j<LU.Nrows();j++)
			sum-=LU(i,j)*x(j);
		x(i)=sum/LU(i,i);
	}
}




void chol(const Matrix& A, Matrix &CH) {
	int n=A.Nrows();
#ifdef RESIZE
	CH.resize(n,n);
#endif
	ASSURE(A.issquare(),"chol(), matrix is not square");
	//EXPECT(A.issymmetric(),"chol(), matrix is not symmetric");
	ASSURE(CH.issize(n,n),"chol(), return size mismatch");

	if(&A!=&CH)
		copy(A,CH);

	double sum;
	for(int i=0;i<n;i++) {
		for(int j=i;j<n;j++) {
			sum=CH(i,j);
			for(int k=i-1;k>=0;k--)
				sum -= CH(k, i)*CH(k, j);
			if(i==j) {
				if(sum<=0) {
					EXPECT(0,"choldc(), matrix must be positive definite, some eigenvalues are zero or less. Result will be inaccurate");
					sum=DBL_EPSILON;
				}
				CH(i,i)=sqrt(sum);
			}
			else {
				CH(i,j)=sum/CH(i,i);
				CH(j,i)=0.;
			}
		}
	}
}

void cholsl(const Matrix &CH, const Matrix &b, Matrix &x) { // cholsl(C, b, x)
	int n=CH.Nrows();
#ifdef RESIZE
	x.resize(n);
#endif
	ASSURE(b.issize(n),"cholsl() input Matrix b size mismatch");
	ASSURE(x.issize(n),"cholsl() return Matrix x size mismatch");

	// optimize this!!!
	int i,k;
	double sum;
	for(i=0;i<n;i++) {
		sum=b(i);
		for(k=i-1;k>=0;k--)
			sum -= CH(k,i)*x(k);
		x(i)=sum/CH(i,i);
	}
	for(i=n-1;i>=0;i--) {
		sum=x(i);
		for(k=i+1;k<n;k++)
			sum -= CH(i,k)*x(k);
		x(i)=sum/CH(i,i);
	}
}




void luinv(const Matrix &LU, const Matrix &P, Matrix &Ainv) {
	int n=LU.Ncols();
#ifdef RESIZE
	Ainv.resize(n,n);
#endif
	ASSURE(Ainv.issize(n,n),"luinv(), output matrix must be same size");	

	static Matrix C;
	C.resize(n);

	for(int c=0;c<n;c++) {
		C=0.;
		C(c)=1.;
		lubksb(LU, P, C, C);
		putcolumn(C,c,Ainv);
	}
}

void cholinv(const Matrix &CH, Matrix &Ainv) {
	int n=CH.Ncols();
#ifdef RESIZE
	Ainv.resize(n,n);
#endif
	ASSURE(Ainv.issize(n,n),"cholinv(), output matrix must be same size");	

	static Matrix C;
	C.resize(n);

	for(int c=0;c<n;c++) {
		C=0.;
		C(c)=1.;
		cholsl(CH, C, C);
		putcolumn(C,c,Ainv);
	}
}




void inv(const Matrix &A, Matrix &Ainv) {
	if(A.issymmetric())
		invchol(A, Ainv);
	else
		invlu(A, Ainv);
}
	
void invlu(const Matrix &A, Matrix &Ainv) {
	static Matrix LU, P;
	LU.resize(A.Nrows(), A.Nrows()); P.resize(A.Nrows());
	int d;

	lu(A, LU, P, d);
	luinv(LU, P, Ainv);
}

void invchol(const Matrix &A, Matrix &Ainv) {
	static Matrix CH;
	CH.resize(A.Nrows(), A.Nrows());

	chol(A,CH);
	cholinv(CH, Ainv);
}




void lusolve(const Matrix &LU, const Matrix &P, const Matrix &B, Matrix &AinvB) {
	int n=LU.Ncols(), N=B.Ncols();
#ifdef RESIZE
	AinvB.resize(n,N);
#endif
	ASSURE(AinvB.issize(n,N),"luinv(), output matrix must be same size");	

	static Matrix C;
	C.resize(n);

	for(int c=0;c<N;c++) {
		column(B,c,C);
		lubksb(LU, P, C, C);
		putcolumn(C,c,AinvB);
	}
}

void cholsolve(const Matrix &CH, const Matrix &B, Matrix &AinvB) {
	int n=CH.Ncols(), N=B.Ncols();
#ifdef RESIZE
	AinvB.resize(n,N);
#endif
	ASSURE(AinvB.issize(n,N),"cholinv(), output matrix must be same size");	

	static Matrix C;
	C.resize(n);

	for(int c=0;c<N;c++) {
		column(B,c,C);
		cholsl(CH, C, C);
		putcolumn(C,c,AinvB);
	}
}

void solve(const Matrix &A, const Matrix &B, Matrix &AinvB) {
	if(A.issymmetric())
		solvechol(A, B, AinvB);
	else
		solvelu(A, B, AinvB);
}

void solvelu(const Matrix &A, const Matrix &B, Matrix &AinvB) {
	static Matrix LU, P;
	LU.resize(A.Nrows(), A.Nrows()); P.resize(A.Nrows());
	int d;

	lu(A, LU, P, d);
	lusolve(LU, P, B, AinvB);
}

void solvechol(const Matrix &A, const Matrix &B, Matrix &AinvB) {
	static Matrix CH;
	CH.resize(A.Nrows(), A.Nrows());

	chol(A,CH);
	cholsolve(CH, B, AinvB);
}




double ludet(const Matrix &LU, int d) {
	ASSURE(LU.issquare(),"ludet(), matrix must be square");

	double D=(double)d;
	for(int i=0;i<LU.Nrows();i++)
		D*=LU(i,i);
	return D;
}

double choldet(const Matrix &CH) {
	ASSURE(CH.issquare(),"choldet(), matrix must be square");

	double D=1;
	for(int i=0;i<CH.Nrows();i++)
		D*=CH(i,i);
	return D*D;
}


double det(const Matrix &A) {
	if(A.issymmetric())
		return detchol(A);
	else
		return detlu(A);
}

double detlu(const Matrix &A) {
	Matrix LU(A.Nrows(),A.Nrows()), P(A.Nrows());
	int d;

	lu(A, LU, P, d);
	return ludet(LU, d);
}

double detchol(const Matrix &A) {
	Matrix CH(A.Nrows(),A.Nrows());

	chol(A, CH);
	return choldet(CH);
}



