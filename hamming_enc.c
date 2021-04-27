#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define END_POSITION 13
#define TRUE (1)
#define FALSE (0)
#define CLOCK CLOCK_REALTIME

char encoded_data[13] = {0,0,0,1,0,1,0,0,0,0,1,0,0};
char received_data[13] = {0}; 

enum parity{
	pW,
	p01,
	p02,
	p03,
	p04,
	p05,
	p06
};

// check bits decoder
bool check_bits[7] = {0}; // start from index 1 

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


// Calculate the XOR of the bits whose bit = parity position is 1 

bool calculateParity(int parity, char data[]){

	int parityPos = 0;
	bool calculateParity;

	if(checkpowTwo(parity) || parity == 1) // to make sure we only check the bit positions 1,2,4,,8,16...
	{
		parityPos = parityPos | parity;
	}
	
	// data bits start at 3rd index
	int first = 0;

	for(int index = 3; index < END_POSITION; index++) // goes over array of the data which starts at 3rd index
	{
		
		if((index & parityPos) == parityPos && index != parity){ // if the bit location matches the parity (0,1,2,3) and the index is not the parity we are calculating
			
			if(first == 1){
				calculateParity =  calculateParity ^ data[index];
			}
			else{
				calculateParity = data[index];
				first = 1;
			}
			
			//printf("data index = %d parity = %d \n",index,calculateParity);			
		}
	}

	return calculateParity;
}

void hamming_encode(){
	
	int index = 0;

	encoded_data[pW] = encoded_data[p01];
	// calculate parity
	for(index = 1; index < END_POSITION; index++)
	{
		if(checkpowTwo(index) || index == 1){ // to make sure the parity positions are 1,2,4,8,16...
			//printf("index = %d \n",index);
			encoded_data[index] = calculateParity(index,encoded_data);	// Calculate the XOR of the data and store at p01(1),p02(2),p03(4)...
		}

		// calculate pW
		encoded_data[pW] ^= encoded_data[index];
	}

	
	 
}

void hamming_decode(){

	bool CParity;
	int Cindex = 1;
	int bit_flipped = 0;
	bool pw2 = received_data[p01];
	// for all the indexes 
	for(int index = 1; index < END_POSITION; index++)
	{	
		// calculate parity 
		if(checkpowTwo(index) || index == 1){ // to make sure the parity positions are 1,2,4,8,16...
			CParity = calculateParity(index,received_data);

			// check if CParity == RParity			
			
			// if No --> corresponding check bit = 1
			if(CParity ^ received_data[index]){	
			 
			// add the parity index positions 
				check_bits[Cindex] = 1;
				bit_flipped = bit_flipped + index;
			}
			else{
				// if yes --> corresponding check bit = 0
				check_bits[Cindex] = 0;
			}

			// sum all the check bits	
			check_bits[0] += check_bits[Cindex]; 
			Cindex++;
		}
		// calculate pW2
		pw2 ^= received_data[index];
	}	

	// if sumOfCheckbits = 0 && pw2 = pw (NO ERRORS)
	if(!check_bits[0] && pw2 == received_data[pW]){
		printf("No Errors \n");	
	}

	// if sumOfCheckbits = 0 && pw2 != pw (parity word error)
	if(!check_bits[0] && pw2 != received_data[pW]){
		//printf("Parity Word error \n");
		received_data[pW] = !received_data[pW];	
	}
	// if sumOfCheckbits != 0 && pw2 != pw (SBE)
	if(check_bits[0] && pw2 != received_data[pW]){
		//printf("Single Bit Error \n");
		received_data[bit_flipped] = !received_data[bit_flipped];	
	}
	// if sumOfCheckbits != 0 && pw2 = pw (DBE)
	if(check_bits[0] && pw2 == received_data[pW]){
		printf("Double Bit Error \n");	
	}
}

struct timespec start,end;

int main(){

	int index = 0;
	clock_gettime(CLOCK,&start);
		
		hamming_encode();

	clock_gettime(CLOCK,&end);

	printf("Hamming encoder time = %ld \n ",end.tv_nsec - start.tv_nsec);

	for(index = 0; index < END_POSITION; index++){
		printf("[%d]",encoded_data[index]);
	}

	printf("\n");

	memcpy(received_data,encoded_data,END_POSITION);
	
	// SBE
	received_data[5] = 0;
	// No error

	// DBE 
	 //received_data[5] = 0;
	 //received_data[6] = 1;
	
	// PW error
	//received_data[pW] = !encoded_data[pW];
	
	clock_gettime(CLOCK,&start);

	hamming_decode();

	clock_gettime(CLOCK,&end);

	printf("Hamming decoder time = %ld \n ",end.tv_nsec - start.tv_nsec);
	
	for(index = 0; index < END_POSITION; index++){
		printf("[%d]",received_data[index]);
	}	

	printf("\n");
return 0;
}