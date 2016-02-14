#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

int main()
{
    Instruction *head;
    Instruction *ptr, *prev, *next;

    head = ReadInstructionList(stdin);
    if (!head){
        ERROR("No instructions\n");
        exit(EXIT_FAILURE);
    }

    ptr = head;
    //ptr->critical = 'c'; //mark head as critical
    prev = head->prev;
    next = head->next;

    int imptRegNum[2048];

    int varOffset[14];

    while(ptr->next!=NULL) {               //go to the end
        ptr=ptr->next;
    }
    
    if(ptr->opcode!=OUTPUTAI)                     //if end statement is not an output, mark those statements dead and move to the first outputai from bottom.
            {
                while(ptr->opcode!=OUTPUTAI){
                ptr=ptr->prev;
                }       
            }

//----------------NOW YOU ARE AT THE VERY LAST OUTPUTAI INSTRUCTION, HAVING ELIMINATED THE GARBAGE AFTER THAT


    while(ptr){

        switch(ptr->opcode){
            case STOREAI:
                //if output register is not relevant, remove instruction
                if(varOffset[(ptr->field3)/4] == 1){  //if the output register is in the important array
                    ptr->critical = 'c'; //mark as critical and save other registers in instruction to the array
                    varOffset[(ptr->field3)/4] = 0;  //mark variable as present in offset array
                    // check if other register(s) are in the important array, else save them in
                    imptRegNum[ptr->field1] = 1;
                }
                break;

            case LOADI:
                //if output register is not relevant, remove instruction
                if(imptRegNum[ptr->field2] == 1){
                    ptr->critical = 'c'; //mark as critical and save other registers in instruction to the array
                    imptRegNum[ptr->field2] = 0; //mark as unimportant
                }
                break;

            case LOADAI:
                if(imptRegNum[ptr->field3] == 1 /*&& varOffset[(ptr->field2)/4] == 1*/){
                    ptr->critical = 'c'; //mark as critical and save other registers in instruction to the array
                    varOffset[(ptr->field2)/4] = 1;  //mark variable as present in offset array
                    // check if other register(s) are in the important array, else save them in
                    imptRegNum[ptr->field1] = 1;
                    imptRegNum[ptr->field3] = 0;
                }
                break;

            case SUB:
            case MUL:                
            case DIV:
            case ADD:
                //if output register is not relevant, remove instruction
                if(imptRegNum[ptr->field3] == 1){  //if the output register is in the important array
                    ptr->critical = 'c'; //mark as critical and save other registers in instruction to the array
                    // check if other register(s) are in the important array, else save them in
                    imptRegNum[ptr->field1] = 1;
                    imptRegNum[ptr->field2] = 1;
                    imptRegNum[ptr->field3] = 0; //mark as unimportant again
                }
                break;

            case OUTPUTAI:
                imptRegNum[ptr->field1] = 1;                                         
                varOffset[(ptr->field2)/4] = 1;                                     
                ptr->critical ='c';
                break;

            default:
                ERROR("Illegal instructions\n");

                }   



        ptr=ptr->prev;

    }


		ptr = head;
		next = ptr->next;

		while(ptr) {
			next = ptr->next;
			if (ptr->critical == 'c')
				PrintInstruction(stdout, ptr);
			free(ptr);								//free all nodes
			ptr = next;
		}
	return EXIT_SUCCESS;
}