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
#ifndef __MATRIX_H__
#define __MATRIX_H__


#include "EST.h"
#include <iostream>
#include <fstream>


#define MTYPE double

//#define ASSURE(e,m) if(!(e)) error(m,__LINE__,__FILE__,"timestamp");
//#define EXPECT(e,m) if(!(e)) warning(m,__LINE__,__FILE__,"timestamp");
#define ASSURE(e,m)
#define EXPECT(e,m)

// Swapping business
#define SWAPINT(x) ((((unsigned)x) & 0xff) << 24 | \
                    (((unsigned)x) & 0xff00) << 8 | \
		    (((unsigned)x) & 0xff0000) >> 8 | \
                    (((unsigned)x) & 0xff000000) >> 24)

void MTXswapdouble(double *);

class Matrix;



void copy(const Matrix&, Matrix&);


//portions
void column(const Matrix&, int, Matrix&);
void column(const Matrix&, const Matrix&, Matrix&);
void row(const Matrix&, int, Matrix&);
void putcolumn(const Matrix&, int, Matrix&);
void putrow(const Matrix&, int, Matrix&);
void submatrix(const Matrix&, int, int, int, int, Matrix&);
void putsubmatrix(const Matrix&, int, int, int, int, Matrix&);

//operations
void matrixplus(const Matrix&, const Matrix&, Matrix&);
void matrixplus(const Matrix&, const MTYPE&, Matrix&);
void matrixminus(const Matrix&, const Matrix&, Matrix&);
void matrixminus(const Matrix &A, const MTYPE &s, Matrix &R);
void times(const Matrix&, const MTYPE&, Matrix&);
void times(const Matrix&, const Matrix&, Matrix&);
void mtimes(const Matrix&, const Matrix&, Matrix&);
void divide(const Matrix&, const MTYPE&, Matrix&);
void power(const Matrix&, const MTYPE&, Matrix&);

void hjoin(const Matrix&, const Matrix&, Matrix&);
void vjoin(const Matrix&, const Matrix&, Matrix&);
void transpose(const Matrix&, Matrix&);

void sum(const Matrix&, Matrix&, int=0);
void prod(const Matrix&, Matrix&, int=0);
void any(const Matrix&, Matrix&, int=0);
void all(const Matrix&, Matrix&, int=0);
double sum(const Matrix&);
double prod(const Matrix&);
bool any(const Matrix&);
bool all(const Matrix&);

void normalize(const Matrix&, Matrix&);
void diag(const Matrix&, Matrix&);

void map(const Matrix&, double (*)(double), Matrix&);
void dist(const Matrix&, const Matrix&, Matrix&);

void error(const char*, int, const char*, const char*);
void warning(const char*, int, const char*, const char*);


class Matrix {
private:
	inline void matrixnew(int, int=1);
	inline void matrixdelete();

protected:
	MTYPE *m;
	int rows;
	int cols;
	static int filemagic;
	static char *fileextension;

public:
	// constructor
	Matrix() { matrixnew(0,0); }
	Matrix(int numr, int numc=1) { matrixnew(numr, numc); }
	// destructor
	~Matrix() { matrixdelete(); }
	// copystructor
	Matrix(const Matrix&);
	Matrix(char*);
	Matrix(MTYPE);
	// NEVER USE (MATRIX)int, because it will use the constructor!
	// UserDefinedCast
	operator MTYPE() const { return (MTYPE)operator()(0); }
	operator bool() const { return !isempty(); }

	// access
	int Nrows() const { return rows; }
	int Ncols() const { return cols; }
	inline MTYPE operator()(int, int=0) const;
	inline MTYPE &operator()(int, int=0);

	// assignment
	Matrix &operator=(const Matrix&);
	Matrix &operator=(const MTYPE[]);
	Matrix &operator=(const MTYPE&);
	
	// creation
	inline void resize(int, int=1);

	void zeros(int, int=1);
	void ones(int, int=1);
	void eye(int);
	void seq(MTYPE, MTYPE, MTYPE); // a:s:b
	void seq(MTYPE a, MTYPE b) { seq(a, 1, b); } // a:b
	void linspace(MTYPE, MTYPE, int);
	void toeplitz(const Matrix&);

	// questions
	bool issize(int r, int c=1) const { return ((Nrows()==r) && (Ncols()==c)); }
	bool isempty() const { return (!(Nrows()) && !(Ncols())); }
	bool issquare() const { return (Nrows()==Ncols()); }
	bool isvector() const { return ((Nrows()==1) || (Ncols()==1)); }
	bool issymmetric() const;

	// portions
	Matrix column(int c) const { Matrix R(Nrows(),1); ::column(*this, c, R); return R; }
	Matrix column(Matrix &C) const { Matrix R(Nrows(),C.Nrows()); ::column(*this, C, R); return R; }
	Matrix submatrix(int r1, int r2, int c1, int c2) const { Matrix R(r2-r1+1,c2-c1+1); ::submatrix(*this, r1, r2, c1, c2, R); return R; }

	// operations
	Matrix operator-() const { return operator*((MTYPE)(-1)); }
	Matrix operator+(MTYPE s) const { Matrix R(Nrows(), Ncols()); matrixplus(*this, s, R); return R; }
	Matrix operator-(MTYPE s) const { return operator+(-s); }
	Matrix operator*(MTYPE s) const { Matrix R(Nrows(), Ncols()); times(*this, s, R); return R; }
	Matrix operator/(MTYPE s) const { Matrix R(Nrows(), Ncols()); divide(*this, s, R); return R; }
	Matrix operator+(const Matrix &B) const { Matrix R(Nrows(), Ncols()); matrixplus(*this, B, R); return R; }
	Matrix operator-(const Matrix &B) const { Matrix R(Nrows(), Ncols()); matrixminus(*this, B, R); return R; }
	Matrix operator*(const Matrix &B) const { Matrix R(Nrows(), B.Ncols()); mtimes(*this, B, R); return R; }
	void operator+=(MTYPE s) { matrixplus(*this, s, *this); }
	void operator-=(MTYPE s) { matrixminus(*this, s, *this); }
	void operator*=(MTYPE s) { times(*this, s, *this); }
	void operator/=(MTYPE s) { divide(*this, s, *this); }
	void operator+=(const Matrix &B) { matrixplus(*this, B, *this); }
	void operator-=(const Matrix &B) { matrixminus(*this, B, *this); }
	void operator*=(const Matrix &B) { mtimes(*this, B, *this); }
	//relop
	Matrix operator==(MTYPE) const;
	Matrix operator!=(MTYPE) const;
	Matrix operator<=(MTYPE) const;
	Matrix operator<(MTYPE) const;
	Matrix operator>=(MTYPE) const;
	Matrix operator>(MTYPE) const;
	bool operator!() const { return !this; }
	//joins
	Matrix operator||(const Matrix &) const; //X = A || B;       	// concatenate horizontally (concatenate the Nrows())
	Matrix operator&&(const Matrix &) const; //X = A && B;       	// concatenate vertically (concatenate the columns)

	Matrix t() const { Matrix R(Ncols(), Nrows()); transpose(*this, R); return R; }
	Matrix sum(int dim=0) const { Matrix R(Ncols()); ::sum(*this, R, dim); return R; }
	Matrix diag() const { Matrix R(Nrows()<Ncols()?Nrows():Ncols()); ::diag(*this, R);	return R; }
	
	//inline Matrix do(MTYPE (*func)(MTYPE)) const { Matrix R(Nrows(), Ncols()); ::map(*this, func, R); return R; } //apply any function to all elements

	//streams
	friend std::ostream& operator<<(std::ostream&, const Matrix&);
	friend std::ofstream& operator<<(std::ofstream&, const Matrix&);
	friend std::ifstream& operator>>(std::ifstream&, Matrix&);
};


inline void Matrix::matrixnew(int numr, int numc) {
	ASSURE((numr>=0) && (numc>=0), "Can't create a matrix with negative size");
	rows=numr;
	cols=numc;

	m=new MTYPE [rows*cols];
	ASSURE(m!=NULL, "Insufficient Memory");
}

inline void Matrix::matrixdelete() {
	delete [] m;
	rows=cols=0;
}

inline void Matrix::resize(int numr, int numc) { 
	if(!((rows==numr) && (cols==numc))) {
		matrixdelete();
		matrixnew(numr, numc);
	}
}

inline MTYPE Matrix::operator()(int r, int c) const {
	ASSURE((r>=0) && (c>=0), "Negative index disallowed");
#ifdef RESIZE
	if(!((r<rows) && (c<cols))) {
		warning("Out of bounds during element read, returning zero");
		return 0;
	}
#else
	ASSURE((r<rows) && (c<cols), "Out of bounds during element read");
#endif
	return m[r+c*rows];
}

inline MTYPE &Matrix::operator()(int r, int c) {
	ASSURE((r>=0) && (c>=0), "Negative index disallowed");
#ifdef RESIZE
	if(!((r<rows) && (c<cols))) {
		warning("Out of bounds during element write, extending size");
		int r2, c2, rows2=rows, cols2=cols;
		if(r>=rows)
			rows2=r+1;
		if(c>=cols)
			cols2=c+1;

		MTYPE *m2=new MTYPE[rows2*cols2];
		ASSURE(m2!=NULL, "Insufficient Memory during extension");
		
		for(r2=0;r2<rows2;r2++) {
			for(c2=0;c2<cols2;c2++)
				if((r2<rows) && (c2<cols))
					m2[r2+c2*rows2]=m[r2+c2*rows];
				else
					m2[r2+c2*rows2]=0;
		}

		rows=rows2; cols=cols2;
		delete [] m;
		m=m2;
	}
#else
	ASSURE((r<rows) && (c<cols), "Out of bounds during element write");
#endif
	return m[r+c*rows];
}




#endif // __MATRIX_H__
