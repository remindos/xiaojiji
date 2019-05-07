/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_resident_var_ext.inc
Creator: zj.zong					Date: 2009-04-10
Description: extern pm1 resident variable for "*.asm"
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
			1. build this module
********************************************************************************/
#ifndef UI_RESIDENT_VAR_EXT_ASM
#define UI_RESIDENT_VAR_EXT_ASM


/**************************key var*****************************/
EXTRN	IDATA	(uc_100us_timer_cnt);20080807
EXTRN	IDATA	(uc_us_timer_cnt)
EXTRN	BIT 	(key0_ir1)
EXTRN	BIT 	(ir_bit)
EXTRN	DATA	(ir_state)
EXTRN	DATA	(ir_state_cnt)
EXTRN	DATA	(ir_data_cnt)
EXTRN	IDATA	(bit_cnt)
EXTRN	IDATA	(ir_data_buf)
EXTRN	BIT		(b_key_row)

EXTRN	IDATA	(uc_200us_timer_cnt) ; jason Insert Cunter
EXTRN	BIT		(b_led_pwm_flag)
EXTRN	BIT		(b_led_flg)
EXTRN	BIT		(b_led_pa3)
EXTRN	BIT		(b_led_pa4)
EXTRN	BIT		(b_led_pa5)
EXTRN	BIT		(b_led_pa6)
EXTRN	BIT		(b_bat_low_flag)


EXTRN	BIT		(b_key_hold_val)
EXTRN	BIT		(b_play_poweron_flg)
EXTRN	BIT		(b_io_adc_scan_flg)
EXTRN	BIT		(b_sd_wppin_flg)

EXTRN	DATA	(uc_key_cnt)
EXTRN	DATA	(key_vol_offset)
EXTRN	IDATA	(uc_key_value)
EXTRN	IDATA	(uc_key_release_cnt)
EXTRN	IDATA	(uc_key_old_value)
EXTRN	IDATA	(uc_key_voltage_table)
EXTRN	IDATA	(uc_timer_cnt)
EXTRN	IDATA	(uc_usb_timer_cnt1)
EXTRN	IDATA	(uc_usb_timer_cnt2)
EXTRN	XDATA	(uc_key_val_table)


/**************************lcd var*****************************/
EXTRN	BIT		(b_unicode_flg)
EXTRN	BIT		(b_lcd_clrall_flg)
EXTRN	BIT		(b_lcd_reverse_flg)
EXTRN	XDATA	(uc_pic_height)
EXTRN	IDATA	(uc_window_no)
EXTRN	DATA	(uc_lcd_page_num)
EXTRN	DATA	(uc_lcd_column_num)
EXTRN	IDATA	(uc_lcd_dis_flg)
EXTRN	IDATA	(uc_pic_high_cnt)
EXTRN	XDATA	(uc_lcd_column_offset)
EXTRN	XDATA	(uc_lcd_width)
EXTRN	XDATA	(uc_lcd_height)
EXTRN	XDATA	(uc_lcd_contrast_base)
EXTRN	XDATA	(uc_lcd_contrast_data)
EXTRN	XDATA	(uc_page_table)
EXTRN	XDATA	(uc_lcd_type)

/************************pc stack var**************************/
EXTRN	IDATA	(STACK)
/**********************adc&vol detect var**********************/
EXTRN	BIT		(b_bat_full_flg)
EXTRN	IDATA	(uc_adc_bat_data)
EXTRN	XDATA	(uc_bat_level)
EXTRN	XDATA	(uc_bat_level_bk)
EXTRN	XDATA	(uc_bat_detec_valid_cnt)
EXTRN	XDATA	(uc_bat_valid_cnt)
EXTRN	XDATA	(uc_bat_charge_idx)
EXTRN	XDATA	(uc_bat_low_cnt)
EXTRN	XDATA	(uc_bat_detect_time)
/***************************usb var***************************/
EXTRN	BIT		(b_usb_poweron)
EXTRN	BIT		(b_usb_is_on)
/***************************msi var***************************/
EXTRN	BIT		(b_msi_plug_flg)
EXTRN	BIT		(b_sd_flash_flg)
EXTRN	BIT		(b_sd_exist_flg)
EXTRN	DATA	(uc_msi_status)

/**********************dynamic load code**********************/
EXTRN	BIT		(b_load_code_flg)
EXTRN	DATA	(uc_load_code_length)
EXTRN	IDATA	(uc_load_rpt_cnt)
EXTRN	IDATA	(uc_load_temp_buf)
EXTRN	XDATA	(uc_load_code_idx)


/***************************Others****************************/
EXTRN	DATA	(ui_sys_event)
EXTRN	XDATA	(uc_user_option1)
EXTRN	XDATA	(uc_user_option2)
EXTRN	XDATA	(uc_user_option3)
EXTRN	XDATA	(uc_reserve_addr0)
EXTRN	XDATA	(uc_reserve_addr1)
EXTRN	XDATA	(uc_reserve_addr2)
EXTRN	XDATA	(uc_reserve_addr3)
EXTRN	XDATA	(uc_poweron_pic_num)
EXTRN	XDATA	(uc_poweroff_pic_num)
EXTRN	XDATA	(uc_language_idx)

EXTRN	XDATA	(uc_save_mode_idx)
EXTRN	XDATA	(uc_bklight_time_level)
EXTRN	XDATA	(ui_bklight_time)
EXTRN	XDATA	(uc_bklight_grade)
EXTRN	XDATA	(uc_idle_poweroff_level)
EXTRN	XDATA	(ui_idle_poweroff_time)
EXTRN	XDATA	(ui_idle_poweroff_cnt)
EXTRN	XDATA	(uc_sleep_poweroff_level)
EXTRN	XDATA	(ui_sleep_poweroff_time)
EXTRN	XDATA	(uc_poweroff_type)
EXTRN	XDATA	(ui_sector_byte_offset)

/******************** for sfc var ********************/
EXTRN	BIT		(sfc_reserved_area_flag)

//EXTRN	DATA	(sfc_status)
/*
EXTRN	DATA	(sfc_zoneaddr)
EXTRN	DATA	(sfc_phyblkaddr)
EXTRN	DATA	(sfc_pageaddr)
*/
EXTRN	DATA	(r3_temp)
EXTRN	DATA	(r2_temp_h)
EXTRN	DATA	(r2_temp)
EXTRN	DATA	(r1_temp)

EXTRN	DATA	(sfc_occ_buf_flag)
EXTRN	DATA	(buf_offset0)
EXTRN	DATA	(buf_offset1)
EXTRN	DATA	(sfc_logblkaddr0)
EXTRN	DATA	(sfc_logblkaddr1)
EXTRN	DATA	(sfc_logblkaddr2)
EXTRN	DATA	(sfc_logblkaddr3)
EXTRN	DATA	(sfc_errcode)

/**************** for sfc interface var **************/
EXTRN	BIT		(b_sfc_immediate_wr_flg)
EXTRN	XDATA	(uc_sfc_wrpage_addr0)
EXTRN	XDATA	(uc_sfc_wrpage_addr1)
EXTRN	XDATA	(uc_sfc_wrpage_addr2)
EXTRN	XDATA	(uc_sfc_wrpage_addr3)
EXTRN	XDATA	(uc_sfc_wrpage_flg0)
EXTRN	XDATA	(uc_sfc_wrpage_flg1)
EXTRN	XDATA	(uc_sfc_page_flag_val)
EXTRN	XDATA	(uc_sfc_bk_logblkaddr0)
EXTRN	XDATA	(uc_sfc_bk_logblkaddr1)
EXTRN	XDATA	(uc_sfc_bk_logblkaddr2)
EXTRN	XDATA	(uc_sfc_bk_logblkaddr3)
EXTRN	XDATA	(uc_sfc_bak_occ_buf_flg)
EXTRN	XDATA	(uc_sfc_bakbuf_offset0)
EXTRN	XDATA	(uc_sfc_bakbuf_offset1)

EXTRN	XDATA	(sfc_logblkaddr0_bk)
EXTRN	XDATA	(sfc_logblkaddr1_bk)
EXTRN	XDATA	(sfc_logblkaddr2_bk)

EXTRN	DATA	(r4_temp)
EXTRN	DATA	(r5_temp)
EXTRN	DATA	(r6_temp)
;EXTRN	DATA	(r7_temp)
EXTRN	DATA	(ef_type)
EXTRN	DATA	(page_flag0)
EXTRN	DATA	(page_flag1)

/*************************dbuf&cbuf**************************/
EXTRN	XDATA	(ui_cbuf)
EXTRN	XDATA	(uc_cbuf)
EXTRN	XDATA	(ui_dbuf0)
EXTRN	XDATA	(uc_dbuf0)
EXTRN	XDATA	(ui_dbuf1)
EXTRN	XDATA	(uc_dbuf1)
EXTRN	XDATA	(ui_sfc_combuf)
EXTRN	XDATA	(uc_sfc_combuf)




#endif