
#pragma once

#include<math.h>
#include "model.h"



__attribute__((section (".container-code")))
float getProjectionComponent(int i, int j){
	unsigned int d = FEATDIM;
	return ldProjectionMatrix[i * d + j];
}

 
__attribute__((section (".container-code")))
int8_t denseLDProjection(float* x, float* x_cap){
	unsigned int d = FEATDIM;
	unsigned int d_cap = LDDIM;
	for (unsigned int i = 0; i < d_cap; i++){
		float dotProd = 0.0;
		for(unsigned int j = 0; j < d; j++){
			float component = getProjectionComponent(i, j);
			dotProd += x[j] * component;
		}
		x_cap[i] = dotProd;
	}
	return 0;
}




__attribute__((section (".container-code")))
int8_t vectorVectorAdd(float *dstVec, float *srcVec,
	unsigned length){

	for(unsigned i = 0; i < length; i++)
		dstVec[i] += srcVec[i];
	return 0;
}


__attribute__((section (".container-code")))
int8_t scalarVectorMul(float *vec, unsigned length,
	float scalar) {

	for(unsigned i = 0; i < length; i++) {
		vec[i] = vec[i] * scalar;
	}
	return 0;
}


__attribute__((section (".container-code")))
int8_t getPrototypeLabel(unsigned index, float *prototypeLabel){
	unsigned int L = NUMLABELS;
	for(unsigned i = 0; i < L; i++){
		float component = prototypeLabelMatrix[index * L + i];
		prototypeLabel[i] = component;
	}
	return 0;
}

__attribute__((section (".container-code")))
float gaussian(const float *x, const float *y, unsigned length, float gamma) {
	float sumSq = 0.0;
	for(unsigned i = 0; i < length; i++){
		sumSq += (x[i] - y[i])*(x[i] - y[i]);
	}
	sumSq = -1*gamma*gamma*sumSq;
	sumSq = exp(sumSq);
	return sumSq;
}


__attribute__((section (".container-code")))
int8_t getPrototype(unsigned index, float *prototype){
	unsigned int d_cap = LDDIM;
	for(unsigned i = 0; i < d_cap; i++){
		float component = prototypeMatrix[index * d_cap + i];
		prototype[i] = component;
	}
	return 0;
}


__attribute__((section (".container-entry")))
float predict(float *x, unsigned length,
	int *scores) {

	unsigned m = NUMPROTOTYPES;
	unsigned int d = FEATDIM;
	unsigned int d_cap = LDDIM;
	unsigned int L = NUMLABELS;
	float gamma = GAMMA;
	if (length != d)
		return -1.0;
	float x_cap[d_cap];
	float y_cap[L];
	float prototype[d_cap];
	float prototypeLabel[L];
	float weight = 0.0;

	for(unsigned i = 0; i < L; i++){
		y_cap[i] = 0.0;
	}
	// Project x onto the d_cap dimension
	denseLDProjection(x, x_cap);
	for(unsigned i = 0; i < m; i++){
		// at this stage, we are holding a feature vector
		// its LD projection and a prototype in memory
		getPrototype(i, prototype);
		weight = gaussian(x_cap, prototype, d_cap, gamma);
		getPrototypeLabel(i, prototypeLabel);
		scalarVectorMul(prototypeLabel, L, weight);
		vectorVectorAdd(y_cap, prototypeLabel, L);
	}

	if (scores != NULL)
		for(unsigned int i = 0; i < L; i++)
			scores[i] = (int)(100000 * y_cap[i]);
	float maxScore = -1000;
	float maxIndex = 0;
	for(unsigned int i = 0; i < length; i++){
		if (scores[i] > maxScore){
			maxIndex = i;
			maxScore = scores[i];
		}
	}
	return maxIndex;
}

