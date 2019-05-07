/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: sh86278_api_macro.h
Creator: AJ.Zhao					Date:2009-04-22
Description: define part of subroutine as MACRO(for asm)
Others:
Version: V0.2
History:
	V0.2	2010-05-14		AJ.Zhao
			1. auto set MSClock when occur timeout event
	V0.1	2009-04-22		AJ.Zhao
			1. build this module
********************************************************************************/
#ifndef sh86278_SUBROUTINE_MACRO
#define sh86278_SUBROUTINE_MACRO

mReset_WatchDog		MACRO
	MOV		MR_WDT, #0FFH
	ENDM
	
mSet_PCStack		MACRO
	MOV		SP, #STACK-1
	ENDM
	
/*** for MSI Driver ***/
mSet_MSClock_351K	MACRO
	ANL		MR_SYSCLK, #0C7H
	ENDM

mSet_MSClock_6M		MACRO
	ANL		MR_SYSCLK, #0C7H
	ORL		MR_SYSCLK, #008H
	ENDM
	
mSet_MSClock_15M	MACRO
	ANL		MR_SYSCLK, #0C7H
	ORL		MR_SYSCLK, #010H
	ENDM	
	
mSet_MSClock_22M	MACRO
	ANL		MR_SYSCLK, #0C7H
	ORL		MR_SYSCLK, #018H
	ENDM
	
/*mAutoSet_MSClock	MACRO
	SWAP	A
	CLR		C
	RRC		A
	ANL		MR_SYSCLK, #0C7H
	ORL		MR_SYSCLK, A				
	ENDM*/			
	
#endif		