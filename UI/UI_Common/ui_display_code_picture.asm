/******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_display_code_picture.asm
Creator: zj.zong					Date: 2009-04-13
Description: 
Others:
Version: V0.1
History:
	V0.1	2009-04-13		aj.zong
		 1. build this module
*******************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_DIS_CODE_PIC

#include "sh86278_sfr_reg.inc"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"
#include "./AP/AP_BootBlock/ap_bootblock_var.inc"

EXTRN	CODE (Api_LcdDriver_Set_PageColumn)
EXTRN	CODE (Api_Driver_Write_LcdData)

BOOTBLOCK_DISPLAY	SEGMENT		CODE
				RSEG		BOOTBLOCK_DISPLAY

#ifdef UI_DISPLAY_BLOCK_CODE_PIC
PUBLIC	Ui_Display_BootCode_Picture
Ui_Display_BootCode_Picture:
	CALL	Api_LcdDriver_Set_PageColumn		;set lcd column and row

	MOV		DPTR, #FILLER_PIC_TABLE1
	MOV		R5, #13
loop_dis1:
	CLR		A
	MOVC	A, @A + DPTR
	MOV		R3, A					  
	CALL	Api_Driver_Write_LcdData
	INC		DPTR
	DJNZ	R5, loop_dis1

	INC		uc_lcd_page_num
	CALL	Api_LcdDriver_Set_PageColumn			;set lcd column and row
	MOV		DPTR, #FILLER_PIC_TABLE2
	MOV		R5, #13
loop_dis2:
	CLR		A
	MOVC	A, @A + DPTR
	MOV		R3, A					   
	CALL	Api_Driver_Write_LcdData
	INC		DPTR
	DJNZ	R5, loop_dis2

	INC		uc_lcd_page_num
	CALL	Api_LcdDriver_Set_PageColumn			;set lcd column and row
	MOV		DPTR, #FILLER_PIC_TABLE3
	MOV		R5, #13
loop_dis3:
	CLR		A
	MOVC	A, @A + DPTR
	MOV		R3, A					   
	CALL	Api_Driver_Write_LcdData
	INC		DPTR
	DJNZ	R5, loop_dis3

	INC		uc_lcd_page_num
	CALL	Api_LcdDriver_Set_PageColumn			;set lcd column and row
	MOV		DPTR, #FILLER_PIC_TABLE4
	MOV		R5, #13
loop_dis4:
	CLR		A
	MOVC	A, @A + DPTR
	MOV		R3, A					   
	CALL	Api_Driver_Write_LcdData
	INC		DPTR
	DJNZ	R5, loop_dis4
	RET


FILLER_PIC_TABLE1:
	DB	0xE0,0xE0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xE0,0xE0
FILLER_PIC_TABLE2:
	DB	0x00,0x1F,0x30,0x60,0xC4,0x88,0x54,0x88,0xC4,0x60,0x30,0x1F,0x00
FILLER_PIC_TABLE3:
	DB	0x00,0xF8,0x0C,0x86,0x43,0xA1,0x54,0xA1,0x43,0x86,0x0C,0xF8,0x00
FILLER_PIC_TABLE4:
	DB	0x07,0x07,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x07,0x07
#endif

#endif
END