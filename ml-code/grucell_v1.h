#ifndef __GRUCELL__
#define __GRUCELL__


#include<cmath>
#include"helpermath.h"
#include"utils.h"
#include"data.h"

/*
 *  The below matrices are generated from tensorflow.
 *	There are basic sanity checks in the constructor.
 */
int blockSize=16;

class GRUCell{
private:
	int initState;
	float h[hidden_dim];
	// Additional tools for helper math
	UTILS<float> utils1;
	
public:
	/* For all the public functions except initStatus() return 0 means success */
	/* 
	 * GRU Cell constructor
	 * Performs sanity checks to verify if the dimensions match
	 * init Status is set to 1 if there are match in dimensions
	*/
	GRUCell(int input_dim, int hidden_dim, int num_labels);
	/* 
	 * Returns the value of init_status
	*/
	int initStatus();
	/* 
	 * Updates the GRUCell for one timestep
	 * The hidden_state matrix "h" is updated
	*/
	int updateCell(float *x, int curr_input_dim);
	/* 
	 * The hidden state matrix is multiplied by output Bias matrix and
	 * bias is added
	 * Note: Unlike LSTM for GRU it does it not add any non-linearity
	*/
	int getOutput(float *scores);

};


GRUCell::GRUCell(int input_dim_input, int hidden_dim_input, int num_labels_input)
{	
	/* 
	 * GRU Cell constructor
	 * Performs sanity checks to verify if the dimensions match
	 * init Status is set to 1 if there are match in dimensions
	*/
	initState = 1;

	if(input_dim!=input_dim_input || hidden_dim!=hidden_dim_input)
		initState = 0;
	else if((hidden_dim + input_dim) != gruKernelMat_x)
		initState = 0;
	else if(hidden_dim!=outWBMAT_x)
		initState = 0;
	else if (outWBMAT_y!=num_labels_input)
		initState = 0;
	for (int i=0; i<hidden_dim; i++)
		h[i] = 0;
}


int GRUCell::initStatus()
{	
	/* 
	 * Returns the value of init_status
	*/
	return initState;
}


int GRUCell::updateCell(float *x, int curr_input_dim)
{	
	/* 
	 * Updates the GRUCell for one timestep
	 * The hidden_state matrix "h" is updated
	*/

	if (curr_input_dim!= input_dim)
		return -1;
	if(!initState)
		return -1;

	// gate_inputs = sigmoid(matmul(concat[inputs,state]), gate_kernel) + gate_bias)
	float x_concat_state[hidden_dim + input_dim];
	utils1.concat(x_concat_state, x, h, 1, input_dim, 1, hidden_dim);
	float input_gate[1 * gruKernelMat_y];
	utils1.matmatmulblockjik(input_gate, x_concat_state, gruKernelMat, 1, gruKernelMat_x, gruKernelMat_y, blockSize);
	utils1.matadd(input_gate, input_gate, gruKernelBias, 1, gruKernelMat_y, 1, gruKernelBias_y);
	vsigmoid(input_gate, gruKernelBias_y);

	// r,u = array_split(gate_inputs)
	// r_state = r * state 
	float *r = input_gate;
	float *u = &input_gate[gruKernelBias_y/2];
	vectorVectorHadamard(r, h, hidden_dim);

	// candidate = tanh(matmul(concat[inputs, r_state], candidate_kernel) + candidate_bias)
	utils1.concat(x_concat_state, x, r, 1, input_dim, 1, hidden_dim);
	float candidate[hidden_dim];
	utils1.matmatmulblockjik(candidate, x_concat_state, gruCandidateMat, 1, gruCandidateMat_x, gruCandidateMat_y, blockSize);
	utils1.matadd(candidate, candidate, gruCandidateBias, 1, gruCandidateMat_y, 1, gruCandidateBias_y);
	vtanh(candidate, hidden_dim);

	// h = u * state + (1-u) * candidate
	vectorVectorHadamard(h, u, hidden_dim);
	//(1-u)
	for(int i=0; i<hidden_dim; i++)
		u[i] = 1 - u[i];
	vectorVectorHadamard(u, candidate, hidden_dim);
	vectorVectorAdd(h, u, hidden_dim);
}


int GRUCell::getOutput(float *scores)
{
	/* 
	 * The hidden state matrix is multiplied by output Bias matrix and
	 * bias is added
	 * Note: Unlike LSTM for GRU it does it not add any non-linearity
	*/
	
	utils1.matmatmulblockjik(scores, h, outWBMat, 1, outWBMAT_x, outWBias_y, blockSize);
	vectorVectorAdd(scores, outWBBias, outWBias_y);
	return 0;
}

#endif