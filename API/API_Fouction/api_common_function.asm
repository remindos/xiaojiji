/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: api_common_func.asm
Creator: zj.zong					Date: 2009-04-10
Description: common function for all mode
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef API_COMMON_FUNC_ASM_DEF
#include "sh86278_sfr_reg.inc"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"

COM_FUNC_SEG	SEGMENT	CODE
					RSEG	COM_FUNC_SEG

#ifdef API_CONVERT_ISN_UNICODE
PUBLIC	_Api_Isn_ConvertTo_Unicode
EXTRN	CODE	(_media_readsector)
EXTRN	CODE	(Read_Ctra)
/*******************************************************************************
Function: _Api_Isn_ConvertTo_Unicode()
Description:
Calls:
Global:uc_language_idx(multi-language index 0~17)
Input: R6, R7(high->low)--internal statement number
output:R6, R7(high->low)--unicode
Others:
*******************************************************************************/
_Api_Isn_ConvertTo_Unicode:
 	CALL	bytes_convert_sector				;input is R7, return is R6(sector number)
	MOV		A, R6
	MOV		DPTR, #sfc_logblkaddr0_bk
	MOVX	@DPTR, A
   
	MOV		MR_LCMNUMH, #5					;segment name 'MR_LCMNUMH + 2' (unicode convert table is lie in segment 6)
	CALL	Read_Ctra
	CLR		C
	MOV		DPTR, #sfc_logblkaddr0_bk
	MOVX	A, @DPTR
	ADD		A, sfc_logblkaddr0
   	MOV		sfc_logblkaddr0, A

	MOV		DPTR, #uc_language_idx				
	MOVX	A, @DPTR
	ADDC	A, sfc_logblkaddr1
	MOV		sfc_logblkaddr1, A

	MOV		A, sfc_logblkaddr2
	ADDC	A, #00H
	MOV		sfc_logblkaddr2, A

	MOV		sfc_occ_buf_flag, #SFC_APP_FBUF		;occupy FBUF
	MOV		buf_offset0, #LOW FCOMBUF_OFFSET_W
	MOV		buf_offset1, #HIGH FCOMBUF_OFFSET_W
	SETB	sfc_reserved_area_flag				;call on reserved area 	
	CALL	_media_readsector
	CLR		sfc_reserved_area_flag				;clr reserved area flag	

	CLR		C
	MOV		DPTR, #ui_sector_byte_offset
	MOVX	A, @DPTR
	ADD		A, #LOW (X_ADDR_FBUF + FCOMBUF_OFFSET_B)
	MOV		R7, A
	INC		DPTR
	MOVX	A, @DPTR
	ADDC	A, #HIGH (X_ADDR_FBUF + FCOMBUF_OFFSET_B)
	MOV		DPH, A
	MOV		DPL, R7
	MOVX	A, @DPTR	
	MOV		R6, A								;Get the Unicode high 8bit
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R7, A								;Get the Unicode low 8bit
	RET



/********************************************************************************
input:R6(high),R7(low)--byte number
output:R6(sector),ui_sector_byte_offset(bytes offset)
********************************************************************************/
bytes_convert_sector:
	MOV		DPTR, #ui_sector_byte_offset		;first byte is LOW, second is HIGH
	MOV		A, R7
	CLR		C
	RLC		A
	MOVX	@DPTR, A
	INC		DPTR
	CLR		A
	MOV		ACC.0, C
	MOVX	@DPTR, A
	RET
#endif



#ifdef API_MERGE_2BYTE_DEF
PUBLIC	_Api_Merge_2Byte_Sub
/*******************************************************************************
Function: __Api_Merge_2Byte_Sub()
Description: merge two bytes into one word
Calls:
Global:Null
Input: R5--high byte, R7--low byte
Output:R6--high byte, R7--low byte
Others:
*******************************************************************************/
_Api_Merge_2Byte_Sub:
	MOV   A, R5
	MOV   R6, A
	RET
#endif




#ifdef API_MERGE_4BYTE_DEF
PUBLIC	_Api_Merge_4Byte
PUBLIC	_Api_Merge_4Byte7Bit
EXTRN	XDATA	(uc_id3_temp_input)
/*******************************************************************************
Function: _Api_Merge_4Byte()
Description: merge four byte into one long int(4*8bit --> 32bit)
Calls:
Global:uc_id3_temp_input(high byte)
Input: R3, R5, R7(high-->low)
Output:R4, R5, R6, R7(high-->low)
Others:
*******************************************************************************/
_Api_Merge_4Byte:
	MOV		A, R5
	MOV		R6, A
	MOV		A, R3
	MOV		R5, A

	MOV		DPTR, #uc_id3_temp_input
	MOVX	A, @DPTR
	MOV		R4, A
	RET



/*******************************************************************************
Function: _Api_Merge_4Byte7Bit()
Description: merge four byte(bit0~6) into one long int(4*7bit --> 28bit)
Calls:
Global:uc_id3_temp_input(high byte)
Input: R3, R5, R7(high-->low)
Output:R4, R5, R6, R7(high-->low)
Others:
*******************************************************************************/
_Api_Merge_4Byte7Bit:
	MOV		A, R5
	RR		A
	MOV		R5, A
	ANL		A, #10000000B
	ORL		A, R7
	MOV		R7, A
	
	MOV		A, R5
	ANL		A, #00111111B
	MOV		R5, A
	MOV		A, R3
	RR		A
	RR		A
	MOV		R3, A
	ANL		A, #11000000B
	ORL		A, R5
	MOV		R6, A
	
	MOV		A, R3
	ANL		A, #00011111B
	MOV		R3, A
	
	MOV		DPTR, #uc_id3_temp_input
	MOVX	A, @DPTR
	MOV		R0, A
	RR		A
	RR		A
	RR		A
	MOV		R0, A
	ANL		A, #11100000B
	ORL		A, R3
	MOV		R5, A
	
	MOV		A, R0
	ANL		A, #00001111B
	MOV		R4, A
	RET
#endif


#ifdef API_SET_VOLUME_DEF
PUBLIC	_Api_Set_Volume
/*******************************************************************************
Function: _Api_Set_Volume()
Description: set volume, modify MR_MVCL & MR_MVCR
Calls:
Global:Null
Input: R7--volume level
Output:NULL
Others:
*******************************************************************************/
_Api_Set_Volume:
	MOV		A, R7
	MOV		DPTR, #VOLUME_VAL_TABLE
	MOVC	A, @A+DPTR
	MOV		DPTR, #MR_MVCL
	MOVX	@DPTR, A

	MOV		DPTR, #MR_MVCR
	MOVX	@DPTR, A
	RET


VOLUME_VAL_TABLE:
	DB	79,65,60,57,53,48,42,39,35,33
	DB	31,29,25,23,21,20,19,17,15,13,12
	DB	11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1		;VOLUME VALUE 0--31 For DAC
#endif


#ifdef API_SET_FM_VOLUME_DEF
PUBLIC	_Api_Set_FM_Volume
/*******************************************************************************
Function: _Api_Set_FM_Volume()
Description: set FM volume, modify MR_MVCL & MR_MVCR
Calls:
Global:Null
Input: R7--volume level
Output:NULL
Others:
*******************************************************************************/
_Api_Set_FM_Volume:
	MOV		A, R7
	MOV		DPTR, #VOLUME_VAL_TABLE
	MOVC	A, @A+DPTR
	MOV		DPTR, #MR_AVCL
	MOVX	@DPTR, A

	MOV		DPTR, #MR_AVCR
	MOVX	@DPTR, A
	RET

VOLUME_VAL_TABLE:
	DB	44,43,41,39,37,35,33,32,31,30
	DB	29,28,27,26,25,24,23,22,21,20
	DB	19,18,17,16,15,14,13,12,11,10			;VOLUME VALUE 0--31 For AVC
	DB	9,8
#endif

#endif
	END
