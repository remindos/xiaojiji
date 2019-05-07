/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: isr_voice.asm
Creator: zj.zong				Date: 2009-05-22
Description: process int and basic func in voice mode
Others:
Version: V0.1
History:
	V0.1	2009-05-22		zj.zong
		 1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef ISR_VOICE_FUNC_DEF

#include "sh86278_sfr_reg.inc"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"

EXTRN	BIT		(b_mpeg_init_ok)
EXTRN	BIT		(b_dac_mute_flg)
EXTRN	BIT		(b_voice_file_err)
EXTRN	BIT		(b_headphone_plug_flg)
EXTRN	DATA	(uc_voice_fill_dbuf_flg)
EXTRN	DATA	(uc_mpeg_state)

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


ISR_VOICE_FUNC		SEGMENT		CODE
				RSEG		ISR_VOICE_FUNC
/*******************************************************************************
mpeg command irq
cmd: 02H--mpeg module is ready
     05H--inform MCU to set/clr go bit of DAC
     08H--inform MCU the state of MPEG (stop/pause/play)
********************************************************************************/
_int_mpeg_cmd:
	PUSH	ACC
	PUSH	PSW
	PUSH	DPH
	PUSH	DPL

	MOV		DPTR, #CR_DTMCMD_L
	MOVX	A, @DPTR
	CJNE	A, #02H, not_cmd_02h
/*cmd 02H*/
	SETB	b_mpeg_init_ok				;mpeg module is ready
	JMP		_int_mpeg_cmd_reti
	
not_cmd_02h:
	CJNE	A, #05H, not_cmd_05h
/*cmd 05H*/
	MOV		DPTR, #CR_DTMDAT1_L			;inform MCU to set /clear go bit of DAC
	MOVX	A, @DPTR
	JB		ACC.0, set_dac_gobit
	
clr_dac_gobit:
	MOV		DPTR, #MR_DACI
	MOVX	A, @DPTR

	ORL		A, #080H					;set bit7, reset ws signal
	MOVX	@DPTR, A
	JMP		_int_mpeg_cmd_reti
	
set_dac_gobit:
	MOV		DPTR, #MR_DACI				;clr bit7 WS signal
	MOVX	A, @DPTR
	ANL		A, #07FH

	ORL		A, #00010000B				;set bit4 -- go bit
	MOVX	@DPTR, A
	JMP		_int_mpeg_cmd_reti

not_cmd_05h:
	CJNE	A, #08H, _int_mpeg_cmd_reti	;Inform MCU the state of MPEG (stop/pause/play)
/*cmd 08H*/
	MOV		DPTR, #CR_DTMDAT1_L
	MOVX	A, @DPTR
	MOV		uc_mpeg_state, A

_int_mpeg_cmd_reti:
	MOV		DPTR, #CR_MPEGIRQ_H			;clr bit7--cmd irq flag
	MOV		A, #01111111B
	MOVX	@DPTR, A

	POP		DPL
	POP		DPH
	POP		PSW
	POP		ACC
	RETI



/*******************************************************************************
mpeg special 1 irq
info MCU to fill decoder buffer with new data
********************************************************************************/
_int_mpeg_s1:
	PUSH	ACC
	PUSH	PSW
	PUSH	DPH
	PUSH	DPL

	MOV		DPTR, #CR_DTMDAT2_H				;bit0: 0--dbuf0 is empty, 1--dbuf1 is empty
	MOVX	A, @DPTR
	JB		ACC.0, fill_dbuf1
	
fill_dbuf0:
	ANL		uc_voice_fill_dbuf_flg, #0FEH	;clr bit0, MCU will fill DBUF0 with new data
	JMP		clr_flg_s1
fill_dbuf1:
	ANL		uc_voice_fill_dbuf_flg, #0FDH	;clr bit1, MCU will fill DBUF1 with new data

clr_flg_s1:
	MOV		DPTR, #CR_MPEGIRQ_H
	MOV		A, #11011111B
	MOVX	@DPTR, A						;clr bit5--sp1 irq flag

	ORL		ui_sys_event, #HIGH EVT_VOICE_FILL_DECBUF

	POP		DPL
	POP		DPH
	POP		PSW
	POP		ACC
	RETI



/*******************************************************************************
mpeg flag irq
flg2--fatal error
flg1--mute dac or cancel mute dac
********************************************************************************/
_int_mpeg_flag:
	PUSH	ACC
	PUSH	PSW
	PUSH	DPH
	PUSH	DPL

	MOV		DPTR, #CR_MPEGIRQ_L
	MOVX	A, @DPTR
	JNB		ACC.2, not_fatal_err_irq
/*flg2--fatal error*/
	SETB	b_voice_file_err
	ORL		ui_sys_event, #HIGH EVT_VOICE_FILE_ERR
	
	MOV		A, #11111011B					;clr bit2--flg2
	MOVX	@DPTR, A
	JMP		int_mpeg_ieq_reti
	
not_fatal_err_irq:
	JNB		ACC.1, not_mute_irq
/*flg1--mute dac or cancel mute dac*/
	MOV		DPTR, #CR_DTMDAT1_L				;bit0: 1--mute DAC; 0--cancel mute DAC
	MOVX	A, @DPTR
	JB		ACC.0, mute_dac
cancel_mute_dac:
	MOV		DPTR, #MR_DACI				;clr bit7 WS signal
	MOVX	A, @DPTR
	ANL		A, #07FH
	MOVX	@DPTR, A
	JMP		clr_flg1
mute_dac:
	MOV		DPTR, #MR_DACI
	MOVX	A, @DPTR
	ORL		A, #080H					;set bit7, reset ws signal
	MOVX	@DPTR, A
clr_flg1:
	MOV		DPTR, #CR_MPEGIRQ_L
	MOV		A, #11111101B
	MOVX	@DPTR, A						;clr bit1--flg1

not_mute_irq:
int_mpeg_ieq_reti:
	MOV		DPTR, #CR_MPEGIRQ_L				;clr others flag irq
	MOV		A, #00000111B
	MOVX	@DPTR, A
	MOV		DPTR, #CR_MPEGIRQ_H
	MOV		A, #11100000B
	MOVX	@DPTR, A

	POP		DPL
	POP		DPH
	POP		PSW
	POP		ACC
	RETI



/*******************************************************************************
ADC irq
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
	ANL		MR_ADCCON,#0BFH				;clr bit6		
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
	JB		b_io_adc_scan_flg, adc_irq_reti;if b_io_adc_scan_flg = 1, and not ADC scan key
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