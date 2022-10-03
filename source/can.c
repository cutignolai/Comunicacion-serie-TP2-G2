#include "can.h"
#include "MK64F12.h"
#include "board.h"
#include "stdlib.h"



#define PIN_CAN_TX      DIO_5
#define PIN_CAN_RX      DIO_15
#define PIN_CAN_STB     DIO_14

#define TX_MB_INDEX 	0
#define RX_MB_INDEX 	1

#define TX_INACTIVE		0b1000
#define RX_EMPTY    	0b0100
#define TX_DATA			0b1100
#define RX_INACTIVE 	0b0000



void CAN_Init( void )
{

	/* 
	 * ------------------------------------------------
	 *
	 *		49.5.1 FlexCAN initialization sequence
	 * 
	 * ------------------------------------------------
	 */


    // Habilito el CLOCK (pag. 327)
	SIM->SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;


    //----------------------------------------------
    //  Module Configuration Registrer (CANx_MCR)
    //----------------------------------------------

    /* 
     * 49.4.10.2 MODULE DISABLE MODE
     * It is requested by the CPU through the assertion of 
     * the MDIS bit in the MCR Register and the acknowledgement is obtained through the 
     * assertion by the FlexCAN of the LPM_ACK bit in the same register.
     * 
     * Primero: Habilito el modulo y fuerzo un hard reset 
     * Segundo: Espero hasta que el modulO CAN module no este en low power mode.
     */
	CAN0->MCR &= ~CAN_MCR_MDIS_MASK;        // (0...010..0)
	while(CAN0->MCR & CAN_MCR_LPMACK_MASK); // (MCR[LPM_ACK] = 0)

	/*
     * SOFT RESET
     * When this bit is asserted, FlexCAN resets its internal state machines and some of the memory mapped
     * registers. The following registers are reset: MCR (except the MDIS bit), TIMER , ECR, ESR1, ESR2,
     * IMSK1, IMASK2, IFLAG1, IFLAG2 and CRCR.
     */
	CAN0->MCR |= CAN_MCR_SOFTRST(1);
	while((CAN0->MCR&CAN_MCR_SOFTRST_MASK)>> CAN_MCR_SOFTRST_SHIFT);    // Espero a que este listo

	/*
     * TX y RX
     * Se configura los puertos PTB18 y PTB19
     */
	uint32_t PCR = PORT_PCR_MUX(2) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; // REVISAR COMO CONFIGURAR BIEN EL PCR
	PORTB->PCR[18] = (uint32_t)0;
	PORTB->PCR[18] = PCR;
	PORTB->PCR[19] = (uint32_t)0;
	PORTB->PCR[19] = PCR;


    /* 
     * Necesito modificar el CAN Engine Clock Source CTRL1[CLK_SRC]
     * In order to guarantee reliable operation, this bit can be written only in Disable
     * mode because it is blocked by hardware in other modes.
     * 
     *-----------------------------------------
     * DESHABILITO EL MODULO CAN
     * ----------------------------------------
     */
	CAN0->MCR |= CAN_MCR_MDIS_MASK;
	while((CAN0->MCR & CAN_MCR_LPMACK_MASK)!=CAN_MCR_LPMACK_MASK);

	// Segun el modo en el que este configurado, se puede utilizar dos modos:
    //      Modo 1: Oscillator Clock  <------ Esta configurado en este modo
    //      Modo 2: Peripheral Clock
	CAN0->CTRL1 = CAN_CTRL1_CLKSRC(0);

    /* 
     *-----------------------------------------
     * VUELVO A HABILITAR EL MODULO CAN
     * ----------------------------------------
     */
	CAN0->MCR &= ~CAN_MCR_MDIS_MASK;        // (0...010..0)
	while(CAN0->MCR & CAN_MCR_LPMACK_MASK); // (MCR[LPM_ACK] = 0)


	/*
	 * For any configuration change/initialization it is required that
	 * FlexCAN is put into Freeze mode
	 */
	CAN0->MCR |= CAN_MCR_HALT_MASK;
	// Espero a que el modulo entre en Freeze mode 
	while((CAN0->MCR & CAN_MCR_FRZACK_MASK)!=CAN_MCR_FRZACK_MASK);  // (MCR[FRZ_ACK] = 1)

	
	
	/*
	 * Determine the bit timing parameters: PROPSEG, PSEG1, PSEG2, RJW
	 * Determine the bit rate by programming the PRESDIV field
	 * Determine the internal arbitration mode (LBUF bit)
	 * 
	 * Pagina 1460 y 1461
	 * Hago para un Baudrate de 125kbit/seg
	 */
	CAN0->CTRL1 |= CAN_CTRL1_PRESDIV(0x13);
	CAN0->CTRL1 |= CAN_CTRL1_PROPSEG(0x07);
	CAN0->CTRL1 |= CAN_CTRL1_PSEG1(0x07);
	CAN0->CTRL1 |= CAN_CTRL1_PSEG2(0x02);

	/*
	 * Initialize the Rx Individual Mask Registers
	 * RXIMR are used as acceptance masks for ID filtering in Rx MBs
	 */
	CAN0->MCR |= CAN_MCR_IRMQ_MASK;

	// Salgo del Freeze mode
	CAN0->MCR &= ~CAN_MCR_HALT_MASK;
	// Espero a que el modulo entre en Freeze mode 
	while((CAN0->MCR & CAN_MCR_FRZACK_MASK)==CAN_MCR_FRZACK_MASK);	// (MCR[FRZ_ACK] = 0).

}

void  CAN_ConfigureRxMB( uint8_t index, uint32_t ID)
{
	/*
	 *
	 *					49.4.3 Receive process
	 * To be able to receive CAN frames into a Mailbox, the CPU must 
	 * prepare it for reception by executing the following steps:	
	 * 
	 */

	// 1. If the Mailbox is active (either Tx or Rx) inactivate the Mailbox, preferably with a safe inactivation
	CAN0->MB[index].CS = ( CAN0->MB[index].CS &= ~CAN_CS_CODE_MASK ) | CAN_CS_CODE(RX_INACTIVE);

	// 2. Write the ID word
	CAN0->MB[index].ID = CAN_ID_STD(ID);

	// 3. Write the EMPTY code (0b0100) to the CODE field of the Control and Status word to activate the Mailbox.
	CAN0->MB[index].CS = CAN_CS_CODE(RX_EMPTY) | CAN_CS_IDE(0);

}

void CAN_WriteTxMB(uint8_t index, CAN_DataFrame * frame)
{
	/*
	 * -----------------------------------------
	 *			49.4.1 Transmit process
	 * -----------------------------------------
	 */

	// Write INACTIVE code (0b1000) to the CODE field to inactivate the MB but then the pending frame may be transmitted without notification.
	CAN0->MB[index].CS = CAN_CS_CODE(TX_INACTIVE);

	// Write the ID word.
	CAN0->MB[index].ID = CAN_ID_STD(frame->ID);

	// Write the data bytes.
	CAN0->MB[index].WORD0 = CAN_WORD0_DATA_BYTE_0(frame->dataByte0) |
							CAN_WORD0_DATA_BYTE_1(frame->dataByte1) |
							CAN_WORD0_DATA_BYTE_2(frame->dataByte2) |
							CAN_WORD0_DATA_BYTE_3(frame->dataByte3);
	CAN0->MB[index].WORD1 = CAN_WORD1_DATA_BYTE_4(frame->dataByte4) |
							CAN_WORD1_DATA_BYTE_5(frame->dataByte5) |
							CAN_WORD1_DATA_BYTE_6(frame->dataByte6) |
							CAN_WORD1_DATA_BYTE_7(frame->dataByte7);

	// Write the DLC and CODE fields of the Control and Status word to activate the MB.
	CAN0->MB[index].CS = CAN_CS_RTR(0) | CAN_CS_CODE(TX_DATA) | CAN_CS_DLC(5) | CAN_CS_SRR(1) | CAN_CS_IDE(0);
	
}

bool CAN_ReadRxMB(uint8_t index, CAN_DataFrame * frame)
{
	/*
	* -----------------------------------------
	*			49.4.3 Receive process
	* -----------------------------------------
	*/
	// Check if the BUSY bit is deasserted, indicating that the Mailbox is locked
	if(CAN0->MB[index].CS>>CAN_CS_CODE_SHIFT & 1UL)
		return false;

	frame->ID = (CAN0->MB[index].ID & CAN_ID_STD_MASK)>> CAN_ID_STD_SHIFT;
	frame->length = (CAN0->MB[index].CS & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT;

	frame->dataWord0 =  ((CAN0->MB[index].WORD0 & CAN_WORD0_DATA_BYTE_0_MASK)>>24)|
						((CAN0->MB[index].WORD0 & CAN_WORD0_DATA_BYTE_1_MASK)>>8)|
						((CAN0->MB[index].WORD0 & CAN_WORD0_DATA_BYTE_2_MASK)<<8)|
						((CAN0->MB[index].WORD0 & CAN_WORD0_DATA_BYTE_3_MASK)<<24);

	frame->dataWord1 =  ((CAN0->MB[index].WORD1 & CAN_WORD1_DATA_BYTE_4_MASK)>>24)|
						((CAN0->MB[index].WORD1 & CAN_WORD1_DATA_BYTE_5_MASK)>>8)|
						((CAN0->MB[index].WORD1 & CAN_WORD1_DATA_BYTE_6_MASK)<<8)|
						((CAN0->MB[index].WORD1 & CAN_WORD1_DATA_BYTE_7_MASK)<<24);


	// Read the Free Running Timer. It is optional but recommended to unlock Mailbox as soon as possible and make it available for reception.
	CAN0->TIMER;

	return true;
}


