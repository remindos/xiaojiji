/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_bootblock_fm_common.asm
Creator: zj.zong					Date: 2009-08-23
Description: fm i2c communication, and basic function
Others:
Version: V0.1
History:
	V0.1	2009-08-23		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_BOOTBLOCK_FM_COMMON_FUNC_DEF
#include "sh86278_sfr_reg.inc"
#include "ap_bootblock_fm.h"


PUBLIC	_fm_i2c_enable
PUBLIC	_fm_i2c_disable
PUBLIC	__fm_i2c_write
PUBLIC	_i2c_set_sda_out
PUBLIC	_i2c_clr_sda
PUBLIC	__fm_i2c_ar1010_write
PUBLIC	__fm_i2c_bk1080_write

EXTRN	XDATA	(uc_fm_write_bytes)
EXTRN	XDATA	(uc_fm_read_bytes)
EXTRN	XDATA	(uc_chip_address)


	DRV_FM_COMMON_SEG	SEGMENT		CODE
						RSEG		DRV_FM_COMMON_SEG
/*******************************************************************************
Function: _fm_i2c_enable(), _fm_i2c_disable()
Description: init fm data and clock port
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
_fm_i2c_enable:
	ORL		FM_DATA_TYPE, #FM_DATA_MASK		;SCL output, SDA output
	ORL		FM_CLK_TYPE, #FM_CLK_MASK
	RET

_fm_i2c_disable:
	RET
	
/*******************************************************************************
Function: __fm_i2c_write()
Description: write operation,write bytes to control the chip
Calls:
Global:Null
Input: R7: the number of bytes that need write, uc_fm_write_bytes[0]~uc_fm_write_bytes[4]
Output:CY: 1 means write successfully, 0 means error
Others:
********************************************************************************/
__fm_i2c_write:
	CALL	_fm_i2c_enable

	CALL	_i2c_start
;	MOV		A, uc_chip_address
	MOV		DPTR,#uc_chip_address
	MOVX	A,@DPTR
	ANL		A, #0FEH						  				;~CHIP_WR0_RD1_MASK =0xfe

	CALL	_i2c_send_data   							;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, i2c_write_end

	MOV		DPTR, #uc_fm_write_bytes
i2c_send:
	MOVX	A, @DPTR
	CALL	_i2c_send_data   							;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, i2c_write_end
	INC		DPTR
	DJNZ	R7, i2c_send

i2c_write_end:
	CALL	_i2c_stop
	MOV		A, R6
	JZ		i2c_wr_err
i2c_wr_ok:
	SETB	C
	CALL	_fm_i2c_disable
	RET
i2c_wr_err:
	CLR		C
	CALL	_fm_i2c_disable
	RET


/********************************************************************************************************
Function:	__fm_ar1010_i2c_write()
Description:	write operation,write bytes to control the chip
Input:		R7(the number of bytes that need write),fm_write_bytes[0]~fm_write_bytes[4]
Output:		R6: 0 means write successfully,1 means error
Temp VAR:	DPTR
Memo:
********************************************************************************************************/
__fm_i2c_ar1010_write:
	CALL	_fm_i2c_enable
	MOV		A, R7
	MOV 	R5, A
	MOV	 	R2, #010
loop_again:
	MOV		R7, #2
	MOV  	R3, #0
	LCALL  _i2c_start
   
;  	MOV		A, uc_chip_address	 ; Write Address
	MOV		DPTR,#uc_chip_address
	MOVX	A,@DPTR
    LCALL   _i2c_send_data   ;Output:R6:0 means successfully check ack  
	MOV  	A, R3
	ORL 	A, R6
	MOV		R3, A
	MOV     A, R5			 ;Write res address

	LCALL   _i2c_send_data
	MOV    A, R3
	ORL    A, R6
	MOV    R3, A

    MOV    DPTR, #uc_fm_write_bytes	   ;write data first high then low
fm_i2c_send:
    MOVX    A,      @DPTR
    LCALL   _i2c_send_data   ;Output:R6:0 means successfully check ack  
	MOV   	A, R3
	ORL   	A, R6
	MOV   	R3, A

   	INC     DPTR
    DJNZ    R7, fm_i2c_send

fm_i2c_write_end:
    LCALL   _i2c_stop
	DJNZ	R2, wd_again1
	JMP		wd_again2
wd_again1:
	CJNE	R3, #0, loop_again
wd_again2:
	CALL	_fm_i2c_disable
    RET

/********************************************************************************************************
Function:	__fm_i2c_bk1080_write()
Description:	write operation,write bytes to control the chip
Input:		R7(the number of bytes that need write),fm_write_bytes[0]~fm_write_bytes[4]
Output:		R6: 0 means write successfully,1 means error
Temp VAR:	DPTR
Memo:
********************************************************************************************************/
__fm_i2c_bk1080_write:

	CALL	_fm_i2c_enable
	CALL	_i2c_start
	MOV		R3,#2

;	MOV		A, uc_chip_address 		;address+wr
	MOV		DPTR,#uc_chip_address
	MOVX	A,@DPTR
	CALL	_i2c_send_data   			;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, bk1080_i2c_write_end

	MOV		A, R7			 			;write reg address Reg*2
	CLR		C
	RLC		A
	CALL	_i2c_send_data   			;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, bk1080_i2c_write_end

	MOV		DPTR, #uc_fm_write_bytes
bk1080_i2c_send:
	MOVX	A, @DPTR
	CALL	_i2c_send_data   							;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, bk1080_i2c_write_end
	INC		DPTR
	DJNZ	R3, bk1080_i2c_send

bk1080_i2c_write_end:
	CALL	_i2c_stop
	CALL	_fm_i2c_disable
	RET
/*******************************************************************************
Function: _i2c_send_data()
Description: i2c send data(one byte)
Calls:
Global:Null
Input: A: the data to be sent
Output:R6: 0 means successfully check ack
Others:
********************************************************************************/
_i2c_send_data:
	CALL	_i2c_set_sda_out
	
	MOV     R4, #8				;send 8 bits data
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
	DJNZ	R4, i2c_send_data_loop

	CALL	_i2c_set_sda		;setting DATA high

	CALL	_i2c_set_sda_in		;check the "ack" bit
	CALL	_delay_4us

	CALL	_i2c_set_scl			;read one bit
	
	MOV     R4, #064H
i2c_check_ack_testloop:
	MOV     R6, #0				;0 means successfully check ack
	MOV     A, FM_DATA_PORT
	ANL		A, #FM_DATA_MASK
	JZ		ack_i2c				;"ack" is low level
	MOV     R6, #1				;1 means unsuccessfully check ack
	DJNZ    R4, i2c_check_ack_testloop
ack_i2c:
	CALL	_i2c_clr_scl
	RET


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
	MOV		R4, #8
i2c_get_data_loop:
	CALL	_i2c_set_scl

	MOV		R5, A				;save ACC
	MOV		A, FM_DATA_PORT
	ANL		A, #FM_DATA_MASK
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
	DJNZ	R4, i2c_get_data_loop

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
	RET



/*******************************************************************************
1. _i2c_start
2. _i2c_stop
3. _i2c_set_scl
4. _i2c_clr_scl
5. _i2c_set_sda
6. _i2c_clr_sda
7. _i2c_set_sda_in
8. _i2c_set_sda_out
********************************************************************************/
_i2c_start:
	CALL	_i2c_set_sda_out
	CALL	_i2c_set_sda
	CALL	_i2c_set_scl
	CALL	_i2c_clr_sda
	JMP		_i2c_clr_scl


_i2c_stop:
	CALL	_i2c_set_sda_out
	CALL	_i2c_clr_sda
	CALL	_i2c_set_scl
	CALL	_i2c_set_sda
	JMP		_i2c_clr_scl


_i2c_set_scl:
	ORL		FM_CLK_PORT, #FM_CLK_MASK
	JMP		_delay_4us


_i2c_clr_scl:
	ANL  	FM_CLK_PORT, #(~FM_CLK_MASK)
	JMP     _delay_4us


_i2c_set_sda:
	ORL  	FM_DATA_PORT, #FM_DATA_MASK
	JMP     _delay_4us


_i2c_clr_sda:
	ANL  	FM_DATA_PORT, #(~FM_DATA_MASK)               
	JMP     _delay_4us
	
	
_i2c_set_sda_in:
	ANL     FM_DATA_TYPE, #(~FM_DATA_MASK)
	RET


_i2c_set_sda_out:
	ORL     FM_DATA_TYPE, #FM_DATA_MASK 
	RET
	
	
/*******************************************************************************
delay for I2C(48 cycle / 12MHz = 4us)
MCU Clock is 12MHz
********************************************************************************/
_delay_4us:
	MOV		R0,	#30			;1Tm	
nop_loop:
	NOP						;1Tm
	DJNZ	R0,	nop_loop	;2Tm
	RET						;2Tm
	
#endif
	END