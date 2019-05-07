/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: hwdriver_lcd.asm
Creator: zj.zong					Date: 2009-04-21
Description: lcd driver
Others: 
Version: V0.1
History:
	V0.1	2009-04-21		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef API_LCD_DRIVER_DEF

#include "sh86278_sfr_reg.inc"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"

EXTRN	CODE	(_Api_Delay_1ms)


PUBLIC	_Api_LcdDriver_ClrOne_Page
PUBLIC	_Api_LcdDriver_Set_BklightLevel
PUBLIC	Api_Driver_Write_LcdCmd
PUBLIC	Api_Driver_Write_LcdData
PUBLIC	Api_LcdDriver_Clr_2345Page
PUBLIC	Api_LcdDriver_Clr_AllPage
PUBLIC	Api_LcdDriver_Set_PageColumn
PUBLIC	Api_LcdDriver_Write_DataBurst

LCD_FUNC		SEGMENT	CODE
					RSEG	LCD_FUNC


#ifdef API_LCD_DRIVER_INIT_FUNC
EXTRN	XDATA	(uc_lcd_cmd_table)
PUBLIC	Api_Init_Lcd
/*******************************************************************************
Function: Api_Init_Lcd()
Description: initianize the lcd driver
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
Api_Init_Lcd:
	MOV		DPTR, #uc_lcd_type
	MOVX	A, @DPTR
	MOV		C, ACC.7
	JC		Label_Serial_Mode
	ORL		MR_MODEN2,#00000100B		;set bit2, enable lcd serial mode	      
	JMP		Label_Parallel_Mode  
Label_Serial_Mode:
	ORL		MR_MODEN2,#00000011B		;set bit2, enable lcd parallel mode	 
Label_Parallel_Mode:
	MOV		MR_LCMTIME1, #044H			;Set the write timer high and low pulse width:(4+1)/45M=111ns
	MOV		MR_LCMTIME2, #004H			;Set the read timer high pulse width:(4+1)/45M=111ns
	MOV		MR_LCMTIME3, #004H			;Set the read timer low pulse width:(4+1)/45M=111ns
	MOV		MR_LCMTIME4, #04FH			;Set the serial clock low or high pulse width and LCMA0/LCMCSn setup or hold time

	MOV		MR_LCMCON, #020H			;Enable 8080 irq
	MOV		R7, #10
	CALL	_Api_Delay_1ms
/***************************************initial command*****************************/
	MOV		DPTR, #uc_lcd_cmd_table		;soft reset
	MOVX	A,@DPTR
	MOV		R5, A				
	CALL	Api_Driver_Write_LcdCmd
	MOV		R7,#10
	CALL	_Api_Delay_1ms

	MOV		DPTR, #uc_lcd_cmd_table+1
loop_wr_cmd:
	MOVX	A, @DPTR
	CJNE	A, #0FFH, cmd_valid
	JMP	lcd_cmd_end
cmd_valid:
	MOV		R5, A				
	CALL	Api_Driver_Write_LcdCmd
	INC		DPTR
	JMP		loop_wr_cmd
lcd_cmd_end:
	MOV		R7, #200
	CALL	_Api_Delay_1ms
   
	CALL	Api_LcdDriver_Clr_AllPage				;clr lcd allpage

	MOV		R7, #100
	CALL	_Api_Delay_1ms

	MOV		DPTR, #uc_bklight_grade
	MOVX	A, @DPTR
	MOV		R7, A
	CALL	_Api_LcdDriver_Set_BklightLevel

	MOV		R5, #0AFH						;Display on
	CALL	Api_Driver_Write_LcdCmd

	MOV		R7, #100
	CALL	_Api_Delay_1ms	 

	RET
#endif	
/*LCD_CMD_TABLE:	DB 0E2H,0AEH,02FH,0A4H,024H,0A2H,040H,0C8H,0A1H,0A6H,0F8H,000H
;;#0E2H		;set lcd "reset"
;;#0AEH		;set lcd "display off"
;;#02FH		;set lcd "external power control"				
;;#0A4H		;set lcd "normal display"
;;#024H		;set lcd "V0 voltage regulator internal resistor"
;;#0A2H		;set lcd "bias"
;;#040H		;set lcd "start line"
;;#0C8H		;set lcd "Com63->0"
;;#0A0H		;set lcd "seg output correspondece"
;;#0A6H		;set lcd "normal display, 1 is on voltage"
;;#0F8
;;#000H		 */
/*********************************clr LCD all page*****************************
input: No
*******************************************************************************/
Api_LcdDriver_Clr_AllPage:
	MOV		R7,#0
	CALL	_Api_LcdDriver_ClrOne_Page		;input is page number

	MOV		R7,#1
	CALL	_Api_LcdDriver_ClrOne_Page

	MOV		R7,#2
	CALL	_Api_LcdDriver_ClrOne_Page
		
	MOV		R7,#7
	CALL	_Api_LcdDriver_ClrOne_Page

Api_LcdDriver_Clr_2345Page:
		
	MOV		R7,#3
	CALL	_Api_LcdDriver_ClrOne_Page

	MOV		R7,#4
	CALL	_Api_LcdDriver_ClrOne_Page		;input is page number

	MOV		R7,#5
	CALL	_Api_LcdDriver_ClrOne_Page

	MOV		R7,#6
	CALL	_Api_LcdDriver_ClrOne_Page
	



	RET

/*****************************clr one page*********************************
input: R7(page number)
Description: clr lcd one page
user register: R7
********************************************************************************/
_Api_LcdDriver_ClrOne_Page:
	MOV		A,R7
	MOV		uc_lcd_page_num,A 				;input is R7(page num)
	CLR		A
	MOV		uc_lcd_column_num,A	  			;column begin from 0
	CALL	Api_LcdDriver_Set_PageColumn		
	CALL	Api_ClrLcd_Combuf				;clr LCD combuf
	MOV		DPTR,#uc_lcd_width
	MOVX	A,@DPTR	
	CLR		C
	RRC		A	
	DEC		A
	MOV		MR_LCMNUML,A
	MOV		MR_LCMIMAL, #LOW FCOMBUF_OFFSET_W
	JMP		Api_LcdDriver_Write_DataBurst

/*****************************clr LCD commonbuf*********************************
input: NO
Description: clr lcd common buffer (0x6600)
user register: R7
********************************************************************************/
Api_ClrLcd_Combuf:
	MOV		DPTR,#X_ADDR_FCOMBUF
	MOV		R7,#132
	CLR		A	
loop_clr_lcdbuf:
	MOVX	@DPTR,A
	INC		DPTR
	DJNZ	R7,loop_clr_lcdbuf
	RET



/***************************************************************
input: R7:uc_lcd_contrast_level(XDATA)
value is from 1~16 level
***************************************************************/
_Api_LcdDriver_Set_BklightLevel:
	MOV		R5, #081H				;Contrast command
	CALL	Api_Driver_Write_LcdCmd
	MOV 	DPTR, #uc_lcd_contrast_data
 	MOVX	A, @DPTR					;Contrast Data
	MOV		B,R7
	MUL		AB							;level * data
	MOV		R7, A
	MOV		DPTR, #uc_lcd_contrast_base
	MOVX	A, @DPTR
	ADD		A, R7	
	MOV		R5,A
	JMP	Api_Driver_Write_LcdCmd

	

#ifdef API_LCD_OFF_FUNC
PUBLIC	Api_LcdDriver_PowerDown
/*******************************************************************************
Function: Api_LcdDriver_PowerDown()
Description: display off lcd and set the lcd "sleep in"
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
Api_LcdDriver_PowerDown:
	MOV		R5, #0ACH					;Display off
	CALL	Api_Driver_Write_LcdCmd

	MOV		R5, #0AEH					;Sleep in & booster off
	CALL	Api_Driver_Write_LcdCmd

	MOV		R5, #0A5H					;Sleep in & booster off
	JMP		Api_Driver_Write_LcdCmd
#endif



/*******************************************************************************
Function: Api_Driver_Write_LcdCmd()
Description: write the initial command
Calls:
Global:Null
Input: R5--Lcd Command
Output: Null
Others:
********************************************************************************/
Api_Driver_Write_LcdCmd:
	ANL		MR_LCMCON, #01110101B		;clr bit3,7, clr single data irq flag and set write cmd
	NOP
	NOP
	MOV		MR_LDMDAT, R5
	JMP		check_wr_sd_end				;check write single data?

/*******************************************************************************
Function: Api_Driver_Write_LcdData()
Description: write the initial data that followed the initial command
Calls:
Global:Null
Input: R3--Lcd Data
Output:Null
Others:
********************************************************************************/
Api_Driver_Write_LcdData:
	MOV		MR_LCMCON, #10000000B		;set bit7, write data
	NOP
	NOP
	MOV		MR_LDMDAT, R3
	JMP		check_wr_sd_end				;check write single data?

check_wr_sd_end:
	MOV		A, MR_LCMCON
	JNB		ACC.3, check_wr_sd_end
	CLR		ACC.3
	MOV		MR_LCMCON, A
	RET

/*******************************************************************************
Function: Api_LcdDriver_Set_PageColumn()
Description: set the row start&end addr and the column start&end addr
Calls:
Global:uc_lcd_page_num--lcd row start address
       uc_lcd_column_num--lcd column start address
       uc_pic_width--width of picture
       uc_pic_height--height of picture
       uc_lcd_column_offset--column offset
Input: Null
Output:Null
Others:
********************************************************************************/
Api_LcdDriver_Set_PageColumn:
	MOV		A,uc_lcd_column_num
	CLR		C
	SUBB	A,#128
	JNC		page_column_error

	MOV		R3,#0b0H				;Set the start page command
 	MOV		A,uc_lcd_page_num
	MOV		DPTR,#uc_page_table
	ADD		A,DPL					;CY=0
	MOV		DPL,A
	MOVX	A,@DPTR					;Get the corresponding page number
	ADD		A,R3
	MOV		R5,A				
	CALL	Api_Driver_Write_LcdCmd
   
   	MOV		R3,#010H				;Set the start column command
	MOV		A,uc_lcd_column_num
	MOV		R2,A
	MOV		DPTR,#uc_lcd_column_offset
	MOVX	A,@DPTR
	ADD		A,R2					;add lcd column offset
	MOV		R2,A
	SWAP	A						;set the column high 4bit
	ANL		A,#0FH
	ADD		A,R3					;0x10 + high 4bit
	MOV		R5,A					
	CALL	Api_Driver_Write_LcdCmd

	MOV		A,R2
	ANL		A,#0FH
	MOV		R5,A					
	CALL	Api_Driver_Write_LcdCmd	;set the column low 4bit
page_column_error:
	RET



/*******************************************************************************
Function: Api_LcdDriver_Write_DataBurst()
Description: display burst data
Calls:
Global:MR_LCMNUML,MR_LCMNUMH: LCM data number register
       MR_LCMIMAL,MR_LCMIMAH: LCM access internal memory address
       MR_MEMSEL1: LCM occupy buffer
Input: Null
Output:Null
Others:
********************************************************************************/
Api_LcdDriver_Write_DataBurst:
	MOV		MR_LCMNUMH, #00H
	ORL		MR_MEMSEL2, #00000100B					;LCMI apply FBUF

	MOV		MR_LCMIMAH, #HIGH FCOMBUF_OFFSET_W
	
	MOV		MR_LCMCON, #10000000B				;set bit7:write data;	 display data
	ORL		MR_LCMCON, #00000001B				;set bit0:startup burst display data operation
check_wr_bd_end:
	MOV		A, MR_LCMCON
	JNB		ACC.4, check_wr_bd_end
	CLR		ACC.4								;clr burst display data irq
	MOV		MR_LCMCON, A
	ANL		MR_MEMSEL2, #11111000B				;LCMI release the occupy buffer
	NOP
	NOP
	NOP
	ANL		MR_MEMSEL2, #11111000B				;LCMI release the occupy buffer
	RET

#endif
	END
