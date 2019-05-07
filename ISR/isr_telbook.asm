/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: isr_telbook.asm
Creator: zj.zong			Date: 2009-06-20
Description: process int and basic func in telbook mode
Others:
Version: V0.1
History:
	V0.1	2009-06-20		zj.zong
		 1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef ISR_TELBOOK_FUNC_DEF

#include "sh86278_sfr_reg.inc"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"

EXTRN	CODE	(_Api_ScanKey)

/*******************************************************************************
interrupt process
********************************************************************************/
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

ISR_TELBOOK_FUNC		SEGMENT		CODE
					RSEG		ISR_TELBOOK_FUNC


/*******************************************************************************
command interrupt
********************************************************************************/
_int_mpeg_cmd:
	RETI



/*******************************************************************************
Inform MCU that the decbuf is empty, need to fill new data
********************************************************************************/
_int_mpeg_s1:
	RETI


/*******************************************************************************
Flag Interrupt
********************************************************************************/
_int_mpeg_flag:
	RETI



/*******************************************************************************
ADC Interrupt
********************************************************************************/
_int_adc:
	PUSH	ACC
	PUSH	PSW
	PUSH	DPH
	PUSH	DPL
	PUSH	00H		;R0
	PUSH	01H		;R1
	PUSH	05H		;R5
	PUSH	06H		;R6
	PUSH	07H		;R7

	MOV		A, MR_ADCDATH			;save ADC data into R0
	MOV		R0, A

	MOV		A, MR_ADCCON
	CLR		ACC.6
	MOV		MR_ADCCON, A					;clr ADCIRQ Flag
	JNB		ACC.4, not_vinkey_data
	CALL	_Api_ScanKey
	JMP		adc_irq_reti

not_vinkey_data:
	JNB		ACC.3, adc_irq_reti			;check ADC output data

get_vbat_data:
	MOV		A, R0
	MOV		R0, #uc_adc_bat_data
	MOV		@R0, A						;get battery adc_dat

	ORL		(ui_sys_event + 1), #LOW EVT_BATTERY	;Set #EVT_BATTERY

adc_irq_reti:
	POP		07H		;R7
	POP		06H		;R6
	POP		05H		;R5
	POP		01H		;R1
	POP		00H		;R0
	POP		DPL
	POP		DPH
	POP		PSW
	POP		ACC
	RETI


_usb_ep0_isr:
	RETI

_usb_iep1_isr:
	RETI

_usb_oep2_isr:
	RETI



#endif
	END