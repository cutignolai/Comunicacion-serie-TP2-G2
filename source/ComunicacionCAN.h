/*******************************************************************************
  @file     ComunicacionCAN.h
  @author   Ignacio Cutignola
 ******************************************************************************/


#ifndef COMUNICACIONCAN_H_
#define COMUNICACIONCAN_H_

#define BASE_ID 0b00100000000
#define MY_BOARD_ID (BASE_ID + 2)

#define NOTHING_YET		0
#define MESSAGE_SENT	1
#define MESSAGE_RECIVE	2


/**
 * @brief Configuracion de modulo CAN
 */
void canComunicationInit(void);

void canMBInit(uint8_t id);


/**
 *  @brief Envia mensajes a traves de CAN
 *  @param message Message
 *  @param board   Number of board
 */
void sendCanMessage(char* message, int board);


/**
 *  @brief Recibe mensajes a traves de CAN
 *  @param message Message
 *  @param board   Number of board
 */
bool receiveCanMessage(char* message, int board);


/**
 *  @brief Indica si hay un mensaje disponible
 *  @param board   Number of board
 */
bool getBoardStatus(int board);


#endif //COMUNICACIONCAN_H_
