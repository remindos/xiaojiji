/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: api_resident_func.asm
Creator: zj.zong					Date: 2009-04-10
Description: common function and dynamicload interface
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

EXTRN CODE (_media_readsector)

PUBLIC	_Api_Delay_1ms
PUBLIC	_Api_ScanKey
PUBLIC  Read_Ctra


 	CSEG	AT		07FAH		;0819H	
/*******************************************************************************
Function: _Api_ScanKey()
Description: b_io_adc_scan_flg 1: I/O scan key
                               0: ADC scan key
Calls:
Global:uc_key_value, ui_sys_event
Input: R0(ADC Data)
Output:Null
Others:R0,R1,R5,R6,R7
********************************************************************************/
_Api_ScanKey:
	MOV		A, ui_sys_event + 1				;check have executed event-key?
	JNB		ACC.2, start_scan_key
	JMP		scan_key_end
start_scan_key:	

	MOV		R7, #KEYVAL_PLAY					;"PLAY", PA0
	MOV		A, MR_PADATA
	JB		ACC.0, get_key_state

	ANL		MR_PBTYPE,#0F5H		;PB1 PB3 INPUT
	ANL		MR_PCTYPE,#0E7H		;PC3 PC4 INPUT

  	MOV		R7, #KEYVAL_3						;"KEY_VAL_3"
	MOV		A, MR_PCDATA
   	JB		ACC.3, get_key_state				;PC3	 

 	MOV		R7, #KEYVAL_4						;"KEY_VAL_4"
 	MOV		A, MR_PCDATA
  	JB		ACC.4, get_key_state				;PC4	 


  	MOV		R7, #KEYVAL_3						;"KEY_VAL_3"
	MOV		A, MR_PCDATA
   	JB		ACC.3, get_key_state				;PC3	 

 	MOV		R7, #KEYVAL_4						;"KEY_VAL_4"
 	MOV		A, MR_PCDATA
  	JB		ACC.4, get_key_state				;PC4	

	MOV		R7, #KEYVAL_NULL					;"NULL"


get_key_state:
	MOV		A, R7						;campare key value
	MOV		R0, #uc_key_old_value
	CLR		C
	SUBB	A, @R0
	JNZ		key_val_change
	
	MOV		A, R7
	CJNE	A, #KEYVAL_NULL, not_null_key	;Check is "NULL"
	MOV		R0, #uc_key_value				;Set keyval "NULL"
	MOV		@R0, A
	MOV		uc_key_cnt, #00H
	MOV		R0, #uc_key_release_cnt
	MOV		@R0, #00H
	JMP		scan_key_end

not_null_key:
	JB		b_play_poweron_flg, scan_key_end
	INC		uc_key_cnt
	MOV		R0, #uc_key_release_cnt
	MOV		@R0, #00H
	MOV		A, uc_key_cnt
	CJNE	A, #KEY_L_TIME, not_long_press	;800ms
	MOV		R6, #KEY_STATE_L				;Long Press
	JMP		get_key_value_evt

not_long_press:
	CJNE	A, #KEY_LC_TIME, scan_key_end	;1200ms
	MOV		uc_key_cnt, #KEY_L_TIME			;For next LC press
	MOV		R6, #KEY_STATE_LC				;Long Continue
	JMP		get_key_value_evt

key_val_change:
	MOV		R0, #uc_key_release_cnt
	INC		@R0
	MOV		A, uc_key_cnt
	CLR		C
	SUBB	A, #KEY_S_TIME				;3*20ms short press
	JNC		key_lable_1
	
	MOV		uc_key_cnt, #00H			;clr uc_key_cnt
	MOV		R0, #uc_key_release_cnt
	MOV		@R0, #00H
	MOV		A, R7						;save the key old value
	MOV		R0, #uc_key_old_value
	MOV		@R0, A
	JMP		scan_key_end
	
key_lable_1:
	MOV		R0, #uc_key_release_cnt
	MOV		A, @R0
	CLR		C
	SUBB	A, #KEY_S_TIME
	JNC		key_lable_2
	INC		uc_key_cnt
	JMP		scan_key_end
	
key_lable_2:
	MOV		R6, #KEY_STATE_LE			;Long Press end
	MOV		A, uc_key_cnt
	CLR		C
	SUBB	A, #KEY_L_TIME				;40*20ms long
	JNC		key_lable_3
	MOV		R6, #KEY_STATE_S			;Short Press
	
key_lable_3:
	MOV		uc_key_cnt, #00H
	MOV		R0, #uc_key_release_cnt
	MOV		@R0, #00H
	MOV		R0, #uc_key_old_value
	MOV		A, @R0
	MOV		R5, A						;save old value to R5
	MOV		A, R7
	MOV		@R0, A
	MOV		A, R5						;load old value to A
	MOV		R7, A						;get the key value

get_key_value_evt:
	ORL		ui_sys_event + 1, #low EVT_KEY	;set EVT_KEY_L8
get_key_value:
	MOV		A, R6			
;	SWAP	A							;key state ---high 4bit
	ORL		A, R7						;key state | key value
	MOV		R0, #uc_key_value
	MOV		@R0, A
scan_key_end:
//	JNB		b_io_adc_scan_flg, adc_scan_key
	
//	ANL		MR_PADATA, #01011111B				;clr PA5, PA7 for I/O scan key
	
adc_scan_key:
	MOV		R0, #uc_key_value
	CJNE	@R0, #KEYVAL_NULL, scan_key_ret
	JNB		b_play_poweron_flg, scan_key_ret
	CLR		b_play_poweron_flg			;"PLAY" Release
	MOV		uc_key_cnt, #00H
scan_key_ret:
	RET




/*******************************************************************************
Function: _Api_Delay_1ms()
Description: delay function
Calls:
Global:Null
Input: R7(R7*1ms=total time)
Output:Null
Others: R5, R6, R7, ACC are used
********************************************************************************/
_Api_Delay_1ms:
	MOV		A, R7				
	JZ		delay_error					;if R7 == 0, error
	MOV		A, MR_SYSCLK
	ANL		A, #00000111B				;Get the CPU Clock
	CJNE	A, #00H, not_1m_clk
	MOV		A, #5						;5*200=1000Cycle/1M=1ms
	JMP	delay_start					;current CPU Clock is 1M

not_1m_clk:
	CJNE	A, #01H, not_12m_clk
	MOV		A, #60						;60*200=12000Cycle/12M=1ms
	JMP	delay_start					;current CPU Clock is 12M

not_12m_clk:
	CJNE	A, #02H, not_22m_clk
	MOV		A, #113						;113*200=22600Cycle/22.5M=1ms
	JMP	delay_start					

not_22m_clk:
	CJNE	A, #03H, not_33m_clk
	MOV		A, #169						;169*200=33800Cycle/33.75M=1ms
	JMP	delay_start

not_33m_clk:
	MOV		A, #193						;193*200=38600Cycle/38.57M=1ms

delay_start:
delay_loop2:
	MOV   	R5, A		
delay_loop1:
	MOV   	R6, #48
	NOP
	NOP
delay_loop0:
	DJNZ  	R6, delay_loop0
	DJNZ  	R5, delay_loop1
	DJNZ  	R7, delay_loop2
delay_error:
	RET

/*******************************************************************************
Function: Read_Ctra
Description:read contents table of reserved area
Calls:
Global:FBUF-Common buf (XDATA:0x6600)
Input: R1--(R1+1) is segment num
Output:Null
Others:
********************************************************************************/
Read_Ctra:										;read 'contents table of reserved area' 
	MOV		DPTR ,#uc_reserve_addr0			;
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	
	MOV		DPTR ,#uc_reserve_addr1			;
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A

	MOV		DPTR ,#uc_reserve_addr2			;
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr2, A

	MOV		DPTR ,#uc_reserve_addr3			;
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A	

	MOV		sfc_occ_buf_flag, #SFC_APP_FBUF
	MOV		buf_offset0, #LOW FCOMBUF_OFFSET_W
	MOV		buf_offset1, #HIGH FCOMBUF_OFFSET_W
	SETB	sfc_reserved_area_flag
	CALL	_media_readsector
	CLR		sfc_reserved_area_flag

read_segment:									;offset point to needed segment
	MOV		R0, #16
	MOV		A, R0
read_only_area_num:
	ADD		A, R0
	DJNZ	MR_LCMNUMH, read_only_area_num

	ADD		A, #LOW (X_ADDR_FBUF + FCOMBUF_OFFSET_B)
	MOV		DPL, A
	MOV		A, #HIGH (X_ADDR_FBUF + FCOMBUF_OFFSET_B) 
	MOV		DPH, A
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr2, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A
	RET

_inc_rd_dptr_sub_r0:			 
	CLR		C
   	MOV		A, @R1
	SUBB	A, R0				 
	JNC		label_positive
	CLR 	C
	CPL		A
	INC		A
label_positive:
 	SUBB	A, key_vol_offset		; vol offset is 10
 	INC		R1	
	RET
END
