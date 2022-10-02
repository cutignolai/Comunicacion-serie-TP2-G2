/***************************************************************************//**
  @file     can.h
  @brief    CAN management
  @author   Ignacio Cutignola
 ******************************************************************************/

#ifndef CAN_H_
#define CAN_H_

/////////////////////////////////////////////////////////////////////////////////
//                             Included header files                           //
/////////////////////////////////////////////////////////////////////////////////
#include "stdbool.h"
#include "stdint.h"
#include "MK64F12.h"


/////////////////////////////////////////////////////////////////////////////////
//                    Enumerations, structures and typedefs                    //
/////////////////////////////////////////////////////////////////////////////////

typedef struct{
	uint32_t ID;
	uint32_t length;
	union{
		struct{
			uint32_t dataWord0;
			uint32_t dataWord1;
		};
		struct{
			uint8_t dataByte0;
			uint8_t dataByte1;
			uint8_t dataByte2;
			uint8_t dataByte3;
			uint8_t dataByte4;
			uint8_t dataByte5;
			uint8_t dataByte6;
			uint8_t dataByte7;
		};
		uint8_t data[8];
	};
}CAN_DataFrame;



/////////////////////////////////////////////////////////////////////////////////
//                         Global function prototypes                          //
/////////////////////////////////////////////////////////////////////////////////

/*
 * @brief Funcion que inicializa la CAN
 * @param puntero que contiene la configuracion del CAN
 * @param clockHz Protocol Engine clock source frequency in Hz.
 */
void CAN_Init( void );


/**
 * @brief Configure a message buffer for receiving.
 * @param base CAN peripheral base address
 * @param index Number of message buffer.
 * @param config Pointer to struct containing MB Rx configuration
 */
void  CAN_ConfigureRxMB(uint8_t index,uint32_t ID);


/*
 * @brief Read a frame from a message buffer.
 * @param base CAN peripheral base address
 * @param index Number of message buffer to read.
 * @param frame Pointer to frame to store received data.
 * @return
 */
bool CAN_ReadRxMB(uint8_t index, CAN_DataFrame * frame);

/*
 * @brief Write a frame to a message buffer to be sent.
 * @param base CAN peripheral base address
 * @param index Number of message buffer to write.
 * @param frame Pointer to frame to be sent.
 * @return
 */
void CAN_WriteTxMB(uint8_t index, CAN_DataFrame * frame);


#endif /* CAN_H_ */
