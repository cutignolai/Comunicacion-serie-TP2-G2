/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "data_manager.h"

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

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

O_EVENT GetPositionEvent(void);   


void GetPositionChange(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
    
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void updateBoard(void){

    switch(GetPositionEvent()){
        case ROLL_EVENT:       
            current_board.roll = getRoll();
            SendData(current_board, ROLL_EVENT);
            break;
        case PITCH_EVENT:       
            current_board.pitch = getPitch();
            SendData(current_board, PITCH_EVENT);
            break;
        case YAW_EVENT:        
            current_board.yaw = getYaw();
            SendData(current_board, YAW_EVENT);
            break;
        default:        
            //do nothing
    }
}



/*******************************************************************************
 ******************************************************************************/
