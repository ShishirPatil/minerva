#include "helpermath.h"
/* Left multiply mxn matrix (max) with n-d vector vec */
void matrixVectorMul(const float *mat, const unsigned m,
	const unsigned n, const float *vec, float *dst){
	for (int i = 0; i < m; i++){
		float dotProd = 0.0f;
		for(int j = 0; j < n; j++){
			int index = i*n + j;
			dotProd += mat[index] * vec[j];
		}
		dst[i] = dotProd;
	}
}

void scalarVectorMul(float *dst, const unsigned length,
	const float scalar) {
	for(unsigned i = 0; i < length; i++) {
		dst[i] = dst[i] * scalar;
	}
}

void vectorVectorAdd(float *dstVec, const float *srcVec,
	const unsigned length){
	for(unsigned i = 0; i < length; i++)
		dstVec[i] += srcVec[i];
}

void vectorVectorHadamard(float *dst, const float *src,
	const unsigned length){
	for(unsigned i = 0; i < length; i++){
		dst[i] = dst[i] * src[i];
	}
}

float gaussian(const float *x, const float *y,
	unsigned length, float gamma) {
	float sumSq = 0.0;
	for(unsigned i = 0; i < length; i++){
		sumSq += (x[i] - y[i])*(x[i] - y[i]);
	}
	sumSq = -1*gamma*gamma*sumSq;
	sumSq = exp(sumSq);
	return sumSq;
}

void vsigmoid(float *vec, unsigned length){
	// Refer to:
	// https://timvieira.github.io/blog/post/2014/02/11/exp-normalize-trick/
	float val;
	for (int i=0; i < length; i++){
		val = vec[i];
		if(val >= 0){
			float z = exp(-1 * val);
			val = 1.0 / (1.0 + z);
		} else {
			float z = exp(val);
			val = z / (1.0 + z);
		}
		vec[i]=val;	
	}
}


void vtanh(float *vec, unsigned length){
	for (int i=0; i < length; i++){
		vec[i] = tanh(vec[i]);
	}	
}

void softmax(float *input, size_t input_len) {
	//https://codereview.stackexchange.com/questions/180467/implementing-softmax-in-c
	float m = -INFINITY;
	for (size_t i = 0; i < input_len; i++) {
		if (input[i] > m) {
			m = input[i];
			}
	}

	float sum = 0.0;
	for (size_t i = 0; i < input_len; i++) {
		sum += expf(input[i] - m);
	}	

	float offset = m + logf(sum);
	for (size_t i = 0; i < input_len; i++) {
		input[i] = expf(input[i] - offset);
	}
}
