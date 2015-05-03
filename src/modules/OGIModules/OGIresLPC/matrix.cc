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
#include "matrix.h"

#ifdef _WIN32
# undef read
#endif



bool SWAP;

int Matrix::filemagic = 0x416C7800;
char *Matrix::fileextension = ".mtx";

void MTXswapdouble(double *d) {
    int *ii = (int *)d;
    int t;
    t = SWAPINT(ii[0]);
    ii[0] = SWAPINT(ii[1]);
    ii[1] = t;
}

Matrix::Matrix(const Matrix &A) {
	int r,c;
	
	matrixnew(A.Nrows(), A.Ncols());
	for(r=0;r<rows;r++) {
		for(c=0;c<cols;c++)
			operator()(r,c)=A(r,c);
	}
}

Matrix::Matrix(char *a) {
	// this function is terrible. Come on!

	char *token;
	char a2[255];
	
	strcpy(a2,a);

	token=strtok(a," ");
	int r=0,c=0;
	while(token!=NULL) {
		c++;
		if(strrchr(token,';')!=NULL)
			break;
		token=strtok(NULL," ");
	}
	if(c)
		r=1;
	while(true) {
		token=strtok(NULL,";");
		if(token==NULL)
			break;
		r++;
	}

	matrixnew(r,c);

	token=strtok(a2," ;");
	for(r=0;r<Nrows();r++) {
		for(c=0;c<Ncols();c++) {
			operator()(r,c)=(MTYPE)atof(token);
			token=strtok(NULL," ;");
		}
	}

}

Matrix::Matrix(MTYPE s) {
	matrixnew(1);
	operator()(0)=s;
}



Matrix &Matrix::operator=(const Matrix &R) {
	int r,c;

	resize(R.Nrows(), R.Ncols());
	
	for(r=0;r<rows;r++) {
		for(c=0;c<cols;c++)
			operator()(r,c)=R(r,c);
	}
	return *this;
}

Matrix &Matrix::operator=(const MTYPE a[]) {
	int r,c;

	for(r=0;r<rows;r++) {
		for(c=0;c<cols;c++)
			operator()(r,c)=a[r*cols+c];
	}
	return *this;
		
}

Matrix &Matrix::operator=(const MTYPE &s) {
	int r,c;

	for(r=0;r<rows;r++) {
		for(c=0;c<cols;c++)
			operator()(r,c)=s;
	}
	return *this;
}


void Matrix::zeros(int numr, int numc) {
	resize(numr, numc);
	*this=(MTYPE)0;
}

void Matrix::ones(int numr, int numc) {
	resize(numr, numc);
	*this=(MTYPE)1;
}

void Matrix::eye(int numrc) {
	resize(numrc, numrc);
	for(int r=0;r<rows;r++) {
		for(int c=0;c<cols;c++) {
			if(r==c)
				operator()(r,c)=(MTYPE)1;
			else
				operator()(r,c)=(MTYPE)0;
		}
	}
}

void Matrix::seq(MTYPE a, MTYPE s, MTYPE b) {
	int numt;
	
	if (((a<b) && (s<0)) || ((a>b) && (s>0)) || (s==0))
		numt=0;
	else
		numt=(int)floor((b-a)/s)+1;
	
	if (numt) {
		int t;

		resize(numt,1);
		operator()(0)=a;
		for(t=1;t<numt;t++)
			operator()(t)=operator()(t-1)+s;
	}
	else
		resize(0); //empty matrix
}

void Matrix::linspace(MTYPE d1, MTYPE d2, int n) {
	resize(n);

	for(int r=0;r<n-1;r++)
		operator()(r)=d1+r*(d2-d1)/(n-1);
	operator()(n-1)=d2;
}

void Matrix::toeplitz(const Matrix &C) {
	int n=C.Nrows();

	ASSURE(C.Ncols()==1,"toeplitz() only vectors are allowed");

	resize(n,n);
	
	int r, c;

	for(r=0;r<n;r++) {
		operator()(r,r)=C(0);
		for(c=r+1;c<n;c++)
			operator()(r,c)=operator()(c,r)=C(c-r);
	}
}



bool Matrix::issymmetric() const {
	bool symmetric=true;
	int r, c;
	
	for(r=0;r<rows;r++) {
		for(c=r+1;c<cols;c++) {
			if(operator()(r,c)!=operator()(c,r)) {
				symmetric=false;
				break;
			}
		}
	}
	return symmetric;
}




Matrix Matrix::operator==(MTYPE s) const {
	Matrix R(Nrows(),Ncols());
	for(int r=0;r<Nrows();r++) {
		for(int c=0;c<Ncols();c++)
			R(r,c)=(operator()(r,c)==s);
	}
	return R;
}

Matrix Matrix::operator!=(MTYPE s) const {
	Matrix R(Nrows(),Ncols());
	for(int r=0;r<Nrows();r++) {
		for(int c=0;c<Ncols();c++)
			R(r,c)=(operator()(r,c)!=s);
	}
	return R;
}

Matrix Matrix::operator<=(MTYPE s) const {
	Matrix R(Nrows(),Ncols());
	for(int r=0;r<Nrows();r++) {
		for(int c=0;c<Ncols();c++)
			R(r,c)=(operator()(r,c)<=s);
	}
	return R;
}
	
Matrix Matrix::operator<(MTYPE s) const {
	Matrix R(Nrows(),Ncols());
	for(int r=0;r<Nrows();r++) {
		for(int c=0;c<Ncols();c++)
			R(r,c)=(operator()(r,c)<s);
	}
	return R;
}

Matrix Matrix::operator>=(MTYPE s) const {
	Matrix R(Nrows(),Ncols());
	for(int r=0;r<Nrows();r++) {
		for(int c=0;c<Ncols();c++)
			R(r,c)=(operator()(r,c)>=s);
	}
	return R;
}

Matrix Matrix::operator>(MTYPE s) const {
	Matrix R(Nrows(),Ncols());
	for(int r=0;r<Nrows();r++) {
		for(int c=0;c<Ncols();c++)
			R(r,c)=(operator()(r,c)>s);
	}
	return R;
}

Matrix Matrix::operator||(const Matrix &B) const {
	Matrix R;
	if(this->isempty())
		R.resize(B.Nrows(),B.Ncols());
	else
		R.resize(this->Nrows(),this->Ncols()+B.Ncols());
	hjoin(*this, B, R);
	return R;
}

Matrix Matrix::operator&&(const Matrix &B) const {
	Matrix R;
	if(this->isempty())
		R.resize(B.Nrows(),B.Ncols());
	else
		R.resize(this->Nrows()+B.Nrows(),this->Ncols()); 
	vjoin(*this, B, R);
	return R;
}




std::ostream& operator<<(std::ostream &stream, const Matrix &A) {
	int r,c;

	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<A.Ncols();c++) {
			stream.width(14);
			stream.setf(ios::right);
			stream << A(r,c);
		}
		if(r!=A.Nrows()-1)
			stream << endl;
	}
	return stream;
}

std::ofstream& operator<<(std::ofstream &stream, const Matrix &A) {

	int r,c;
	MTYPE temp;

	r=A.Nrows();
	stream.write((char *) &r, sizeof(int));
	c=A.Ncols();
	stream.write((char *) &c, sizeof(int));

	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<A.Ncols();c++) {
			temp=A(r,c);
			stream.write((char *) &temp, sizeof(MTYPE));
		}
	}
	return stream;
}

std::ifstream& operator>>(std::ifstream &stream, Matrix &A) {
	int r,c;
	double temp;

	stream.read((char *) &r, sizeof(int));
	ASSURE(!stream.eof(),"Matrix::>>() no rows info");
	if(SWAP)
		r=SWAPINT(r);

	stream.read((char *) &c, sizeof(int));
	ASSURE(!stream.eof(),"Matrix::>>() no cols info");
	if(SWAP)
		c=SWAPINT(c);

	A.resize(r,c); // always renews matrix. is that good philosophy?

	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<A.Ncols();c++) {
			stream.read((char *) &temp, sizeof(MTYPE));
			ASSURE(!stream.eof(),"Matrix::>>() not enough element info");
			if(SWAP)
				MTXswapdouble(&temp);
			A(r,c)=temp;
		}
	}
	return stream;
}

void matrixplus(const Matrix &A, const Matrix &B, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(B.isvector() || B.issize(A.Nrows(),A.Ncols()),"Matrix::plus() dimensions must agree");
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::plus() return Matrix size mismatch");

	int r,c;
	if(B.Ncols()==1) {
		for(r=0;r<A.Nrows();r++) {
			for(c=0;c<A.Ncols();c++)
				R(r,c)=A(r,c)+B(r);
		}
	} else {
		if(B.Nrows()==1) {
			for(r=0;r<A.Nrows();r++) {
				for(c=0;c<A.Ncols();c++)
					R(r,c)=A(r,c)+B(0,c);
			}
		} else {
			for(r=0;r<A.Nrows();r++) {
				for(c=0;c<A.Ncols();c++)
					R(r,c)=A(r,c)+B(r,c);
			}
		}
	}
}

void matrixplus(const Matrix &A, const MTYPE &s, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::plus() return Matrix size mismatch");

	int r,c;
	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<A.Ncols();c++)
			R(r,c)=A(r,c)+s;
	}
}

void matrixminus(const Matrix &A, const MTYPE &s, Matrix &R) { 
  matrixplus(A, -s, R); 
}


void matrixminus(const Matrix &A, const Matrix &B, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(A.issize(B.Nrows(),B.Ncols()),"Matrix::minus() dimensions must agree");
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::minus() return Matrix size mismatch");

	int r,c;
	if(B.Ncols()==1) {
		for(r=0;r<A.Nrows();r++) {
			for(c=0;c<A.Ncols();c++)
				R(r,c)=A(r,c)-B(r);
		}
	} else {
		if(B.Nrows()==1) {
			for(r=0;r<A.Nrows();r++) {
				for(c=0;c<A.Ncols();c++)
					R(r,c)=A(r,c)-B(0,c);
			}
		} else {
			for(r=0;r<A.Nrows();r++) {
				for(c=0;c<A.Ncols();c++)
					R(r,c)=A(r,c)-B(r,c);
			}
		}
	}
}

void mtimes(const Matrix &A, const Matrix &B, Matrix &C) {
#ifdef RESIZE
	C.resize(A.Nrows(), B.Ncols());
#endif
	ASSURE( A.Ncols()==B.Nrows(), "Matrix::mtimes() dimensions must agree");
	ASSURE( C.issize(A.Nrows(),B.Ncols()), "Matrix::mtimes() return Matrix size mismatch");

	// Winograd's Algorithm
	int i,j,k;
	MTYPE *temp;
	
	static Matrix AW, BW;
	AW.resize(A.Nrows()); BW.resize(B.Ncols());


	for(i=0;i<A.Nrows();i++) {
		AW(i)=0;
		for(k=0;k<A.Ncols()-1;k+=2)
			AW(i) += A(i,k) * A(i,k+1);
	}

	for(j=0;j<B.Ncols();j++) {
		BW(j)=0;
		for(k=0;k<B.Nrows()-1;k+=2)
			BW(j) += B(k,j) * B(k+1,j);
	}
	
	for(i=0;i<A.Nrows();i++) {
		for(j=0;j<B.Ncols();j++) {
			temp=&C(i,j);
			*temp=0;
			for(k=0;k<A.Ncols()-1;k+=2)
				*temp += ( A(i,k)+B(k+1,j)) * (A(i,k+1)+B(k,j));
			if(A.Ncols()%2)
				*temp += A(i,k)*B(k,j);
			*temp -= (AW(i)+BW(j));
		}
	}
}

void times(const Matrix &A, const MTYPE &s, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::mult() return Matrix size mismatch");

	int r,c;
	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<A.Ncols();c++)
			R(r,c)=A(r,c)*s;
	}
}

void times(const Matrix &A, const Matrix &B, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(A.issize(B.Nrows(),B.Ncols()),"Matrix::times() dimensions must agree");
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::times() return Matrix size mismatch");

	int r,c;
	if(B.Ncols()==1) {
		for(r=0;r<A.Nrows();r++) {
			for(c=0;c<A.Ncols();c++)
				R(r,c)=A(r,c)*B(r);
		}
	} else {
		if(B.Nrows()==1) {
			for(r=0;r<A.Nrows();r++) {
				for(c=0;c<A.Ncols();c++)
					R(r,c)=A(r,c)*B(0,c);
			}
		} else {
			for(r=0;r<A.Nrows();r++) {
				for(c=0;c<A.Ncols();c++)
					R(r,c)=A(r,c)*B(r,c);
			}
		}
	}
}

void divide(const Matrix &A, const MTYPE &s, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::div() return Matrix size mismatch");

	int r,c;

	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<A.Ncols();c++)
			R(r,c)=A(r,c)/s; //rewritten for better fp accuracy, instead of mult(A,1/s,R);
	}
}

void hjoin(const Matrix &A, const Matrix &B, Matrix &R) {
#ifdef RESIZE
	R.resize(A.isempty() ? B.Nrows() : A.Nrows(), A.Ncols()+B.Ncols());
#endif
	ASSURE(A.isempty() || B.isempty() || (A.Nrows()==B.Nrows()),"Matrix::hjoin() input matrices must have the same number of rows, or one of them must be empty");
	ASSURE(R.issize(A.isempty() ? B.Nrows() : A.Nrows(),A.Ncols()+B.Ncols()),"Matrix::hjoin() return Matrix size mismatch");

	int r,c;
	int rmax=A.isempty()? B.Nrows() : A.Nrows();

	for(c=0;c<A.Ncols();c++) {
		for(r=0;r<rmax;r++)
			R(r,c)=A(r,c);
	}
	for(c=0;c<B.Ncols();c++) {
		for(r=0;r<rmax;r++)
			R(r,c+A.Ncols())=B(r,c);
	}
}


void vjoin(const Matrix &A, const Matrix &B, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows()+B.Nrows(), A.isempty() ? B.Ncols() : A.Ncols());
#endif
	ASSURE(A.isempty() || B.isempty() || (A.Ncols()==B.Ncols()),"Matrix::vjoin() input matrices must have the same number of columns, or one of them must be empty");
	ASSURE(R.issize(A.Nrows()+B.Nrows(),A.isempty() ? B.Ncols() : A.Ncols()),"Matrix::vjoin() return Matrix size mismatch");

	int r,c;
	int cmax=A.isempty()? B.Ncols() : A.Ncols();

	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<cmax;c++)
			R(r,c)=A(r,c);
	}
	for(r=0;r<B.Nrows();r++) {
		for(c=0;c<cmax;c++)
			R(r+A.Nrows(),c)=B(r,c);
	}
}



// call from m(const m), m=m?
void copy(const Matrix &A, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::copy() return Matrix size mismatch");

	for(int r=0;r<A.Nrows();r++) {
		for(int c=0;c<A.Ncols();c++)
			R(r,c)=A(r,c);
	}
}


void column(const Matrix &A, const Matrix &i, Matrix &V) {
#ifdef RESIZE
	V.resize(A.Nrows(), i.Nrows());
#endif
	ASSURE(i.Ncols()==1,"column(), index Matrix must be a vector");
	ASSURE(all(i>=0.),"column(), negative index disallowed");
	ASSURE(all(i<(MTYPE)A.Ncols()),"column(), out of bounds");
	ASSURE(V.issize(A.Nrows(), i.Nrows()),"column(), return size mismatch");

	int r,c;

	for(c=0;c<i.Nrows();c++) {
		for(r=0;r<A.Nrows();r++)
			V(r,c)=A(r,(int)i(c));
	}
}

void column(const Matrix &A, int c, Matrix &v) {
#ifdef RESIZE
	v.resize(A.Nrows());
#endif
	ASSURE(c>=0,"column(), negative index disallowed");
	ASSURE(c<A.Ncols(),"column(), out of bounds");
	ASSURE(v.issize(A.Nrows()),"column(), return size mismatch");

	int r;

	for(r=0;r<A.Nrows();r++)
		v(r)=A(r,c);
}

void row(const Matrix &A, int r, Matrix &v) {
#ifdef RESIZE
	v.resize(A.Ncols());
#endif
	ASSURE(r>=0,"negative index disallowed");
	ASSURE(r<A.Nrows(),"out of bounds");
	ASSURE(v.issize(A.Ncols()),"return matrix size mismatch");

	int c;

	for(c=0;c<A.Ncols();c++)
		v(c)=A(r,c);
}

void putrow(const Matrix &v, int r, Matrix &A) {
	ASSURE(v.issize(A.Ncols()),"input matrix size mismatch");
	ASSURE(r>=0,"negative index disallowed");
	ASSURE(r<A.Nrows(),"out of bounds");

	int c;

	for(c=0;c<A.Ncols();c++)
		A(r,c)=v(c);
}

void putcolumn(const Matrix &v, int c, Matrix &A) {
	ASSURE(v.issize(A.Nrows()),"input matrix size mismatch");
	ASSURE(c>=0,"negative index disallowed");
	ASSURE(c<A.Ncols(),"out of bounds");

	int r;

	for(r=0;r<A.Nrows();r++)
		A(r,c)=v(r);
}


void submatrix(const Matrix &A, int r1, int r2, int c1, int c2, Matrix &V) {
#ifdef RESIZE
	V.resize(r2-r1+1,c2-c1+1);
#endif
	ASSURE((r1>=0) && (r2>=0) && (c1>=0) && (c2>=0),"index can't be negative");
	ASSURE(r1<A.Nrows(),"r1 out of bounds");
	ASSURE(r2<A.Nrows(),"r2 out of bounds");
	ASSURE(c1<A.Ncols(),"c1 out of bounds");
	ASSURE(c2<A.Ncols(),"c2 out of bounds");
	ASSURE(r1<=r2,"r1 is greater than r2");
	ASSURE(c1<=c2,"c1 is greater than c2");
	ASSURE(V.issize(r2-r1+1,c2-c1+1),"return size mismatch");

	int r,c;

	for(r=0;r<r2-r1+1;r++) {
		for(c=0;c<c2-c1+1;c++)
			V(r,c)=A(r+r1,c+c1);
	}
}

void putsubmatrix(const Matrix &V, int r1, int r2, int c1, int c2, Matrix &A) {
	ASSURE(V.issize(r2-r1+1,c2-c1+1),"input size mismatch");
	ASSURE((r1>=0) && (r2>=0) && (c1>=0) && (c2>=0),"index can't be negative");
	ASSURE(r1<A.Nrows(),"r1 out of bounds");
	ASSURE(r2<A.Nrows(),"r2 out of bounds");
	ASSURE(c1<A.Ncols(),"c1 out of bounds");
	ASSURE(c2<A.Ncols(),"c2 out of bounds");
	ASSURE(r1<=r2,"r1 is greater than r2");
	ASSURE(c1<=c2,"c1 is greater than c2");

	int r,c;

	for(r=0;r<r2-r1+1;r++) {
		for(c=0;c<c2-c1+1;c++)
			A(r+r1,c+c1)=V(r,c);
	}
}

void transpose(const Matrix &A, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Ncols(), A.Nrows());
#endif
	ASSURE(&R!=&A,"input and return matrix cannot be the same");
	ASSURE(R.issize(A.Ncols(),A.Nrows()), "return size mismatch");

	int r,c;

	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<A.Ncols();c++)
			R(c,r)=A(r,c);
	}
}

void sum(const Matrix &A, Matrix &R, int dim) {
	ASSURE((dim==0) || (dim==1),"sum(), dim must be zero or one");
#ifdef RESIZE
	if(dim==1)
		R.resize(A.Nrows());
	else
		R.resize(A.Ncols());
#endif
	int r,c;
	
	if (dim==1) {
		ASSURE(R.issize(A.Nrows()),"Matrix::sum() return Matrix size mismatch");
		for(r=0;r<A.Nrows();r++) {
			R(r)=0;
			for(c=0;c<A.Ncols();c++)
				R(r)+=A(r,c);
		}
	}
	else {
		ASSURE(R.issize(A.Ncols()),"Matrix::sum() return Matrix size mismatch");
		for(c=0;c<A.Ncols();c++) {
			R(c)=0;
			for(r=0;r<A.Nrows();r++)
				R(c)+=A(r,c);
		}
	}
}

void prod(const Matrix &A, Matrix &R, int dim) {
	ASSURE((dim==0) || (dim==1),"prod(), dim must be zero or one");
#ifdef RESIZE
	if(dim==1)
		R.resize(A.Nrows());
	else
		R.resize(A.Ncols());
#endif
	int r,c;
	
	if (dim==1) {
		ASSURE(R.issize(A.Nrows()),"Matrix::sum() return Matrix size mismatch");
		for(r=0;r<A.Nrows();r++) {
			R(r)=1;
			for(c=0;c<A.Ncols();c++)
				R(r)*=A(r,c);
		}
	}
	else {
		ASSURE(R.issize(A.Ncols()),"Matrix::sum() return Matrix size mismatch");
		for(c=0;c<A.Ncols();c++) {
			R(c)=0;
			for(r=0;r<A.Nrows();r++)
				R(c)*=A(r,c);
		}
	}
}

double sum(const Matrix &A) {
	double s=0;
	for(int c=0;c<A.Ncols();c++) {
		for(int r=0;r<A.Nrows();r++)
			s+=A(r,c);
	}
	return s;
}

double prod(const Matrix &A) {
	double s=1;
	for(int c=0;c<A.Ncols();c++) {
		for(int r=0;r<A.Nrows();r++)
			s*=A(r,c);
	}
	return s;
}

bool any(const Matrix &A) {
	for(int c=0;c<A.Ncols();c++) {
		for(int r=0;r<A.Nrows();r++)
			if(A(r,c))
				return true;
	}
	return false;
}

bool all(const Matrix &A) {
	for(int c=0;c<A.Ncols();c++) {
		for(int r=0;r<A.Nrows();r++)
			if(!A(r,c))
				return false;
	}
	return true;
}

void normalize(const Matrix &A, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::normalize() return Matrix size mismatch");

	int r,c;
	MTYPE s;
	
	for(c=0;c<A.Ncols();c++) {
		s=0.;
		for(r=0;r<A.Nrows();r++)
			s+=A(r,c);
		if(s) {
			for(r=0;r<A.Nrows();r++)
				R(r,c)=A(r,c)/s;
		}
		else {
			MTYPE rec=(MTYPE)1./A.Nrows();
			for(r=0;r<A.Nrows();r++)
				R(r,c)=rec;
		}
	}
}

void diag(const Matrix &A, Matrix &R) {
	int numt=A.Nrows()<A.Ncols() ? A.Nrows() : A.Ncols();
#ifdef RESIZE
	R.resize(numt);
#endif
	ASSURE(R.issize(numt,1),"Matrix::diag() return Matrix size mismatch");

	int t;

	for(t=0;t<numt;t++)
		R(t)=A(t,t);
}


void power(const Matrix &A, const MTYPE &s, Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::power() return Matrix size mismatch");

	int r,c;

	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<A.Ncols();c++)
			R(r,c)=(MTYPE)pow(A(r,c),s);
	}
}

void map(const Matrix &A, double (*func)(double), Matrix &R) {
#ifdef RESIZE
	R.resize(A.Nrows(), A.Ncols());
#endif
	ASSURE(R.issize(A.Nrows(),A.Ncols()),"Matrix::map() return Matrix size mismatch");

	int r,c;

	for(r=0;r<A.Nrows();r++) {
		for(c=0;c<A.Ncols();c++)
			R(r,c)=(MTYPE)func(A(r,c));
	}
}


void dist(const Matrix &A, const Matrix &B, Matrix &D) {
	int n=A.Nrows(), NA=A.Ncols(), NB=B.Ncols();
#ifdef RESIZE
	D.resize(NA, NB);
#endif
	ASSURE(B.Nrows()==n,"dist() Dimensions must agree");
	ASSURE((D.Nrows()==NA) && (D.Ncols()==NB),"dist() return matrix size mismatch");

	int ac, bc, r;
	MTYPE d, temp;

	for(ac=0; ac<NA; ac++) {
		for(bc=0; bc<NB; bc++) {
			d=0;
			for(r=0; r<n; r++) {
				temp=A(r,ac)-B(r,bc);
				d+=temp*temp;
			}
			D(ac,bc)=(MTYPE)sqrt(d);
		}
	}
}




void error(const char *msg, int line, const char *file, const char *stamp) {
	cerr << endl << "ERROR: " << msg << endl;
	cerr << file << " (" << line << ") "<< stamp << endl;
	//exit(1);
}

void warning(const char *msg, int line, const char *file, const char *stamp) {
	cerr << endl << "WARNING: " << msg << endl;
	cerr << file << " (" << line << ") "<< stamp << endl;
}

