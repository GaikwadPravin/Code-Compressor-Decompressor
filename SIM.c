
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAXCHAR 1000

struct inputBinary
{
    char instruction[40] ;
	unsigned long int dec;
};

struct dictionary
{
    char dicInstr[33] ;
	unsigned long int dicVal;
	char index[4];
};

struct ele
{
    char val[33] ;
	int count, index;
	unsigned long int decVal;
};

struct compressedBinary
{
	char compAlgoindex[4];
	char compInstr[36];
	int compLength,compAlgo,instrNum;
	char index[4];
};

struct MaskSturct
{
	char mask[5];
};

/*
	0. RLE
	1. bitmask-based
	2. 1 bit Mismatch
	2. 2 bit consecutive mismatches
	3. 2 bit mismatches anywhere
	4. Direct Matching
	5. Original Binaries
*/

struct ele element[10000], temp;						// malloc
struct inputBinary instructions[10000] = {0};			//	 malloc
struct inputBinary ip_compr_instructions[10000] = {0};			//	 malloc
struct inputBinary dec_to_original[10000] = {0};			//	 malloc
struct dictionary dictEntries[8];
struct compressedBinary compInstructions[10000]; // malloc
struct compressedBinary decompInstr[10000]; // malloc
struct MaskSturct bitMaskSturct[16];

int no_of_instructions = 0, no_of_ip_compr_instructions = 0,compInstrcnt = 0, no_of_decompr_instructions = 0;

int pc = 0, comprpc = 0,rleFlag, decpc = 0,decompDicno = 0;


int directoryMatchingFlag, RLEFlag, oneMismatchFlag, twocMismatchFlag, twoaMismatchFlag, bitMaskFlag;

char *compressed_code;

int converttoBin(long unsigned int dec)
{
	long unsigned int temp;
    if (dec == 0)
    {
		temp = 0;
        return temp;
    }
    else
    {
		temp = (dec % 2 + 10 * converttoBin(dec / 2));
        return temp;
    }
}

void createBitMaskstr()
{
    int bitMaskCount = 0, bitMaskCountDec = 0;
    char tempData[5];

    while(bitMaskCount<16)
	{
		bitMaskCountDec = 0;

		if(bitMaskCount < 2)
		{
			strcpy(bitMaskSturct[bitMaskCount].mask,"000");
			sprintf(tempData,"%d",bitMaskCount);
			strcat(bitMaskSturct[bitMaskCount].mask,tempData);
		}
		else if(bitMaskCount < 4)
		{
			strcpy(bitMaskSturct[bitMaskCount].mask,"00");
			bitMaskCountDec = converttoBin(bitMaskCount);
			sprintf(tempData,"%d",bitMaskCountDec);
			strcat(bitMaskSturct[bitMaskCount].mask,tempData);
		}
		else if(bitMaskCount < 8)
		{
			strcpy(bitMaskSturct[bitMaskCount].mask,"0");
			bitMaskCountDec = converttoBin(bitMaskCount);
			sprintf(tempData,"%d",bitMaskCountDec);
			strcat(bitMaskSturct[bitMaskCount].mask,tempData);
		}
		else
		{
			bitMaskCountDec = converttoBin(bitMaskCount);
			sprintf(bitMaskSturct[bitMaskCount].mask,"%d",bitMaskCountDec);
		}

		bitMaskCount++;
	}



}
void bitMask(int inputpc)
{
	int bitMaskCountDec, i, j, noOfone = 0, misMatchfirst = 0, misMatchlast = 0,misMatchfirstbin = 0, base = 1;
	char tempData[5],tempData1[5], tempDicData[33];

	createBitMaskstr();

	//for(i=0;i<16;i++)
	//	printf("---->:%s\n",bitMaskSturct[i].mask);
    i=0;
	while(i<8 && bitMaskFlag != 1)
	//for(i=0;i<8;i++)
	{
		strcpy(tempDicData,"00000000000000000000000000000000");
		noOfone = 0;
		misMatchfirst = 0;
		for(j=0;j<32;j++) // check for last 3 bit mismatch
		{
			if((int)dictEntries[i].dicInstr[j] == (int)instructions[inputpc].instruction[j])
				tempDicData[j] = '0';
			else
			{
				tempDicData[j] = '1';
				noOfone += 1;

				if(noOfone == 1)
					misMatchfirst = j;
				misMatchlast = j;
			}
		}
		if(noOfone <= 4 && (misMatchlast - misMatchfirst) < 4)
		{

			strcpy(compInstructions[comprpc].compAlgoindex,"001");

			if(misMatchfirst < 2)
			{
				strcpy(tempData1,"0000");
				sprintf(tempData,"%d",misMatchfirst);
				strcat(tempData1, tempData);
				strcat(compInstructions[comprpc].compInstr,tempData1);
			}
			else if(misMatchfirst < 4)
			{
				strcpy(tempData1,"000");
				misMatchfirstbin = converttoBin(misMatchfirst);
				sprintf(tempData,"%d",misMatchfirstbin);
				strcat(tempData1, tempData);
				strcat(compInstructions[comprpc].compInstr,tempData1);
			}
			else if(misMatchfirst < 8)
			{
				strcpy(tempData1,"00");
				misMatchfirstbin = converttoBin(misMatchfirst);
				sprintf(tempData,"%d",misMatchfirstbin);
				strcat(tempData1, tempData);
				strcat(compInstructions[comprpc].compInstr,tempData1);
			}
			else if(misMatchfirst < 16)
			{
				strcpy(tempData1,"0");
				misMatchfirstbin = converttoBin(misMatchfirst);
				sprintf(tempData,"%d",misMatchfirstbin);
				strcat(tempData1, tempData);
				strcat(compInstructions[comprpc].compInstr,tempData1);
			}
			else
			{
				misMatchfirstbin = converttoBin(misMatchfirst);
				sprintf(compInstructions[comprpc].compInstr,"%d",misMatchfirstbin);
			}
			bitMaskCountDec = 0;
			base = 1;
			for(j=3;j>=0;j--)
			{
				bitMaskCountDec += ((int)tempDicData[j+misMatchfirst] - 48) * base;
				base = base*2;
			}
			strcat(compInstructions[comprpc].compInstr,bitMaskSturct[bitMaskCountDec].mask);
			strcat(compInstructions[comprpc].compInstr,dictEntries[i].index);
			bitMaskFlag = 1;
			pc++;
		}
		if(bitMaskFlag == 1)
			comprpc++;
		if(bitMaskFlag == 1)
			break;
	i++;
	}

}

void twoaMismatch(int inputpc)
{
	int i,j,bitsMisnum1,bitsMisnum2, bitsMisbit,misCount=0, misFlag1 = 0;
	char compData[6], tempData[6];
	//char inputInstr[33],checkInstr[33];

	for(i=0;i<8;i++)
	{
		//printf("--------Instr:%s\n",instructions[inputpc].instruction);
		//printf("-----dicInstr:%s\n",dictEntries[i].dicInstr);

		for(j=0;j<32;j++)
		{
            //printf("instr:%d\n",(int)instructions[pc].instruction[j]);
            //printf("instr:%d\n",(int)dictEntries[i].dicInstr[j]);
            if( ( int)instructions[inputpc].instruction[j] != (int)dictEntries[i].dicInstr[j] )

			{
				if(misFlag1 == 0)
				{
					bitsMisnum1 = j;
					misFlag1 = 1;
				}
				if(misFlag1 == 1 && misCount == 1)
					bitsMisnum2 = j;
				misCount++;
			}
		}
		if(misCount==2)
		{
			strcpy(compInstructions[comprpc].compAlgoindex,"100");
			//bitsMisbit = converttoBin(bitsMisnum);
			//printf("bitsMisbit1:%d",bitsMisnum1);
			//printf("bitsMisbit2:%d",bitsMisnum2);
			if(bitsMisnum1 < 2)
			{
				strcpy(compData,"0000");
				sprintf(tempData,"%d",bitsMisnum1);
				strcat(compData,tempData);
			}
			else if(bitsMisnum1 < 4)
			{
				strcpy(compData,"000");
				bitsMisbit = converttoBin(bitsMisnum1);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else if(bitsMisnum1 < 8)
			{
				strcpy(compData,"00");
				bitsMisbit = converttoBin(bitsMisnum1);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else if(bitsMisnum1 < 16)
			{
				strcpy(compData,"0");
				bitsMisbit = converttoBin(bitsMisnum1);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else
			{
				bitsMisbit = converttoBin(bitsMisnum1);
				sprintf(compData,"%d",bitsMisbit);
			}

			strcpy(compInstructions[comprpc].compInstr,compData);//-----------------------------


			if(bitsMisnum2 < 2)
			{
				strcpy(compData,"0000");
				sprintf(tempData,"%d",bitsMisnum2);
				strcat(compData,tempData);
			}
			else if(bitsMisnum2 < 4)
			{
				strcpy(compData,"000");
				bitsMisbit = converttoBin(bitsMisnum2);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else if(bitsMisnum2 < 8)
			{
				strcpy(compData,"00");
				bitsMisbit = converttoBin(bitsMisnum2);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else if(bitsMisnum2 < 16)
			{
				strcpy(compData,"0");
				bitsMisbit = converttoBin(bitsMisnum2);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else
			{
				bitsMisbit = converttoBin(bitsMisnum2);
				sprintf(compData,"%d",bitsMisbit);
			}

			strcat(compInstructions[comprpc].compInstr,compData);//-----------------------------
			strcpy(compInstructions[comprpc].index,dictEntries[i].index);
			comprpc++;
			pc++;
			twoaMismatchFlag = 1;
			break;
		}
		misCount = 0;
		bitsMisnum1 = 0;
		misFlag1 = 0;
		bitsMisnum2 = 0;
	}
}

void twocMismatch(int inputpc)
{
	int i,j,bitsMisnum, bitsMisbit,misCount=0, totalMisCount = 0;
	char compData[6], tempData[6];
	//char inputInstr[33],checkInstr[33];

	for(i=0;i<8;i++)
	{
		//printf("--------Instr:%s\n",instructions[inputpc].instruction);
		//printf("-----dicInstr:%s\n",dictEntries[i].dicInstr);

		for(j=1;j<32;j++)
		{
            //printf("instr:%d\n",(int)instructions[pc].instruction[j]);
            //printf("instr:%d\n",(int)dictEntries[i].dicInstr[j]);
            if( ( (int)instructions[inputpc].instruction[j] != (int)dictEntries[i].dicInstr[j] ) &&
			  	( (int)instructions[inputpc].instruction[j-1] != (int)dictEntries[i].dicInstr[j-1] )
			  )
			{
				bitsMisnum = j-1;
				misCount++;
			}
			if((int)instructions[inputpc].instruction[j] != (int)dictEntries[i].dicInstr[j] )
				totalMisCount += 1;

		}
		if(misCount==1 && totalMisCount == 2)
		{
			strcpy(compInstructions[comprpc].compAlgoindex,"011");
			//bitsMisbit = converttoBin(bitsMisnum);
			//printf("bitsMisbit:%d",bitsMisnum);
			if(bitsMisnum < 2)
			{
				strcpy(compData,"0000");
				sprintf(tempData,"%d",bitsMisnum);
				strcat(compData,tempData);
			}
			else if(bitsMisnum < 4)
			{
				strcpy(compData,"000");
				bitsMisbit = converttoBin(bitsMisnum);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else if(bitsMisnum < 8)
			{
				strcpy(compData,"00");
				bitsMisbit = converttoBin(bitsMisnum);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else if(bitsMisnum < 16)
			{
				strcpy(compData,"0");
				bitsMisbit = converttoBin(bitsMisnum);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else
			{
				bitsMisbit = converttoBin(bitsMisnum);
				sprintf(compData,"%d",bitsMisbit);
			}
			strcpy(compInstructions[comprpc].compInstr,compData);//-----------------------------
			strcpy(compInstructions[comprpc].index,dictEntries[i].index);
			comprpc++;
			pc++;
			twocMismatchFlag = 1;
			break;
		}
		bitsMisnum = 0;
		bitsMisbit = 0;
		misCount = 0;
		totalMisCount = 0;

	}
}

void oneMismatch(int inputpc)
{
	int i,j,bitsMisnum, bitsMisbit,misCount=0;
	char compData[6], tempData[6];
	//char inputInstr[33],checkInstr[33];

	for(i=0;i<8;i++)
	{
		//printf("--------Instr:%s\n",instructions[inputpc].instruction);
		//printf("-------dicInstr:%s\n",dictEntries[i].dicInstr);
	//strcpy(instructions[pc].instruction,inputInstr);
		//strcpy(dictEntries[i].dicInstr,checkInstr);

		for(j=0;j<32;j++)
		{
            //printf("instr:%d\n",(int)instructions[pc].instruction[j]);
            //printf("instr:%d\n",(int)dictEntries[i].dicInstr[j]);
            if((int)instructions[inputpc].instruction[j] != (int)dictEntries[i].dicInstr[j] )
			{
				bitsMisnum = j;
				misCount++;
			}
		}
		if(misCount==1)
		{
			strcpy(compInstructions[comprpc].compAlgoindex,"010");
			//bitsMisbit = converttoBin(bitsMisnum);
			//printf("bitsMisbit:%d",bitsMisnum);
			if(bitsMisnum < 2)
			{
				strcpy(compData,"0000");
				sprintf(tempData,"%d",bitsMisnum);
				strcat(compData,tempData);
			}
			else if(bitsMisnum < 4)
			{
				strcpy(compData,"000");
				bitsMisbit = converttoBin(bitsMisnum);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else if(bitsMisnum < 8)
			{
				strcpy(compData,"00");
				bitsMisbit = converttoBin(bitsMisnum);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else if(bitsMisnum < 16)
			{
				strcpy(compData,"0");
				bitsMisbit = converttoBin(bitsMisnum);
				sprintf(tempData,"%d",bitsMisbit);
				strcat(compData,tempData);
			}
			else
			{
				bitsMisbit = converttoBin(bitsMisnum);
				sprintf(compData,"%d",bitsMisbit);
			}
			strcpy(compInstructions[comprpc].compInstr,compData);//-----------------------------
			strcpy(compInstructions[comprpc].index,dictEntries[i].index);
			comprpc++;
			pc++;
			oneMismatchFlag = 1;
			break;
		}
		bitsMisnum = 0;
		bitsMisbit = 0;
		misCount = 0;

	}
}

void directoryMatching(int inputpc)
{
	int i;
	for(i=0;i<8;i++)
	{
		if(strcmp(instructions[inputpc].instruction, dictEntries[i].dicInstr) == 0)
		{
			strcpy(compInstructions[comprpc].compAlgoindex,"101");
			strcpy(compInstructions[comprpc].compInstr,dictEntries[i].index);//-----------------------------
			comprpc++;
			pc++;
			directoryMatchingFlag = 1;
			break;
		}
	}
}

void RLE(int inputpc)
{
	int i, rleCount = -1;
	char compData[3];
	if(inputpc == 0)
	{
		RLEFlag = 0;
	}
	else
	{
		for(i=inputpc;i<no_of_instructions && rleCount < 3;i++)
		{
			if(strcmp(instructions[i].instruction, instructions[i-1].instruction) == 0)
			{
				rleCount++;
				rleFlag = 1;
			}
			else
				break;
		}
		if(rleCount != -1)
		{
			strcpy(compInstructions[comprpc].compAlgoindex,"000");
			switch(rleCount)
			{
				case 0:
					strcpy(compData,"00");
					break;
				case 1:
					strcpy(compData,"01");
					break;
				case 2:
					strcpy(compData,"10");
					break;
				case 3:
					strcpy(compData,"11");
			}
			strcpy(compInstructions[comprpc].compInstr,compData);//-----------------------------
			pc = pc + rleCount + 1;
			rleCount = -1;
			rleFlag = 0;
			RLEFlag = 1;
			comprpc++;
		}
	}
}

void printComp()
{
	int i;
	for(i=0;i<comprpc;i++)
		printf("i:%d\tAlgo index :%s\tcompressed code :%s\tdicIndex:%s\n",i,compInstructions[i].compAlgoindex,compInstructions[i].compInstr,compInstructions[i].index);
		//printf("i:%d\tAlgo index :%s\tcompressed code :%s\tdicIndex:%s\n",i,compInstructions[i].compAlgoindex,compInstructions[i].compInstr,compInstructions[i].index);
}

void createDict()
{
    int i,j, equalFlag;

    // to create dictionary
	for(i=0;i<no_of_instructions;i++)
	{
		element[i].index = i;
        element[i].count = 0;
        strcpy(element[i].val,instructions[i].instruction);
		element[i].decVal = instructions[i].dec;

	}

//	for(i=0;i<no_of_instructions;i++)
//		printf("Index:%d\tCount:%d\tValue:%s\n",element[i].index,element[i].count,element[i].val);
//	printf("**********************************************************************************\n");
/*
	for(i = 0; i < no_of_instructions; i++)
	{
		for (j = 0 ; j < no_of_instructions - i - 1 ; j++)
		{
			if ( element[j].decVal < element[j+1].decVal)
			{
				temp = element[j];
				element[j] = element[j+1];
				element[j+1] = temp;
			}
		}
	}
*/
	//printf("Sorted instructions:\n");
	//for(i=0;i<no_of_instructions;i++)
	//	printf("Index:%d\tCount:%d\tValue:%s\tdec:%lu\n",element[i].index,element[i].count,element[i].val,element[i].decVal);

	/* initialize count of first element as 1 */
    element[0].count = 1;

	/* Count occurrences of remaining elements */
    for (i = 0 ; i < no_of_instructions; i++)
    {
		for(j=i+1;j < no_of_instructions;j++)
		{
			if (element[i].decVal == element[j].decVal)
			{
				element[i].count += element[j].count+1;

				/* Set count of previous element as -1 , we are
				   doing this because we'll again sort on the
				   basis of counts (if counts are equal than on
				   the basis of index)*/
				element[j].count = -1;

				/* Retain the first index (Remember first index
				   is always present in the first duplicate we
				   used stable sort. */
				//element[i].index = element[i-1].index;
				//break;
				equalFlag = 1;
			}

			/* Else If previous element is not equal to current
			  so set the count to 1 */
		}
		if(equalFlag != 1)
            element[i].count = 1;
		equalFlag = 0;
    }

    //printf("Sorted instructions............2:\n");
	//for(i=0;i<no_of_instructions;i++)
	//{
	//	printf("Index:%d\tCount:%d\tValue:%s\tdec:%lu\n",element[i].index,element[i].count,element[i].val,element[i].decVal);
	//}
	//printf("**********************************************************************************\n");

	for(i = 0; i < no_of_instructions; i++)
	{
		for (j = 0 ; j < no_of_instructions - i - 1 ; j++)
		{
			if ( element[j].count < element[j+1].count)
			{
				temp = element[j];
				element[j] = element[j+1];
				element[j+1] = temp;
			}
		}
	}

	//printf("Sorted instructions............2:\n");
	//for(i=0;i<no_of_instructions;i++)
	//{
	//	printf("Index:%d\tCount:%d\tValue:%s\tdec:%lu\n",element[i].index,element[i].count,element[i].val,element[i].decVal);
	//}
	//printf("**********************************************************************************\n");

	strcpy(dictEntries[0].index,"000");
	strcpy(dictEntries[1].index,"001");
	strcpy(dictEntries[2].index,"010");
	strcpy(dictEntries[3].index,"011");
	strcpy(dictEntries[4].index,"100");
	strcpy(dictEntries[5].index,"101");
	strcpy(dictEntries[6].index,"110");
	strcpy(dictEntries[7].index,"111");

	for(i=0;i<8;i++)
	{
		strcpy(dictEntries[i].dicInstr,element[i].val);
		dictEntries[i].dicVal = element[i].decVal;
	}

	//for(i=0;i<8;i++)
	//	printf("Index:%s\tdicInstr:%s\n",dictEntries[i].index, dictEntries[i].dicInstr);
        //printf("Index:%s\tdicInstr:%s\tdicVal:%lu\n",dictEntries[i].index, dictEntries[i].dicInstr, dictEntries[i].dicVal);

	//printf("**********************************************************************************\n");

}
void compressedFile()
{
    FILE *instrFile;
    char str[MAXCHAR], *token;
    int dicFlag = 0;

    // dictionary index
    strcpy(dictEntries[0].index,"000");
	strcpy(dictEntries[1].index,"001");
	strcpy(dictEntries[2].index,"010");
	strcpy(dictEntries[3].index,"011");
	strcpy(dictEntries[4].index,"100");
	strcpy(dictEntries[5].index,"101");
	strcpy(dictEntries[6].index,"110");
	strcpy(dictEntries[7].index,"111");

    instrFile = fopen("compressed.txt","r");
	while(fgets(str, 40, instrFile)!=NULL )
    {
		token=strtok(str, "\n\r");
		if(dicFlag == 1)
		{
		    strcpy(dictEntries[decompDicno].dicInstr, token);
            decompDicno++;
		}
        if(strcmp(token, "xxxx") == 0)
            dicFlag = 1;

        if(dicFlag == 0)
        {
            strcpy(ip_compr_instructions[no_of_ip_compr_instructions].instruction,token);
            no_of_ip_compr_instructions = no_of_ip_compr_instructions + 1;
        }

	}
	fclose(instrFile);
}
void decompressedFile()
{
    int base,j,i=0;
    char str[MAXCHAR], *token;
    FILE *instrFile;

    instrFile = fopen("original.txt","r");
	while(fgets(str, 40, instrFile)!=NULL )
    {
		token=strtok(str, "\n\r");
        strcpy(instructions[no_of_instructions].instruction,token);

		instructions[i].dec = 0;
		base = 1;
		for(j=31;j>=0;j--)
		{
			instructions[i].dec += ((int)instructions[no_of_instructions].instruction[j] - 48 ) * base;
			base = base*2;
        }
		i++;
        no_of_instructions = no_of_instructions + 1;
	}
	fclose(instrFile);

	//printf("No of instructions : %d\n", no_of_instructions);
}

int main(int argc,char *argv[])
//int main()
{
    int i =0, j=0, base, dicIndex = 0, dec_to_ori_pc = 0, misMatch1=0, bitMaskMisLoc = 0, bitMaskIndex = 0;
    char compr_code_lines = 0, pad_ones = 0, bits_in_last_line = 0, dec_file_over_flag = 0, algono = 0;
	char compressed_code[100000], decompressed_code[100000],last_line[33], decAlgoIndex[4]="", tempDicData[33];
	FILE *outputFile;

	if(strcmp(argv[1],"1")==0)
    {
    // to store original file
        decompressedFile();
    // to create dictionary
        createDict();
    // to perform compression
        while(pc < no_of_instructions)
        {

            RLEFlag = 0;
            directoryMatchingFlag = 0;
            oneMismatchFlag = 0;
            twocMismatchFlag = 0;
            twoaMismatchFlag = 0;
            bitMaskFlag = 0;

            RLE(pc);
            if(RLEFlag == 1)
                continue;
            // repeating instr > 5??????????
            directoryMatching(pc);
            if(directoryMatchingFlag == 1)
                continue;

            oneMismatch(pc);
            if(oneMismatchFlag == 1)
                continue;

            twocMismatch(pc);
            if(twocMismatchFlag == 1)
                continue;

            bitMask(pc);
            if(bitMaskFlag == 1)
                continue;

            twoaMismatch(pc);
            if(twoaMismatchFlag == 1)
                continue;

            strcpy(compInstructions[comprpc].compAlgoindex,"110");
            strcpy(compInstructions[comprpc].compInstr,instructions[pc].instruction);
            comprpc++;
            pc++;

        }
    // logic to write data in 32 bit format
        for(i=0;i<comprpc;i++)
        {
            strcat(compressed_code, compInstructions[i].compAlgoindex);
            strcat(compressed_code, compInstructions[i].compInstr);
            if(compInstructions[i].index != NULL)
                strcat(compressed_code, compInstructions[i].index);
        }

        compr_code_lines = strlen(compressed_code)/32;
        pad_ones = 32 - strlen(compressed_code)%32;

        //printf("compressed_code:\n");
        outputFile = fopen("cout.txt","w");
        for(i=0;i<compr_code_lines;i++)
        {
            for(j=i*32;j<(i+1)*32;j++)
                fprintf(outputFile,"%c",compressed_code[j]);
                //printf("%c",compressed_code[j]);
            fprintf(outputFile,"\n");
            //printf("\n");
        }

        if(pad_ones != 32)
        {
            bits_in_last_line = strlen(compressed_code) - compr_code_lines*32;
            for(i=0;i<bits_in_last_line;i++)
                last_line[i] = compressed_code[i+compr_code_lines*32];
            for(i=0;i<pad_ones;i++)
                last_line[bits_in_last_line+i] = '1';
            for(i=0;i<32;i++)
                fprintf(outputFile,"%c",last_line[i]);
                //printf("%c",last_line[i]);
            fprintf(outputFile,"\n");
            //printf("\n");
        }
        fprintf(outputFile,"xxxx\n");
        //printf("xxxx\n");
        for(i=0;i<8;i++)
            fprintf(outputFile,"%s\n",dictEntries[i].dicInstr);
            //printf("%s\n",dictEntries[i].dicInstr);
        fclose(outputFile);
    }
	else if(strcmp(argv[1],"2")==0)
    {
        compressedFile();
        // to perform decompression bits are stored in decompressed_code
        for(i=0;i<no_of_ip_compr_instructions;i++)
            strcat(decompressed_code,ip_compr_instructions[i].instruction);

        //printf("code:%s\n",decompressed_code);

        while(dec_file_over_flag!=1)
        //while(decpc < 500)
        {
            //to obtain compression algorithm apllied to original input
            i = 0;
            while(i<3)
            {
                decAlgoIndex[i] = decompressed_code[decpc];
                i++;
                decpc++;
            }
            //to obtain algorithm number in decimal
            base = 1;
            for(i=2;i>=0;i--)
            {
                algono += ((int)decAlgoIndex[i] - 48 ) * base;
                base = base*2;
            }
            //to avoid printing last line if algono == 7
            if(algono != 7)
                strcpy(decompInstr[no_of_decompr_instructions].compAlgoindex, decAlgoIndex);
            decompInstr[no_of_decompr_instructions].compAlgo = algono;

            switch(algono)
            {
                case 0: // RLE
                    decompInstr[no_of_decompr_instructions].compInstr[0] = decompressed_code[decpc];
                    decompInstr[no_of_decompr_instructions].compInstr[1] = decompressed_code[decpc+1];
                    decpc += 2;
                    break;
                case 1: // decompression of bitmask-based compression (algo:loc(5bits):mask(4bits):dic(3bits))
                    for(i=0;i<12;i++)
                        decompInstr[no_of_decompr_instructions].compInstr[i] = decompressed_code[decpc+i];
                    decpc += 12;
                    break;
                case 2: // decompression of  1 bit Mismatch based compression (algo:loc(5bits):dic(3bits))
                    for(i=0;i<8;i++)
                        decompInstr[no_of_decompr_instructions].compInstr[i] = decompressed_code[decpc+i];
                    decpc += 8;
                    break;
                case 3:// decompression of  2 bit consecutive Mismatch based compression (algo:loc(5bits):dic(3bits))
                    for(i=0;i<8;i++)
                        decompInstr[no_of_decompr_instructions].compInstr[i] = decompressed_code[decpc+i];
                    decpc += 8;
                    break;
                case 4:// decompression of  2 bit anywhere Mismatch based compression (algo:loc1(5bits):loc2(5bits):dic(3bits))
                    for(i=0;i<13;i++)
                        decompInstr[no_of_decompr_instructions].compInstr[i] = decompressed_code[decpc+i];
                    decpc += 13;
                    break;
                case 5:// decompression of Direct Matching compression
                    for(i=0;i<3;i++)
                        decompInstr[no_of_decompr_instructions].compInstr[i] = decompressed_code[decpc+i];
                    decpc += 3;
                    break;
                case 6://  Original Binaries
                    for(i=0;i<32;i++)
                        decompInstr[no_of_decompr_instructions].compInstr[i] = decompressed_code[decpc+i];
                    decpc += 32;
                    break;
                default:
                    dec_file_over_flag = 1;
            }
            if(algono != 7)
                no_of_decompr_instructions = no_of_decompr_instructions + 1;
            algono = 0;
        }

    //-------------------------------------------------------------------------------------------------------------------
    // intnermediate to original file
    //-------------------------------------------------------------------------------------------------------------------


        for(i=0;i<no_of_decompr_instructions;i++)
        {
            dicIndex = 0;
            misMatch1 = 0;
            bitMaskMisLoc = 0;
            bitMaskIndex = 0;
            switch(decompInstr[i].compAlgo)
            {
                case 0:// RLE

                    //to obtain dictionary index
                    base = 1;
                    for(j=1;j>=0;j--)
                    {
                        dicIndex += ((int)decompInstr[i].compInstr[j] - 48 ) * base;
                        base = base*2;
                    }
                    // to print repreating instructions
                    for(j=0;j<dicIndex+1;j++)
                    {
                        strcpy(dec_to_original[dec_to_ori_pc].instruction,dec_to_original[dec_to_ori_pc-1].instruction);
                        if(j < dicIndex)
                            dec_to_ori_pc += 1;
                    }
                    break;

                case 1:// decompression of bitmask-based compression (algo:loc(5bits):mask(4bits):dic(3bits))

                    createBitMaskstr();
                    // to obtain mismatch starting location
                    base = 1;
                    for(j=4;j>=0;j--)
                    {
                        bitMaskMisLoc += ((int)decompInstr[i].compInstr[j] - 48 ) * base;
                        base = base*2;
                    }
                    // to obtain bitmask index
                    base = 1;
                    for(j=3;j>=0;j--)
                    {
                        bitMaskIndex += ((int)decompInstr[i].compInstr[j+5] - 48 ) * base;
                        base = base*2;
                    }
                    // to obtain dictionary index
                    base = 1;
                    for(j=2;j>=0;j--)
                    {
                        dicIndex += ((int)decompInstr[i].compInstr[j+9] - 48 ) * base;
                        base = base*2;
                    }
                    // to obtain original instruction
                    strcpy(tempDicData,dictEntries[dicIndex].dicInstr);
                    //apply bitmask from mimatch location with applicable mask
                    for(j=0;j<4;j++)
                    {
                        if(	(int)tempDicData[j+bitMaskMisLoc] == 	(int)bitMaskSturct[bitMaskIndex].mask[j] )
                            tempDicData[j+bitMaskMisLoc] = '0';
                        else
                            tempDicData[j+bitMaskMisLoc] = '1';
                    }
                    strcpy(dec_to_original[dec_to_ori_pc].instruction,tempDicData);
                    break;

                case 2:// decompression of  1 bit Mismatch based compression (algo:loc(5bits):dic(3bits))

                    // to obtain dictionary index
                    base = 1;
                    for(j=2;j>=0;j--)
                    {
                        dicIndex += ((int)decompInstr[i].compInstr[j+5] - 48 ) * base;
                        base = base*2;
                    }
                    // to obtain mismatch starting location
                    base = 1;
                    for(j=4;j>=0;j--)
                    {
                        misMatch1 += ((int)decompInstr[i].compInstr[j] - 48 ) * base;
                        base = base*2;
                    }
                    strcpy(dec_to_original[dec_to_ori_pc].instruction,dictEntries[dicIndex].dicInstr);

                    if((int)dictEntries[dicIndex].dicInstr[misMatch1] == 48)
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1] = '1';
                    else
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1] = '0';

                    break;

                case 3:// decompression of  2 bit consecutive Mismatch based compression (algo:loc(5bits):dic(3bits))

                    // to obtain dictionary index
                    base = 1;
                    for(j=2;j>=0;j--)
                    {
                        dicIndex += ((int)decompInstr[i].compInstr[j+5] - 48 ) * base;
                        base = base*2;
                    }
                    base = 1;
                    // to obtain mismatch starting location
                    for(j=4;j>=0;j--)
                    {
                        misMatch1 += ((int)decompInstr[i].compInstr[j] - 48 ) * base;
                        base = base*2;
                    }
                    strcpy(dec_to_original[dec_to_ori_pc].instruction,dictEntries[dicIndex].dicInstr);

                    if((int)dictEntries[dicIndex].dicInstr[misMatch1] == 48)
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1] = '1';
                    else
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1] = '0';
                    if((int)dictEntries[dicIndex].dicInstr[misMatch1+1] == 48)
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1+1] = '1';
                    else
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1+1] = '0';
                    break;

                case 4:// decompression of  2 bit anywhere Mismatch based compression (algo:loc1(5bits):loc2(5bits):dic(3bits))

                    // to obtain dictionary index
                    base = 1;
                    for(j=2;j>=0;j--)
                    {
                        dicIndex += ((int)decompInstr[i].compInstr[j+10] - 48 ) * base;
                        base = base*2;
                    }
                    strcpy(dec_to_original[dec_to_ori_pc].instruction,dictEntries[dicIndex].dicInstr);
                    //for 1st mismatch
                    base = 1;
                    for(j=4;j>=0;j--)
                    {
                        misMatch1 += ((int)decompInstr[i].compInstr[j+5] - 48 ) * base;
                        base = base*2;
                    }
                    if((int)dictEntries[dicIndex].dicInstr[misMatch1] == 48)
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1] = '1';
                    else
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1] = '0';
                    //for 2nd mismatch
                    misMatch1 = 0;
                    base = 1;
                    for(j=4;j>=0;j--)
                    {
                        misMatch1 += ((int)decompInstr[i].compInstr[j] - 48 ) * base;
                        base = base*2;
                    }
                    if((int)dictEntries[dicIndex].dicInstr[misMatch1] == 48)
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1] = '1';
                    else
                        dec_to_original[dec_to_ori_pc].instruction[misMatch1] = '0';

                    break;
                case 5:// decompression of Direct Matching compression

                    // to obtain dictionary index
                    base = 1;
                    for(j=2;j>=0;j--)
                    {
                        dicIndex += ((int)decompInstr[i].compInstr[j] - 48 ) * base;
                        base = base*2;
                    }
                    strcpy(dec_to_original[dec_to_ori_pc].instruction,dictEntries[dicIndex].dicInstr);
                    break;

                case 6://  Original Binaries
                    strcpy(dec_to_original[dec_to_ori_pc].instruction,decompInstr[i].compInstr);
                    break;
            }
            dec_to_ori_pc += 1;
        }
        outputFile = fopen("dout.txt","w");
        //printf("\noriginal file:\n");
        for(i=0;i<dec_to_ori_pc;i++)
            //printf("i:%d\tdec_to_original:%s\n",i,dec_to_original[i].instruction);
            fprintf(outputFile,"%s\n",dec_to_original[i].instruction);
            //printf("%s\n",dec_to_original[i].instruction);
		fclose(outputFile);

    }
    else
        printf("wrong input\n");



	return 0;
}

