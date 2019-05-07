/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_msi_driver.asm
Creator: aj.zhao					Date: 2009-06-11
Description: msi driver, include these below function
	1. _init_sd_mmc_card()
	2. _read_msi_sec_sub() --- for normal mode
	3. _write_msi_sec_sub() --- for normal mode
Others: 
Version: V0.4
History:
	V0.4	2010-11-20		aj.zhao
			1. optimize read&write function
			2. add bit4 in uc_user_option1 for ajudge if the card has been protected via MSWP pin
	V0.3	2010-05-31		aj.zhao
			1. modify the arithmetic of SDSD/MMC capacity
			2. modify "_set_read_write_timeout", when in USB, timeout is 280ms, contrarily it is 5ms
			3. when SD card is unstable, set SD/MMC CLK for lower clock, and the lowest is 6M
			4. matching "enable" and "disable" in multi read and write
			5. add process of correct SD error status when timeout occur
	V0.2	2010-01-20		aj.zhao
			1. optimize SDI for init SD/MMC card
			2. optimize SDI for issue read or write error twice
			3. set read and write timeout respectively, for read opration is 9ms, and write is 43ms
	V0.1	2009-06-11		aj.zhao
			1. build this module							  
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef 	MSI_DRIVER_DEF

#include "sh86278_sfr_reg.inc"
#include "sh86278_api_macro.inc"
#include "hwdriver_resident.h"		   
#include "drv_msi_var_ext.inc"
#include "drv_msi.h"


#define MSI_SD_4BIT_MODE


MSI_DRIVER_FUNC		SEGMENT	CODE
					RSEG	MSI_DRIVER_FUNC

#ifdef MSI_DRIVER_INIT_DEF
PUBLIC	_init_sd_mmc_card
/*******************************************************************************
Function: _init_sd_mmc_card()
Description: initianize the sd card driver
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
_init_sd_mmc_card:
   	mReset_WatchDog
	CALL	_enable_msi
	MOV		DPTR, #uc_user_option2
	MOVX	A, @DPTR
	MOV		B, MR_MSCON4
	JNB		ACC.2, MSCDn_Normal_1
	JNB		B.7, begin_init_routine
	JMP		_disable_msi	
MSCDn_Normal_1:	
	JB		B.7, begin_init_routine
	JMP		_disable_msi
								   
begin_init_routine:
	CALL	_init_msi_common
/*** initial command flow ***/
;MR_MSCON1(bit6:4->clock number in one NOP operation;bit3:0->operation select)
;MR_MSCON2(bit5:4->response type;bit2->plug state change inerrupt enable)
;MR_MSCON3(bit5:4-> WRNUM---Write Block Data Prepare Time = 2 eWRNUM+1;bit3:0--BSIZE)
;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode)
;MR_MSARGU1
;MR_MSARGU2
;MR_MSARGU3
;MR_MSARGU4
;MR_MSCMD(bit5:0---command index)

;<<send CMD0---software reset, resets all cards to idle state>>
	CALL	CMD0_GoIdleState
	
;<<send CMD8---send SD Memory Card interface condition for supply voltage range>>
	MOV		MR_MSCON1 ,#02H			;Command with Response
	MOV 	MR_MSCON2, #00H			;response type is R1(R7 has the same format with R1)			
	MOV 	MR_MSCON3, #00H			
	MOV 	MR_MSCON4, #00H			
	MOV 	MR_MSARGU1, #0AAH		;it is recommended 0xaa for the "check pattern"
	MOV 	MR_MSARGU2, #01H		;supply voltage range is select for 2.7~3.6V		
	MOV 	MR_MSARGU3, #00H		
	MOV		MR_MSARGU4, #00H		
	MOV 	MR_MSCMD, #08H
	CALL	_write_command	
	
/*** send ACMD41 in succession for check card busy, if card leisure, and can obtain OCR ***/ 
	MOV		uc_msi_count0, #20
	MOV		uc_msi_count1, #50
init_sd_routine:
/*** ACMD41 for SD card ***/
;<<send CMD55---*1.for indicate the next CMD is a application specific CMD, *2.for check SD/MMC card>>
;*1---ACMD is consist of CMD55 and CMDx which issue immediately after CMD 55 in SD card
;*2---if there has a echo back, and the card is a SD card, contrarily it is a MMC
	MOV		MR_MSCON1, #02H			;Command with Response				
	MOV 	MR_MSCON2, #00H			;response type is R1				
	MOV 	MR_MSCON3, #00H				
	MOV 	MR_MSCON4, #00H				
	MOV 	MR_MSARGU1, #00H			
	MOV 	MR_MSARGU2, #00H			
	MOV 	MR_MSARGU3, #00H			
	MOV		MR_MSARGU4, #00H			
	MOV 	MR_MSCMD, #55
	CALL	_write_command

/*** check command Timeout, and if Timeout occur, it is no response ***/
	MOV		A, uc_msi_err_flg
	JB		ACC.0, init_mmc_routine

;<<send CMD41---ask the card to echo back its OCR>>
	MOV		MR_MSCON1 ,#02H			;Command with Response						
	MOV 	MR_MSCON2, #20H			;response type is R3(OCR)				
	MOV 	MR_MSCON3, #00H				
	MOV 	MR_MSCON4, #00H				
	MOV 	MR_MSARGU1, #00H			
	MOV 	MR_MSARGU2, #80H		;set support supply voltage range 2.7~2.8V			
	MOV 	MR_MSARGU3, #0ffH		;set support supply voltage range 2.9~3.6V			
	MOV		MR_MSARGU4, #40H		;set support SDHC			
	MOV 	MR_MSCMD, #41				
	CALL	_write_command	
	JMP		check_busy

/*** send CMD1 for init MMC ***/
init_mmc_routine:
	CLR		b_msi_sd1_mmc0_flg
;<<send CMD1---ask the card to echo back its OCR>>
	MOV		MR_MSCON1 ,#02H			;Command with Response						
	MOV 	MR_MSCON2, #20H			;response type is R3(OCR)				
	MOV 	MR_MSCON3, #00H				
	MOV 	MR_MSCON4, #00H				
	MOV 	MR_MSARGU1, #00H			
	MOV 	MR_MSARGU2, #00H					
	MOV 	MR_MSARGU3, #80H		;set support supply voltage range 2.7~2.8V			
	MOV		MR_MSARGU4, #0ffH		;set support supply voltage range 2.9~3.6V			
	MOV 	MR_MSCMD, #1				
	CALL	_write_command	

check_busy:
/*** check busy for xs ***/
	DJNZ	uc_msi_count0, check_busy_sub
	MOV		A, uc_msi_err_flg
	JZ		continue_1326
	JMP		_disable_msi
continue_1326:
	MOV		uc_msi_count0, #20
	DJNZ	uc_msi_count1, check_busy_sub
	JMP		_disable_msi
check_busy_sub:
	MOV		DPTR, #MR_MSRP1
	MOVX	A, @DPTR
	CJNE	A, #3FH, check_sd_mmc_card_busy
	MOV		DPTR, #MR_MSRP2
	MOVX	A, @DPTR
	JNB		ACC.7, check_sd_mmc_card_busy	;bit39 of OCR is "busy--card busy status"
	JNB		ACC.6, identification_routine	;bit38 of OCR is "CCS--card capacity status"
	SETB	b_msi_sdhc_flg
	JMP		identification_routine

check_sd_mmc_card_busy: 
	JNB		b_msi_sd1_mmc0_flg, init_mmc_routine
	JMP		init_sd_routine
	
identification_routine:
;<<send CMD2---ask the card to send the CID number on the CMD line>>
	MOV		MR_MSCON1, #02H			;Command with Response				
	MOV 	MR_MSCON2, #10H			;response type is R2(CID)				
	MOV 	MR_MSCON3, #00H				
	MOV 	MR_MSCON4, #00H				
	MOV 	MR_MSARGU1, #00H			
	MOV 	MR_MSARGU2, #00H			
	MOV 	MR_MSARGU3, #00H			
	MOV		MR_MSARGU4, #00H			
	MOV 	MR_MSCMD, #2				
	CALL	_write_command	
	
;<<!send CMD3---ask the card to publish a new relative address(RCA)>>
	MOV		MR_MSCON1, #02H			;Command with Response				
	MOV 	MR_MSCON2, #30H			;response type is R6(RCA)				
	MOV 	MR_MSCON3, #00H				
	MOV 	MR_MSCON4, #00H				
	MOV 	MR_MSARGU1, #00H			
	MOV 	MR_MSARGU2, #00H			
	MOV 	MR_MSARGU3, #01H		;defaut {RCA = 1---only issue one card}			
	MOV		MR_MSARGU4, #00H			
	MOV 	MR_MSCMD, #3				
	CALL	_write_command	

	JNB		b_msi_sd1_mmc0_flg, mmc_RCA_fixed_1	

;<read response {RCA}	;uc_msi_response_buf:bit39:32,uc_msi_response_buf--bit31:bit24
	MOV		DPTR, #MR_MSRP2
	MOVX	A, @DPTR
	MOV		uc_msi_response_buf, A
	MOV		DPTR, #MR_MSRP3
	MOVX	A, @DPTR
	MOV		uc_msi_response_buf+1, A
	JMP		obtain_CSD_info	

mmc_RCA_fixed_1:
	MOV		A, #0
	MOV		uc_msi_response_buf, A
	MOV		A, #1
	MOV		uc_msi_response_buf+1, A

obtain_CSD_info:	
;<<!send CMD9---addressed card sends its card-specific data(CSD) on the CMD line >>
	MOV		MR_MSCON1, #02H				;Command with Response				
	MOV 	MR_MSCON2, #10H				;response type is R2(CSD)				
	MOV 	MR_MSCON3, #00H				
	MOV 	MR_MSCON4, #00H				
	MOV 	MR_MSARGU1, #00H			
	MOV 	MR_MSARGU2, #00H			
	MOV 	MR_MSARGU3, uc_msi_response_buf+1	;argument [31:16] is {RCA} 	
	MOV		MR_MSARGU4, uc_msi_response_buf		
	MOV 	MR_MSCMD, #9				
	CALL	_write_command	
	CALL	_obtain_memory_capacity
	
;<<send CMD7---select/deselect_card>>
	MOV		MR_MSCON1, #02H				;Command with Response				
	MOV 	MR_MSCON2, #00H			
	MOV 	MR_MSCON3, #08H				;set BSIZE is 512 bytes(1 sector)				
	MOV 	MR_MSCON4, #00H				
	MOV 	MR_MSARGU1, #00H			
	MOV 	MR_MSARGU2, #00H			
	MOV 	MR_MSARGU3, uc_msi_response_buf+1	;argument [31:16] is {RCA}
	MOV		MR_MSARGU4, uc_msi_response_buf	
	MOV 	MR_MSCMD, #07H				
	CALL	_write_command	

	JNB		b_msi_sd1_mmc0_flg,	init_card_over	
/*** ACMD6 is used to set data bus mode<1-bit mode, 4-bit mode>, only used in SD card ***/
;<<send CMD55---use to set next command is ACMD6>>
	MOV		MR_MSCON1, #02H				;Command with Response				
	MOV 	MR_MSCON2, #00H				
	MOV 	MR_MSCON3, #00H				
	MOV 	MR_MSCON4, #00H				
	MOV 	MR_MSARGU1, #00H			
	MOV 	MR_MSARGU2, #00H			
	MOV 	MR_MSARGU3, uc_msi_response_buf+1	;argument [31:16] is {RCA}	
	MOV		MR_MSARGU4, uc_msi_response_buf		
	MOV 	MR_MSCMD, #55				
	CALL	_write_command	

;<<send CMD6---!Defines the data bus width>>
	MOV		MR_MSCON1, #02H				;Command with Response
	MOV 	MR_MSCON2, #00H				
	MOV 	MR_MSCON3, #00H				
	MOV 	MR_MSCON4, #00H				
	MOV 	MR_MSARGU1, #00H			;default is 1-bit mode			
#ifdef MSI_SD_4BIT_MODE
	MOV		MR_MSARGU1, #02H			;set data bus is 4-bit mode			
#endif
	MOV 	MR_MSARGU2, #00H			
	MOV 	MR_MSARGU3, #00H			
	MOV		MR_MSARGU4, #00H			
	MOV 	MR_MSCMD, #6				
	CALL	_write_command	

init_card_over:	
;<set fast speed at 6MHz>
	/*MOV		A, #03
	MOV		DPTR, #uc_msi_clk_sel_cnt
	MOVX	@DPTR, A
	mAutoSet_MSClock*/
	mSet_MSClock_6M
	MOV		uc_msi_status, #00H			;init successful
	JMP   	_disable_msi


_init_msi_common:
	SETB	b_msi_sd1_mmc0_flg			;default is standard SD card
	CLR		b_msi_sdhc_flg							
	MOV		uc_msi_status, #SD_MMC_ERROR
	CLR		A
	MOV		DPTR, #uc_msi_sec_num_max_3
	MOVX	@DPTR, A
	MOV		DPTR, #uc_msi_sec_num_max_2
	MOVX	@DPTR, A
	MOV		DPTR, #uc_msi_sec_num_max_1
	MOVX	@DPTR, A
	MOV		DPTR, #uc_msi_sec_num_max_0
	MOVX	@DPTR, A
	/*MOV		DPTR, #uc_msi_clk_sel_cnt
	MOVX	@DPTR, A
	MOV		DPTR, #uc_msi_clk_351K_cnt
	MOVX	@DPTR, A*/
	
;<set low speed below 351.56KHz>
	//mAutoSet_MSClock
	mSet_MSClock_351K
	ANL		MR_MSCON2, #0FBH			;clr PIUGIREQEN
	
	CLR		A
	MOV		DPTR, #MR_MSTOUTH
	MOVX	@DPTR, A
	MOV		A, #01H
	MOV		DPTR, #MR_MSTOUTL			;set timeout : 1*2^10 MSI CLK
	MOVX	@DPTR, A
	RET		


/*******************************************************************************
Function: _obtain_memory_capacity()
Description: obtain SD/MMC card memory capacity
Calls:
Global:uc_msi_sec_num_max_0,uc_msi_sec_num_max_1,uc_msi_sec_num_max_2
Input: Null
Output:Null
Others:
uc_msi_c_size_h------R7
uc_msi_c_size_m------R6
uc_msi_c_size_l------R5
uc_msi_read_bl_len---R4
uc_msi_c_size_mult---R3
********************************************************************************/
sdhc_size:
	MOV		DPTR, #MR_MSRP2
	MOVX	A, @DPTR
	JNB		ACC.6, standard_sd_size
	MOV		R5, #00H					;sector numner low

	MOV		DPTR, #MR_MSRP9				;high byte
	MOVX	A, @DPTR
	MOV		R4, A
	MOV		DPTR, #MR_MSRP10
	MOVX	A, @DPTR
	MOV		R7, A
	MOV		DPTR, #MR_MSRP11
	MOVX	A, @DPTR
	ADD		A, #01H
	MOV		R6, A
	CLR		A
	ADDC	A, R7
	MOV		R7, A
	CLR		A
	ADDC	A, R4
	MOV		R4, A
	CALL	_rl_1bit
	CALL	_rl_1bit
	JMP		card_size_ok
	
_rl_1bit:
	CLR		C
	MOV		A, R6
	RLC		A
	MOV		R6, A
	MOV		A, R7
	RLC		A
	MOV		R7, A
	MOV		A, R4
	RLC		A
	MOV		R4, A
	RET


_obtain_memory_capacity:
;<get R4:BIT83:80 of CSD>
	JB		b_msi_sdhc_flg, sdhc_size

standard_sd_size:
	CLR		b_msi_sdhc_flg
	MOV		R7, #00H
	MOV		DPTR, #MR_MSRP7
	MOVX	A, @DPTR
	ANL		A, #0FH						;get read_bl_len
	CLR		C
	SUBB	A, #09H						;get sector number
	MOV		R4, A						;get read_bl_len

;<R6---bit11:bit8,R5--bit7:bit0>
	MOV		DPTR, #MR_MSRP8
	MOVX	A, @DPTR
	ANL		A, #03H						;get bit11:10 of C_SIZE(bit73:72 of CSD)
	MOV		R6, A
	MOV		DPTR, #MR_MSRP9				;get bit9:bit2 of C_SIZE(bit71:64 of CSD)
	MOVX	A, @DPTR
	CLR		C
	RLC		A
	MOV		R5, A
	MOV		A, R6
	RLC		A
	MOV		R6, A
	MOV		A, R5
	CLR		C
	RLC		A
	MOV		R5, A
	MOV		A, R6
	RLC		A
	MOV		R6, A
	MOV		DPTR, #MR_MSRP10			;get bit1:0 of 	C_SIZE(bit 63:62 of CSD)
	MOVX	A, @DPTR
	ANL		A, #0C0H
	SWAP	A
	RR		A
	RR		A
	ORL		A, R5						;get bit7:0 of C_SIZE
	CLR		C
	INC		A
	MOV		R5, A
	JNZ		loop1212
	MOV		A, R6
	INC		A
	MOV		R6, A
loop1212:
;<get device size multiplier>
	MOV		DPTR, #MR_MSRP11
	MOVX	A, @DPTR
	ANL		A, #03H
	RL		A				
	MOV		R3, A						;get bit2:1 of c_size_mult(bit49:48 of CSD)
	MOV		DPTR, #MR_MSRP12
	MOVX	A, @DPTR
	JB		ACC.7, loop1223
	MOV		A, #00H
	JMP		loop1224
loop1223:
	MOV		A, #01H
loop1224:
	ORL		A, R3
	INC		A
	INC		A
	ADD		A, R4
	MOV		R3, A						;c_size_mult= c_size_mult + 2 + read_bl_len
	
loop1231:
;< calculate card size(sector number)>
	MOV		A, R5
	CLR		C
	RLC		A
	MOV		R5, A
	MOV		A, R6
	RLC		A
	MOV		R6, A
	MOV		A, R7
	RLC		A
	MOV		R7, A
	DJNZ	R3, loop1231
	MOV		R4, #00H

card_size_ok:
	MOV		A, R4
	MOV		DPTR, #uc_msi_sec_num_max_3
	MOVX	@DPTR, A
	MOV		A, R7
	MOV		DPTR, #uc_msi_sec_num_max_2
	MOVX	@DPTR, A
	MOV		A, R6
	MOV		DPTR, #uc_msi_sec_num_max_1
	MOVX	@DPTR, A
	MOV		A, R5
	MOV		DPTR, #uc_msi_sec_num_max_0
	MOVX	@DPTR, A
	RET
#endif


_write_command:
	ORL		MR_MSSTA, #01H				;set MSGO bit	
check_MSGO_bit:
	mReset_WatchDog
	MOV		A, MR_MSSTA
	JB		ACC.0, check_MSGO_bit		;gobit is 0 indicate that have no use for check MSI error
	MOV		A, MR_MSSTA
	MOV		uc_msi_err_flg, #00H
check_MSI_status_err:
	JNB		ACC.1, check_CRC16_err
	ORL		uc_msi_err_flg, #08H		;SD/MMC status error
	RET
check_CRC16_err:
	JNB		ACC.2, check_CRC7_err
	ORL		uc_msi_err_flg, #04H		;CRC16 error
	RET
check_CRC7_err:
	JNB		ACC.3, check_Timeout_err
	ORL		uc_msi_err_flg, #02H		;CRC7 error
	RET
check_Timeout_err:
	mReset_WatchDog
	JNB		ACC.4, check_MSIRQ
	ORL		uc_msi_err_flg, #01H		;Timeout error
process_TimeoutErr:	
	/*CLR		C
	MOV		A, MR_MSCMD
	SUBB	A, #08H
	JZ		cmd8_timeout_invalid
	MOV		DPTR, #uc_msi_clk_sel_cnt
	MOVX	A, @DPTR
	CJNE	A, #01, debase_clock
	MOV		DPTR, #uc_msi_clk_351K_cnt
	MOVX	A, @DPTR
	INC		A
	MOVX	@DPTR, A
	CJNE	A, #30, not_reach_351K
debase_clock:		
	DEC		A
	MOVX	@DPTR, A		
	mAutoSet_MSClock
	JNZ		not_reach_351K
	CALL	CMD0_GoIdleState
cmd8_timeout_invalid:
not_reach_351K:*/	
	RET 

check_MSIRQ:
	JB		ACC.5, write_cmd_ok			;wait MSI interrupt flag
	JMP		check_Timeout_err

write_cmd_ok:
	CLR		ACC.5						;clr msi irq
	MOV		MR_MSSTA, A	
	/*CLR		A
	MOV		DPTR, #uc_msi_clk_351K_cnt
	MOVX	@DPTR, A*/	
	RET
	
	
CMD0_GoIdleState:
;<<send CMD0---software reset, resets all cards to idle state>>
	MOV		MR_MSCON1 ,#01H			;Command without Response			
	MOV 	MR_MSCON2, #00H			
	MOV 	MR_MSCON3, #00H			
	MOV 	MR_MSCON4, #00H			
	MOV 	MR_MSARGU1, #00H		
	MOV 	MR_MSARGU2, #00H		
	MOV 	MR_MSARGU3, #00H		
	MOV		MR_MSARGU4, #00H		
	MOV 	MR_MSCMD, #00H						
	JMP		_write_command	
	

;set Timeout:for read opration is 004BH*2^10 MSI CLK(5ms), and for write is 0fFF*2^10MSI CLK(280ms)
_set_read_write_timeout:
	JNB		b_read_block_flg, set_timeout_h 	
	MOV		A, #009H
	JMP		set_timeout_reg
set_timeout_h:
	MOV		A, #00FH
set_timeout_reg:
	MOV		DPTR, #MR_MSTOUTH
	MOVX	@DPTR, A  
#ifdef MSI_API_MULTI_SEC_DEF
	MOV		A, #0FFH
#else
	MOV		A, #04BH
#endif	
	MOV		DPTR, #MR_MSTOUTL			
	MOVX	@DPTR, A
	RET				



_enable_msi:
	MOV		A, MR_MODEN2
	MOV		R0, #uc_msi_save_moden2
	MOV		@R0, A
	ANL		A, #0F8H
	ORL		A, #006H
	MOV		MR_MODEN2, A		
	RET

_disable_msi:
	MOV		R0, #uc_msi_save_moden2
	MOV		MR_MODEN2, @R0
	RET



#ifdef	MSI_DRIVER_READ_SECTOR_DEF
PUBLIC	_read_msi_sec_sub
/*******************************************************************************
Function: _read_msi_sec_sub()
Description: read 512 bytes data from SD/MMC
Calls:
Global:uc_msi_sec_addr_0,uc_msi_sec_addr_1,uc_msi_sec_addr_2,uc_msi_sec_addr_3
       uc_msi_occ_buf_flg: 1--cbuf
                           2--dbuf0
                           3--dbuf1
                           4--fbuf
       uc_msi_byte_offset_0,uc_msi_byte_offset_1:buffer offset(word)
Input: Null
Output:CY:	1--read ok
            0--read error
Others:
********************************************************************************/
_read_msi_sec_sub:
	mReset_WatchDog
	CALL	_clr_msi_occ_buf
	
	CALL	_enable_msi
	CALL	_judge_card_ready
	JNC		read_sec_ret
		
	CALL	_set_read_write_timeout
	CALL	_convert_address			//output: cmdrequest_blockaddress1,cmdrequest_blockaddress0
	
;<<send CMD17---reads a block of the size selected by the SET_BLOCKLEN command >>		
	MOV		MR_MSCON1, #04H				;MR_MSCON1(bit3:0->4command with response and read single block bit6:4->clock number in one NOP operation;bit3:0->operation select)	
	MOV		MR_MSCON2, #00H				;MR_MSCON2(bit5:4->2 response1 type;bit3->plag state change inerrupt enable) 				   
	MOV		MR_MSCON3, #08H				;MR_MSCON3?(bit5:4----Write Block Data Prepare Time;bit3:0---Data transfer  2^ (BSIZE+1)= 2^(8+1)=512Byte
	MOV		MR_MSCON4, #00H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 	
#ifdef MSI_SD_4BIT_MODE
	JNB		b_msi_sd1_mmc0_flg, mmc_1bit
	ORL		MR_MSCON4, #01H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 				   
mmc_1bit:
#endif

	MOV		MR_MSCMD, #17				;MR_MSCMD(bit5:0---command index)	
	MOV		MR_MSIMAL, uc_msi_byte_offset_0		;FCEIMAL(bit7:0->Correspond Memory page Offset Address)
	MOV		MR_MSIMAH, uc_msi_byte_offset_1		;FCEIMAH(bit3:0->bit[11:8] of Correspond Memory page Offset Address)
	
	CALL  	_sel_msi_occ_buf			;return is "CY" 1--ok
	JNC		read_sec_ret
	
	CALL	_write_command
	CALL 	_release_msi_occ_buf	
read_sec_ret:
	JMP		_disable_msi
#endif



#ifdef	MSI_DRIVER_WRITE_SECTOR_DEF
PUBLIC	_write_msi_sec_sub
/*******************************************************************************
Function: _write_msi_sec_sub()
Description: write 512 bytes data to the SD/MMC
Calls:
Global:uc_msi_sec_addr_0,uc_msi_sec_addr_1,uc_msi_sec_addr_2,uc_msi_sec_addr_3
       b_msi_sd1_mmc0_flg: 1--SD Card
                           0--MMC
       uc_msi_occ_buf_flg: 1--cbuf
                           2--dbuf0
                           3--dbuf1
                           4--fbuf
       uc_msi_byte_offset_0,uc_msi_byte_offset_1:buffer offset(word)
Input: Null
Output:CY:	1--read ok
            0--read error
Others:
********************************************************************************/
_write_msi_sec_sub:
	mReset_WatchDog
	CALL	_enable_msi
	
	CALL	_judge_card_ready
	JNC		write_sec_ret
	CALL	_judge_card_protect
	JC		write_sec_ret
	
	CALL	_set_read_write_timeout
	CALL	_convert_address			;output: cmdrequest_blockaddress1,cmdrequest_blockaddress0
	
	MOV		MR_MSCON1, #05H				;MR_MSCON1(bit3:0->4command with response and write single block bit6:4->clock number in one NOP operation;bit3:0->operation select)	
	MOV		MR_MSCON2, #00H				;MR_MSCON2(bit5:4->2 response1 type;bit3->plag state change inerrupt enable) 				   
	MOV		MR_MSCON3, #08H				;MR_MSCON3?(bit5:4----Write Block Data Prepare Time;bit3:0---Data transfer  2^ (BSIZE+1)= 2^(8+1)=512Byte
	MOV		MR_MSCON4, #00H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 	
#ifdef MSI_SD_4BIT_MODE
	JNB		b_msi_sd1_mmc0_flg, mmc_1bit_1
	ORL		MR_MSCON4, #01H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 				   
mmc_1bit_1:
#endif
	
	MOV		MR_MSCMD, #24				;MR_MSCMD(bit5:0---command index)	
	MOV		MR_MSIMAL, uc_msi_byte_offset_0		;FCEIMAL(bit7:0->Correspond Memory page Offset Address)
	MOV		MR_MSIMAH, uc_msi_byte_offset_1		;FCEIMAH(bit3:0->bit[11:8] of Correspond Memory page Offset Address)		
	
	CALL  	_sel_msi_occ_buf			;return is "CY" 1--ok
	JNC		write_sec_ret
	
	CALL	_write_command
	CALL 	_release_msi_occ_buf	
write_sec_ret:
	JMP		_disable_msi
#endif


#ifdef MSI_CONVERT_ADDR_DEF
/*******************************************************************************
Function: _convert_address()
Description: convert logical sector addr into bytes addr
Calls:
Global:uc_msi_sec_addr_0,uc_msi_sec_addr_1,uc_msi_sec_addr_2,uc_msi_sec_addr_3: sector addr
       MR_MSARGU1,MR_MSARGU2,MR_MSARGU3,MR_MSARGU4: bytes addr
Input: Null
Output:Null
Others:
********************************************************************************/
_convert_address:
	JB		b_msi_sdhc_flg, cnv_sdhc_addr
	MOV		MR_MSARGU1, #00H			;MR_MSARGU1(bit7:0=00H) argument:bit31:bit0----data address
	MOV		A, uc_msi_sec_addr_0
	CLR		C							;CLR CY
	RLC		A
	MOV		MR_MSARGU2, A				;MR_MSARGU2(bit15:8),bit8=0
	MOV		A, uc_msi_sec_addr_1
	RLC		A
	MOV		MR_MSARGU3, A				;MR_MSARGU3(bit23:16)
	MOV		A, uc_msi_sec_addr_2
	RLC		A
	MOV		MR_MSARGU4, A 				;MR_MSARGU4(bit31:24)
	RET
	
cnv_sdhc_addr:
	MOV		MR_MSARGU1, uc_msi_sec_addr_0
	MOV		MR_MSARGU2, uc_msi_sec_addr_1
	MOV		MR_MSARGU3, uc_msi_sec_addr_2
	MOV		MR_MSARGU4, uc_msi_sec_addr_3
	RET

#endif


#ifdef MSI_COMMON_USED_DEF
/*******************************************************************************
Function: _sel_msi_occ_buf()
Description: MSI occupy buffer
Calls:
Global:uc_msi_occ_buf_flg: 1--cbuf
                           2--dbuf0
                           3--dbuf1
                           4--fbuf
Input: Null
Output:CY: 1--ok
           0--fail
Others:
********************************************************************************/
_sel_msi_occ_buf:
	ANL		MR_MEMSEL2, #08fH
	MOV		A, uc_msi_occ_buf_flg
	ANL		A, #0FH
	SWAP	A
	ORL		MR_MEMSEL2, A				;MSI occupy buffer
	NOP
	NOP
	
	MOV		R7, uc_msi_occ_buf_flg
	CJNE	R7, #SFC_APP_CBUF, not_occupy_cbuf
	MOV		A, MR_MEMSTA2
	ANL		A, #07H
	CJNE	A, #07H, occupy_buf_err
	JMP		occupy_buf_ok
not_occupy_cbuf:
	CJNE	R7, #SFC_APP_DBUF0, not_occupy_dbuf0
	MOV		A, MR_MEMSTA1
	ANL		A, #07H
	CJNE	A, #07H, occupy_buf_err
	JMP		occupy_buf_ok
not_occupy_dbuf0:
	CJNE	R7, #SFC_APP_DBUF1, not_occupy_dbuf1
	MOV		A, MR_MEMSTA1
	ANL		A, #70H
	CJNE	A, #70H, occupy_buf_err
	JMP		occupy_buf_ok
not_occupy_dbuf1:
	CJNE	R7, #SFC_APP_FBUF, occupy_buf_err
	MOV		A, MR_MEMSTA2
	ANL		A, #70H
	CJNE	A, #70H, occupy_buf_err
occupy_buf_ok:
	SETB	C
	RET

occupy_buf_err:
	CLR		C
	RET


_release_msi_occ_buf:
	MOV		R5, #03H
again_release:
	ANL		MR_MEMSEL2, #08fH
	NOP
	NOP
	NOP
	MOV		A, MR_MEMSEL2
	ANL		A, #70H
	JZ		release_ok
	DJNZ	R5, again_release
release_ok:
	RET



_clr_msi_occ_buf:
	MOV		A, uc_msi_occ_buf_flg
	CJNE	A, #SFC_APP_CBUF, not_clr_cbuf
	MOV		DPTR, #CBUF_ADDR
	JMP		clr_buf
not_clr_cbuf:
	CJNE	A, #SFC_APP_DBUF0, not_clr_dbuf0
	MOV		DPTR, #DBUF0_ADDR
	JMP		clr_buf
not_clr_dbuf0:
	CJNE	A, #SFC_APP_DBUF1, not_clr_dbuf1
	MOV		DPTR, #DBUF1_ADDR
	JMP		clr_buf
not_clr_dbuf1:
	CJNE	A, #SFC_APP_FBUF, not_clr_fbuf
	MOV		DPTR, #FBUF_ADDR
clr_buf:
	MOV		A, DPL
	ADD		A, uc_msi_byte_offset_0
	MOV		DPL, A
	MOV		A, DPH
	ADDC	A, uc_msi_byte_offset_1
	ADD		A, uc_msi_byte_offset_1
	MOV		DPH, A

	MOV		R6, #4						;clr buffer length is 512bytes
	MOV		R7, #128
	CLR		A
loop_clr_buf:
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R6, loop_clr_buf
	MOV		R6, #4
	DJNZ	R7, loop_clr_buf
not_clr_fbuf:
	RET
	
	
_judge_card_ready:
	MOV		DPTR, #uc_user_option2
	MOVX	A, @DPTR
	MOV		B, MR_MSCON4
	JNB		ACC.2, MSCDn_Normal_2
	JNB		B.7, card_ready
	JMP		card_not_ready	
MSCDn_Normal_2:	
	JB		B.7, card_ready	
card_not_ready:	
	MOV		uc_msi_status, #0FFH
	CLR		C
	JMP		judge_complete
card_ready:
	SETB	C
	JMP		judge_complete
	
_judge_card_protect:
	MOV		DPTR, #uc_user_option1
	MOVX	A, @DPTR
	JNB		ACC.4, not_check_MSWP_pin
	JNB		b_msi_sd1_mmc0_flg, not_check_MSWP_pin	;if it's MMC, ant do not check protect pin
	MOV		A, MR_MSCON4
	JB		ACC.6, not_check_MSWP_pin				;BIT6 1--not protect; 0--protect
	SETB	C
	JMP		judge_complete
not_check_MSWP_pin:	
	CLR		C
judge_complete:	
	RET
		
#endif


#ifdef MSI_API_MULTI_SEC_DEF
PUBLIC	_read_msi_multisector_b
PUBLIC	_read_msi_multisector
PUBLIC	_read_msi_multisector_e
PUBLIC	_write_msi_multisector_b
PUBLIC	_write_msi_multisector
PUBLIC	_write_msi_multisector_e

/*******************************************************************************
Function: _read_msi_multisector_b()
          _read_msi_multisector()
          _read_msi_multisector_e()
Description:start read multi sector & read 512 bytes data from the SD/MMC card
Calls:
Global:uc_msi_occ_buf_flg: 1--cbuf
                           2--dbuf0
                           3--dbuf1
                           4--fbuf
       uc_msi_sec_addr_0,uc_msi_sec_addr_1,uc_msi_sec_addr_2,uc_msi_sec_addr_3: logic sector address
Input: Null
Output:Null
Others:
********************************************************************************/
_read_msi_multisector_b:
	mReset_WatchDog
	
	CALL	_enable_msi
	CALL	_judge_card_ready
	JNC		read_multi_b_ret
	
	CALL	_set_read_write_timeout
	CALL	_convert_address			;output: cmdrequest_blockaddress1,cmdrequest_blockaddress0
	
;<<send CMD18---sends data continuously until a STOP_TRANSMISSION >> <Read multi block begin>
	MOV		MR_MSCON1, #06H				;MR_MSCON1(bit3:0->4command with response and read single block bit6:4->clock number in one NOP operation;bit3:0->operation select)	
	MOV		MR_MSCON2, #00H				;MR_MSCON2(bit5:4->2 response1 type;bit3->plag state change inerrupt enable) 				   
	MOV		MR_MSCON3, #08H				;MR_MSCON3?(bit5:4----Write Block Data Prepare Time;bit3:0---Data transfer  2^ (BSIZE+1)= 2^(8+1)=512Byte					
	MOV		DPTR, #uc_msi_block_num
	MOVX	A, @DPTR
	DEC		A
	RL		A
	CLR 	ACC.0
	MOV		MR_MSCON4, A				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode;bit5:1---BNUM,Block Number = BNUM + 1 = 3 +1=4)			   			
#ifdef MSI_SD_4BIT_MODE
	JNB		b_msi_sd1_mmc0_flg, mmc_1bit_2
	ORL		MR_MSCON4, #01H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 				   
mmc_1bit_2:
#endif
	
	MOV		MR_MSCMD, #18				;MR_MSCMD(bit5:0---command index)
	MOV		MR_MSIMAL, uc_msi_byte_offset_0		;FCEIMAL(bit7:0->Correspond Memory page Offset Address)
	MOV		MR_MSIMAH, uc_msi_byte_offset_1		;FCEIMAH(bit3:0->bit[11:8] of Correspond Memory page Offset Address)
	
	CALL  	_sel_msi_occ_buf			;return is "CY" 1--ok
	JNC		read_multi_b_ret
	
	CALL	_write_command
	CALL 	_release_msi_occ_buf
read_multi_b_ret:
	JMP		_disable_msi



/*******************************************************************************
continue read data from card
********************************************************************************/
_read_msi_multisector:
	mReset_WatchDog
	
	CALL	_enable_msi
	CALL	_judge_card_ready
	JNC		read_multi_ret
		
	MOV		MR_MSCON1, #08H				;MR_MSCON1(bit3:0->4command with response and read single block bit6:4->clock number in one NOP operation;bit3:0->operation select)	
	MOV		MR_MSCON2, #00H				;MR_MSCON2(bit5:4->2 response1 type;bit3->plag state change inerrupt enable) 				   
	MOV		MR_MSCON3, #08H				;MR_MSCON3?(bit5:4----Write Block Data Prepare Time;bit3:0---Data transfer  2^ (BSIZE+1)= 2^(8+1)=512Byte			
	MOV		DPTR, #uc_msi_block_num
	MOVX	A, @DPTR
	DEC		A
	RL		A
	CLR 	ACC.0
	MOV		MR_MSCON4, A				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode;bit5:1---BNUM,Block Number = BNUM + 1 = 3 +1=4)				   			
#ifdef MSI_SD_4BIT_MODE
	JNB		b_msi_sd1_mmc0_flg, mmc_1bit_3
	ORL		MR_MSCON4, #01H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 				   
mmc_1bit_3:
#endif
	
	MOV		MR_MSIMAL, uc_msi_byte_offset_0		;FCEIMAL(bit7:0->Correspond Memory page Offset Address)
	MOV		MR_MSIMAH, uc_msi_byte_offset_1		;FCEIMAL(bit3:0->bit[11:8] of Correspond Memory page Offset Address)
	
	CALL  	_sel_msi_occ_buf			;return is "CY" 1--ok
	JNC		read_multi_ret
	CALL	_write_command_for_multi_con
read_multi_ret:
	JMP		_disable_msi



/*******************************************************************************
force the card to stop transmission
********************************************************************************/
_read_msi_multisector_e:
	mReset_WatchDog
	
	CALL	_enable_msi
	CALL	_judge_card_ready
	JNC		read_multi_e_ret
	
;<<send CMD12---terminates a multiple block read/write operation >>
	MOV		MR_MSCON1, #02H				;MR_MSCON1(bit3:0->3command with response and check busy; bit6:4->clock number in one NOP operation;bit3:0->operation select)	
	MOV		MR_MSCON2, #00H				;MR_MSCON2(bit5:4->0 response1 type;bit3->plag state change inerrupt enable) 				   
	MOV		MR_MSCON3, #08H				;MR_MSCON3?(bit5:4----Write Block Data Prepare Time;bit3:0---Data transfer  2^ (BSIZE+1)= 2^(8+1)=512Byte
	MOV		MR_MSCON4, #00H		   
#ifdef MSI_SD_4BIT_MODE
	JNB		b_msi_sd1_mmc0_flg, mmc_1bit_4
	ORL		MR_MSCON4, #01H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 				   
mmc_1bit_4:
#endif
	
	MOV		MR_MSCMD, #12				;MR_MSCMD(bit5:0---command index)  								
	CALL	_write_command
read_multi_e_ret:	
	JMP		_disable_msi



/*******************************************************************************
Function: _write_msi_multisector_b()
          _write_msi_multisector()
          _write_msi_multisector_e()
Description:start write multi sector & write 512 bytes data to the SD card
Calls:
Global:uc_msi_occ_buf_flg: 1--cbuf
                           2--dbuf0
                           3--dbuf1
                           4--fbuf
       uc_msi_block_num:   write block number
       uc_msi_sec_addr_0,uc_msi_sec_addr_1,uc_msi_sec_addr_2,uc_msi_sec_addr_3: logic sector address
Input: Null
Output:Null
Others:
********************************************************************************/
_write_msi_multisector_b:
	mReset_WatchDog	
	CALL	_enable_msi
	
	CALL	_judge_card_ready
	JNC		write_multi_b_ret
	CALL	_judge_card_protect
	JC		write_multi_b_ret
	
	CALL	_set_read_write_timeout
	CALL	_convert_address			;output: cmdrequest_blockaddress1,cmdrequest_blockaddress0
	
//<<send CMD25---writes blocksof data continuously until a STOP_TRANSMISSION >>
	MOV		MR_MSCON1, #07H				;MR_MSCON1(bit3:0->4command with response and read single block bit6:4->clock number in one NOP operation;bit3:0->operation select)	
	MOV		MR_MSCON2, #00H				;MR_MSCON2(bit5:4->2 response1 type;bit3->plag state change inerrupt enable) 				   
	MOV		MR_MSCON3, #08H				;MR_MSCON3?(bit5:4----Write Block Data Prepare Time;bit3:0---Data transfer  2^ (BSIZE+1)= 2^(8+1)=512Byte			
	MOV		DPTR, #uc_msi_block_num
	MOVX	A, @DPTR
	DEC		A
	RL		A
	CLR 	ACC.0
	MOV		MR_MSCON4, A				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode;bit5:1---BNUM,Block Number = BNUM + 1 = 3 +1=4) 				   			
#ifdef MSI_SD_4BIT_MODE
	JNB		b_msi_sd1_mmc0_flg, mmc_1bit_5
	ORL		MR_MSCON4, #01H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 				   
mmc_1bit_5:
#endif
	
	MOV		MR_MSCMD, #25
	MOV		MR_MSIMAL, uc_msi_byte_offset_0		;FCEIMAL(bit7:0->Correspond Memory page Offset Address)
	MOV		MR_MSIMAH, uc_msi_byte_offset_1		;FCEIMAH(bit3:0->bit[11:8] of Correspond Memory page Offset Address)
	
	CALL  	_sel_msi_occ_buf			;return is "CY" 1--ok
	JNC		write_multi_b_ret
	
	CALL	_write_command
	CALL 	_release_msi_occ_buf
write_multi_b_ret:
	JMP		_disable_msi



/*******************************************************************************
continue write data to card
********************************************************************************/
_write_msi_multisector:
	mReset_WatchDog	
	CALL	_enable_msi
	
	CALL	_judge_card_ready
	JNC		write_multi_ret
	CALL	_judge_card_protect
	JC		write_multi_ret
		
	MOV		MR_MSCON1, #09H				;MR_MSCON1(bit3:0->4command with response and read single block bit6:4->clock number in one NOP operation;bit3:0->operation select)	
	MOV		MR_MSCON2, #00H				;MR_MSCON2(bit5:4->2 response1 type;bit3->plag state change inerrupt enable) 				   
	MOV		MR_MSCON3, #08H				;MR_MSCON3?(bit5:4----Write Block Data Prepare Time;bit3:0---Data transfer  2^ (BSIZE+1)= 2^(8+1)=512Byte			
	MOV		DPTR, #uc_msi_block_num
	MOVX	A, @DPTR
	DEC		A
	RL		A
	CLR		ACC.0
	MOV		MR_MSCON4, A				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode;bit5:1---BNUM,Block Number = BNUM + 1 = 3 +1=4) 				   			
#ifdef MSI_SD_4BIT_MODE
	JNB		b_msi_sd1_mmc0_flg, mmc_1bit_6
	ORL		MR_MSCON4, #01H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 				   
mmc_1bit_6:
#endif
	
	MOV		MR_MSIMAL, uc_msi_byte_offset_0		;FCEIMAL(bit7:0->Correspond Memory page Offset Address)
	MOV		MR_MSIMAH, uc_msi_byte_offset_1		;FCEIMAL(bit3:0->bit[11:8] of Correspond Memory page Offset Address)			 
	
	CALL  	_sel_msi_occ_buf			;return is "CY" 1--ok
	JNC		write_multi_ret
	CALL	_write_command_for_multi_con
write_multi_ret:
	JMP		_disable_msi



/*******************************************************************************
force the card to stop transmission
********************************************************************************/
_write_msi_multisector_e:
	mReset_WatchDog	
	CALL	_enable_msi
	
	CALL	_judge_card_ready
	JNC		write_multi_e_ret
	CALL	_judge_card_protect
	JC		write_multi_e_ret
	
;<<send CMD12---terminates a multiple block read/write operation >>
	MOV		MR_MSCON1, #03H				;R0------MR_MSCON1(bit3:0->3command with response and check busy; bit6:4->clock number in one NOP operation;bit3:0->operation select)
	MOV		MR_MSCON2, #00H				;R1------MR_MSCON2(bit5:4->0 response1 type;bit3->plag state change inerrupt enable)
	MOV		MR_MSCON3, #08H				;R2------MR_MSCON3?(bit5:4----Write Block Data Prepare Time;bit3:0---Data transfer  2^ (BSIZE+1)= 2^(8+1)=512Byte
	MOV		MR_MSCON4, #00H				;R3------MR_MSCON4(bit0->0:MMC/SD 1 bit mode;bit0->0:MMC/SD 1-bit mode)
#ifdef MSI_SD_4BIT_MODE
	JNB		b_msi_sd1_mmc0_flg, mmc_1bit_7
	ORL		MR_MSCON4, #01H				;MR_MSCON4(bit0->1:SD 4 bit mode;bit0->0:MMC/SD 1-bit mode) 				   
mmc_1bit_7:
#endif
	
	MOV		MR_MSCMD, #12				;R4------MR_MSCMD(bit5:0---command index)
	CALL	_write_command
write_multi_e_ret:	
	JMP		_disable_msi



_write_command_for_multi_con:
	MOV		DPTR, #uc_user_option2
	MOVX	A, @DPTR
	MOV		B, MR_MSCON4
	JNB		ACC.2, MSCDn_Normal_3
	JNB		B.7, have_card_11
	JMP		_release_msi_occ_buf	
MSCDn_Normal_3:	
	JB		B.7, have_card_11
	JMP		_release_msi_occ_buf
	
have_card_11:
	ORL		MR_MSSTA, #01H				;set go bit
	NOP
	NOP
check_msi_irq:
	MOV		A, MR_MSSTA
	JNB		ACC.5, check_msi_irq
	CLR		ACC.5
	MOV		MR_MSSTA, A
	
	CALL 	_release_msi_occ_buf
	RET	

#endif

#endif
	END