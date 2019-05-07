/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: rtc_driver.asm
Creator: andyliu				Date: 2009-03-30
Description: rtc driver function, 
Others:
Version: V0.1
History:
	V0.1	2009-03-30		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef RTC_I2C_DRIVER_DEF

#include "sh86278_sfr_reg.inc"
//#include "ui_resident.h"
//#include "ui_resident_var_ext.h"
//#include "hwdriver_resident.h"
#include "rtc.h"

				


PUBLIC  _rtc_i2c_read
PUBLIC	__rtc_i2c_write

PUBLIC	_read_time

EXTRN	XDATA	(uc_rtc_second)
EXTRN	XDATA	(uc_rtc_minute)


RTC_DRIVER_SEG	SEGMENT		CODE
				RSEG		RTC_DRIVER_SEG


/*******************************************************************************
Function: _rtc_i2c_write()
Description: 
Calls:
Global:
Input: R7--register addr
       r5--register data 
Output:
Others:
********************************************************************************/
__rtc_i2c_write:
	CALL	_rtc_iic_enable
	CALL	_i2c_start
	
	MOV		A, #RTC_WRITE			;slave write mode
	CALL	_i2c_send_data
	CJNE	R3, #0, write_fail

	MOV		A, R7					;write register addr
	CALL	_i2c_send_data
	CJNE	R3, #0, write_fail

	MOV		A, R5					;write register data
	CALL	_i2c_send_data
	CJNE	R3, #0, write_fail

write_ok:
	SETB	C
	JMP		write_end
write_fail:
	CLR		C
write_end:	
	CALL	_i2c_stop
	RET


/*******************************************************************************
Function: bcd_to_hex()
Description: 
Calls:
Global:
Input: A
Output:A
Others:
********************************************************************************/
bcd_to_hex:
	MOV		R1, A
	SWAP	A
	ANL		A, #0FH
	MOV		B, #10
	MUL		AB
	MOV		B, A
	MOV		A, R1
	ANL		A, #0FH
	ADD		A, B
	RET


/*******************************************************************************
Function: _rtc_i2c_read()
Description:read rtc register(00~15) 
Calls:
Global:
Input: 
Output: CY: 1--OK; 0--Error
Others:
********************************************************************************/
rtc_I2C_read_end_jmp:
	MOV		A,#00H
	JMP		rtc_I2C_read_fail
_rtc_i2c_read:
	CALL	 _rtc_iic_enable
	CALL	_i2c_start

	MOV		A, #RTC_WRITE			;slave write mode
	CALL	_i2c_send_data
	CJNE	R3, #0, rtc_I2C_read_end_jmp

	MOV		A, R7					;write register addr
	CALL	_i2c_send_data
	CJNE	R3, #0, rtc_I2C_read_end_jmp

	CALL	_i2c_start

	MOV		A, #RTC_READ		;slave write mode
	CALL	_i2c_send_data   	;Output:R3:0 means successfully check ack        
	CJNE	R3, #0, rtc_I2C_read_end_jmp

	MOV		R6, #0				;"00H"
	CALL	_i2c_get_data   	;Input:R6, R6 = 1 means need send ack
		
rtc_I2C_read_fail:
	MOV		R7, A
rtc_I2C_read_end:
	JMP		_i2c_stop

_read_time:
	CALL	 _rtc_iic_enable
	CALL	_i2c_start

	MOV		A, #RTC_WRITE			;slave write mode
	CALL	_i2c_send_data
	CJNE	R3, #0, rtc_I2C_read_end_jmp

	MOV		A, R7					;write register addr
	CALL	_i2c_send_data
	CJNE	R3, #0, rtc_I2C_read_end_jmp

	CALL	_i2c_start

	MOV		A, #RTC_READ		;slave write mode
	CALL	_i2c_send_data   	;Output:R3:0 means successfully check ack        
	CJNE	R3, #0, rtc_I2C_read_end_jmp

;	MOV		R4,#01
	MOV		DPTR, #uc_rtc_minute
i2c_get:
	MOV		R6, #1
	CALL	_i2c_get_data   	;Input:R6, R6 = 1 means need send ack
	MOVX	@DPTR, A
	INC		DPTR
;	DJNZ	R4, i2c_get

	MOV		R6, #0
	CALL	_i2c_get_data   	;Input:R6, R6 = 0 means no need send ack; Output is ACC
	MOVX	@DPTR, A

	JMP		_i2c_stop


/*******************************************************************************
rtc iic enable 
********************************************************************************/
_rtc_iic_enable:
	ORL		MR_PADATA, #(RTC_CLK_MASK|RTC_DATA_MASK)
	ORL		MR_PATYPE, #(RTC_CLK_MASK|RTC_DATA_MASK)		;SCL output, SDA output
	RET


/*******************************************************************************
Function: _i2c_send_data()
Description: i2c send data(one byte)
Calls:
Global:Null
Input: A: the data to be sent
Output:R3: 0 means successfully check ack
Others:user A, CY, R3, R2
********************************************************************************/
_i2c_send_data:
	CALL	_i2c_set_sda_out
	
	MOV     R2, #8				;send 8 bits data
i2c_send_data_loop:
	RLC		A
	JC		bitvaleq1
bitvaleq0:
	CALL	_i2c_clr_sda
	JMP		i2c_turn_clock
bitvaleq1:
	CALL	_i2c_set_sda
i2c_turn_clock:       
	CALL	_i2c_set_scl
	CALL	_i2c_clr_scl
	DJNZ	R2, i2c_send_data_loop

	CALL	_i2c_set_sda_in		;check the "ack" bit
	CALL	_delay_4us

	CALL	_i2c_set_scl			;read one bit
	
	MOV     R2, #035H
i2c_check_ack_testloop:
	MOV     R3, #0				;0 means successfully check ack
	MOV     A, MR_PADATA
	ANL		A, #RTC_DATA_MASK
	JZ		ack_i2c				;"ack" is low level
	MOV     R3, #1				;1 means unsuccessfully check ack
	DJNZ    R2, i2c_check_ack_testloop
ack_i2c:
	CALL	_i2c_clr_scl
	CALL	_delay_4us
	CALL	_delay_4us
	JMP		_delay_4us

 	
/*******************************************************************************
Function: _i2c_get_data()
Description: i2c get data(one byte)
Calls:
Global:Null
Input: R6: 0 means no need send ack
Output:A: the data received
Others:
********************************************************************************/
_i2c_get_data:
	CALL	_i2c_set_sda_in
	CLR		A					;clr A
	MOV		R7, #8
i2c_get_data_loop:
	CALL	_i2c_set_scl
  
	MOV		R5, A				;save ACC
	MOV		A, MR_PADATA

	ANL		A, #RTC_DATA_MASK
	JZ		data0_lable
data1_lable:
	SETB	C
	JMP		save_data
data0_lable:
	CLR		C
save_data:
	MOV		A, R5				;restore ACC
	RLC		A
	CALL	_i2c_clr_scl
	DJNZ	R7, i2c_get_data_loop

	CALL	_i2c_set_sda_out

	CJNE	R6, #0, i2c_need_set_ack
i2c_no_need_set_ack:
	CALL	_i2c_set_sda
	JMP		i2c_get_data_end
i2c_need_set_ack:
	CALL	_i2c_clr_sda		;send "ack"
i2c_get_data_end:
	CALL	_i2c_set_scl
	CALL	_i2c_clr_scl
	CALL	_delay_4us
	CALL	_delay_4us
	JMP		_delay_4us


/*******************************************************************************
delay for I2C
********************************************************************************/
_delay_4us:
	MOV		R0,	#30			;1Tm	
nop_loop:
	NOP						;1Tm
	DJNZ	R0,	nop_loop	;2Tm
	RET						;2Tm


/*******************************************************************************/
_i2c_set_scl:
	ORL		MR_PATYPE, #RTC_CLK_MASK     ;SCL output
	ORL		MR_PADATA, #RTC_CLK_MASK
	JMP		_delay_4us

_i2c_clr_scl:
	ORL     MR_PATYPE, #RTC_CLK_MASK     ;SCL output
	ANL  	MR_PADATA, #(~RTC_CLK_MASK)
	JMP     _delay_4us


/*******************************************************************************/
_i2c_set_sda:
	ORL  	MR_PADATA, #RTC_DATA_MASK
	JMP     _delay_4us


_i2c_clr_sda:
	ANL  	MR_PADATA, #(~RTC_DATA_MASK)               
	JMP     _delay_4us
	
	
/*******************************************************************************/
_i2c_set_sda_in:
	ANL     MR_PATYPE, #(~RTC_DATA_MASK)
	RET


_i2c_set_sda_out:
	ORL     MR_PATYPE, #RTC_DATA_MASK 
	RET
	
	
/*******************************************************************************/
_i2c_start:
	CALL	_i2c_set_sda_out
	CALL	_i2c_set_sda
	CALL	_i2c_set_scl
	CALL	_i2c_clr_sda
	JMP		_i2c_clr_scl


/*******************************************************************************/
_i2c_stop:
	CALL	_i2c_set_sda_out
	CALL	_i2c_clr_sda
	CALL	_i2c_set_scl
	JMP		_i2c_set_sda


#endif
        END