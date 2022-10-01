/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
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

//testing
static uint8_t rand_event;
static bool position_event;
static bool data_event;
static board periferic;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


void GetPositionChange(void);
//updates the current board to the one indicated
void setBoard (void);

void updateBoard(void);

bool checkAngle (int16_t angle);

//testing
bool GetDataEvent(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
      
    dataManager_init();

    //testing
    rand_event = 0;
    position_event = false; 
    data_event = false;
    periferic.roll = 0;
    periferic.pitch = 0;
    periferic.yaw = 0;

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
    updateBoard();
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void updateBoard(void){

    if(GetDataEvent()){
        data_event = false;                     //turn off flag

        //codigo de testeo

        if (rand_event == 0){
        	char message1 []= "S2R+030E";
        	can_message_ptr = &message1[0];
        	rand_event++;
        }
        else if ((rand_event == 1) || (rand_event == 3)){
			char message2 []= "S2R+045E";
			can_message_ptr = &message2[0];
			rand_event++;
            if (rand_event == 4)
        	    rand_event = 0;
		}
        else if (rand_event == 2){
			char message3 []= "S2R+060E";
			can_message_ptr = &message3[0];
			rand_event++;
		}
        



        //can_message_ptr = &message[0];          //get the pointer to the new message
        setBoard();                             //set the board ready to update         
        sendData(current_board, current_group); //send board to pc  

    }
}

void setBoard (void){
	//get the current board being used
	current_group = *(can_message_ptr + M_GROUP) - '0';

    //gets the group board
    current_board = getBoard(current_group);
    
    int16_t angle;

    int16_t c = *(can_message_ptr + M_ROLL + 2) - '0';
    int16_t d = *(can_message_ptr + M_ROLL + 3) - '0';
    int16_t u = *(can_message_ptr + M_ROLL + 4) - '0';

    angle = c*100 + d*10 + u;

    if ((*(can_message_ptr + M_ROLL + 1)) == '-')
        angle = -angle;

    if ((*(can_message_ptr + M_ROLL)) == 'R'){
        if(checkAngle(angle)){
            current_board.roll = angle;
        }
    }
    else if ((*(can_message_ptr + M_ROLL)) == 'P'){
        if(checkAngle(angle)){
            current_board.pitch = angle;
        }
    }
    else if ((*(can_message_ptr + M_ROLL)) == 'Y'){
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

//testing functions

bool GetDataEvent(void){
	return true;
}

/*******************************************************************************
 ******************************************************************************/
