/******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_bootblock_pm1_start.asm
Creator: zj.zong					Date: 2009-04-10
Description: 
Others:
Version: V0.11
History:
	V0.11	2009-11-25		andyliu
			1. modify "Ap_Backup_BootBlock_Mirror"
	V0.1	2009-04-10		zj.zong
		 1. build this module
*******************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_BOOTBLOCK_DEF

#include "sh86278_sfr_reg.inc"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"
#include "../../AP/AP_BootBlock/ap_bootblock_var.inc"

EXTRN	CODE (Api_DynamicLoad_CodeCpm)
EXTRN	CODE (Api_DynamicLoad_CodeMpm1_8kExecute)
EXTRN	CODE (Api_Read_SysPara_ToDbuf0)
EXTRN	CODE (_Api_Delay_1ms)
EXTRN	CODE (_flash_check_id)
EXTRN	CODE (_media_initialize)
EXTRN	CODE (_sfc_erase_phy_block)
EXTRN	CODE (_sfc_phy_read_page)
EXTRN	CODE (_sfc_phy_write_page)
EXTRN   CODE (Ui_PowerON_Backlight)
EXTRN   CODE (_get_flash_capacity)
EXTRN   CODE (Api_LcdDriver_PowerDown)

PUBLIC	Ap_Check_FlashPara
PUBLIC	Ap_Get_DiskCapacity
PUBLIC	Ap_Backup_BootBlock_Mirror	
PUBLIC	Ap_Check_PowerOn_Mp3
PUBLIC	Ap_PowerOff_MP3	
PUBLIC	Ap_Check_InitFlash	
PUBLIC	Ap_Clear_XdataMemory	
PUBLIC	Ap_Init_Timer	
PUBLIC 	Ap_Enter_ModePoweron
PUBLIC  Ap_Enter_ModeUSB
PUBLIC  Ap_Detect_USBConnected
PUBLIC  Ap_InitLcm_Var



/*******************************************************************************
interrupt process
*******************************************************************************/
	CSEG	AT		1000H		;mpeg command interrupt vector address	
	 	JMP		_int_mpeg_cmd

	CSEG	AT		1003H		;mpeg special interrupt 1 vector address
	 	JMP		_int_mpeg_s1

	CSEG	AT		1006H		;mpeg flag interrupt vector address	 	
		JMP		_int_mpeg_flag 

	CSEG	AT		1009H		;ADC interrupt vector address
 		JMP		_int_adc

	CSEG	AT		100CH		;USB EP0 interrupt vector address
		JMP		_usb_ep0_isr

	CSEG	AT		100FH		;USB IEP1 interrupt vector address
		JMP		_usb_iep1_isr

	CSEG	AT		1012H		;USB OEP2 interrupt vector address
		JMP		_usb_oep2_isr

AP_BOOTBLOCK	SEGMENT		CODE
				RSEG		AP_BOOTBLOCK

Ap_Check_FlashPara:

	MOV		DPTR, #RAIT_PARA_BASE + 70H	
	MOVX	A, @DPTR
	MOV		ef_type, A

	MOV		DPTR, #RAIT_PARA_BASE + 074H		;valid page num one block
	MOVX	A, @DPTR
	MOV		uc_valid_page_num, A

	MOV		DPTR, #RAIT_PARA_BASE + 095H		;flash reverse only read area address3
	MOVX	A, @DPTR
	MOV		DPTR, #uc_reserve_addr3
	MOVX	@DPTR, A

	MOV		DPTR, #RAIT_PARA_BASE + 096H		;flash reverse only read area address2
	MOVX	A, @DPTR
	MOV		DPTR, #uc_reserve_addr2
	MOVX	@DPTR, A

	MOV		DPTR, #RAIT_PARA_BASE + 097H		;flash reverse only read area address1
	MOVX	A, @DPTR
	MOV		DPTR, #uc_reserve_addr1
	MOVX	@DPTR, A

	MOV		DPTR, #RAIT_PARA_BASE + 098H		;flash reverse only read area address0
	MOVX	A, @DPTR
	MOV		DPTR, #uc_reserve_addr0
	MOVX	@DPTR, A

 	MOV		DPTR, #RAIT_PARA_BASE + 0B0H		;bootcode current zone
	MOVX	A, @DPTR
	MOV		uc_bootcode_cur_zone, A

	MOV		DPTR, #RAIT_PARA_BASE + 0B1H		;bootcode current block
	MOVX	A, @DPTR
	MOV		uc_bootcode_cur_block, A

	MOV		DPTR, #RAIT_PARA_BASE + 0B2H		;bootcode current block high
	MOVX	A, @DPTR
	MOV		uc_bootcode_cur_block_h, A

	MOV		DPTR, #RAIT_PARA_BASE + 0B5H		;other rait address
	MOVX	A, @DPTR
;	MOV		uc_others_rait_address, A
	MOV		DPTR,#uc_others_rait_address
	MOVX	@DPTR,A

	MOV		DPTR, #RAIT_PARA_BASE + 0B8H		;current rait address 
	MOVX	A, @DPTR
	MOV		uc_current_rait_address, A

	MOV		DPTR, #RAIT_PARA_BASE + 0CAH		;first bootblk backup zone
	MOVX	A, @DPTR
	MOV		uc_bootcode_zone_bk1, A

	MOV		DPTR, #RAIT_PARA_BASE + 0CBH		;first bootblk backup block
	MOVX	A, @DPTR
	MOV		uc_bootcode_block_bk1, A

	MOV		DPTR, #RAIT_PARA_BASE + 0CCH		;second bootblk backup zone
	MOVX	A, @DPTR
	MOV		uc_bootcode_zone_bk2, A

	MOV		DPTR, #RAIT_PARA_BASE + 0CDH		;second bootblk backup block
	MOVX	A, @DPTR
	MOV		uc_bootcode_block_bk2, A

	MOV		DPTR, #RAIT_PARA_BASE + 0CEH		;third bootblk backup zone
	MOVX	A, @DPTR
	MOV		uc_bootcode_zone_bk3, A

	MOV		DPTR, #RAIT_PARA_BASE + 0CFH		;third bootblk backup block
	MOVX	A, @DPTR
	MOV		uc_bootcode_block_bk3, A

  MOV		DPTR, #RAIT_PARA_BASE + 0D0H		;first bootblk backup block high
	MOVX	A, @DPTR
	MOV		uc_bootcode_block_bk1_h, A	

	MOV		DPTR, #RAIT_PARA_BASE + 0D1H		;second bootblk backup block high
	MOVX	A, @DPTR
	MOV		uc_bootcode_block_bk2_h, A	
	
	MOV		DPTR, #RAIT_PARA_BASE + 0D2H		;third bootblk backup block high
	MOVX	A, @DPTR
	MOV		uc_bootcode_block_bk3_h, A

  MOV		DPTR, #RAIT_PARA_BASE + 1BFH		;lcd type offset
	MOVX	A, @DPTR
	MOV		DPTR, #uc_lcd_type
	MOVX	@DPTR, A
	MOV		DPTR, #RAIT_PARA_BASE + 1C6H		;lcd contrast base
	MOVX	A, @DPTR
	MOV		DPTR, #uc_lcd_contrast_base
	MOVX	@DPTR, A
	MOV		DPTR, #RAIT_PARA_BASE + 1C7H		;lcd contrast data
	MOVX	A, @DPTR
	MOV		DPTR, #uc_lcd_contrast_data
	MOVX	@DPTR, A

	MOV		DPTR, #RAIT_PARA_BASE + 1C8H	 	;for LCD cmd table
	MOV		R2, DPL
	MOV		R3,	DPH
	MOV		DPTR, #uc_lcd_cmd_table
	MOV		R4, DPL
	MOV		R5,DPH
	MOV		R0, #0FH
	CALL	Ap_fill_table_with_rait

	MOV		DPTR, #RAIT_PARA_BASE + 1D7H	;for LCD page table
	MOV		R2, DPL
	MOV		R3,	DPH
	MOV		DPTR, #uc_page_table		
	MOV		R4, DPL
	MOV		R5, DPH
	MOV		R0, #08H	
	CALL	Ap_fill_table_with_rait
	
	MOV		DPTR, #RAIT_PARA_BASE + 1F0H		;system setting option
	MOVX	A, @DPTR
	MOV		DPTR, #uc_user_option1
	MOVX	@DPTR, A

	MOV		DPTR, #RAIT_PARA_BASE + 1F1H		;system setting option
	MOVX	A, @DPTR
	MOV		DPTR, #uc_user_option2
	MOVX	@DPTR, A

	MOV		DPTR, #RAIT_PARA_BASE + 1F2H		;system setting option
	MOVX	A, @DPTR
	MOV		DPTR, #uc_user_option3
	MOVX	@DPTR, A

	MOV		DPTR, #RAIT_PARA_BASE + 1F8H		;lcd bklight grade
	MOVX	A, @DPTR
	MOV		DPTR, #uc_bklight_grade
	MOVX	@DPTR, A

	MOV		DPTR, #RAIT_PARA_BASE + 1FAH		;system setting 3.0V,1.7V
	MOVX	A, @DPTR
	ANL		A, #0f0H
	;ORL		A, #02H
	ORL		A, #05H								;1.9V
	MOV		MR_LDOOUT, A
	
	MOV		DPTR, #RAIT_PARA_BASE + 29CH		;for block page mapping
	MOV		R2, DPL
	MOV		R3, DPH
	MOV		DPTR, #uc_pagemapping_table		
	MOV		R4, DPL
	MOV		R5, DPH
	MOV		R0, #00H							;256 bytes	
	CALL	Ap_fill_table_with_rait
	
/****** read some speciality parameters in flash ******/
	JMP		_flash_check_id						;return bit 'CY' (0:ERROR, 1:OK)


/*Input: R2,R3,R4,R5, R0*/
Ap_fill_table_with_rait:
	MOV		DPL, R2
	MOV		DPH, R3
	MOVX	A, @DPTR
	INC		DPTR
	MOV		R2, DPL
	MOV		R3,	DPH

	mov		DPL,R4
	MOV		DPH, R5
	MOVX	@DPTR, A
	INC		DPTR
	MOV		R4, DPL
	MOV		R5,DPH
	DJNZ	R0, Ap_fill_table_with_rait
	RET

 
Ap_InitLcm_Var:
	MOV		DPTR, #uc_lcd_column_offset
	MOV		A, #00H
	MOVX	@DPTR, A
	MOV		DPTR, #uc_lcd_width
	MOV		A, #132
	MOVX	@DPTR, A

	CLR		b_lcd_reverse_flg
	RET


Ap_Check_PowerOn_Mp3:
//	CALL	Ap_Check_LongPress_KeyPlay			;return bit 'CY' (0:not play, 1:play)
	SETB	C	
	CALL	_Ap_Check_PowerOn
	RET

Ap_Check_InitFlash:
	CALL	_media_initialize
/*	MOV		A, sfc_status
	JZ		init_ok
	CLR		C	
	RET	*/	
init_ok:
	SETB	C
	RET

Ap_Get_DiskCapacity:
	SETB	sfc_reserved_area_flag
	CALL	_get_flash_capacity
	CLR		sfc_reserved_area_flag
	RET


/************************************************************************
Function: Ap_Check_LongPress_KeyPlay
Discription:return bit 'CY', 1---press	0---not press
************************************************************************/
Ap_Check_LongPress_KeyPlay:
	CLR		b_poweroff_flg
	MOV		R0, #000H
check_press_key_play_again:
	MOV		MR_WDT, #0FFH						   
	MOV		A, MR_PADATA
	JNB		ACC.0, not_press_key_play
	MOV		R7, #10
	CALL	_Api_Delay_1ms							  
	INC		R0
	MOV		A, R0
	CJNE	A, #2, check_press_key_play_again		;wip off wobble for 3 times, ok return 'CY' 1
	SETB	C
	RET
not_press_key_play:
	MOV		R0, 000H
	MOV		R7, #10
	CALL	_Api_Delay_1ms
	MOV		A, MR_PADATA							;bit0 play flag
	JB		ACC.0, check_press_key_play_again		;check if has a wobble
	CLR		C
	RET


/******************************************************************************
Function: check if power on via key play or directly
******************************************************************************/
_Ap_Check_PowerOn:
	MOV		DPTR, #RAIT_PARA_BASE + 1F1H
	MOVX	A, @DPTR
	JB		ACC.1, not_check_key_play				;read uc_user_option2, bit1 is power on control
check_usb_play:
	MOV		A, MR_USBCON 
	JB		ACC.0, power_on							;if has a USB power supply
	JC		power_on								;check if has a long press on key play
	SETB	b_poweroff_flg
	RET
not_check_key_play:
	MOV		A, MR_RESET								;check if has a power on reset
	ANL		A, #03H
	JZ		check_usb_play
	ANL		MR_RESET, #0FCH
power_on:
	RET


/************************************************************************
Function: check block bk1 or bk2 that it's invalid or valid
************************************************************************/
Ap_Backup_BootBlock_Mirror:
  	MOV		uc_block_init_pageaddr, #00H
 	MOV		uc_block_copy_cnt, uc_valid_page_num	
	INC		uc_block_copy_cnt						;copy length is uc_valid_page_num + 1
	MOV		R0, #uc_bootcode_cur_zone				;check if current zone is zone_bk1
	MOV		A, uc_bootcode_zone_bk1
	CLR		C
	SUBB	A, @R0
	JNZ		bootcode_bk1_invalid					;if not one and the same, zone_bk1 is bad, and need repair 
	MOV		R0, #uc_bootcode_cur_block
	MOV		A, uc_bootcode_block_bk1
	CLR		C
	SUBB	A, @R0
	JNZ		bootcode_bk1_invalid					;if not one and the same, block_bk1 is bad, and need repair
	MOV		R0, #uc_bootcode_cur_block_h
	MOV		A, uc_bootcode_block_bk1_h
	CLR		C
	SUBB	A, @R0
	JNZ		bootcode_bk1_invalid					;if not one and the same, block_bk1 is bad, and need repair
	RET

copyback_over_ret:
	RET

bootcode_bk1_invalid:
	MOV		A, ef_type
	JNB		ACC.0, copyback_over_ret				    ;small block not copyback

	MOV		R0, #uc_bootcode_cur_zone				;check if current zone is zone_bk2
	MOV		A, uc_bootcode_zone_bk2
	CLR		C
	SUBB	A, @R0
	JNZ		bootcode_bk2_invalid 					;if not one and the same, zone_bk2 is bad, and need repair
	MOV		R0, #uc_bootcode_cur_block_h				;check if current zone is zone_bk2
	MOV		A, uc_bootcode_block_bk2_h
	CLR		C
	SUBB	A, @R0
	JNZ		bootcode_bk2_invalid 					;if not one and the same, zone_bk2 is bad, and need repair
	MOV		R0, #uc_bootcode_cur_block
	MOV		A, uc_bootcode_block_bk2
	CLR		C
	SUBB	A, @R0
	JNZ		bootcode_bk2_invalid					;if not one and the same, block_bk2 is bad, and need repair

	MOV		r3_temp, uc_bootcode_zone_bk1
	MOV		r2_temp, uc_bootcode_block_bk1
	MOV		r2_temp_h, uc_bootcode_block_bk1_h
	MOV		uc_block_target_zoneaddr, uc_bootcode_zone_bk1		
	MOV		uc_block_target_blkaddr, uc_bootcode_block_bk1
;	MOV		uc_block_target_blkaddr_h, uc_bootcode_block_bk1_h
	MOV		DPTR,#uc_block_target_blkaddr_h
	MOV		A,uc_bootcode_block_bk1_h
	MOVX	@DPTR,A	
	CALL	_sfc_erase_phy_block
	MOV		uc_block_source_zoneaddr, uc_bootcode_zone_bk2		;source zone
	MOV		uc_block_source_blkaddr, uc_bootcode_block_bk2		;source block
;	MOV		uc_block_source_blkaddr_h, uc_bootcode_block_bk2_h		;source block
	MOV		DPTR,#uc_block_source_blkaddr_h
	MOV		A,uc_bootcode_block_bk2_h
	MOVX	@DPTR,A	
	JMP		copyback_to_invalid_block 
	
bootcode_bk2_invalid:
	MOV		r3_temp, uc_bootcode_zone_bk2
	MOV		uc_block_target_zoneaddr, uc_bootcode_zone_bk2		
	MOV		r2_temp, uc_bootcode_block_bk2
	MOV		r2_temp_h, uc_bootcode_block_bk2_h
	MOV		uc_block_target_blkaddr, uc_bootcode_block_bk2
;	MOV		uc_block_target_blkaddr_h, uc_bootcode_block_bk2_h
	MOV		DPTR,#uc_block_target_blkaddr_h
	MOV		A,uc_bootcode_block_bk2_h
	MOVX	@DPTR,A	
	CALL	_sfc_erase_phy_block
	MOV		uc_block_source_zoneaddr, uc_bootcode_zone_bk3		;source zone
	MOV		uc_block_source_blkaddr, uc_bootcode_block_bk3		;source block
;	MOV		uc_block_source_blkaddr_h, uc_bootcode_block_bk3_h		;source block
	MOV		DPTR,#uc_block_source_blkaddr_h
	MOV		A,uc_bootcode_block_bk3_h
	MOVX	@DPTR,A	
	JMP		copyback_to_invalid_block  

copyback_to_invalid_block:
/*read 2k page data*/
	MOV		sfc_occ_buf_flag, #SFC_APP_CBUF					;read one page data
	MOV		r3_temp, uc_block_source_zoneaddr
	MOV		r2_temp, uc_block_source_blkaddr
;	MOV		r2_temp_h, uc_block_source_blkaddr_h
    MOV		DPTR,#uc_block_source_blkaddr_h
	MOVX	A,@DPTR
	MOV		r2_temp_h,A
	MOV		R7, uc_block_init_pageaddr
	CALL	_get_sfc_pageaddr	
	MOV		buf_offset0, #00H
	MOV		buf_offset1, #00H
	CALL	_sfc_phy_read_page

	ACALL	restore_block_bk_parameter
	
/*write 2k page data*/
	MOV		sfc_occ_buf_flag, #SFC_APP_CBUF	
	MOV		r3_temp, uc_block_target_zoneaddr
	MOV		r2_temp, uc_block_target_blkaddr
;	MOV		r2_temp_h, uc_block_target_blkaddr_h
	MOV		DPTR,#uc_block_target_blkaddr_h
	MOVX	A,@DPTR
	MOV		r2_temp_h,A
	MOV		R7, uc_block_init_pageaddr
	CALL	_get_sfc_pageaddr	
	MOV		buf_offset0, #00H
	MOV		buf_offset1, #00H
	CALL	_sfc_phy_write_page
	INC		uc_block_init_pageaddr	
	DJNZ	uc_block_copy_cnt, copyback_to_invalid_block	
copyback_over:
	RET

restore_block_bk_parameter:

	MOV		A, uc_block_init_pageaddr				;is current rait page address ?
	CJNE	A, uc_current_rait_address, modify_end
	
	MOV		sfc_occ_buf_flag, #SFC_APP_DBUF0		;read one page data
	MOV		r3_temp, uc_block_source_zoneaddr
	MOV		r2_temp, uc_block_source_blkaddr
;	MOV		r2_temp_h, uc_block_source_blkaddr_h
    MOV		DPTR,#uc_block_source_blkaddr_h
	MOVX	A,@DPTR
	MOV		r2_temp_h,A
;	MOV		R7, uc_others_rait_address
	MOV		DPTR,#uc_others_rait_address
	MOVX	A,@DPTR
	MOV		R7,A		
	CALL	_get_sfc_pageaddr						;page6 is RAIT Backup	
	MOV		buf_offset0, #00H
	MOV		buf_offset1, #00H
	CALL	_sfc_phy_read_page
	MOV		sfc_occ_buf_flag, #SFC_APP_CBUF			;read one page data

	MOV		A, uc_block_target_blkaddr
	CJNE	A, uc_bootcode_block_bk2, restore_bootcode_bk1
;	MOV		A, uc_block_target_blkaddr_h
	MOV		DPTR, #uc_block_target_blkaddr_h
	MOVX	A,@DPTR
	CJNE	A, uc_bootcode_block_bk2_h, restore_bootcode_bk1
restore_bootcode_bk2:
	MOV		DPTR, #X_ADDR_DBUF0 + 400H	;for bootcode bk 2
	MOV		R2, DPL
	MOV		R3, DPH
 	JMP	   target_block_address
restore_bootcode_bk1:
	MOV		DPTR, #X_ADDR_DBUF0			;for bootcode bk 1 
	MOV		R2, DPL
	MOV		R3, DPH
target_block_address:
	MOV		DPTR, #X_ADDR_CBUF			;for bootcode bk address
	MOV		R4, DPL
	MOV		R5, DPH
loop_fill_xdata:
	MOV		DPL, R2
	MOV		DPH, R3
	MOVX	A, @DPTR
	INC		DPTR
	MOV		R2, DPL
	MOV		R3,	DPH

	MOV		DPL,R4
	MOV		DPH, R5
	MOVX	@DPTR, A
	INC		DPTR
	MOV		R4, DPL
	MOV		R5,DPH
	CJNE	R4, #LOW (X_ADDR_CBUF), loop_fill_xdata
	CJNE	R5, #HIGH (X_ADDR_CBUF+400H), loop_fill_xdata
modify_end:	
	RET


_get_sfc_pageaddr:
	MOV		A, R7
	MOV		DPTR, #uc_pagemapping_table
	ADD		A, DPL
	MOV		DPL, A
	CLR		A
	ADDC	A, DPH
	MOV		DPH, A
	MOVX	A, @DPTR
  	MOV		r1_temp, A
	RET
			



/********************************ZONG*************************************************************************/


/************************************************************************
Function: clear external DATA
************************************************************************/
Ap_Clear_XdataMemory:
	MOV     DPTR, #X_ADDR_FBUF					;clr  XDATA 0x6000-0x0x6090
clr_xdata1:
	CLR		A								
	MOVX    @DPTR, A
	INC     DPTR
	MOV		A, DPL
	CJNE	A, #LOW X_ADDR_FBUF_COMMON, clr_xdata1
	MOV		A, DPH
	CJNE	A, #HIGH X_ADDR_FBUF_COMMON, clr_xdata1

	MOV     DPTR, #X_ADDR_IBUF + 200H			;clr  XDATA 0x7200-0x0x7420
clr_xdata2:
	CLR		A								
	MOVX    @DPTR, A
	INC     DPTR
	MOV		A, DPL
	CJNE	A, #LOW X_ADDR_IBUF_COMMON, clr_xdata2
	MOV		A, DPH
	CJNE	A, #HIGH X_ADDR_IBUF_COMMON, clr_xdata2
	RET



/************************************************************************
Function: initial timer
************************************************************************/
Ap_Init_Timer:
;	MOV		MR_TMPLCL, #020H					;init timer interrupt 20ms once
;	MOV		MR_TMPLCH, #04EH
	MOV		MR_TMPLCL, #064H					;init timer interrupt 100us once
	MOV		MR_TMPLCH, #000H

	MOV		MR_TMCON, #002H						;enable time irq
	RET


/************************************************************************
Function: check USB connected
Description: return 'CY', 1(connected) or 0(not)
************************************************************************/
Ap_Detect_USBConnected:
	CLR		b_usb_poweron
	MOV		R0, #000H
check_usb_connected_again:
	MOV		MR_WDT, #0FFH
	MOV		A, MR_USBCON
	JNB		ACC.0, usb_not_connected
	MOV		R7, #20
	CALL	_Api_Delay_1ms	
	INC		R0
	MOV		A, R0
	CJNE	A, #3, check_usb_connected_again	;check if USB connected for 3 times
	SETB	C
	RET 	 
usb_not_connected:
	CLR		C
	RET


/************************************************************************
Function: dynamicload mode USB
************************************************************************/
Ap_Enter_ModeUSB:
	CALL	Ui_PowerON_Backlight				;restore bklight when USB connected
	SETB	b_usb_is_on
	MOV		DPTR, #uc_load_code_idx
	MOV		A, #CODE_USB_MODE_CPM_IDX
	MOVX	@DPTR, A
	CALL	Api_DynamicLoad_CodeCpm
	MOV		DPTR, #uc_load_code_idx
	MOV		A, #CODE_USB_MODE_IDX
	MOVX	@DPTR, A
	JMP		Api_DynamicLoad_CodeMpm1_8kExecute

/************************************************************************
Function: dynamicload mode Power On
************************************************************************/
Ap_Enter_ModePoweron:
	MOV		DPTR, #uc_load_code_idx				;dynamic loading mode Power On
	MOV		A, #CODE_POWERON_MODE_IDX
	MOVX	@DPTR, A
	JMP		Api_DynamicLoad_CodeMpm1_8kExecute
		

Ap_PowerOff_MP3:
	CALL	Api_LcdDriver_PowerDown

	MOV		MR_PMUCON, #05AH					;power down mp3
	SJMP	$					



/*******************************************************************************
interrupt subroutine
*******************************************************************************/
_usb_ep0_isr:
_usb_iep1_isr:
_usb_oep2_isr:
_int_adc:
_int_mpeg_cmd:
_int_mpeg_s1:
_int_mpeg_flag:
	RETI

#endif
	END