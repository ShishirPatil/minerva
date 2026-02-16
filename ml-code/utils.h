#ifndef __UTILS__
#define __UTILS__

template<typename T>
class UTILS
{
private:
	int initStatus;
public:
	UTILS(bool instructions = false);
	int isInitialised();
	/* 
	 * matmatmul
	 * matrix a is of dimension [x,y] and matrix b is of dimension [y,z]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> row major
	 * Matrix out of size [x,z] is returned
	*/
	int matmatmul(T out[], T a[], T b[], int x, int y, int z, bool debug=false);
	int matmatmulkij(T out[], T a[], T b[],int x, int y, int z, bool debug=false);
	/* 
	 * matmatmul
	 * matrix a is of dimension [x,y] and matrix b is of dimension [y,z]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> row major
	 * B is the blockSize for block matrix multiplication
	 * Matrix out of size [x,z] is returned
	*/
	int matmatmulblock(T out[], T a[], T b[],int x, int y, int z, int B, bool debug=false);
	int matmatmulblockjik(T out[], T a[], T b[],int x, int y, int z, int B, bool debug=false);
	/*
	 * matmatmulcol
	 * matrix a is of dimension [x,y] and matrix b is of dimension [y,z]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> column major (transpose of Matrix B)
	 * Matrix out of size [x,z] is returned
	*/
	int matmatmulcol(T out[], T a[], T b[], int x, int y, int z, bool debug=false);
	int transpose(T out[], T a[], int x, int y);
	/*
	 * concat
	 * matrix a is of dimension [x,y] and matrix b is of dimension [a,b]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> row major
	 * Matrix out of size [x,y+b] is returned
	 * THis is not the most optimum implementation -> Lots of space is wasted
	*/
	int concat(T out[], T a[], T b[],int x, int y, int m, int n, bool debug=false);
	int matadd(T out[], T a[], T b[], int x, int y, int m, int n, bool debug=false);

	
};

template<typename T>
int UTILS<T>::matmatmul(T out[], T a[], T b[],int x, int y, int z, bool debug)
{	/*
	 * matmatmul
	 * matrix a is of dimension [x,y] and matrix b is of dimension [y,z]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> row major
	 * Matrix out of size [x,z] is returned
	*/
	T tempSum;
	for(int i=0; i<x; i++)
	{
		for (int j=0; j<z; j++)
		{
			tempSum = 0;
			for (int k = 0; k < y; k++)
			{	
				int aindex = i*y+k;
				int bindex = k*z+j;
				
				tempSum += a[aindex]*b[bindex];
			}
			
			out[i*z+j] = tempSum;
		}
	}
	return 0; 
}

template<typename T>
int UTILS<T>::matmatmulkij(T out[], T a[], T b[],int x, int y, int z, bool debug)
{	/*
	 * matmatmulkij
	 * matrix a is of dimension [x,y] and matrix b is of dimension [y,z]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> row major
	 * Matrix out of size [x,z] is returned
	*/
	// T tempSum;
	T r;
	r =0;
	for (int k = 0; k < y; k++){
		for(int i=0; i<x; i++){
			r = a[i*y+k];
			for (int j=0; j<z; j++)
				out[i*z+j] += r * b[k*z+j]; 
		}
	}
	return 0; 
}

template<typename T>
int UTILS<T>::matmatmulblock(T out[], T a[], T b[],int x, int y, int z, int B, bool debug)
{	/*
	 * matmatmulblock
	 * matrix a is of dimension [x,y] and matrix b is of dimension [y,z]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> row major
	 * B is the blockSize for block matrix multiplication
	 * Matrix out of size [x,z] is returned
	*/
	for(int i=0; i<x; i+=B)
		for (int j=0; j<z; j+=B)
			for (int k = 0; k < y; k+=B)
			/* B X B mat mul */
			for (int i1=i; i1<((i+B)>x?x:i+B); i1++)
				for (int j1=j; j1<((j+B)>z?z:j+B); j1++)
					for (int k1=k; k1<((k+B)>y?y:k+B); k1++)
						out[i1*z+j1]+= a[i1*y+k1] * b[k1*z+j1];			
}

template<typename T>
int UTILS<T>::matmatmulblockjik(T out[], T a[], T b[],int x, int y, int z, int B, bool debug)
{	/*
	 * matmatmulblockjik
	 * matrix a is of dimension [x,y] and matrix b is of dimension [y,z]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> row major
	 * B is the blockSize for block matrix multiplication
	 * Matrix out of size [x,z] is returned
	*/
	for (int k = 0; k < y; k+=B)
			for(int i=0; i<x; i+=B)
				for (int j=0; j<z; j+=B)
					/* Block Mat Mul */
					for (int k1 = k; k1 < ((k+B)>y?y:k+B); k1++)
						for (int i1 = i; i1 < ((i+B)>x?x:i+B); i1++)
							for (int j1 = j; j1 < ((j+B)>z?z:j+B); j1++)
								out[i1*z+j1] += a[i1*y+k1] * b[k1*z+j1]; 
		
			
}


template<typename T>
int UTILS<T>::matmatmulcol(T out[], T a[], T b[],int x, int y, int z, bool debug)
{	/*
	 * matmatmulcol
	 * matrix a is of dimension [x,y] and matrix b is of dimension [y,z]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> column major (transpose of Matrix B)
	 * Matrix out of size [x,z] is returned
	*/
	T tempSum;
	for(int i=0; i<x; i++)
	{
		for (int j=0; j<z; j++)
		{
			tempSum = 0;
			for (int k = 0; k < y; k++)
			{	
				int aindex = i*y+k;
				int bindex = j*y+k;
				
				tempSum += a[aindex]*b[bindex];
			}
			
			out[i*z+j] = tempSum;
		}
	}
	return 0; 
}


template<typename T>
UTILS<T>::UTILS(bool instructions)
{
	this->initStatus = 1;
	
}


template<typename T>
int UTILS<T>::transpose(T out[], T a[], int row, int col)
{
	for (int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			out[j*row+i] = a[i*col + j];
		}
	}
	return 0;
}

template<typename T>
int UTILS<T>::isInitialised()
{
	return (this->initStatus);
}

template<typename T>
int UTILS<T>::concat(T out[], T a[], T b[],int x, int y, int m, int n, bool debug)
{
	/* 
	 * concat
	 * matrix a is of dimension [x,y] and matrix b is of dimension [m,n]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> row major
	 * Matrix out of size [x,y+n] is returned
	 * THis is not the most optimum implementation -> Lots of space is wasted
	*/
	if(x!=m)
		return -1;
	for (int i=0; i<x;i++)
	{
		for(int j=0; j<y; j++)
		{
			out[i*(y+n)+j] = a[i*y+j];
		}
		for(int k=0; k<n; k++)
		{
			out[i*(y+n)+y+k] = b[i*n+k];
		}
	}
	return 0;
}

template<typename T>
int UTILS<T>::matadd(T out[], T a[], T b[], int x, int y, int m, int n, bool debug)
{
	/* 
	 * matadd
	 * matrix a is of dimension [x,y] and matrix b is of dimension [m,n]
	 * a is stored as a vector -> row major
	 * b is also stored as a vector -> row major
	 * Matrix out of size [x,y] is returned
	 * THis is not the most optimum implementation -> Lots of space is wasted
	*/
	if(x!=m || y!=n)
		return -1;
	for (int i=0; i<x ;i++)
	{
		for(int j=0; j<y;j++)
		{
			out[i*y+j] = a[i*y+j] + b[i*y+j];
		}
	}
	return 0; 
}



#endif