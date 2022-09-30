/***************************************************************************//**
  @file     data_manager.h
  @brief    provides functions to send coded data to the pc
  @author   Pedro DL
 ******************************************************************************/

#ifndef _DATA_MANAGER_
#define _DATA_MANAGER_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MESSAGE_LENGHT 18   //SS+G+RPY+ES   
#define M_GROUP 1
#define M_ROLL 2
#define M_PITCH 7
#define M_YAW 12

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



typedef struct{
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
}board;			//board meassures

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief initializes data management functions
 */
void dataManager_init(void);

/**
 * @brief returns the matrix with all the board values
 * @return A pointer to an board type matrix
 */
board getBoard(uint8_t group);




/**
 * @brief sends the data that changed to the pc via UART protocol
 * @param current_board the board being modified with it's current values
 * @param group group number of such board
 */
void sendData (board current_board, uint8_t group);


/*******************************************************************************
 ******************************************************************************/

#endif // _DATA_MANAGER_
