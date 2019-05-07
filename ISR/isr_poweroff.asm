/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: isr_poweroff.asm
Creator: zj.zong					Date: 2009-04-10
Description: process int and basic func in power off mode
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
		 1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef ISR_POWER_OFF_MODE

#include "sh86278_sfr_reg.inc"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"



EXTRN	CODE	(_Api_ScanKey)

	CSEG	AT		1000H		;mpeg command interrupt vector address	
	 	JMP		_int_mpeg_cmd

	CSEG	AT		1003H		;mpeg special interrupt 1 vector address
	 	JMP		_int_mpeg_s1

	CSEG	AT		1006H		;mpeg flag interrupt vector address	 	
		JMP		_int_mpeg_flag 

	CSEG	AT		1009H		;ADC interrupt vector address
 		JMP		_int_adc

	CSEG	AT		100CH		;USB EP0 interrupt vector address
		JMP		_usb_ep0_isr

	CSEG	AT		100FH		;USB IEP1 interrupt vector address
		JMP		_usb_iep1_isr

	CSEG	AT		1012H		;USB OEP2 interrupt vector address
		JMP		_usb_oep2_isr

ISR_POWER_OFF_FUNC	SEGMENT		CODE
				RSEG		ISR_POWER_OFF_FUNC

_int_mpeg_cmd:
	RETI


_int_mpeg_s1:
	RETI


_int_mpeg_flag:
	RETI


_int_adc:
	RETI

_usb_ep0_isr:
	RETI

_usb_iep1_isr:
	RETI

_usb_oep2_isr:
	RETI



#endif
	END