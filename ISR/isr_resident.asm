/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: isr_resident.asm
Creator: zj.zong					Date: 2009-04-10
Description: process resident timer and usb detect interrupt 
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
		 1. build this module
********************************************************************************/
#include "sh86278_sfr_reg.inc"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"

IR_IDLE		EQU	0
IR_9MS		EQU	1
IR_BEG_OR_CON	EQU	2
IR_WAIT_DATA	EQU	3
IR_PASS0	EQU	4
IR_GET_DATA	EQU	5
IR_PASS1	EQU	6

EXTRN	CODE (_Api_Delay_1ms)
EXTRN	CODE (_Api_ScanKey)

	CSEG	AT		07F1H			;0810H				;Timer interrupt vector address
		LJMP		_Int_Timer_Process

	CSEG	AT		07F4H			;0813H				;USBDET interrupt vector address
		LJMP		_Int_Usb_Detect

	CSEG	AT		07F7H			;0816H				;MSI plug state change interrupt vector address
		LJMP		_Int_Msi_Plug

  	CSEG	AT		1083H
/*******************************************************************************
Function: _Int_Timer_Process
Description: timer(20ms) interrupt process(include scan key)
Calls:
Global:uc_timer_cnt, ui_sys_event, uc_usb_time_cnt
Input: Null
Output:Null
Others:
********************************************************************************/
_Int_Timer_Process:
	PUSH	ACC
	PUSH	PSW
	PUSH	DPH
	PUSH	DPL
	PUSH	00H		;R0
	PUSH	01H		;R1
	PUSH	05H		;R5
	PUSH	06H		;R6
	PUSH	07H		;R7
	
	ANL		MR_TMCON, #11111110B
					;clr timer IRQ flag
	MOV		R0, #uc_200us_timer_cnt					;+200*100us=20ms
	INC		@R0
	MOV		A, @R0
	CLR		C
	SUBB	A, #100
	JC		T100us
	MOV		@R0, #00H
T100us:
	MOV		R0, #uc_100us_timer_cnt					;+1/100us
	INC		@R0
	MOV		A,@R0
	CJNE    A,#100,_ir_100us_process
	MOV		@R0,#00H;

	MOV		R0, #uc_timer_cnt					;+1/20ms
	INC		@R0
	MOV		R0, #uc_usb_timer_cnt1
	INC		@R0
	MOV		R0, #uc_usb_timer_cnt2
	INC		@R0	

	ORL		MR_ADCCON,#02H				
	ORL		ui_sys_event + 1, #low EVT_TIMER	;SET EVT_TIMER_H8 20ms

/*	
	MOV		R0, #uc_us_timer_cnt
	INC		@R0
	MOV		A, @R0
	CJNE	A, #2, _ir_100us_process
	MOV		@R0,#00H;
*/
;	JB		b_io_adc_scan_flg, io_scan_key1	;if b_io_adc_scan_flg = 1, and not ADC scan key
;	JMP		_int_timer_reti
;io_scan_key1:
;	CALL	_Api_ScanKey						;Scan key and get key value
_ir_100us_process:									;100us
	ACALL	_ir_process

_int_timer_reti:
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
#if 1
_ir_process:
	MOV   	A,	ir_state
	CJNE  	A,	#IR_IDLE,CHK_IR_STATE1
	MOV   	A,	MR_PADATA
	JB    	ACC.2,	JMP_IR_EXIT_2
	;9ms (begin code or continue code)
	MOV   	ir_state,	#IR_9MS
JMP_IR_EXIT:
	MOV   	ir_state_cnt,	#00H
JMP_IR_EXIT_2:
	RET

CHK_IR_STATE1:
	CJNE  	A,	#IR_9MS,CHK_IR_STATE2
	INC   	ir_state_cnt
	MOV   	A,	ir_state_cnt
	CJNE  	A,	#80,	ir_9ms_chk_hi
	;80*100=8000us,8ms
	;next,after 9ms,check begin code or continue code
	MOV   	ir_state,	#IR_BEG_OR_CON
	JMP  	JMP_IR_EXIT

ir_9ms_chk_hi:
	;if <160(8000ms),high level occours,err
	ORL		MR_PADATA, #078H
	MOV   	A,	MR_PADATA
	JNB   	ACC.2,	IR_EXIT
	;ERR
	MOV   	ir_state,	#IR_IDLE
	JMP  	JMP_IR_EXIT
	
CHK_IR_STATE2:
	CJNE  	A,	#IR_BEG_OR_CON,	CHK_IR_STATE3
	MOV   	A,	MR_PADATA
	JNB   	ACC.2,	IR_EXIT	;wait 9ms low level end
	;now 9ms low level is over
	INC   	ir_state_cnt
	MOV   	A,	ir_state_cnt
	CJNE  	A,	#24,	IR_EXIT
	;wait 2400us(24*100),check level.  
	;low:continue code;high:begin code
	MOV   	A,	MR_PADATA
	JB    	ACC.2,	get_ir_begin_code
	;continue code
	MOV   	ir_state,	#IR_IDLE
	SJMP  	JMP_IR_EXIT

get_ir_begin_code:
	MOV   	ir_state,	#IR_WAIT_DATA
	SJMP  	JMP_IR_EXIT

CHK_IR_STATE3:
	CJNE  	A,	#IR_WAIT_DATA,	CHK_IR_STATE4
	MOV   	A,	MR_PADATA
	JB    	ACC.2,	chk_high_level_time
	;begin lead code is over
	MOV   	ir_state,	#IR_PASS0
	SJMP  	JMP_IR_EXIT

chk_high_level_time:
	INC   	ir_state_cnt
	MOV   	A,ir_state_cnt
	CJNE  	A,#100,IR_EXIT
	;high level is too long,ingore,enter idle
	MOV   	ir_data_cnt,#00H
	MOV   	R0,#bit_cnt
	MOV   	@R0,#00H
	MOV   	ir_state,#IR_IDLE
	SJMP  	JMP_IR_EXIT

CHK_IR_STATE4:
	CJNE  	A,#IR_PASS0,CHK_IR_STATE5
	MOV   	A,MR_PADATA
	JNB   	ACC.2,IR_EXIT	;wait high level
	;now high level,begin to judge "0" or "1"
	MOV   	ir_state,#IR_GET_DATA
	SJMP  	JMP_IR_EXIT

CHK_IR_STATE5:
	CJNE  	A,#IR_GET_DATA,CHK_IR_STATE6	
	INC   	ir_state_cnt
	MOV   	A,ir_state_cnt
	CJNE  	A,#8,JMP_IR_EXIT_2
	;after 0.80ms,judge level
	MOV   	A,MR_PADATA

	MOV   	C,ACC.2
	MOV   	ir_bit,C

	JB    	ACC.2,ir_dat_1
	MOV   	ir_state,#IR_PASS0
	JMP  	ir_data_merge	


ir_dat_1:
	MOV   	ir_state,#IR_PASS1
	JMP  	ir_data_merge
	
CHK_IR_STATE6:
	CJNE  	A,#IR_PASS1,CHK_IR_STATE7
	MOV   	A,MR_PADATA
	JB    	ACC.2,chk_high_level_time		;wait high level
	MOV   	ir_state,#IR_PASS0
	JMP  	JMP_IR_EXIT	

CHK_IR_STATE7:
	MOV   	ir_state,#IR_IDLE
	
IR_EXIT:

	RET
ir_data_merge:
	MOV   R0,ir_data_cnt
	MOV   A,#ir_data_buf
	ADD   A,R0
	MOV   R0,A
	
	MOV   C,ir_bit
	MOV   A,@R0
	RRC   A
	MOV   @R0,A

	MOV   R0,#bit_cnt
	INC   @R0
	CJNE  @R0,#08H,IR_DM_RET
	MOV   @R0,#00H

	INC   ir_data_cnt
	MOV   A,ir_data_cnt
	CJNE  A,#04H,IR_DM_RET

	MOV   ir_data_cnt,#00H
	MOV   ir_state,#IR_IDLE

	SETB  key0_ir1
	ORL	  ui_sys_event + 1, #low EVT_KEY	;set EVT_KEY_L8

IR_DM_RET:
	JMP  JMP_IR_EXIT


#endif

/*******************************************************************************
Function: _Int_Usb_Detect()
Description: usb plug or unplug interrupt
Calls:
Global:ui_sys_event
Input: Null
Output:Null
Others:
********************************************************************************/
_Int_Usb_Detect:
	PUSH	ACC
	PUSH	PSW
	PUSH	00H		;R0
	PUSH	01H		;R1
	PUSH	02H		;R2
	PUSH	03H		;R3
	PUSH	04H		;R4
	PUSH	05H		;R5
	PUSH	06H		;R6
	PUSH	07H		;R7

	MOV		R3, #10
usb_cnt_10times:
	MOV		R4, #00H					;now usb_on_count
	MOV		R2, #00H					;now usb_off_count
	MOV		R1, #03H
wt_3_times:
 	MOV		R7, #01H
	CALL	_Api_Delay_1ms

	MOV		A, MR_USBCON
	JB		ACC.0, now_usb_plug_on
	INC		R2							;now_usb_plug_off
	JMP		jg_3_times
now_usb_plug_on:
	INC		R4							;now_usb_plug_on
jg_3_times:
	DJNZ	R1, wt_3_times
	CJNE	R4, #03H,usb_jg_R5

set_usb_is_on:
	SETB	b_usb_is_on
	ORL		ui_sys_event + 1, #low EVT_USB	;SET EVT_USB_H8     
	JMP	usb_plug_irq_end

usb_jg_R5:
	CJNE	R2, #03H,usb_continue_wt
set_usb_is_off:
	CLR		b_usb_is_on
	ANL		MR_MODEN1, #0CFH			;BIT4,5-----0,disable usb model/subsystem
	JMP		usb_plug_irq_end
usb_continue_wt:
	DJNZ	R3, usb_cnt_10times			;set_usb_unsteady

usb_plug_irq_end:
	ANL		MR_USBCON, #0FDH			;clr irqdetusb bit1
  	JMP		exit_irq_end
/*	POP		07H		;R7
	POP		06H		;R6
	POP		05H		;R5
	POP		04H		;R4
	POP		03H		;R3
	POP		02H		;R2
	POP		01H		;R1
	POP		00H		;R0
	POP		PSW
	POP		ACC
	RETI  */
/*******************************************************************************
Function: _Int_Msi_Plug()
Description: usb plug or unplug interrupt
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
_Int_Msi_Plug:
	PUSH	ACC
	PUSH	PSW
	PUSH	00H		;R0
	PUSH	01H		;R1
	PUSH	02H		;R2
	PUSH	03H		;R3
	PUSH	04H		;R4
	PUSH	05H		;R5
	PUSH	06H		;R6
	PUSH	07H		;R7

	MOV		R3, #10
msi_cnt_10times:
	MOV		R4, #00H							;now usb_on_count
	MOV		R2, #00H							;now usb_off_count
	MOV		R1, #03H
msi_wt_3_times:
	MOV			R7, #01H			
	CALL	_Api_Delay_1ms

;	MOV		DPTR, #uc_user_option2
;	MOVX	A, @DPTR
	MOV		A, MR_MSCON4
	JNB		b_sd_wppin_flg, MSCDn_Normal_3
	JNB		ACC.7, now_msi_plug_on
	JMP		have_not_plug_on	
MSCDn_Normal_3:	
	JB		ACC.7, now_msi_plug_on	
have_not_plug_on:
	INC		R2									;now_msi_plug_off
	JMP		msi_jg_3_times
now_msi_plug_on:
	INC		R4									;now_msi_plug_on
msi_jg_3_times:
	DJNZ	R1, msi_wt_3_times
	CJNE	R4, #03H,msi_jg_R5

set_msi_is_on:
	SETB	b_msi_plug_flg
	JMP		msi_plug_irq_end

msi_jg_R5:
	CJNE	R2, #03H, msi_continue_wt
set_msi_is_off:
	CLR		b_msi_plug_flg
	CLR		b_sd_exist_flg						;lihuan 20100804 for telbook
	MOV		uc_msi_status, #0FFH
	JMP		msi_plug_irq_end
msi_continue_wt:
	DJNZ	R3, msi_cnt_10times					;set_usb_unsteady

msi_plug_irq_end:
	ORL		ui_sys_event + 1, #LOW EVT_MSI		;SET EVT_EVT low 8
	MOV     R7, ui_sys_event + 1 
	ANL		MR_MSCON2, #0FBH					;clr msi plug state flag

exit_msi_plug_irq:
	JMP		exit_irq_end

exit_irq_end:
 	POP		07H		;R7
	POP		06H		;R6
	POP		05H		;R5
	POP		04H		;R4
	POP		03H		;R3
	POP		02H		;R2
	POP		01H		;R1
	POP		00H		;R0
	POP		PSW
	POP		ACC
	RETI

	
	END