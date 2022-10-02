/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Pedro DL
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "data_manager.h"
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

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
      
    dataManager_init();

    //chequear si se tiene que inicializar el CAN

}

/* Función que se llama constantemente en un ciclo infinito */
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

    can_message_ptr = &message[0];              //get the pointer to the new message via CAN. It should be done for each can
    setBoard(can_message_ptr);                  //set the board ready to update         
    sendData(current_board, current_group);     //send board to pc  
}

    

void setBoard (char* ptr){
	//get the current board being used
	current_group = *(ptr + M_GROUP) - '0';

    //gets the group board
    current_board = getBoard(current_group);
    
    int16_t angle;

    int16_t c = *(ptr + M_ROLL + 2) - '0';
    int16_t d = *(ptr + M_ROLL + 3) - '0';
    int16_t u = *(ptr + M_ROLL + 4) - '0';

    angle = c*100 + d*10 + u;

    if ((*(ptr + M_ROLL + 1)) == '-')
        angle = -angle;

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
