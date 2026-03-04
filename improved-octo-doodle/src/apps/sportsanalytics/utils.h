#pragma once

#include <math.h>

__attribute__((section (".container-code")))
float relu(float x) {
  if (x < 0.0) return 0.0;
  else return x;
}

__attribute__((section (".container-code")))
float sigmoid(float x) {
  return 1;
}

__attribute__((section (".container-code")))
void v_relu(float *vec, int len, float *prod) {
  for (int i = 0; i < len; i++) prod[i] = (float)relu(vec[i]);
}

__attribute__((section (".container-code")))
void v_sigmoid(float *vec, int len, float *prod) {
  for (int i = 0; i < len; i++) prod[i] = sigmoid(vec[i]);
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

/* 
 * matmatmul
 * matrix a is of dimension [x,y] and matrix b is of dimension [y,z]
 * a is stored as a vector -> row major
 * b is also stored as a vector -> row major
 * B is the blockSize for block matrix multiplication
 * Matrix out of size [x,z] is returned
*/
__attribute__((section (".container-code")))
int matmatmulkij(float out[], float a[], float b[],int x, int y, int z){
  float r;
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


__attribute__((section (".container-code")))
void vv_add(float *vec1, float *vec2, int len) {
  for (int i = 0; i < len; i++) {
    vec2[i] += vec1[i];
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

__attribute__((section (".container-entry")))
int PrateekModel(float *W1, float *W2, float *B1, float *B2, 
  float *input_vector, int input_dim, int hidden_dim, 
  int output_dim, float *inter_1, float *inter_2) {
  // FC1 
  matmatmulkij(inter_1, input_vector, W1, 1, input_dim, hidden_dim);
  vv_add(B1, inter_1, hidden_dim);
  v_relu(inter_1, hidden_dim, inter_1);
  // FC2
  matmatmulkij(inter_2, inter_1, W2, 1, hidden_dim, output_dim);
  vv_add(B2, inter_2, output_dim);
  v_sigmoid(inter_2, output_dim, inter_2);

  return (maxIndexVec(inter_2, output_dim)+3);
}
