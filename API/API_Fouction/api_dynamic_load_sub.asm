/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: hwdriver_dynamic_load.asm
Creator: zj.zong					Date: 2009-04-10
Description: dynamic load code into mcu code memory
Others: 
Version: V0.2
History:
	V0.2	2010-06-18		AndyLiu
			1. add backup addr "sfc_logblkaddr2_bk"
	V0.1	2009-06-11		aj.zhao
			1. build this module
********************************************************************************/
#include "sh86278_sfr_reg.inc"
#include "ui_resident_var_ext.inc"
#include "sh86278_memory_assign.h"
#include "hwdriver_resident.h"

#define	 LOAD_CODE_ERR			0FFH
#define	 LOAD_CODE_OK			000H
#define	 LOAD_CODE_CNT			010H

EXTRN	CODE	(_media_readsector)
EXTRN   CODE	(Read_Ctra)
PUBLIC	Api_DynamicLoading_Sub


LOAD_FUNC	SEGMENT		CODE
			RSEG		LOAD_FUNC
/*******************************************************************************
Function: Api_DynamicLoading_Sub()
Description: dynamic loading code from original area or  mirror area
Calls:
Global:uc_load_code_idx,(0~126Block)
       buf_offset1, buf_offset0,(PM1 offset, 0x?? bytes)
Intput:Null
Output:Null
Others:
********************************************************************************/
Api_DynamicLoading_Sub:
	MOV		MR_WDT, #0FFH					;clr watchdog

	CALL	Save_Input_Parameter			;save the dynamic loading parameter
	CLR		b_load_code_flg					;0:Load original code; 1:Load mirror code
	
	MOV		MR_LCMNUMH, #1					;segment name 'MR_LCMNUMH + 1' (code infomation is lie in segment 3)
	CALL	Read_Ctra
											;
	CALL	Get_Sfc_Outbuf_Info				;get the sfc_occ_buf_flag,buf_offset0,buf_offset1
	SETB	sfc_reserved_area_flag			;set reserved area flag
	CALL	_media_readsector				;read one sector (original code)

	MOV		DPTR ,#sfc_logblkaddr2_bk
	MOV	   	A, sfc_logblkaddr2
	MOVX 	@DPTR, A

	MOV		DPTR ,#sfc_logblkaddr1_bk
	MOV	   	A, sfc_logblkaddr1
	MOVX 	@DPTR, A

	MOV		DPTR ,#sfc_logblkaddr0_bk
	MOV	   	A, sfc_logblkaddr0
	MOVX 	@DPTR, A

	CALL	Check_Cvit_ok_Error			;RTS is "CY", 0-Error, 1-OK
	JNC		read_cvit_m					;original code is error, and prepare mirror code

ok_cvit_o:
	SJMP	read_cbit_o



/************************************************************************
read SBIT-original area and verify
logic address is 0x0000 0601
*************************************************************************/
read_cbit_o:
	INC		sfc_logblkaddr0					;original address + 1Sector
	CALL	Get_Sfc_Outbuf_Info				;get the sfc_occ_buf_flag,buf_offset0,buf_offset1
	CALL	_media_readsector				;read one sector original area
	JMP		prepare_loading_code



/************************************************************************
if SVIT-original A error,and read CVIT-mirror B and verify
*************************************************************************/
read_cvit_m:
	SETB	b_load_code_flg					;load mirror code
	CALL	Get_Mirror_Logic_Addr			;get the mirror code logic address
	CALL	Get_Sfc_Outbuf_Info				;get the sfc_occ_buf_flag,buf_offset0,buf_offset1
	CALL	_media_readsector				;read one sector (original code)

	CALL	Check_Cvit_ok_Error			;RTS is "CY", 0-Error, 1-OK
	JNC		dynamic_load_err

OK_CVIT_M:
	SJMP	read_cbit_m

dynamic_load_err:							;SJMP to JMP (because of taget out of range)
	JMP		dynamic_load_err_ret



/************************************************************************
read mirror and verify, if mirror error
logic address is mirror + 1Sector
*************************************************************************/
read_cbit_m:
	CALL	Get_Mirror_Logic_Addr			;get the mirror code logic address
	MOV		A, #01H							;mirror address + 1sector = mirror address 
	MOV		B, #00H
	CALL	Get_Cur_Logic_Addr				;input is A, get current logic address	
	CALL	Get_Sfc_Outbuf_Info				;get the sfc_occ_buf_flag, buf_offset0, buf_offset1
	CALL	_media_readsector				;read one sector mirror code

prepare_loading_code:
	MOV     A, sfc_errcode			;	sfc_errorcode: 0x00	noerror,0x01 write error,0x02 read error
	CJNE	A, #00H, error_cbit_zone

ok_cbit_zone:
	CLR		A
	MOV		R0, #uc_load_rpt_cnt			;count the loading time
	MOV		@R0, A

again_load_code_block:
	CALL	Get_Code_Logic_Addr				;get Code Area logic address of original or mirror
	CALL	Get_Cbit_Code_Index_Addr		;RTS is DPTR
	MOVX	A, @DPTR						;get the current code block offset(logic address)
	MOV		B, A							;offset high
	INC		DPTR
	MOVX	A, @DPTR						;offset low
	CALL	Get_Cur_Logic_Addr				;input is A,B, get current logic address

	CALL	Get_Cbit_Code_Index_Addr		;RTS is DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	MOV		uc_load_code_length, A			;get the length of block sector(high)

start_load:
	ACALL	Restore_Input_Parameter			;restore dynamic loading input parameter
	CALL	_media_readsector				;read one sector	
	MOV     A, sfc_errcode				;sfc_errorcode: 0x00 noerror,0x01 write error,0x02 read error
	CJNE	A, #00H, again_load_code_block_1
	DEC		uc_load_code_length				;uc_load_code_length(sector) - 1
	MOV		A, uc_load_code_length
	JZ		load_end
	
loop_load:
	MOV		MR_WDT, #0FFH					;clr watchdog
	
	MOV		A, #01H							;logic address + 1
	MOV		B, #00H
	CALL	Get_Cur_Logic_Addr				;input is A, get current logic address
	
	MOV		A, sfc_occ_buf_flag
	CJNE	A, #SFC_APP_MPM1, not_loading_mpm1
	MOV		A, buf_offset1					;MPM(unit is 8bit)
	ADD		A, #02H
	MOV		buf_offset1, A					;buf_offset+512byte meaning buf_offset1+2
	JMP		continue_load
	
not_loading_mpm1:
	CJNE	A, #SFC_APP_CPM, not_loading_cpm
	MOV		A, buf_offset1					;CBUF(unit is 16bit)
	ADD		A, #01H
	MOV		buf_offset1, A					;buf_offset+256word meaning buf_offset1+1
	JMP		continue_load
not_loading_cpm:
	CJNE	A, #SFC_APP_CBUF, not_loading_cbuf
	MOV		A, buf_offset1					;CBUF(unit is 16bit)
	ADD		A, #01H
	MOV		buf_offset1, A					;buf_offset+256word meaning buf_offset1+1
	JMP		continue_load
not_loading_cbuf:
	MOV		A, buf_offset0					;DCM(unit is 24bit)
	ADD		A, #170
	MOV		buf_offset0, A
	MOV		A, buf_offset1
	ADDC	A, #00H
	MOV		buf_offset1, A					;buf_offset+170
	
continue_load:
	CALL	_media_readsector				;read one sector 
	MOV     A, sfc_errcode				;sfc_errorcode: 0x00 noerror,0x01 write error,0x02 read error
	CJNE	A, #00H, again_load_code_block_1
	DJNZ	uc_load_code_length, loop_load	;reloading sector

load_end:
	SJMP	dynamic_load_ok_ret				;loading code ok

again_load_code_block_1:
	MOV		R0, #uc_load_rpt_cnt
	INC		@R0
	MOV		A, @R0
	CJNE	A, #LOAD_CODE_CNT, again_load_code_block	;again loading block code

error_cbit_zone:
	JB		b_load_code_flg, dynamic_load_err_ret		;loading mirror error
	JMP		read_cvit_m					;loading original error, then loading mirror


dynamic_load_err_ret:
	MOV		sfc_errcode, #LOAD_CODE_ERR		;set the dynamic loading error flag
	RET
dynamic_load_ok_ret:
	MOV		sfc_errcode, #LOAD_CODE_OK		;clr the dynamic loading error flag

	RET



/****************************************************************************
Save dynamic loading input
****************************************************************************/
Save_Input_Parameter:  
	MOV		R0, #uc_load_temp_buf			;PM1 Offset (bytes)
	MOV		A, buf_offset0					;0x?? bytes
	MOV		@R0, A

	INC		R0
	MOV		A, buf_offset1					;0x?? bytes
	MOV		@R0, A

	INC		R0
	MOV		A, sfc_occ_buf_flag 
	MOV		@R0, A
	RET



/****************************************************************************
restore dynamic loading input
****************************************************************************/
Restore_Input_Parameter:				
	MOV		R0, #uc_load_temp_buf			;PM1 Offset (bytes)
	MOV		A, @R0
	MOV		buf_offset0, A

	INC		R0
	MOV		A, @R0
	MOV		buf_offset1, A

	INC		R0
	MOV		A, @R0
	MOV		sfc_occ_buf_flag, A
	RET	



/****************************************************************************
description: get current logic address(sfc_logblkaddr0,sfc_logblkaddr1,sfc_logblkaddr2)
input: A(logic address offset low),B(high)
****************************************************************************/
Get_Cur_Logic_Addr:
	CLR		C
	ADD		A, sfc_logblkaddr0		;sfc_logblkaddr + block offset address
	MOV		sfc_logblkaddr0, A
	MOV		A, B
	ADDC	A, sfc_logblkaddr1
	MOV		sfc_logblkaddr1, A
	CLR		A
	ADDC	A, sfc_logblkaddr2
	MOV		sfc_logblkaddr2, A
	RET



/****************************************************************************
b_load_code_flg: 0--get original logic addr
                 1--get mirror logic addr
sfc_logblkaddr0,sfc_logblkaddr1,sfc_logblkaddr2,sfc_logblkaddr3
****************************************************************************/
Get_Code_Logic_Addr:
	JB		b_load_code_flg, Get_Mirror_Code_Logic_Addr

	MOV	   	sfc_logblkaddr3, #00H

	MOV		DPTR ,#sfc_logblkaddr2_bk
	MOVX 	A, @DPTR 
	MOV	   	sfc_logblkaddr2, A

	MOV		DPTR ,#sfc_logblkaddr1_bk
	MOVX 	A, @DPTR 
	MOV	   	sfc_logblkaddr1, A

	MOV		DPTR ,#sfc_logblkaddr0_bk
	MOVX 	A, @DPTR 
	ADD		A, #02H
	MOV	   	sfc_logblkaddr0, A
 	RET


Get_Mirror_Code_Logic_Addr:
	ACALL	Get_Mirror_Logic_Addr			;firstly get the mirror start logic address
	MOV		A, #02H							;mirror + 2sector = Code Area mirror
	MOV		B, #00H
	JMP		Get_Cur_Logic_Addr				;input is A, get current logic address
	


/****************************************************************************
func description:  get mirror address
****************************************************************************/
Get_Mirror_Logic_Addr:
	MOV		MR_LCMNUMH, #003						 ;for 5 segment
	CALL	Read_Ctra
	SETB	sfc_reserved_area_flag			;set reserved area flag
	RET

/****************************************************************************
description: get the corresponding address of load code index in CBIT
input: uc_load_code_idx
output:DPTR
****************************************************************************/
Get_Cbit_Code_Index_Addr:
	MOV		DPTR, #uc_load_code_idx			;uc_load_code_idx*4 + #X_ADDR_FCOMBUF
	MOVX	A, @DPTR
	MOV		DPTR, #X_ADDR_FCOMBUF			;get the offset of current block in X_ADDR_FCOMBUF
	MOV		B, #4
	MUL		AB
	ADD		A, DPL
	MOV		DPL, A
	MOV		A, B
	ADDC	A, DPH
	MOV		DPH, A
	RET



/****************************************************************************
description: get outbuffer info buf_offset0,buf_offset1(0x?? word)
output:sfc_occ_buf_flag,buf_offset0,buf_offset1
****************************************************************************/
Get_Sfc_Outbuf_Info:
	MOV		sfc_occ_buf_flag, #SFC_APP_FBUF	;occupy ef common buffer(0x6600)
	MOV		buf_offset0, #LOW FCOMBUF_OFFSET_W
	MOV		buf_offset1, #HIGH FCOMBUF_OFFSET_W
	RET



/****************************************************************************
description: Verify the lable of CVIT
output:CY:  0---Error;  1---OK
****************************************************************************/
Check_Cvit_ok_Error:
	MOV		DPTR, #X_ADDR_FCOMBUF
	MOVX	A, @DPTR
	CJNE	A, #43H, ERROR_CVIT			;FLAG 1 (0x43)
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #56H, ERROR_CVIT			;FLAG 2 (0x56)
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #49H, ERROR_CVIT			;FLAG 3 (0x49)
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #54H, ERROR_CVIT			;FLAG 4 (0x54)
	
	MOV     A, sfc_errcode			;sfc_errorcode: 0x00 noerror,0x01 write error,0x02 read error
	CJNE	A, #00H, ERROR_CVIT			 

ok_cvit:
	SETB	C
	RET

error_cvit:
	CLR	C
	RET



	END