#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define END_POSITION 13
#define TRUE (1)
#define FALSE (0)
#define CLOCK CLOCK_REALTIME

bool encoded_data[13] = {0,0,0,1,0,1,0,0,0,0,1,0,0}; 

enum parity{
	pW,
	p01,
	p02,
	p03,
	p04,
	p05,
	p06
};

bool checkpowTwo(uint32_t parity){

	int countSetBits = 0;

	for(uint8_t index = 0; index < 32; index++)
	{
		if( (parity & (1 << index)) == parity)
		{
			countSetBits++;		
		}
	}

	if(countSetBits == 1){
		return TRUE; // if only one bit of the number is set
	}
	
	return FALSE;	
}

bool calculateParity(int parity){

	int parityPos = 0;
	bool calculateParity;

	if(checkpowTwo(parity) || parity == 1)
	{
		parityPos = parityPos | parity;
	}
	
	// data bits start at 3rd index
	int first = 0;

	for(int index = 3; index < END_POSITION; index++) // goes over array of the data
	{
		
		if((index & parityPos) == parityPos && index != parity){ // if the bit location matches the parity (0,1,2,3)
			
			if(first == 1){
				calculateParity =  calculateParity ^ encoded_data[index];
			}
			else{
				calculateParity = encoded_data[index];
				first = 1;
			}
			
			//printf("data index = %d parity = %d \n",index,calculateParity);			
		}
	}

	return calculateParity;
}

void hamming_encode(){
	
	int index = 0;
	
	// calculate parity
	for(index = 1; index < END_POSITION; index++)
	{
		if(checkpowTwo(index) || index == 1){
			//printf("index = %d \n",index);
			encoded_data[index] = calculateParity(index);	
		}
	}

	// calculate pW
	encoded_data[pW] = encoded_data[p01]; 
	for(index = 1; index < END_POSITION; index++){
		encoded_data[pW] ^= encoded_data[index];
	}
}



struct timespec start,end;

int main(){

	int index = 0;
	clock_gettime(CLOCK,&start);
		
		hamming_encode();

	clock_gettime(CLOCK,&end);

	printf("time = %ld \n ",end.tv_nsec - start.tv_nsec);

	for(index = 0; index < END_POSITION; index++){
		printf("[%d]",encoded_data[index]);
	}

return 0;
}