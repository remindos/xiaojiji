/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: pm1_resident_var.asm
Creator: andyliu					Date: 2007-11-15
Description: define variable that is used in all mode
Others: 
Version: V0.1
History:
	V0.1	2007-11-15		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_memory_assign.h"






/*******************************ZONG*************/
PUBLIC	STACK

/*** for bit ***/
;PUBLIC	b_sfc_immediate_wr_flg
PUBLIC	b_key_hold_val
PUBLIC	b_usb_poweron
PUBLIC	b_play_poweron_flg
PUBLIC	b_msi_plug_flg
PUBLIC	b_unicode_flg
PUBLIC	b_lcd_clrall_flg
PUBLIC	b_lcd_reverse_flg
PUBLIC	b_bat_full_flg
PUBLIC	b_load_code_flg
PUBLIC	b_usb_is_on
PUBLIC	b_io_adc_scan_flg
PUBLIC	b_record_key_flag
PUBLIC	b_bat_changed_flg
PUBLIC	b_sd_flash_flg
PUBLIC	b_sd_exist_flg
PUBLIC	b_sd_wppin_flg
/*** for data ***/
PUBLIC	uc_load_code_length
PUBLIC	ui_sys_event
PUBLIC	uc_lcd_page_num
PUBLIC	uc_lcd_column_num
PUBLIC	uc_lcd_dis_flg
PUBLIC	uc_window_no
PUBLIC	uc_pic_height
PUBLIC	uc_pic_high_cnt
/*** for idata ***/
PUBLIC	uc_key_cnt
PUBLIC	uc_key_value
PUBLIC	key_vol_offset
PUBLIC	uc_key_old_value
PUBLIC	uc_key_release_cnt
PUBLIC	uc_adc_bat_data
PUBLIC	uc_timer_cnt
PUBLIC	uc_usb_timer_cnt1
PUBLIC	uc_usb_timer_cnt2
/*** for xdata ***/
PUBLIC	uc_user_option1
PUBLIC	uc_user_option2
PUBLIC	uc_user_option3
PUBLIC	uc_reserve_addr0
PUBLIC	uc_reserve_addr1
PUBLIC	uc_reserve_addr2
PUBLIC	uc_reserve_addr3
PUBLIC	uc_lcd_column_offset
PUBLIC	uc_lcd_width
PUBLIC	uc_lcd_height
PUBLIC	uc_lcd_contrast_base
PUBLIC	uc_lcd_contrast_data
PUBLIC  uc_page_table
PUBLIC  uc_lcd_type
PUBLIC  uc_refresh_menu_num
PUBLIC  uc_refresh_select_flg

PUBLIC	uc_key_val_table
PUBLIC	uc_key_voltage_table
PUBLIC	uc_poweron_pic_num
PUBLIC	uc_poweroff_pic_num
PUBLIC	uc_language_idx
PUBLIC	uc_save_mode_idx
PUBLIC	uc_bklight_time_level
PUBLIC	ui_bklight_time
PUBLIC	uc_bklight_grade
PUBLIC	uc_idle_poweroff_level
PUBLIC	ui_idle_poweroff_time
PUBLIC	ui_idle_poweroff_cnt
PUBLIC	uc_sleep_poweroff_level
PUBLIC	ui_sleep_poweroff_time
PUBLIC	uc_bat_level
PUBLIC	uc_bat_level_bk
PUBLIC	uc_bat_detec_valid_cnt
PUBLIC	uc_bat_valid_cnt
PUBLIC	uc_bat_charge_idx
PUBLIC	uc_bat_low_cnt
PUBLIC	uc_bat_detect_time
PUBLIC	uc_poweroff_type
PUBLIC	ui_sector_byte_offset
PUBLIC	uc_pic_size_high
PUBLIC	uc_pic_size_low
PUBLIC	uc_load_code_idx
PUBLIC	uc_load_rpt_cnt
PUBLIC	uc_load_temp_buf
PUBLIC uc_lcd_page_num_bk

/*** sfc ***/
PUBLIC	uc_sfc_wrpage_addr0
PUBLIC	uc_sfc_wrpage_addr1
PUBLIC	uc_sfc_wrpage_addr2
PUBLIC	uc_sfc_wrpage_addr3
PUBLIC	uc_sfc_wrpage_flg0
PUBLIC	uc_sfc_wrpage_flg1
PUBLIC	uc_sfc_page_flag_val
PUBLIC	uc_sfc_bk_logblkaddr0
PUBLIC	uc_sfc_bk_logblkaddr1
PUBLIC	uc_sfc_bk_logblkaddr2
PUBLIC	uc_sfc_bk_logblkaddr3
PUBLIC	uc_sfc_bak_occ_buf_flg
PUBLIC	uc_sfc_bakbuf_offset0
PUBLIC	uc_sfc_bakbuf_offset1

/*** for buf ***/
PUBLIC	ui_cbuf
PUBLIC  ul_cbuf
PUBLIC	uc_cbuf
PUBLIC	ul_dbuf0
PUBLIC	ui_dbuf0
PUBLIC	uc_dbuf0
PUBLIC	uc_game_map
PUBLIC	ul_dbuf1
PUBLIC	ui_dbuf1
PUBLIC	uc_dbuf1
PUBLIC	ui_sfc_combuf
PUBLIC	uc_sfc_combuf
PUBLIC	si_dbuf0
PUBLIC	si_dbuf1

/*** register ***/
PUBLIC	MR_DACI
PUBLIC	MR_DACCLK
PUBLIC	MR_DACSRST
PUBLIC	MR_DACPCON
PUBLIC	MR_AVCL
PUBLIC	MR_AVCR
PUBLIC	MR_MVCL
PUBLIC	MR_MVCR
PUBLIC	MR_DACSET
PUBLIC	MR_DACSTA
PUBLIC	MR_DACOPT1
PUBLIC	MR_DACOPT2
PUBLIC	MR_DACOPT3H
PUBLIC	MR_DACOPT3L
PUBLIC	MR_DACOPT4H
PUBLIC	MR_DACOPT4L
PUBLIC	MR_DACOPT5


PUBLIC	CR_MCUIRQEN_H
PUBLIC	CR_MCUIRQEN_L
PUBLIC	CR_MPEGIRQEN_H
PUBLIC	CR_MPEGIRQEN_L
PUBLIC	CR_MCUIRQ_H
PUBLIC	CR_MCUIRQ_L
PUBLIC	CR_MPEGIRQ_H
PUBLIC	CR_MPEGIRQ_L
PUBLIC	CR_MTDCMD_H
PUBLIC	CR_MTDCMD_L
PUBLIC	CR_DTMCMD_H
PUBLIC	CR_DTMCMD_L
PUBLIC	CR_MTDDAT1_H
PUBLIC	CR_MTDDAT1_L
PUBLIC	CR_MTDDAT2_H
PUBLIC	CR_MTDDAT2_L
PUBLIC	CR_DTMDAT1_H
PUBLIC	CR_DTMDAT1_L
PUBLIC	CR_DTMDAT2_H
PUBLIC	CR_DTMDAT2_L
PUBLIC	CR_DTMDAT3_H
PUBLIC	CR_DTMDAT3_L
PUBLIC	CR_DTMDAT4_H
PUBLIC	CR_DTMDAT4_L

PUBLIC	sfc_logblkaddr0_bk
PUBLIC	sfc_logblkaddr1_bk
PUBLIC	sfc_logblkaddr2_bk

PUBLIC	ir_state
PUBLIC	ir_state_cnt
PUBLIC	ir_data_cnt
PUBLIC	bit_cnt
PUBLIC	ir_data_buf
PUBLIC	uc_100us_timer_cnt	;20080807
PUBLIC	uc_200us_timer_cnt
PUBLIC	uc_us_timer_cnt
PUBLIC	key0_ir1
PUBLIC	ir_bit
PUBLIC	b_key_row

PUBLIC	b_led_pwm_flag
PUBLIC	b_led_flg
PUBLIC	b_led_pa3
PUBLIC	b_led_pa4
PUBLIC	b_led_pa5
PUBLIC	b_led_pa6
PUBLIC	b_bat_low_flag


	BSEG	AT		B_ADDR_COMMON

;b_sfc_immediate_wr_flg:	DBIT	1
b_key_row:
b_key_hold_val:			DBIT	1
b_usb_poweron:			DBIT	1
b_play_poweron_flg:		DBIT	1
b_unicode_flg:;			DBIT	1
b_lcd_clrall_flg:;		DBIT	1

b_msi_plug_flg:			DBIT	1
b_lcd_reverse_flg:;		DBIT	1
b_sd_flash_flg:;			DBIT	1
b_bat_full_flg:			DBIT	1
b_load_code_flg:		DBIT	1

b_usb_is_on:			DBIT	1
b_bat_changed_flg:		DBIT	1


b_led_pwm_flag:	;		DBIT	1
b_led_flg:				DBIT	1
b_led_pa3:	  			DBIT	1
b_led_pa4:				DBIT	1
b_led_pa5:				DBIT	1
b_led_pa6:				DBIT	1
b_bat_low_flag:			DBIT	1

b_sd_exist_flg:;			DBIT	1
b_sd_wppin_flg:;			DBIT	1

b_io_adc_scan_flg:;		DBIT	1
b_record_key_flag:;		DBIT	1

key0_ir1:					DBIT	1
ir_bit:						DBIT	1

	DSEG	AT		D_ADDR_COMMON
uc_load_code_length:	DS	1
ui_sys_event:			DS	2
uc_key_cnt:				DS	1
uc_lcd_page_num:		DS	1
uc_lcd_column_num:		DS	1

key_vol_offset:			DS	1

ir_state:				DS	1
ir_state_cnt:			DS	1
ir_data_cnt:			DS	1


	ISEG	AT		I_ADDR_COMMON
uc_key_value:			DS	1
uc_key_old_value:		DS	1
uc_key_release_cnt:		DS	1
uc_adc_bat_data:		DS	1
uc_timer_cnt:			DS	1
uc_usb_timer_cnt1:		DS	1
uc_usb_timer_cnt2:		DS	1
uc_load_rpt_cnt:		DS	1
uc_load_temp_buf:		DS	3
uc_key_voltage_table:	DS	5

uc_200us_timer_cnt:		DS	1
uc_100us_timer_cnt:		DS	1	;20080807
uc_us_timer_cnt:		DS	1
ir_data_buf:			DS	4
bit_cnt:				DS	1

uc_lcd_dis_flg:			DS	1
uc_window_no:			DS	1
;uc_pic_height:			DS	1
uc_pic_high_cnt:		DS	1


	ISEG	AT		I_ADDR_STACK
STACK:					DS	(100H - I_ADDR_STACK)


	XSEG	AT		X_ADDR_FBUF_COMMON


/*** for common ***/

uc_user_option1:		DS	1
uc_user_option2:		DS	1
uc_user_option3:		DS	1
uc_reserve_addr0:		DS	1
uc_reserve_addr1:		DS	1
uc_reserve_addr2:		DS	1
uc_reserve_addr3:		DS	1

/*** for system ***/

uc_language_idx:		DS	2
uc_save_mode_idx:		DS	1
uc_idle_poweroff_level:	DS	1
ui_idle_poweroff_time:	DS	2
ui_idle_poweroff_cnt:	DS	2
uc_sleep_poweroff_level:DS	1
ui_sleep_poweroff_time:	DS	2
ui_bklight_time:		DS	2

/*** for lcd ***/
uc_pic_height:			DS	1
uc_lcd_page_num_bk:     DS	1
uc_pic_size_high:		DS	1
uc_pic_size_low:		DS	1
uc_lcd_column_offset:	DS	1
uc_lcd_width:			DS	1
uc_lcd_height:			DS	1
uc_lcd_contrast_base:	DS	1
uc_poweron_pic_num:		DS	1
uc_poweroff_pic_num:	DS	1
uc_bklight_time_level:	DS	1
uc_bklight_grade:		DS	1
uc_lcd_contrast_data:	DS	1
uc_lcd_type:			DS	1
uc_page_table:			DS	8
uc_refresh_menu_num:	DS	1
uc_refresh_select_flg:	DS	1

/*** other ***/
uc_key_val_table:		DS	6
uc_poweroff_type:		DS	1
uc_load_code_idx:		DS	1
ui_sector_byte_offset:	DS	2

/*** bat ***/
uc_bat_level:			DS	1
uc_bat_level_bk:		DS	1
uc_bat_detec_valid_cnt:	DS	1
uc_bat_valid_cnt:		DS	1
uc_bat_charge_idx:		DS	1
uc_bat_low_cnt:			DS	1
uc_bat_detect_time:		DS	1

/*** sfc ***/
uc_sfc_wrpage_addr0:		DS		1
uc_sfc_wrpage_addr1:		DS		1
uc_sfc_wrpage_addr2:		DS		1
uc_sfc_wrpage_addr3:		DS		1
uc_sfc_bk_logblkaddr0:		DS		1
uc_sfc_bk_logblkaddr1:		DS		1
uc_sfc_bk_logblkaddr2:		DS		1
uc_sfc_bk_logblkaddr3:		DS		1
uc_sfc_wrpage_flg0:			DS		1
uc_sfc_wrpage_flg1:			DS		1
uc_sfc_page_flag_val:		DS		1
uc_sfc_bak_occ_buf_flg:		DS		1
uc_sfc_bakbuf_offset0:		DS		1
uc_sfc_bakbuf_offset1:		DS		1
sfc_logblkaddr0_bk:			DS		1
sfc_logblkaddr1_bk:			DS		1
sfc_logblkaddr2_bk:			DS		1


/*** for buf ***/
 	XSEG	AT		X_ADDR_CBUF
ul_cbuf:
ui_cbuf:
uc_cbuf:		DS		14336


	XSEG	AT		X_ADDR_DBUF0
si_dbuf0:
ul_dbuf0:
ui_dbuf0:
uc_dbuf0:		DS		4096


	XSEG	AT		X_ADDR_DBUF1
uc_game_map:
si_dbuf1:
ul_dbuf1:
ui_dbuf1:
uc_dbuf1:		DS		4096


	XSEG	AT		X_ADDR_FCOMBUF
ui_sfc_combuf:
uc_sfc_combuf:		DS		512


	XSEG	AT		X_ADDR_DAC
MR_DACI:		DS		1
MR_DACCLK:		DS		1
MR_DACSRST:		DS		1
MR_DACPCON:		DS		1
MR_AVCL:		DS		1
MR_AVCR:		DS		1
MR_MVCL:		DS		1
MR_MVCR:		DS		1
MR_DACSET:		DS		1
MR_DACSTA:		DS		1
MR_DACOPT1:		DS		1
MR_DACOPT2:		DS		1
MR_DACOPT3H:	DS		1
MR_DACOPT3L:	DS		1
MR_DACOPT4H:	DS		1
MR_DACOPT4L:	DS		1
MR_DACOPT5:		DS		1

	XSEG	AT		X_ADDR_CRA
CR_MCUIRQEN_H:	DS		1
CR_MCUIRQEN_L:	DS		1
CR_MPEGIRQEN_H:	DS		1
CR_MPEGIRQEN_L:	DS		1
CR_MCUIRQ_H:	DS		1
CR_MCUIRQ_L:	DS		1
CR_MPEGIRQ_H:	DS		1
CR_MPEGIRQ_L:	DS		1
CR_MTDCMD_H:	DS		1		;invalid command
CR_MTDCMD_L:	DS		1
CR_DTMCMD_H:	DS		1
CR_DTMCMD_L:	DS		1
CR_MTDDAT1_H:	DS		1
CR_MTDDAT1_L:	DS		1
CR_MTDDAT2_H:	DS		1
CR_MTDDAT2_L:	DS		1
CR_DTMDAT1_H:	DS		1
CR_DTMDAT1_L:	DS		1
CR_DTMDAT2_H:	DS		1
CR_DTMDAT2_L:	DS		1
CR_DTMDAT3_H:	DS		1
CR_DTMDAT3_L:	DS		1
CR_DTMDAT4_H:	DS		1
CR_DTMDAT4_L:	DS		1


	END