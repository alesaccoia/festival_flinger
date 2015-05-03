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
#include "gmm.h"

#ifdef _WIN32
# undef read
#endif

extern bool SWAP;

int GmmBase::filemagic = 0x416C7801;
char *GmmBase::fileextension = ".gmm";

void GmmBase::gmmnew(int q) {
	Q=q;
	n=0;
	a.resize(Q);
	m=new Matrix [Q];
	C=new Matrix [Q];
	W=new Matrix [Q];
	b=new Matrix [Q];
}

void GmmBase::gmmdelete() {
	delete [] m;
	delete [] C;
	delete [] W;
	delete [] b;
	Q=0;
}


bool GmmBase::load(char *filename) {
	int filecheck;
	char file[256];

	strcpy(file,filename);
	strcat(file,".gmm");

	////#ifdef _WIN32
#ifdef SYSTEM_IS_WIN32
	std::ifstream fin(file,ios::in | ios::binary);
#else
	std::ifstream fin(file,ios::in);
#endif

	if (!fin) {
	  cerr << "Couldn't find file " << filename;
	  return false;
	}
	
	fin.read((char *) &filecheck, sizeof(int));
	if(filecheck==filemagic)
		SWAP=false;
	else {
		if(SWAPINT(filecheck)==(unsigned int) filemagic)
			SWAP=true;
		else {
			cerr << "File Format invalid" << filename;
		}
	}

	fin >> *this;
	fin.close();
	return true;
}


std::ostream& operator<<(std::ostream &stream, const GmmBase &G) {
	int q;

	stream << "Prior:" << endl;
	stream << G.a.t() << endl;

	stream << "Mean:" << endl;
	for(q=0;q<G.Q;q++)
		stream << "Component " << q+1 << ":" << endl << G.m[q].t() << endl;
	stream << "Covariance:" << endl;
	for(q=0;q<G.Q;q++)
		stream << "Component " << q+1 << ":" << endl << G.C[q] << endl;
	stream << "Weight:" << endl;
	for(q=0;q<G.Q;q++)
		stream << "Component " << q+1 << ":" << endl << G.W[q] << endl;
	stream << "Bias:" << endl;
	for(q=0;q<G.Q;q++)
		stream << "Component " << q+1 << ":" << endl << G.b[q].t() << endl;
	return stream;
}

std::ofstream& operator<<(std::ofstream &stream, const GmmBase &G) {
	int q;

	stream.write((char *) &G.Q, sizeof(int));
	stream.write((char *) &G.n, sizeof(int));

	stream << G.a;

	for(q=0;q<G.Q;q++)
		stream << G.m[q] << G.C[q] << G.W[q] << G.b[q];

	return stream;
}

std::ifstream& operator>>(std::ifstream &stream, GmmBase &G) {
	int q;

	stream.read((char *) &q, sizeof(int));
	ASSURE(!stream.eof(),"Gmm::>>() no mixture info");
	if(SWAP)
		q=SWAPINT(q);
	
	G.resize(q);

	stream.read((char *) &G.n, sizeof(int));
	ASSURE(!stream.eof(),"Gmm::>>() no dimension info");
	if(SWAP)
		G.n=SWAPINT(G.n);

	stream >> G.a;

	for(q=0;q<G.Q;q++)
		stream >> G.m[q] >> G.C[q] >> G.W[q] >> G.b[q];

	return stream;
}







int Gmm::filemagic = 0x416C7801;
char *Gmm::fileextension = ".gmm";


void Gmm::post(const Matrix &X, Matrix &h) const { //X(n,N), h(Q,N)
	int N=X.Ncols();
#ifdef RESIZE
	h.resize(Q,N);
#endif
	ASSURE(h.issize(Q,N),"Gmm::post() return matrix size mismatch");

	int q, t;

	static Matrix temp;
	temp.resize(N);

	for(q=0;q<Q;q++) {
		normpdf(X,m[q],C[q],temp);
		for(t=0;t<N;t++)
			h(q,t)=a(q)*temp(t);
	}
	normalize(h,h);
}



void Gmm::mapping(const Matrix &X, Matrix &Y) const {
	int yn=W[0].Nrows(), N=X.Ncols();
#ifdef RESIZE
	Y.resize(yn,N);
#endif
	ASSURE(Y.issize(yn,N),"Gmm::mapping() return matrix size mismatch, or weights not initialized");

	Matrix h(Q,N), tempY(yn, N);
	int q, r, c;
	
	post(X,h);

	Y=0.;
	for(q=0;q<Q;q++) {
		mtimes(W[q],X,tempY);
		for(r=0;r<yn;r++) {
			for(c=0;c<N;c++) {
				tempY(r,c)+=b[q](r);
				tempY(r,c)*=h(q,c);
			}
		}
		Y+=tempY;
	}
}
