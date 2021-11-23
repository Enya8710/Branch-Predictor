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
// Threshold = 1.93*h + h/2 = 1.93*122 + 61 = 296.46 ~ 296
// Total perceptron table size = 512*8*123= 503,808 bits
// Total perceptron steps table size = 512 * 8 = 4,096 bits
// Total Size = perceptron table size + history register size + perceptron steps table = 503,808 + 4,096 + 122 = 508,026 bits
/////////////////////////////////////////////////////////////

PREDICTOR::PREDICTOR(void){
	sum = 0;
	//initialize the local history register
	lhr = bitset<70>();
	//initialize the global history register
  	ghr = bitset<52>();
	//initialize the perceptron table based on the global history length and the number of perceptron
  	for(UINT32 i=0; i < 512; i++){
    	for(UINT32 j=0; j < 123; j++){
	    	table[i][j] = 0;                
    	}
  	}	
}

bool   PREDICTOR::GetPrediction(UINT32 PC){
	//new
	index[0] = PC%512;
	//initialize the predict
	INT32 predict = 0;
	// calculate predict based on the perceptron table and global history
	// calculate the bias
	predict = table[index[0]][0];
	for(UINT32 i=1; i < 123; i++){
    	if(i<=71){
			// hash the local history register
			index[i] = (lhr[i-1]^PC)%512;
			// calcualte the predicted outcome
			predict = predict+table[index[i]][i];
		}
		else{
			// hash the global history register
			index[i] = (ghr[i-71]^PC)%512;
			// calculate the predicted outcome
			predict = predict+table[index[i]][i];
		}
  	}
	//update the perceptron based on the absolute value of predict
  	sum = abs(predict);
	//If predict is positive or zero, it is taken.
  	if(predict >=0){
    	return TAKEN;                
  	}
	//If predict is negative, it is not taken
  	else{
    	return NOT_TAKEN;           
  	}
}

void  PREDICTOR::UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget){
  	//Update the prediction table, if threshold is not reached or the predicted outcome is different from true result
	//Update the bias first and then update the weights.
	if(resolveDir != predDir || sum <= 296){
		//If branch is taken, it will increment the bias value.
    	if(resolveDir == TAKEN){
		    table[index[0]][0] = table[index[0]][0]+1;
	  	}
		//If the branch is not taken, it will decrement the bias value.
    	else{
		    table[index[0]][0] = table[index[0]][0]-1;
	  	}
		// update the weight
		for(UINT32 i = 0; i < 123; i++){
			//If the branch result matches the true result, it will increment the weight
			if(resolveDir == TAKEN ){
				
				table[index[i]][i] = table[index[i]][i]+1;
		    		
      		}
			//If the branch result does not match the true result, it will decrement the weight
	    	else{
			    table[index[i]][i] = table[index[i]][i]-1;
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

