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
#ifndef __GMM_H_
#define __GMM_H_

#include "matrix.h"
#include "numeric.h"
#include "stat.h"

class GmmBase;
class Gmm;




class GmmBase {
private:
	void gmmnew(int);
	void gmmdelete();

protected:
	static int filemagic;
	static char *fileextension;

public:
	int Q;
	int n;
	Matrix a;
	Matrix *m;
	Matrix *C;
	Matrix *W;
	Matrix *b;

	GmmBase() { gmmnew(0); }
	GmmBase(int q) { gmmnew(q); }
	~GmmBase() { gmmdelete(); }

	void resize(int q) { if(q!=Q) {gmmdelete(); gmmnew(q); }}

	bool load(char *);

	friend std::ostream& operator<<(ostream&, const GmmBase&);
	friend std::ofstream& operator<<(ofstream&, const GmmBase&);
	friend std::ifstream& operator>>(ifstream&, GmmBase&);
};


class Gmm : public GmmBase {
protected:
	static int filemagic;
	static char *fileextension;

public:
	Gmm() : GmmBase() {};
	Gmm(int q) : GmmBase(q) {};
	~Gmm() {};
	void post(const Matrix&, Matrix&) const;
	void mapping(const Matrix&, Matrix&) const;
};

#endif // __GMM_H_
