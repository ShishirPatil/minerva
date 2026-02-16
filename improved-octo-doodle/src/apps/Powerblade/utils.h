#pragma once

#include <math.h>


__attribute__((section (".container-code")))
float relu(float x) {
  if(x < 0.0) 
    return 0.0;
  else 
    return x;
}

__attribute__((section (".container-code")))
float my_min(float a, float b) {
  if(a < b) 
    return a;
  else 
    return b;
}

__attribute__((section (".container-code")))
float my_max(float a, float b) {
  if (a > b) return a;
  else return b;
}

__attribute__((section (".container-code")))
float quantTanh(float x) {
  return my_max(my_min(x, 1.0), -1.0);
}

__attribute__((section (".container-code")))
float quantSigm(float x) {
  return my_max(my_min((x + 1.0) / 2, 1.0), 0.0);
}

__attribute__((section (".container-code")))
float sigmoid(float x) {
  return 1 / (1 + exp((double)-x));
}

__attribute__((section (".container-code")))
void v_relu(float *vec, int len, float *prod) {
  for (int i = 0; i < len; i++) prod[i] = relu(vec[i]);
}

__attribute__((section (".container-code")))
void v_quantTanh(float *vec, int len, float *prod) {
  for (int i = 0; i < len; i++) prod[i] = quantTanh(vec[i]);
}

__attribute__((section (".container-code")))
void v_quantSigm(float *vec, int len, float *prod) {
  for (int i = 0; i < len; i++) prod[i] = quantSigm(vec[i]);
}

__attribute__((section (".container-code")))
void dMdV(float *mat, int nrows, int ncols, float *vec, float *prod) {
  for (int row = 0; row < nrows; row++) {
    float sum = 0.0;
    for (int col = 0; col < ncols; col++) {
      sum += mat[row*ncols + col] * vec[col];
    }
    prod[row] = sum;
  }
}

__attribute__((section (".container-code")))
void dMdV_add(float *mat, int nrows, int ncols, float *vec, float *prod) {
  for (int row = 0; row < nrows; row++) {
    float sum = 0.0;
    for (int col = 0; col < ncols; col++) {
      sum += mat[row*ncols + col] * vec[col];
    }
    prod[row] += sum;
  }
}

__attribute__((section (".container-code")))
void vv_add(float *vec1, float *vec2, int len) {
  for (int i = 0; i < len; i++) {
    vec2[i] += vec1[i];
  }
}

__attribute__((section (".container-code")))
void vv_div(float *vec1, float *vec2, int len) {
  for (int i = 0; i < len; i++) {
    vec2[i] = vec2[i] / vec1[i];
  }
}

__attribute__((section (".container-code")))
void scalardV_scalardV_add(float scalar1, float *vec1, float scalar2, float *vec2, int len, float *prod) {
  for (int i = 0; i < len; i++) {
    prod[i] = scalar1*vec1[i] + scalar2*vec2[i];
  }
}

__attribute__((section (".container-code")))
int maxIndexVec(float *vec, int len) {
  int maxId = 0;
  float maxScore = -100000;
  for (int i = 0; i < len; i++) {
    if (vec[i] > maxScore) {
      maxScore = vec[i];
      maxId = i;
    }
  }
  return maxId;
}

__attribute__((section (".container-code")))
void classifier(float *FC1, float *FC2, float *FCbias, float *vec, int FCrank, int numClasses, int vecLen, float *tempFC, float *classScores) {
  dMdV(FC1, FCrank, vecLen, vec, tempFC);
  dMdV(FC2, numClasses, FCrank, tempFC, classScores);
  vv_add(FCbias, classScores, numClasses);
}

__attribute__((section (".container-code")))
void generateHiddenStateFastGRNN(float *preComp, float *Bg, float *Bh, float *hiddenState,
  float zeta, float nu, float hiddenDims) {
  for (int i = 0; i < hiddenDims; i++) {
    float gate = sigmoid(preComp[i] + Bg[i]);
    float update = tanh(preComp[i] + Bh[i]);
    hiddenState[i] = gate*hiddenState[i] + (zeta*(1.0 - gate) + nu)*update;
  }
}

__attribute__((section (".container-code")))
void generatePreCompFastGRNN(float *W1, float *W2, float *U1, float *U2, float *a, float *hiddenState,
  int inputDims, int wRank, int hiddenDims, int uRank,
  float *preComp, float *tempLRW, float *tempLRU) {
  dMdV(W1, wRank, inputDims, a, tempLRW);
  dMdV(W2, hiddenDims, wRank, tempLRW, preComp);
  dMdV(U1, uRank, hiddenDims, hiddenState, tempLRU);
  dMdV_add(U2, hiddenDims, uRank, tempLRU, preComp);
}

__attribute__((section (".container-code")))
void FastGRNNUpdate(float *W1, float *W2, float *U1, float *U2, float *Bg, float *Bh, float zeta, float nu, float *a, float *hiddenState,
  int inputDims, int wRank, int hiddenDims, int uRank,
  float *preComp, float *tempLRW, float *tempLRU) {
  generatePreCompFastGRNN(W1, W2, U1, U2, a, hiddenState, inputDims, wRank, hiddenDims, uRank, preComp, tempLRW, tempLRU);
  generateHiddenStateFastGRNN(preComp, Bg, Bh, hiddenState, zeta, nu, hiddenDims);
}

__attribute__((section (".container-entry")))
int FastGRNN(float *W1, float *W2, float *U1, float *U2, float *Bg, float *Bh, float zeta, float nu, float *FC1, float *FC2, float *FCbias,
  float *a, float *mean, float *stdDev, float *hiddenState,
  int inputDims, int wRank, int hiddenDims, int uRank,
  float *preComp, float *tempLRW, float *tempLRU, int timeSteps, float* tempFC, float *classScores, int FCrank, int numClasses, float *normalizedFeats) {
  for (int t = 0; t < timeSteps; t++) {
    scalardV_scalardV_add(1, a + t*inputDims, -1, mean + t*inputDims, inputDims, normalizedFeats);
    vv_div(stdDev + t*inputDims, normalizedFeats, inputDims);
    FastGRNNUpdate(W1, W2, U1, U2, Bg, Bh, zeta, nu, normalizedFeats, hiddenState, inputDims, wRank, hiddenDims, uRank, preComp, tempLRW, tempLRU);
  }
  classifier(FC1, FC2, FCbias, hiddenState, FCrank, numClasses, hiddenDims, tempFC, classScores);
  return maxIndexVec(classScores, numClasses);
}
