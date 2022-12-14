/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Pedro DL
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "data_manager.h"
#include "ComunicacionCAN.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

static board current_board;
static uint8_t current_group;
static char* can_message_ptr;
//flag for wrong CAN messages, like "SXEEXFSD"
static bool message_error;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

//updates the current board to the one indicated
void setBoard (char* ptr);

//updates a certain board and sends it to the pc
void updateBoard(void);

//checks the angle received is between the values allowed
bool checkAngle (int16_t angle);


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Funci??n que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
      
    dataManager_init();
	
	canComunicationInit();
	canMBInit(0);
	// canMBInit(1);
	// canMBInit(2);
	// canMBInit(3);
	// canMBInit(4);
	// canMBInit(5);

    // senCANmessage, receive message,
}

/* Funci??n que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
    //add function to check our own board

    //if necesary, send info to other boards via can

    updateBoard();      //check if there's info comming from other boards and update it
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void updateBoard(void){
    uint8_t i;
    for (i=0; i<6; i++){		// FIXME: TENER CUIDADO EL MAXIMO DEL FOR
        if(getBoardStatus(i) == 1){           //get the pointer to the new message via CAN. It should be done for each can
			char messageAux[] = "XXXXXXXX";
			receiveCanMessage(messageAux, i);

			// ANALIZO SI HUBO ERROR
			uint8_t j;
			for(j = 0; j<8; j++){
				if(messageAux[j] != 'X')
				{
					break;
				}
			}

			if(j == 8){
				//ERROR
			}
			else{
				can_message_ptr = &messageAux[0];   
            	setBoard(can_message_ptr);                  //set the board ready to update
                if (message_error == false){
                    sendData(current_board, current_group);     //send board to pc 
                }
                else{
                    message_error = false;                    //a problem ocurred, message wasn't send
                }         
            	
			} 
        }
    }
}

    

void setBoard (char* ptr){
	//get the current board being used
	current_group = *(ptr + M_GROUP) - '0';

    if (current_group >= 6){
        message_error = true;
    }    
    else if (current_group < 0){
        message_error = true;
    }
    
    if(message_error == false){
        
        //gets the group board
        current_board = getBoard(current_group);
        
        int16_t angle;

        int16_t c = *(ptr + M_ROLL + 2) - '0';
        int16_t d = *(ptr + M_ROLL + 3) - '0';
        int16_t u = *(ptr + M_ROLL + 4) - '0';

        angle = c*100 + d*10 + u;

        if ((*(ptr + M_ROLL + 1)) == '-'){
            angle = -angle;
        }
        else if ((*(ptr + M_ROLL + 1)) != '+'){
            message_error = true;
        }
        if ((*(ptr + M_ROLL)) == 'R'){
            if(checkAngle(angle)){
                current_board.roll = angle;
            }
        }
        else if ((*(ptr + M_ROLL)) == 'P'){
            if(checkAngle(angle)){
                current_board.pitch = angle;
            }
        }
        else if ((*(ptr + M_ROLL)) == 'Y'){
            if(checkAngle(angle)){
                current_board.yaw = angle;
            }
        }
        else{
            //do nothing
        }
    }
    
}

bool checkAngle (int16_t angle){
    if (angle > 180)
        return false;
    if (angle < -180)
        return false;
    else
        return true;
}

/*******************************************************************************
 ******************************************************************************/
