/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: isr_start.asm
Creator: zj.zong					Date: 2009-04-10
Description: interrupt interface
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
			1. build this module
********************************************************************************/

PUBLIC	jump_to_pm1_start

 	CSEG	AT		0003H		;USBDET interrupt vector address   
   		JMP		07F4H			;0813H			


	CSEG	AT		000BH		;USB EP1_INT interrupt vector address
		JMP		100FH


	CSEG	AT		0013H		;USB EP2_INT interrupt vector address
		JMP		1012H		


	CSEG	AT		001BH		;USB EP0 interrupt vector address	
	  	JMP		100CH


	CSEG	AT		0023H		;SFC interrupt vector address;	
	  	RETI				


	CSEG	AT		002BH		;LCMI interrupt vector address	
	  	RETI 			

	CSEG	AT		0033H		;MSI interrrupt vector address
		RETI


	CSEG	AT		003BH		;MSI plug state change interrrupt vector address
		JMP		07F7H			;0816H

	CSEG	AT		0043H		;ADC interrupt vector address	
		JMP		1009H


	CSEG	AT		004BH		;Timer interrupt vector address			
		JMP		07F1H			;0810H


	CSEG	AT		0053H		;MPEG command interrupt vector address	
		JMP		1000H

	
	CSEG	AT		005BH		;MPEG special interrupt  vector address	
		JMP		1003H


	CSEG	AT		0063H		;MPEG flag interrupt vector address	
		JMP		1006H


jump_to_pm1_start:
		JMP		1300H//1250H		;1190H
	END
