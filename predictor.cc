#include "predictor.h"
#include <bitset>
#include <stdint.h>
#include <cmath>
/////////////// STORAGE BUDGET JUSTIFICATION ////////////////
//My version
// Total storage budget: 64KB ~ 512000bits
// Total bits in history register:  122 bits
// Total number of weights per perceptron:  122 + 1 = 123
// Total bits to store weight: 8
// Total number of perceptrons: 512
// Threshold = 1.93*h + 14 = 1.93*122 + 14 = 249.46 ~ 250
// Total perceptron table size = 512*8*123= 503,808 bits
// Total perceptron steps table size = 512 * 8 = 4,096 bits
// Total Size = perceptron table size + history register size + perceptron steps table = 503,808 + 4,096 + 122 = 508,026 bits
/////////////////////////////////////////////////////////////

UINT32 PREDICTOR::divide(bitset<122> n,int m){
    // calculate Remainder by
    // n AND (m-1) -> the bitwise operation 
    bitset<122> bs(m-1);
	// then convert the result to long long integer
    UINT32 res = ((n)&(bs)).to_ullong();
    return res;
}
UINT32 PREDICTOR::HashPC(UINT32 PC){
	UINT32 PCend = PC % 512;
	// use a new function to get the remainder because to_ulong() function cannot hold that number
	UINT32 ghrend = divide(ghr,512);
	return PCend ^ ghrend;
}

PREDICTOR::PREDICTOR(void){
	sum = 0;
	//initialize the local history register
	lhr = bitset<31>();
	//initialize the global history register
  	ghr = bitset<122>();
	//initialize the perceptron table based on the global history length and the number of perceptron
  	for(UINT32 i=0; i < 512; i++){
    	for(UINT32 j=0; j < 122; j++){
	    	table[i][j] = 0;                
    	}
  	}	
}

bool   PREDICTOR::GetPrediction(UINT32 PC){
	// hash the history address to get perceptron index
  	index[0] = HashPC(PC);
	// initialize the predict
  	INT32 predict = 0;
	// calculate predict based on the perceptron table and global history
	// add the bias
  	predict = predict + table[index[0]][0];
  	for(UINT32 i=1; i < 123; i++){
    	if(i <= 31){
			index[i] = HashPC(lhr[i-1]^PC);
	  		predict = predict + table[index[i]][i]; 
			// //if history is taken
			// if(lhr[i - 1] == 1){
			// 	// add the weight
			// 	index[i] = HashPC(lhr[i-1]^PC) 
	  		// 	predict = predict + table[index[i]][i];   
    		// }
			// //if history is not taken
    		// else{
			// 	//substract the weight
			// 	index[i] = HashPC(lhr[i-1]^PC) 
	  		// 	predict = predict - table[index[i]][i]; 
    		// }
		}
		else{
			index[i] = HashPC(ghr[i-1]^PC);
	  		predict = predict + table[index[i]][i]; 
			// if(ghr[i - 1] == 1){
	  		// 	predict = predict + table[index][i];   
    		// }
    		// else{
	  		// 	predict = predict - table[index][i]; 
    		// }
		}
  	}
	//update the perceptron based on the absolute value of predict
  	sum = abs(predict);
	//If predict is positive or zero, it is taken.
  	if(predict > 0){
    	return TAKEN;                
  	}
	//If predict is negative, it is not taken
  	else{
    	return NOT_TAKEN;           
  	}
}

void  PREDICTOR::UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget){
  	index[0] = HashPC(PC);
  	//Update the prediction table, if threshold is not reached or the predicted outcome is different from true result
	//Update the bias first and then update the weights.
	if(resolveDir != predDir || sum <= 249){ 
		//If branch is taken, it will increment the bias value.
    	if(resolveDir == TAKEN){
	    	int32_t updateVal = table[index[0]][0]+1;
	    	if (updateVal > 250){
		    	table[index[0]][0] = 250;
	    	}
	    	else{
		    	table[index[0]][0] = table[index[0]][0]+1;
	    	}
	  	}
		//If the branch is not taken, it will decrement the bias value.
    	else{
	    	int32_t updateVal = table[index[0]][0]-1;
	    	if (updateVal < -250){
		    	table[index[0]][0] = -250;
	    	}
	    	else{
		    	table[index[0]][0] = table[index[0]][0]-1;
	    	}
	  	}
		// update the weight
		for(UINT32 i = 1; i < 123; i++){
			//If the branch result matches the true result, it will increment the weight
      		if((resolveDir == TAKEN && ghr[i - 1] == 1) || (resolveDir == NOT_TAKEN && ghr[i - 1] == 0)){
				int32_t updateVal = table[index[i]][i] + 1;
	    		if (updateVal > 249){
		    		table[index[i]][i] = 249;
	      		}
	      		else{
					table[index[i]][i] = table[index[i]][i]+1;
		    	}
      		}
			//If the branch result does not match the true result, it will decrement the weight
	    	else{
	    		int32_t updateVal = table[index[i]][i] - 1;
	    		if (updateVal < -249){
		    		table[index[i]][i] = -249;
	      		}
		    	else{
			    	table[index[i]][i] = table[index[i]][i]-1;
		    	}
	    	}
		}
  	}
	lhr = (lhr << 1);
	ghr = (ghr << 1);
  	if(resolveDir == TAKEN){
    	ghr.set(0,1);
		lhr.set(0,1);
  	}
  	else{
		ghr.set(0,0);
		lhr.set(0,0);
  	}
}

void    PREDICTOR::TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget){

  // This function is called for instructions which are not
  // conditional branches, just in case someone decides to design
  // a predictor that uses information from such instructions.
  // We expect most contestants to leave this function untouched.

  return;
}

