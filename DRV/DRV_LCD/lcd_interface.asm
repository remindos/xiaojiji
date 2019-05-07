/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
Creator: zj.zong					Date: 2009-04-20
Description: lcd interface subroutine
Others: lcd api functions: 
  1. _Api_Display_Picture()
  2. _Api_Display_Unicode()
  3. _Api_Display_8x6_ZiKu()
  4. _Api_ClrAssigned_Area()
Version: V0.1
History:
	V0.1	2009-04-20		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef LCD_API_DEF
#include "sh86278_sfr_reg.inc"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"

EXTRN	CODE (_media_readsector)
EXTRN	CODE (Api_LcdDriver_Set_PageColumn)
EXTRN	CODE (Api_Driver_Write_LcdCmd)
EXTRN	CODE (Api_Driver_Write_LcdData)
EXTRN	CODE (Api_LcdDriver_Write_DataBurst)
EXTRN   CODE (Read_Ctra)
#ifdef API_AUDIO_FILL_DBUF_EVENT
EXTRN   CODE (Ap_ProcessEvt_FillDecBuf)
#endif
#ifdef	API_DISPLAY_REVERSE_DEFINE
	PUBLIC	Api_Dis_FileBrowser_Reverse
#endif

LCD_INTERFACE_FUNC	SEGMENT	CODE
					RSEG	LCD_INTERFACE_FUNC

#ifdef API_DISPLAY_REVERSE_DEFINE
Api_Dis_FileBrowser_Reverse:
	MOV		A,#128
	CLR		C
	SUBB	A, uc_lcd_column_num	 		;get reverses column nunber
	JNZ		dis_reserved
	RET
dis_reserved:
	MOV		R7,A							;reverse 2 page number
	MOV		R6,A
	CALL	Dis_FileBrowser_Reverse_Sub	
	INC		uc_lcd_page_num
	MOV		A,R6
	MOV		R7,A		
Dis_FileBrowser_Reverse_Sub:
	CALL	Api_LcdDriver_Set_PageColumn		;Set the LCD page and column
loop_display_data:
	MOV		A,#0FFH
	MOV		R3,A				  
	CALL	Api_Driver_Write_LcdData			;input is ar
	DJNZ	R7,loop_display_data
	RET	
#endif

												
#ifdef	API_LCD_DISPLAY_PICTURE
PUBLIC	_Api_Display_Picture
/*******************************************************************************
Function:   _Api_Display_Picture
sDescription: interface for display picture
Calls:
Global:Null
Input:   R2¡BR3--index of picture in flash memory
Output:Null
Others:
********************************************************************************/
_Api_Display_Picture:
	CALL	_Get_Picture_Header				;get the pic header(8bytes)
	JNC		dis_end							;if picture error and ret
	MOV		R0,#uc_pic_high_cnt
	MOV		@R0,#00H
	;MOV		uc_pic_high_cnt, #00H			;for pic cnt

label_read_another_sector:
	CALL	Read_OneSector_Picture
 	MOV		MR_LCMIMAL, #LOW FCOMBUF_OFFSET_W	
#ifdef API_LCD_REVERSE_DISPLAY
	JNB		b_lcd_reverse_flg,get_next_page_pic					
	CLR		b_lcd_reverse_flg
	MOV		DPTR,#X_ADDR_FCOMBUF			;read 512Bytes from 0x6600
	MOV		R7,#32
	MOV		R6,#16
loop_reverse_data_0:
	MOVX	A,@DPTR
	CPL		A
	MOVX	@DPTR,A
	INC		DPTR
	DJNZ	R7,loop_reverse_data_0
	MOV		R7,#32
	DJNZ	R6,loop_reverse_data_0
#endif	 

get_next_page_pic:
	CALL	Api_LcdDriver_Set_PageColumn		;set lcd column and row
	CALL	Api_LcdDriver_Write_DataBurst	;display 255+1 word

	MOV		DPTR,#uc_pic_height
	MOVX	A,@DPTR	
	DEC		A
	JZ		dis_end						;display over
	MOVX	@DPTR,A	
	INC		uc_lcd_page_num
/***********************************more than one sector***********************/
	MOV		R0,#uc_pic_high_cnt
	INC		@R0
	MOV		A,@R0
	;INC		uc_pic_high_cnt
	;MOV		A, uc_pic_high_cnt
	CLR		C
	SUBB	A, #04H
	JNZ		label_not_512
	INC		sfc_logblkaddr0
	MOV		A, sfc_logblkaddr0
	JNZ		label_read_another_sector
	INC		sfc_logblkaddr1
	JMP		label_read_another_sector
/***********************************more than one sector***********************/
label_not_512:	
	MOV		A,MR_LCMNUML				;Get the data number(bytes)
	INC		A
	MOV		R7,A

	MOV		A,MR_LCMIMAL
	ADD		A,R7						;Original offset + disdata number
	MOV		MR_LCMIMAL,A
	
	JMP		get_next_page_pic
dis_end:

#ifdef API_AUDIO_FILL_DBUF_EVENT	
	CALL	Api_Check_Fill_Dbuf_Event
#endif
	RET

Read_OneSector_Picture:
	MOV		sfc_logblkaddr3, #000H

	MOV		sfc_occ_buf_flag, #SFC_APP_FBUF			;occupy FBUF
	MOV		buf_offset0, #LOW FCOMBUF_OFFSET_W
	MOV		buf_offset1, #HIGH FCOMBUF_OFFSET_W
	SETB	sfc_reserved_area_flag
	CALL	_media_readsector
	CLR		sfc_reserved_area_flag
	RET
/*******************************************************************************
Function: _Get_Picture_Header()
Description: get 8 bytes infomation which is width, height etc. of picture
Calls:
Global:uc_pic_width--picture width
       uc_pic_height--picture height
       MR_LCMIMAH,MR_LCMIMAL--picture start addr(sector)
       uc_pic_size_high,uc_pic_size_low--picture size(word)
Input: R2,R3--index of picture in flash memory(the max value is 0x7fff)
Output:Null
Others:
********************************************************************************/
_Get_Picture_Header:
	MOV		A, R7								
	ANL		A, #00111111B						;get picture index in one sector (one sector include 64 picture header)					
	MOV		MR_LCMIMAL, A						;MR_LCMIMAL--save offset in one sector of header infomation, as temp reg

	MOV		A, R7						
	ANL		A, #11000000B				
	MOV		B, R6								
	MOV		C, B.0
	RRC		A
	MOV		C, B.1
	RRC		A
	MOV		C, B.2
	RRC		A
	MOV		C, B.3
	RRC		A
	MOV		C, B.4
	RRC		A
	MOV		C, B.5
	RRC		A
	MOV		MR_LCMNUML, A						;MR_LCMNUML--save the address (sector) of area of pic_header_info, as temp reg
	
	MOV		C, B.6
	CLR		A
	ADDC	A, #00H
	MOV		MR_LCMIMAH, A

	MOV		MR_LCMNUMH, #2						;segment name 'MR_LCMNUMH + 2' (picture infomation is lie in segment 4)
	CALL	Read_Ctra

	MOV		A, sfc_logblkaddr0		 
	MOV		DPTR, #sfc_logblkaddr0_bk
	MOVX	@DPTR, A
	CLR		C
	ADD		A, MR_LCMNUML						;segment logic address add pic logic address
	MOV		sfc_logblkaddr0, A

	MOV		A, sfc_logblkaddr1		 
	MOV		DPTR, #sfc_logblkaddr1_bk
	MOVX	@DPTR, A
	ADDC	A, MR_LCMIMAH						;segment logic address add pic logic address
	MOV		sfc_logblkaddr1, A

	MOV		A, sfc_logblkaddr2		 
	MOV		DPTR, #sfc_logblkaddr2_bk
	MOVX	@DPTR, A
	ADDC	A, #00H								;segment logic address add pic logic address
	MOV		sfc_logblkaddr2, A

	SETB	sfc_reserved_area_flag
	MOV		sfc_occ_buf_flag, #SFC_APP_FBUF
	MOV		buf_offset0, #LOW FCOMBUF_OFFSET_W
	MOV		buf_offset1, #HIGH FCOMBUF_OFFSET_W
	CALL	_media_readsector
	CLR		sfc_reserved_area_flag
/****** current picture header 8 bytes data ******/
 	MOV		A, MR_LCMIMAL						;MR_LCMIMAL--save offset in one sector of header infomation, as temp reg
	MOV		B, #08H
	MUL		AB
	ADD		A, #LOW (X_ADDR_FBUF + FCOMBUF_OFFSET_B)
	MOV		DPL, A
	MOV		A, B
	ADDC	A, #HIGH (X_ADDR_FBUF + FCOMBUF_OFFSET_B)
	MOV		DPH, A

/*** for var_bk is xdata ***/
	MOV		R3, DPL
	MOV		R4, DPH

	MOV		DPTR, #sfc_logblkaddr0_bk				;restore bk address
	MOVX	A, @DPTR
	MOV		R0, A

	MOV		DPTR, #sfc_logblkaddr1_bk				;restore bk address
	MOVX	A, @DPTR
	MOV		R1, A

	MOV		DPTR, #sfc_logblkaddr2_bk			   	;restore bk address
	MOVX	A, @DPTR
	MOV		R2, A

	MOV		DPL, R3
	MOV		DPH, R4
/*** read dptr for pic info ***/
	CLR		C
	MOVX	A, @DPTR
	ADD		A, R0
	MOV		sfc_logblkaddr0, A
													;get pic data address low word low byte
	INC		DPTR
	MOVX	A, @DPTR
	ADDC	A, R1
	MOV		sfc_logblkaddr1, A						;get pic data address low word high byte

	INC		DPTR
	MOVX	A, @DPTR
	ADDC	A, R2
	MOV		sfc_logblkaddr2, A						;get pic data address high word low byte

	INC		DPTR
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A					    ;get pic data address high word high byte

  	INC		DPTR									;get picture width
	MOVX	A, @DPTR
	CLR		ACC.0
	RR		A
	DEC		A						
	MOV		MR_LCMNUML, A							;picture width for word

	INC		DPTR									
	MOVX	A, @DPTR
 	MOV		DPTR, #uc_pic_height						;get the picture height
	MOVX	@DPTR, A
   	JZ		_pic_error								;judge pic error
   	CLR		C
	SUBB	A,#09									;the height <= 8 pages
	JC		_pic_ok
_pic_error:
	CLR		C
	RET
_pic_ok:
	SETB	C
	RET

#endif



#ifdef API_LCD_DISPLAY_UNICODE
PUBLIC	_Api_Display_Unicode
/*******************************************************************************
Function:   _Api_Display_Unicode
Description: interface for display unicode
Calls:
Global:Null
Input:   R6¡BR7--unicode in flash memory reserved area
Output:Null
Others:
********************************************************************************/
_Api_Display_Unicode:
	CLR		b_unicode_flg						;0--16*8, 1--16*16
	MOV		A, R6
	JZ		unicode_16x8
	SETB	b_unicode_flg
	JMP		display_unicode
unicode_16x8:
	MOV		A, R7
	JNZ		display_unicode
	SETB	b_unicode_flg
display_unicode:
	CALL	_Read_OneSector_FontData	

#ifdef API_LCD_REVERSE_DISPLAY
	JNB		b_lcd_reverse_flg,_not_reverse_data
	MOV		DPTR,#X_ADDR_FCOMBUF					;512Bytes
	MOV		R7,	#32
	MOV		R6,	#16
loop_reverse_data:
	MOVX	A,@DPTR
	CPL		A
	MOVX	@DPTR,A
	INC		DPTR
	DJNZ	R7,	loop_reverse_data
	MOV		R7,	#32
	DJNZ	R6,	loop_reverse_data
#endif	
_not_reverse_data:  
	CALL	Api_LcdDriver_Set_PageColumn
	MOV		MR_LCMNUML, #07H
	CALL	Api_LcdDriver_Write_DataBurst			;display first page(16*8)

	INC		uc_lcd_page_num

	MOV		A, MR_LCMIMAL
	ADD		A, #08H									;Unicode data addr + 8W
	MOV		MR_LCMIMAL, A
	CALL	Api_LcdDriver_Set_PageColumn
	CALL	Api_LcdDriver_Write_DataBurst			;display next page(16*8)	

	DEC		uc_lcd_page_num							;for display next font 
	MOV		A, uc_lcd_column_num
	MOV		R7, #16									;16*16
	JB		b_unicode_flg, unicode_16x16
	MOV		R7, #8									;16*8
unicode_16x16:
	ADD		A, R7
	MOV		uc_lcd_column_num, A					;column start addr + 16(8)

#ifdef API_AUDIO_FILL_DBUF_EVENT	
	CALL	Api_Check_Fill_Dbuf_Event
#endif
	RET



/*******************************************************************************
Function:
Description:
	1. read 32bytes unicode data (unicode font addr = unicode * 32byte)
	   R2(bit7~0)&R3(bit7~4) is the sector offset(total 12bit)
	   R3(big3~0)*16word is the word offset in one sector (rotate 4bit right)
	   R3(big3~0)*16words + 0x11A is the word offset in fbuf(0x5000)
	2. convert unicolor into multicolor
Calls:
Global:FBUF-Common buf (XDATA:0x6234)
Input: R2(high),R3(low)---unicode
Output:Null
Others:
********************************************************************************/
_Read_OneSector_FontData:
/*1. read 32bytes unicode data*/
	MOV		A, R7								;get the R7 high 4bit
	ANL		A, #11110000B
	SWAP	A
	MOV		R2, A								;save the sector number
	
	MOV		A, R7
	SWAP	A
	ANL		A, #11110000B
	MOV		MR_LCMIMAL, A						;get the word offset in one sector: R7(bit3~0)*16word


	MOV		A, R6								;get the R6 high 4bit
	SWAP	A
	ANL		A, #00001111B			
	MOV		MR_LCMIMAH, A						;get sector offset high 4bit

	MOV		A, R6						 		
	SWAP	A									;get the R6 low 4bit
	ANL		A, #11110000B
	ORL		A, R2								;get sector offset low 8bit
	MOV		MR_LCMNUML, A						;get the sector addr low8bit, the UNICODE_FONT_ADDR low 8bit is 0x00(MR_LCMNUML--temp reg)
 
	MOV		MR_LCMNUMH, #4						;segment name 'MR_LCMNUMH + 2' (font infomation is lie in segment 6)
	CALL	Read_Ctra

	CLR		C
	MOV		A, sfc_logblkaddr0
	ADD		A, MR_LCMNUML						; get UNICODE address low word low byte
	MOV		sfc_logblkaddr0, A

	MOV		A, sfc_logblkaddr1
	ADDC	A, MR_LCMIMAH	
	MOV		sfc_logblkaddr1, A					; get UNICODE address low word high byte

	MOV		A, sfc_logblkaddr2
	ADDC	A, #00H	
	MOV		sfc_logblkaddr2, A					; get UNICODE address high word low byte

	MOV		A, MR_LCMIMAL
	ADD		A,#LOW FCOMBUF_OFFSET_W		;R7(bit3~0)*16W + 0x00A((0x6600-0x6000)/2) 
	MOV		MR_LCMIMAL, A				;get the word offset in one sector: R7(bit3~0)*16word
		
	SETB	sfc_reserved_area_flag
	MOV		sfc_occ_buf_flag, #SFC_APP_FBUF
	MOV		buf_offset0, #LOW FCOMBUF_OFFSET_W
	MOV		buf_offset1, #HIGH FCOMBUF_OFFSET_W
	CALL	_media_readsector
	CLR		sfc_reserved_area_flag
	RET

#endif



#ifdef API_LCD_DISPLAY_8x6ZIKU
	PUBLIC _Api_Display_8x6_ZiKu
/*****************************Display 8*6 Ziku **********************************
Function:
Description: display 8*6 Ziku
input:
		R7:ziku_8x6_index low byte
       	R6:ziku_8x6_index high byte
user register: R6,R7
********************************************************************************/
_Api_Display_8x6_ZiKu:
	CLR		C
	MOV		A, #LOW	ADRESS_8x6_ZIKU			 					;picture address 4800~4809
	ADD		A, R7
	MOV		R7, A
	MOV		A, #HIGH ADRESS_8x6_ZIKU
	ADDC	A,#00H
	MOV		R6, A
	CALL	_Api_Display_Picture
	MOV		A, uc_lcd_column_num
	ADD		A,#006H
	MOV		uc_lcd_column_num, A
	RET
#endif



#ifdef API_LCD_CLR_ASSIGNED_AREA 
	PUBLIC _Api_ClrAssigned_Area
/*******************************************************************************
Function: _Api_ClrAssigned_Area
input: R7(width)
Description: Clr one page area(width is R7)
*******************************************************************************/
_Api_ClrAssigned_Area:
	CALL	Api_LcdDriver_Set_PageColumn				;Set the LCD page and column
loop_display_data_1:
	CLR		A
	MOV		R3,A							 
	CALL	Api_Driver_Write_LcdData						;input is  ar
	DJNZ	R7,loop_display_data_1
	RET

#endif



#ifdef API_AUDIO_FILL_DBUF_EVENT	
	PUBLIC Api_Check_Fill_Dbuf_Event
/*******************************************************************************
Function: Api_Check_Fill_Dbuf_Event
input: 
Description: fill dbuf for voice and music
*******************************************************************************/
Api_Check_Fill_Dbuf_Event:
	MOV		A, ui_sys_event				;fill dbuf event is 0x0500 in music or voice
	JNB		ACC.2, Label_Not_Fill_Dbuf
	ANL		ui_sys_event, #11111011B	;clr fill dbuf event

	CALL	Ap_ProcessEvt_FillDecBuf	;fill dbuf with new data
Label_Not_Fill_Dbuf:
	RET
#endif

#endif
	END