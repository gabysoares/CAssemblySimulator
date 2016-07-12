// Gabriella Soares
//gsoa420
//8889895

#include <stdio.h>
#include <stdint.h>

#define HALT 0xf025
#define LD 0x2
#define LEA 0xe
#define LDI 0xa
#define AND 0x5
#define NOT 0x9
#define ADD 0x1
#define BR 0x0

int printRegisters();
int setCCVal();

	uint16_t registerValues[8] = {0,0,0,0,0,0,0,0};
	uint16_t instructions[100];
	uint16_t IR = 0;
	uint16_t PC = 0;
	char CC[3] = {'N','Z','P'};
	char firstOrigin;
	char secondOrigin;
	int i = 0;
	uint16_t instructionSet;
	uint16_t check;
	uint16_t origin; 
	char firstByte;
	char secondByte; 
	int instructionCounter = 0; //counts file
	int countHalt = 0;
	uint16_t bytes;
	uint16_t getReg;
	uint16_t pcOffset;
	
	char ccVal;
	uint16_t tempLDI;
	uint16_t andType;
	uint16_t SR1, SR2, DR;
	uint16_t immVal;
	uint16_t notVal;
	uint16_t addType;
	uint16_t addVal;
	uint16_t brType;
	uint16_t bitCCval = 0; //to allow NZP flags to be set simultaneously
	

int main(int argc, char *argv[]) { 
	FILE *file;
	file = fopen(argv[1], "r");
	firstOrigin = fgetc(file);
	secondOrigin = fgetc(file);
	origin = (firstOrigin << 8)|secondOrigin &0xff;
	PC = origin;

	while (!feof(file))

	{
		firstByte = fgetc(file);
	
		
		// read all instructions upto HALT into instructions[]
		// instruction counter is the number of instructions
		secondByte = fgetc(file);

		//joins two bytes together to get 16bit instruction line
		bytes = (firstByte << 8)|secondByte &0xff;
		

		instructions[instructionCounter]=bytes;
	
		if(instructions[instructionCounter] == HALT){
			countHalt = instructionCounter;
		}
		instructionCounter += 1;
		
	}
	
	//check instruction
	for(i; i < countHalt; i++)
	{	
		instructionSet = instructions[i];
		PC = origin + i;
		
		check = instructionSet >> 12;
		
		
		if(check == LD){
			getReg = instructionSet >> 9;
			getReg = getReg & 0x7;
			pcOffset = instructionSet & 0x1FF;
			registerValues[getReg] = instructions[pcOffset + i + 1];
			
			setCCVal();
		
		}	

		else if(check == LEA)
		{
			getReg = instructionSet >> 9;
			getReg = getReg & 0x7;
			pcOffset = instructionSet & 0x1FF;
			registerValues[getReg] = pcOffset + origin + i + 1;
			
			setCCVal();
			
		}

		else if(check == LDI)
		{
			getReg = instructionSet >> 9;
			getReg = getReg & 0x7;
			pcOffset = instructionSet & 0x1FF;
			
			tempLDI = pcOffset + origin + i + 1;
			
			
			// read contents of location
			tempLDI = tempLDI - origin;
			tempLDI = instructions[tempLDI];

			tempLDI = tempLDI - origin;
			
			registerValues[getReg] = instructions[tempLDI];
			
			setCCVal();
			
			
		}

		else if(check == AND)
		{	
		
			andType = instructionSet & 0x0010;
			andType = instructionSet >> 5;
			andType = andType & 0x01;
			
			
			if (andType == 0)
			{
			
			SR2 = instructionSet & 0x07;
			SR1 = instructionSet >> 6;
			SR1 = SR1 & 0x07;
			DR = instructionSet >> 9;
			DR = DR & 0x07;
			getReg = DR;
			registerValues[DR] = SR1 & SR2;
			registerValues[DR] = registerValues[SR1] & registerValues[SR2];  
			
			setCCVal();
			

			}

			else if (andType == 1)
			{

				immVal = instructionSet & 0x05;
				
				SR1 = instructionSet >> 6;
				SR1 = SR1 & 0x07;
				DR = instructionSet >> 9;
				DR = DR & 0x07;
				
				registerValues[DR] = registerValues[SR1] & immVal;
			
				setCCVal();
			
			}
		
		}	

		else if(check == NOT)
		{
			
			SR1 = instructionSet >> 6;
			SR1 = SR1 & 0x07;
			
			DR = instructionSet >> 9;
	
			DR = DR & 0x07;
			
			getReg = DR;
			notVal = registerValues[SR1];
			registerValues[DR] = ~notVal; 
			
			setCCVal();
		
		}
		
		else if(check == ADD)
		{	
			addType = instructionSet & 0x0010;
			addType = instructionSet >> 5;
			addType = addType & 0x01;

			if (addType == 0)
			{
				SR2 = instructionSet & 0x07;
				SR1 = instructionSet >> 6;
				SR1 = SR1 & 0x07;
				DR = instructionSet >> 9;
				DR = DR & 0x07;
				getReg = DR;
				registerValues[DR] = registerValues[SR1] + registerValues[SR2];
				
				
				setCCVal();
			
			}
			
			if (addType == 1)
			{


			addVal = instructionSet & 0x1f;
			SR1 = instructionSet >> 6;
			SR1 = SR1 & 0x07;
			DR = instructionSet >> 9;
			DR = DR & 0x07;
			getReg = DR;
			registerValues[DR] = registerValues[SR1] + addVal;
			
			setCCVal();
		
			}
		}
	
	else if(check == BR)
		{	
			
			
			brType = instructionSet & 0x0e00;
			if ((brType & 0x0600) == 0x0600)
			{	
				
				if (bitCCval & 0x04) //check if p
				{	

					
					pcOffset = instructionSet & 0x1FF;
					
					PC = PC + pcOffset;
					
				}
				else if (bitCCval & 0x02) //else check if z
				{
					
					pcOffset = instructionSet & 0x1FF;
					PC = PC + pcOffset;
					
					


				}
			// loop to next instruction
				printRegisters();
				continue;
			}

			//BRnz
			if ((brType & 0x0c00) == 0x0c00)
			{	
				//check if nz flag set in CC
				
				if (bitCCval & 0x01) //check if n
				{	
					
					pcOffset = instructionSet & 0x1FF;
					PC = PC + pcOffset;
	
				
				}
				else if (bitCCval & 0x02) //else check if z
				{
					
					pcOffset = instructionSet & 0x1FF;
					PC = PC + pcOffset;
					
				


				}
			// loop to next instruction 
				printRegisters();
				continue;
			}

				
			if (brType & 0x0200)
			{	
				
				if (bitCCval & 0x04)
				{	
					
					pcOffset = instructionSet & 0x1FF;
					PC = PC + pcOffset;
					
				}
			// loop to next instruction 
				printRegisters();
				continue;
			}	
			
			// check if BRz
			if (brType & 0x0400)
			{
				//
				if (bitCCval & 0x02)
				{
					pcOffset = instructionSet & 0x1FF;
					PC = PC + pcOffset;
					
				}
				printRegisters();
				continue;
			}
			//check if BRn
			if (brType & 0x0800)
			{	
				//check if n flag set in CC
				if (bitCCval & 0x01)
				{	
				
					pcOffset = instructionSet & 0x1FF;
					PC = PC + pcOffset;
					
				}
				printRegisters();
				continue;
			}
			
			//check if BRnzp - branch always
			if ((brType & 0x0e00) == 0x0e00)
			{
				
				pcOffset = instructionSet & 0x1FF;
				PC = PC + pcOffset;
				printRegisters();
			
				continue;
			}

		}
	}
	fclose(file);
	return 1;
}  

int setCCVal()
{	

	bitCCval = 0;
	// Z
	if(registerValues[getReg] == 0)
	{
   	ccVal = CC[1];
   	bitCCval = bitCCval | 0x02;
   	return 1;
  	}
	
	
	// N
	if(registerValues[getReg] & 0x8000)
	{
   	ccVal = CC[0];
   	bitCCval = bitCCval | 0x01;
   	return 1;
   	}
   	

   // P
   if(!(registerValues[getReg] & 0x8000))
   {
   	ccVal = CC[2];
   	bitCCval = bitCCval | 0x04;
   	return 1;
   }
  
return 1;
}

int printRegisters()
{
    printf("after executing instruction\t0x%04x\n",instructions[i]);
	printf("R0\t0x%04x\n",registerValues[0]);
	printf("R1\t0x%04x\n",registerValues[1]);
	printf("R2\t0x%04x\n",registerValues[2]);
	printf("R3\t0x%04x\n",registerValues[3]);
	printf("R4\t0x%04x\n",registerValues[4]);
	printf("R5\t0x%04x\n",registerValues[5]);
	printf("R6\t0x%04x\n",registerValues[6]);
	printf("R7\t0x%04x\n",registerValues[7]);
	printf("PC\t0x%04x\n",PC+1);
	printf("IR\t0x%04x\n",instructions[i]);
	printf("CC\t%c\n",ccVal);
	
	
    printf("==================\n");

	return 1;        
  
}


