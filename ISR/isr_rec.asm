/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: rec_func.asm
Creator: andyliu					Date: 2007-12-10
Description: record int and basic func in rec mode
Others:
Version: V0.1
History:
	V0.1	2007-12-10		andyliu
		 1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef ISR_RECORD_FUNC_DEF

#include "sh86278_sfr_reg.inc"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"

EXTRN	BIT		(b_rec_samplerate_flg)
EXTRN	BIT		(b_rec_mode_flg)
EXTRN	BIT		(b_rec_adcbuf_full_flg)
EXTRN	BIT		(b_rec_decbuf_flg)
EXTRN	DATA	(uc_rec_adcbuf_dph)
EXTRN	DATA	(uc_rec_adcbuf_dpl)
EXTRN	DATA	(uc_rec_playkey_cnt_h)
EXTRN	DATA	(uc_rec_playkey_cnt_l)
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

ISR_RECORD_FUNC		SEGMENT		CODE
				RSEG		ISR_RECORD_FUNC



_int_mpeg_cmd:
	RETI


_int_mpeg_s1:
	RETI


_int_mpeg_flag:
	RETI


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

	JNB		b_io_adc_scan_flg, adc_scankey
	JMP		adc_irq_reti					;if b_io_adc_scan_flg = 1, and not ADC scan key
adc_scankey:
	CALL	_Api_ScanKey
	JMP		adc_irq_reti						;Vin-Key Data

not_vinkey_data:
	JNB		ACC.3, not_vinbat_data
	JMP		get_vbat_data					;Vin-Battery Data

not_vinbat_data:
	JB		b_rec_mode_flg, in_recording
	JMP		adc_irq_reti	;Vin-Mic Data
/*debounce play key*/
in_recording:
	MOV		A, MR_PADATA
	JB		ACC.0, io_playkey_cnt				;PA0	 
	MOV		A, MR_PADATA
	JB		ACC.2, io_playkey_cnt				;PA1	 
	MOV		A, MR_PADATA
   	JB		ACC.4, io_playkey_cnt				;PA4	 
	MOV		A, MR_PADATA
   	JB		ACC.5, io_playkey_cnt				;PA5	 
	MOV		A, MR_PADATA
  	JB		ACC.6, io_playkey_cnt				;PA6	 
	MOV		A, MR_PADATA
  	JB		ACC.7, io_playkey_cnt				;PA7

	MOV		A, uc_rec_playkey_cnt_l
	JNZ		key_cnt_l_dec
key_cnt_h_dec:
	MOV		A, uc_rec_playkey_cnt_h
	JZ		not_press_play
	DEC		uc_rec_playkey_cnt_h
	MOV		uc_rec_playkey_cnt_l, #65
key_cnt_l_dec:
	DEC		uc_rec_playkey_cnt_l
	JMP		adc_irq_reti

io_playkey_cnt:
	JB		b_rec_samplerate_flg, io_rec_16KHz
	MOV		uc_rec_playkey_cnt_l, #65
	MOV		uc_rec_playkey_cnt_h, #80
	JMP		adc_irq_reti
io_rec_16KHz:
	MOV		uc_rec_playkey_cnt_l, #65
	MOV		uc_rec_playkey_cnt_h, #160
	JMP		adc_irq_reti
	
not_press_play:
	MOV		DPH, uc_rec_adcbuf_dph
	MOV		DPL, uc_rec_adcbuf_dpl
	MOV 	A, DPL
	CJNE	A, #(LOW X_ADDR_DBUF0)+0E4H, fill_mic_data
	MOV		A, DPH

	JB		b_rec_decbuf_flg, check_fill_dbuf1
	CJNE	A, #(HIGH X_ADDR_DBUF0)+7, fill_mic_data
	JNB		b_rec_adcbuf_full_flg, adc_irq_reti
	CLR     b_rec_adcbuf_full_flg
	MOV		DPTR, #X_ADDR_DBUF1
	SETB	b_rec_decbuf_flg
	JMP		set_mic_evt

check_fill_dbuf1:
	CJNE	A, #(HIGH X_ADDR_DBUF1)+7, fill_mic_data
	JNB		b_rec_adcbuf_full_flg, adc_irq_reti
	CLR		b_rec_adcbuf_full_flg
	MOV		DPTR, #X_ADDR_DBUF0
	CLR		b_rec_decbuf_flg

set_mic_evt:
	ORL		ui_sys_event, #HIGH EVT_REC_ENCODE	;set EVT_MIC_REC_H8
fill_mic_data:
	MOV		A, R0								;get mic data
	MOVX    @DPTR, A
	INC		DPTR
	CLR		A
	MOVX    @DPTR, A
	
	INC		DPTR
	MOV     uc_rec_adcbuf_dpl, DPL
	MOV     uc_rec_adcbuf_dph, DPH
	JMP		adc_irq_reti

get_vbat_data:
	MOV		A, R0
	MOV		R0, #uc_adc_bat_data
	MOV		@R0, A									;get battery adc_dat

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