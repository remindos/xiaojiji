/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: api_dynamic_load.asm
Creator: zj.zong					Date: 2009-04-10
Description: dynamic load interface
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

EXTRN	CODE (Api_DynamicLoading_Sub)

PUBLIC	Api_DynamicLoad_DataCbuf
PUBLIC	Api_DynamicLoad_CodeMpeg
PUBLIC	Api_DynamicLoad_CodeCpm
PUBLIC	Api_DynamicLoad_CodeCpm_4k
PUBLIC	Api_DynamicLoad_CodeMpm1_3k
PUBLIC	Api_DynamicLoad_CodeMpm1_5k
PUBLIC	Api_DynamicLoad_CodeMpm1_5kExecute
PUBLIC	Api_DynamicLoad_CodeMpm1_6k
PUBLIC	Api_DynamicLoad_CodeMpm1_6kExecute
PUBLIC	Api_DynamicLoad_CodeMpm1_8kExecute

	CSEG	AT	1015H   ;089fH
/*******************************************************************************
dynamic loading into CBUF data addr: 0x1900~0x1BFF
Global: uc_load_code_idx
*******************************************************************************/
Api_DynamicLoad_DataCbuf:
	CLR		EA
	MOV		buf_offset1, #00H
	MOV		buf_offset0, #00H
	MOV		sfc_occ_buf_flag, #SFC_APP_CBUF
	JMP		loading_api_common

/*******************************************************************************
dynamic loading into DCM code addr: 0x0000~0x17FF(x24bit)
Global: uc_load_code_idx
*******************************************************************************/
Api_DynamicLoad_CodeMpeg:
	CLR		EA
	MOV		buf_offset1, #00H
	MOV		buf_offset0, #00H
	MOV		sfc_occ_buf_flag, #SFC_APP_DCM
	JMP		loading_api_common

/*******************************************************************************
dynamic loading into CPM code addr: 0x3000~0x4FFF(x16bit)
Global: uc_load_code_idx
*******************************************************************************/
Api_DynamicLoad_CodeCpm:
	CLR		EA
	MOV		buf_offset1, #00H
	JMP		loading_codecpm_common


/*******************************************************************************
dynamic loading into CPM code addr: 0x4000~0x4FFF(x16bit)
Global: uc_load_code_idx
*******************************************************************************/
Api_DynamicLoad_CodeCpm_4k:
	CLR		EA
	MOV		buf_offset1, #08H
loading_codecpm_common:
	MOV		buf_offset0, #00H
	MOV		sfc_occ_buf_flag, #SFC_APP_CPM
	JMP		loading_api_common

/*******************************************************************************
1. dynamic loading into MPM1 code addr: 0x1000~0x2FFF(x8bit)
2. execute the code
Global: uc_load_code_idx
********************************************************************************/
Api_DynamicLoad_CodeMpm1_8kExecute:
	CLR		EA
	MOV		buf_offset1, #08H
	MOV		buf_offset0, #00H
	MOV		sfc_occ_buf_flag, #SFC_APP_MPM1
	CALL	Api_Dynamicloading
	SETB	EA
	JMP		1300H//1250H		;1190H		;the address is function entry



/*******************************************************************************
1. dynamic loading into MPM1 code addr: 0x2400~0x2FFF(x8bit)
Global: uc_load_code_idx
********************************************************************************/
Api_DynamicLoad_CodeMpm1_3k:
	CLR		EA
	MOV		buf_offset1, #1DH			//2.75K
	JMP		dynamic_loading_mpm1_common
;	MOV		buf_offset0, #00H
;	MOV		sfc_occ_buf_flag, #SFC_APP_MPM1
;	JMP		loading_api_common


/*******************************************************************************
1. dynamic loading into MPM1 code addr: 0x1C00~0x2FFF(x8bit)
Global: uc_load_code_idx
********************************************************************************/
Api_DynamicLoad_CodeMpm1_5k:
	CLR		EA
	MOV		buf_offset1, #14H
dynamic_loading_mpm1_common:
	MOV		buf_offset0, #00H
	MOV		sfc_occ_buf_flag, #SFC_APP_MPM1
loading_api_common:
	ACALL	Api_Dynamicloading
	SETB	EA
	RET

/*******************************************************************************
1. dynamic loading into MPM1 code addr: 0x1A00~0x2FFF(x8bit),used for fm 
Global: uc_load_code_idx
********************************************************************************/
Api_DynamicLoad_CodeMpm1_6k:
	CLR		EA
	MOV		buf_offset1, #12H
	JMP		dynamic_loading_mpm1_common	

Api_Dynamicloading:
	SETB	sfc_reserved_area_flag			;set reserved area flag
	CALL	Api_DynamicLoading_Sub
	CLR		sfc_reserved_area_flag			;clr reserved area flag
	MOV		A, sfc_errcode
	JNZ		load_err_powerdown
	RET

load_err_powerdown:
	MOV		MR_WDT, #05H			;watch dog reset
	JMP		$
	
;		CSEG	AT	7F1H
		CSEG	AT	0973H	;0986H
/*******************************************************************************
1. dynamic loading into MPM1 code addr: 0x1C00~0x2FFF(x8bit)
2. execute the code
Global: uc_load_code_idx
********************************************************************************/
Api_DynamicLoad_CodeMpm1_5kExecute:
	CALL	Api_DynamicLoad_CodeMpm1_5k
	JMP		1C00H
			
/*******************************************************************************
1. dynamic loading into MPM1 code addr: 0x1A00~0x2FFF(x8bit)
2. execute the code
Global: uc_load_code_idx
********************************************************************************/
Api_DynamicLoad_CodeMpm1_6kExecute:
	CALL	Api_DynamicLoad_CodeMpm1_6k
	JMP		1A00H
	
	END
