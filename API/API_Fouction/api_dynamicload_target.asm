/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: api_dynamicload_target.asm
Creator: aj.zhao					Date: 2009-04-22
Description: interface of mode music for functions include dynamic loading
Others:
Version: V0.1
History:
	V0.1	2009-04-22		aj.zhao
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef	API_DYNAMICLOAD_TARGET_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"

EXTRN	CODE (Api_DynamicLoad_CodeMpm1_3k)
EXTRN	CODE (Api_DynamicLoad_CodeMpm1_5k)
EXTRN	CODE (Api_DynamicLoad_CodeMpm1_5kExecute)
EXTRN	CODE (Api_DynamicLoad_CodeMpm1_6k)
EXTRN	CODE (Api_DynamicLoad_CodeMpm1_6kExecute)

DYNAMIC_FUNC_SEG	SEGMENT	CODE
					RSEG	DYNAMIC_FUNC_SEG


#ifdef API_DYNAMICLOAD_MUSIC_TARGET
PUBLIC	_Api_DynamicLoad_Music_Target
/*******************************************************************************
Function:   _Api_DynamicLoad_Music_Target
Description: interface for music dynamic load every target
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
_Api_DynamicLoad_Music_Target:
	MOV		DPTR, #uc_load_code_idx
	MOV		A, R7
	MOVX	@DPTR, A
	CALL	Api_DynamicLoad_CodeMpm1_5kExecute
	MOV		DPTR, #uc_load_code_idx
	MOV		A, #CODE_MUSIC_D_MAIN_IDX 				//restore music main program
	MOVX	@DPTR, A
	JMP		Api_DynamicLoad_CodeMpm1_5k
#endif


#ifdef API_DYNAMICLOAD_REC_TARGET
PUBLIC	_Api_DynamicLoad_Record_Target
/*******************************************************************************
Function:   _Api_DynamicLoad_Record_Target
Description: interface for movie dynamic load every target
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
_Api_DynamicLoad_Record_Target:
	MOV		DPTR, #uc_load_code_idx
	MOV		A, R7
	MOVX	@DPTR, A
	CALL	Api_DynamicLoad_CodeMpm1_5kExecute
	MOV		DPTR, #uc_load_code_idx
	MOV		A, #CODE_RECORD_9_MAIN_IDX				//restore record main program
	MOVX	@DPTR, A
	JMP		Api_DynamicLoad_CodeMpm1_5k
#endif





#ifdef API_DYNAMICLOAD_VOICE_TARGET
PUBLIC	_Api_DynamicLoad_Voice_Target
/*******************************************************************************
Function:   _Api_DynamicLoad_Voice_Target
Description: interface for voice dynamic load every target
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
_Api_DynamicLoad_Voice_Target:
	MOV		DPTR, #uc_load_code_idx
	MOV		A, R7
	MOVX	@DPTR, A
	CALL	Api_DynamicLoad_CodeMpm1_5kExecute
	MOV		DPTR, #uc_load_code_idx
	MOV		A, #CODE_VOICE_8_MAIN_IDX 				//restore voice main program
	MOVX	@DPTR, A
	JMP		Api_DynamicLoad_CodeMpm1_5k
#endif


#ifdef API_DYNAMICLOAD_EBOOK_TARGET
PUBLIC	_Api_DynamicLoad_Ebook_Target
/*******************************************************************************
Function:   _Api_DynamicLoad_Ebook_Target
Description: interface for ebook dynamic load every target
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
_Api_DynamicLoad_Ebook_Target:
	MOV		DPTR, #uc_load_code_idx
	MOV		A, R7
	MOVX	@DPTR, A
	CALL	Api_DynamicLoad_CodeMpm1_5kExecute
	MOV		DPTR, #uc_load_code_idx
	MOV		A, #CODE_EBOOK_8_MAIN_IDX 				//restore ebook main program
	MOVX	@DPTR, A
	JMP		Api_DynamicLoad_CodeMpm1_5k
#endif


#ifdef API_DYNAMICLOAD_TEL_TARGET
PUBLIC	_Api_DynamicLoad_Tel_Target
/*******************************************************************************
Function:   _Api_DynamicLoad_Tel_Target
Description: interface for tel dynamic load every target
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
_Api_DynamicLoad_Tel_Target:
	MOV		DPTR, #uc_load_code_idx
	MOV		A, R7
	MOVX	@DPTR, A
	CALL	Api_DynamicLoad_CodeMpm1_5kExecute
	MOV		DPTR, #uc_load_code_idx
	MOV		A, #CODE_TEL_5_MAIN_IDX 				//restore tel main program
	MOVX	@DPTR, A
	JMP		Api_DynamicLoad_CodeMpm1_5k
#endif



#ifdef API_DYNAMICLOAD_FM_TARGET
PUBLIC	_Api_DynamicLoad_FM_Target
/*******************************************************************************
Function:   _Api_DynamicLoad_FM_Target
Description: interface for FM dynamic load every target
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
_Api_DynamicLoad_FM_Target:
	MOV		DPTR, #uc_load_code_idx
	MOV		A, R7
	MOVX	@DPTR, A
	CALL	Api_DynamicLoad_CodeMpm1_6kExecute
	MOV		DPTR, #uc_load_code_idx
	MOV		A, #CODE_FM_A_MAIN_IDX 				//restore fm main program
	MOVX	@DPTR, A
	JMP		Api_DynamicLoad_CodeMpm1_6k
#endif



#ifdef API_DYNAMICLOAD_FM_CODE_MPM1_3K
PUBLIC	_Api_DynamicLoad_FM_CodeMpm1_3k
/*******************************************************************************
Function:   _Api_DynamicLoad_FM_Target_3K
Description: interface for FM dynamic load 3K target
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
_Api_DynamicLoad_FM_CodeMpm1_3k:
	MOV		DPTR, #uc_load_code_idx
	MOV		A, R7
	MOVX	@DPTR, A
	JMP		Api_DynamicLoad_CodeMpm1_3k

#endif

#endif
	END