/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: isr_fm_func.asm
Creator: zj.zong				Date: 2009-05-25
Description: process int and basic func in fm mode
Others:
Version: V0.1
History:
	V0.1	2009-05-25		zj.zong
		 1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef ISR_FM_FUNC_DEF

#include "sh86278_sfr_reg.inc"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"


EXTRN	CODE	(_Api_ScanKey)
EXTRN	BIT		(b_headphone_plug_flg)

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


ISR_FM_FUNC		SEGMENT		CODE
			RSEG		ISR_FM_FUNC


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

	MOV		R0,MR_ADCDATH
	ANL		MR_ADCCON,#0BFH							;clr bit6		
	MOV		A, MR_ADCCON
	JNB		ACC.4, not_vinkey_data

	MOV  	A, R0
	CLR  	C
	SUBB	A, #230
	JNC		no_headphone_plug
	SETB	b_headphone_plug_flg	 	
	JMP		check_key_adc 
no_headphone_plug:
 	CLR		b_headphone_plug_flg
		
check_key_adc:
	JB		b_io_adc_scan_flg, adc_irq_reti		;if b_io_adc_scan_flg = 1, and not ADC scan key
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