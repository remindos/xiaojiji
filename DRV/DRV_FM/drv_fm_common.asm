/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_fm_common.asm
Creator: andyliu					Date: 2009-07-18
Description: fm i2c communication, and basic function
Others:
Version: V0.1
History:
	V0.1	2009-07-18		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef DRV_FM_COMMON
#include "sh86278_sfr_reg.inc"
#include "drv_fm.h"

PUBLIC	_fm_i2c_enable
PUBLIC	_fm_i2c_disable
PUBLIC	__fm_i2c_read
PUBLIC	__fm_i2c_write
PUBLIC	_i2c_set_sda_out
PUBLIC	_i2c_clr_sda
PUBLIC	__delay_1ms
EXTRN	XDATA	(uc_fm_write_bytes)
EXTRN	XDATA	(uc_fm_read_bytes)

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
	

#ifdef DRV_FM_TEA5767
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

	MOV		A, #(CHIP_ADDRESS & (~CHIP_WR0_RD1_MASK))	;address+wr
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
#endif


#ifdef DRV_FM_TEA5767
/*******************************************************************************
Function: __fm_i2c_read()
Description: read operation, read 5 bytes from the chip to check the status
Calls:
Global:uc_fm_read_bytes[5]
Input: R7:the number of bytes that need read, uc_fm_read_bytes[0]~uc_fm_read_bytes[4]
Output:Null
Others:
********************************************************************************/
__fm_i2c_read:
	CALL	_fm_i2c_enable

	CALL	_i2c_start

	MOV		A, #(CHIP_ADDRESS | (CHIP_WR0_RD1_MASK))	;address+rd
	CALL	_i2c_send_data   							;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, i2c_read_end

	MOV		DPTR, #uc_fm_read_bytes
i2c_get:
	MOV		R6, #1
	CALL	_i2c_get_data   	;Input:R6, R6 = 1 means need send ack
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R7, i2c_get

	MOV		R6, #0
	CALL	_i2c_get_data   	;Input:R6, R6 = 0 means no need send ack; Output is ACC
	MOVX	@DPTR, A

i2c_read_end:
	CALL	_i2c_stop
	CALL	_fm_i2c_disable

	RET
#endif

#ifdef DRV_FM_AR1010
/********************************************************************************************************
Function:	__fm_i2c_write()
Description:	write operation,write bytes to control the chip
Input:		R7(the number of bytes that need write),fm_write_bytes[0]~fm_write_bytes[4]
Output:		R6: 0 means write successfully,1 means error
Temp VAR:	DPTR
Memo:
********************************************************************************************************/
__fm_i2c_write:
	CALL	_fm_i2c_enable
	MOV		A, R7
	MOV 	R5, A
	MOV	 	R2, #064H
loop_again:
	MOV		MR_WDT, #0FFH
	MOV		R7, #2
	MOV  	R3, #0
	LCALL  _i2c_start
   
  	MOV		A, #AR1010_I2C_WRITE_ADDR	 ; Write Address
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
#endif


#ifdef DRV_FM_AR1010
/********************************************************************************************************
Function:	__fm_i2c_read()
Description:	read operation,read 5 bytes from the chip to check the status
Input:		None	
Output:		fm_read_bytes((gobal))
Temp VAR:	DPTR,R4
Memo:
********************************************************************************************************/
__fm_i2c_read:
	CALL	_fm_i2c_enable
 	MOV   	A, R7
	MOV   	R5, A
	MOV  	R3, #0
label_loop1:
	LCALL	_i2c_start
	MOV     A, #AR1010_I2C_WRITE_ADDR

    LCALL   _i2c_send_data   ;Output:R6:0 means successfully check ack        
	MOV   	A, R3
	ORL   	A, R6
	MOV   	R3, A
 	MOV    	A, R5
   	LCALL   _i2c_send_data
	 
	MOV   	A, R3
	ORL     A, R6
	MOV   	R3, A   
	CJNE   	R3, #0,	label_loop1
	
label_loop2:
    LCALL	_i2c_start
  	
	MOV     A, #AR1010_I2C_READ_ADDR
	LCALL   _i2c_send_data   ;Output:R6:0 means successfully check ack   
	  	 
    CJNE    R6, #0, label_loop2
	MOV     DPTR, #uc_fm_read_bytes
_fm_I2C_get:
    MOV     R6, #1
    LCALL   _i2c_get_data   ;Input:R6, R6 = 1 means need send ack
    MOVX    @DPTR,	A
    INC     DPTR

    MOV     R6,     #0
    LCALL   _i2c_get_data   ;Input:R6, R6 = 0 means no need send ack
    MOVX    @DPTR,  A
   
_fm_I2C_read_end:
    LCALL  _i2c_stop
	CALL	_fm_i2c_disable
	RET
#endif


#ifdef DRV_FM_SI4702
/*******************************************************************************
Function: __fm_i2c_write()
Description: write operation,write bytes to control the chip
Calls:
Global:Null
Input: R7: the number of bytes that need write, uc_fm_write_bytes[2]~uc_fm_write_bytes[11]
Output:R6: 0 means write successfully, 1 means error
Others:
********************************************************************************/
__fm_i2c_write:
	CALL	_fm_i2c_enable
	/*** START ***/
	CALL	_i2c_start
	/*** ADDRESS + WR ***/
	MOV		A, #(CHIP_ADDRESS & (~CHIP_WR0_RD1_MASK))	;address+wr
	CALL	_i2c_send_data
	/*** ACK ***/
	CJNE	R6, #0, i2c_write_end
	/*** DATA ***/
	MOV		DPTR, #uc_fm_write_bytes
i2c_send:
	MOVX	A, @DPTR
	CALL	_i2c_send_data
	/*** ACK ***/
	CJNE	R6, #0, i2c_write_end
	INC		DPTR
	DJNZ	R7, i2c_send
i2c_write_end:
	/*** STOP ***/
	CALL	_i2c_stop
	CALL	_fm_i2c_disable	
	RET


/*******************************************************************************
Function: __fm_i2c_read()
Description: read operation,write bytes to control the chip
Calls:
Global:Null
Input :R7: start addr of uc_fm_write_bytes[], R5: end addr of uc_fm_write_bytes[]
Output:R6: 0 means write successfully, 1 means error
Others:
********************************************************************************/
__fm_i2c_read:
	CALL	_fm_i2c_enable
	/*** START ***/
	CALL	_i2c_start
	/*** ADDRESS + RD ***/
	MOV		A, #(CHIP_ADDRESS | CHIP_WR0_RD1_MASK)	;address+wr
	CALL	_i2c_send_data
	/*** ACK ***/
	CJNE	R6, #0, i2c_read_end
	/*** DATA ***/
	MOV		DPTR, #uc_fm_read_bytes
i2c_get:
	MOV		R6, #1
	CALL	_i2c_get_data   	;Input:R6, R6 = 1 means need send ack
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R7, i2c_get

	MOV		R6, #0
	CALL	_i2c_get_data   	;Input:R6, R6 = 0 means no need send ack; Output is ACC
	MOVX	@DPTR, A

i2c_read_end:
	/*** STOP ***/
	CALL	_i2c_stop
	CALL	_fm_i2c_disable	
	RET
#endif

#ifdef DRV_FM_CL6017S
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

	MOV		A, #(CHIP_ADDRESS & (~CHIP_WR0_RD1_MASK))	;address+wr
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
#endif


#ifdef DRV_FM_CL6017S
/*******************************************************************************
Function: __fm_i2c_read()
Description: read operation, read 5 bytes from the chip to check the status
Calls:
Global:uc_fm_read_bytes[5]
Input: R7:the number of bytes that need read, uc_fm_read_bytes[0]~uc_fm_read_bytes[4]
Output:Null
Others:
********************************************************************************/
__fm_i2c_read:
	CALL	_fm_i2c_enable

	CALL	_i2c_start

	MOV		A, #(CHIP_ADDRESS | (CHIP_WR0_RD1_MASK))	;address+rd
	CALL	_i2c_send_data   							;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, i2c_read_end

	MOV		DPTR, #uc_fm_read_bytes
i2c_get:
	MOV		R6, #1
	CALL	_i2c_get_data   	;Input:R6, R6 = 1 means need send ack
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R7, i2c_get

	MOV		R6, #0
	CALL	_i2c_get_data   	;Input:R6, R6 = 0 means no need send ack; Output is ACC
	MOVX	@DPTR, A

i2c_read_end:
	CALL	_i2c_stop
	CALL	_fm_i2c_disable
	RET
#endif


#ifdef DRV_FM_RDA5807SP
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

	MOV		A, #(CHIP_ADDRESS & (~CHIP_WR0_RD1_MASK))	;address+wr
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
#endif


#ifdef DRV_FM_RDA5807SP
/*******************************************************************************
Function: __fm_i2c_read()
Description: read operation, read 5 bytes from the chip to check the status
Calls:
Global:uc_fm_read_bytes[5]
Input: R7:the number of bytes that need read, uc_fm_read_bytes[0]~uc_fm_read_bytes[4]
Output:Null
Others:
********************************************************************************/
__fm_i2c_read:
	CALL	_fm_i2c_enable

	CALL	_i2c_start

	MOV		A, #(CHIP_ADDRESS | (CHIP_WR0_RD1_MASK))	;address+rd
	CALL	_i2c_send_data   							;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, i2c_read_end

	MOV		DPTR, #uc_fm_read_bytes
i2c_get:
	MOV		R6, #1
	CALL	_i2c_get_data   	;Input:R6, R6 = 1 means need send ack
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R7, i2c_get

	MOV		R6, #0
	CALL	_i2c_get_data   	;Input:R6, R6 = 0 means no need send ack; Output is ACC
	MOVX	@DPTR, A

i2c_read_end:
	CALL	_i2c_stop
	CALL	_fm_i2c_disable

	RET
#endif


#ifdef DRV_FM_BK1080
/********************************************************************************************************
Function:	__fm_i2c_write()
Description:	write operation,write bytes to control the chip
Input:		R7(the number of bytes that need write),fm_write_bytes[0]~fm_write_bytes[4]
Output:		R6: 0 means write successfully,1 means error
Temp VAR:	DPTR
Memo:
********************************************************************************************************/
__fm_i2c_write:

	CALL	_fm_i2c_enable
	CALL	_i2c_start
	MOV		R3,#2

	MOV		A, #CHIP_ADDRESS 			;address+wr
	CALL	_i2c_send_data   			;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, i2c_write_end

	MOV		A, R7			 			;write reg address Reg*2
	CLR		C
	RLC		A
	CALL	_i2c_send_data   			;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, i2c_write_end

	MOV		DPTR, #uc_fm_write_bytes
i2c_send:
	MOVX	A, @DPTR
	CALL	_i2c_send_data   							;Output:R6:0 means successfully check ack        
	CJNE	R6, #0, i2c_write_end
	INC		DPTR
	DJNZ	R3, i2c_send

i2c_write_end:
	CALL	_i2c_stop
	CALL	_fm_i2c_disable
	RET

#endif


#ifdef DRV_FM_BK1080
/********************************************************************************************************
Function:	__fm_i2c_read()
Description:	read operation,read 5 bytes from the chip to check the status
Input:		None	
Output:		fm_read_bytes((gobal))
Temp VAR:	DPTR,R4
Memo:
********************************************************************************************************/
__fm_i2c_read:

	CALL	_fm_i2c_enable
	CALL	_i2c_start

	MOV		A, #CHIP_ADDRESS 			;address+rd	0x80
	CALL	_i2c_send_data   			;Output:R6:0 means successfully check ack     
	CJNE	R6, #0, i2c_read_end

	MOV		A, R7						 ;write read reg address reg*2+1
	CLR		C
	RLC		A
	INC		A
	CALL	_i2c_send_data   			;Output:R6:0 means successfully check ack       
	CJNE	R6, #0, i2c_read_end

	MOV		DPTR, #uc_fm_read_bytes
i2c_get:
	MOV		R6, #1
	CALL	_i2c_get_data   	;Input:R6, R6 = 1 means need send ack
	MOVX	@DPTR, A
	INC		DPTR

	MOV		R6, #0
	CALL	_i2c_get_data   	;Input:R6, R6 = 0 means no need send ack
	MOVX	@DPTR, A

i2c_read_end:
	CALL	_i2c_stop
	CALL	_fm_i2c_disable
	RET

#endif

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
#ifdef DRV_FM_RDA5807SP
	MOV		R0,	#100			;1Tm	
#else
	MOV		R0,	#30			;1Tm	
#endif
nop_loop:
	NOP						;1Tm
	DJNZ	R0,	nop_loop	;2Tm
	RET						;2Tm


__delay_1ms:
	MOV		A, R7				
	JZ		delay_error					;if R7 == 0, error

	MOV		A, MR_SYSCLK
	ANL		A, #00000111B				;Get the CPU Clock
	CJNE	A, #00H, not_1m_clk
	MOV		A, #5						;5*200=1000Cycle/1M=1ms
	JMP		delay_start					;current CPU Clock is 1M

not_1m_clk:
	CJNE	A, #01H, not_12m_clk
	MOV		A, #60						;60*200=12000Cycle/12M=1ms
	JMP		delay_start					;current CPU Clock is 12M

not_12m_clk:
	CJNE	A, #02H, not_22m_clk
	MOV		A, #113						;113*200=22600Cycle/22.5M=1ms
	JMP		delay_start					

not_22m_clk:
	CJNE	A, #03H, not_33m_clk
	MOV		A, #169						;169*200=33800Cycle/33.75M=1ms
	JMP		delay_start

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
	
#endif
	END