/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_resident_var_ext.h
Creator: zj.zong					Date: 2009-04-10
Description: extern pm1 resident variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
			1. build this module
********************************************************************************/
#ifndef UI_RESIDENT_VAR_EXT
#define UI_RESIDENT_VAR_EXT

#ifdef API_DETECT_HEADPHONE_PLUG_DEF
	extern uc8	xdata uc_headphone_plug_cnt;
	extern uc8	xdata uc_headphone_unplug_cnt;
	extern bit	b_headphone_plug_flg;
#endif

/**************************key var*****************************/
extern uc8 idata ir_data_buf[];
extern bit key0_ir1;
extern bit b_key_row;
extern bit b_bat_low_flag;

extern bit b_key_hold_val;
extern bit b_play_poweron_flg;
extern bit b_io_adc_scan_flg;
extern bit b_record_key_flag;
extern bit b_bat_changed_flg;
extern bit b_msi_plug_flg;
extern bit b_sd_flash_flg;
extern bit b_sd_wppin_flg;
extern uc8  data uc_key_cnt;
extern uc8  data key_vol_offset;
extern uc8  data uc_msi_status;

extern uc8 idata uc_key_value;
extern uc8 idata uc_key_release_cnt;
extern uc8 idata uc_key_old_value;
extern uc8 idata uc_key_voltage_table[];
extern uc8 idata uc_timer_cnt;
extern uc8 idata uc_usb_timer_cnt1;
extern uc8 idata uc_usb_timer_cnt2;
extern uc8 xdata uc_key_val_table[];

/***********************lcd and dis var*************************/
extern bit b_unicode_flg;
extern bit b_lcd_clrall_flg;
extern bit b_lcd_reverse_flg;
extern uc8   xdata uc_pic_height;
extern uc8   data uc_lcd_column_num;
extern uc8   data uc_lcd_page_num;
extern uc8   idata uc_lcd_dis_flg;
extern uc8 	xdata uc_lcd_page_num_bk;
extern uc8  xdata uc_lcd_column_offset;
extern uc8  xdata uc_lcd_width;
extern uc8  xdata uc_lcd_height;
extern uc8  xdata uc_lcd_contrast_base;
extern uc8  xdata uc_pic_size_high;
extern uc8  xdata uc_pic_size_low;
extern uc8  xdata uc_page_table[];
extern uc8  xdata uc_lcd_contrast_data;
extern uc8  xdata uc_lcd_type;
extern uc8  xdata uc_refresh_menu_num;
extern uc8  xdata uc_refresh_select_flg;

/************************pc stack var**************************/
extern uc8 idata STACK[];

/**********************adc&vol detect var**********************/
extern bit b_bat_full_flg;
extern uc8  idata uc_adc_bat_data;
extern uc8  xdata uc_bat_level;
extern uc8  xdata uc_bat_level_bk;
extern uc8  xdata uc_bat_charge_idx;
extern uc8  xdata uc_bat_detec_valid_cnt;
extern uc8  xdata uc_bat_valid_cnt;
extern uc8  xdata uc_bat_low_cnt;
extern uc8  xdata uc_bat_detect_time;

/************************pc stack var**************************/
extern bit b_usb_poweron;
extern bit b_usb_is_on;
extern uc8   idata uc_window_no;	 
extern ui16  data ui_sys_event;

extern uc8  xdata uc_user_option1;
extern uc8  xdata uc_user_option2;
extern uc8  xdata uc_user_option3;
extern uc8  xdata uc_reserve_addr0;
extern uc8  xdata uc_reserve_addr1;
extern uc8  xdata uc_reserve_addr2;
extern uc8  xdata uc_reserve_addr3;
extern uc8  xdata uc_poweron_pic_num;
extern uc8  xdata uc_poweroff_pic_num;
extern uc8  xdata uc_poweroff_type;
extern uc8  xdata uc_language_idx;

extern uc8  xdata uc_save_mode_idx;
extern uc8  xdata uc_bklight_time_level;
extern uc8  xdata uc_sleep_poweroff_level; 
extern uc8  xdata uc_bklight_grade;
extern uc8  xdata uc_idle_poweroff_level;
extern ui16 xdata ui_idle_poweroff_time;
extern ui16 xdata ui_idle_poweroff_cnt;
extern ui16 xdata ui_sleep_poweroff_time;
extern ui16 xdata ui_bklight_time;
extern ui16 xdata ui_sector_byte_offset;


/**********************dynamic load code**********************/
extern bit b_load_code_flg;
extern uc8  xdata uc_load_code_idx;

/******************** for sfc var ********************/
extern bit	sfc_reserved_area_flag;
extern uc8	data	sfc_occ_buf_flag;
extern uc8	data	buf_offset0;
extern uc8	data	buf_offset1;
extern uc8	data	sfc_logblkaddr0;
extern uc8	data	sfc_logblkaddr1;
extern uc8	data	sfc_logblkaddr2;
extern uc8	data	sfc_logblkaddr3;
extern uc8	data	sfc_errcode;
extern uc8	data	ef_type;

/*
extern uc8	data	sfc_zoneaddr;
extern uc8	data	sfc_phyblkaddr;
extern uc8	data	sfc_pageaddr;
*/
extern uc8	data	r3_temp;
extern uc8	data	r2_temp_h;
extern uc8	data	r2_temp;
extern uc8	data	r1_temp;
extern uc8	idata	max_zone_num_per_chip;


/**************** for sfc interface var **************/
extern bit	b_sfc_immediate_wr_flg;
extern uc8	xdata uc_sfc_wrpage_addr0;	
extern uc8	xdata uc_sfc_wrpage_addr1;	
extern uc8	xdata uc_sfc_wrpage_addr2;	
extern uc8	xdata uc_sfc_wrpage_addr3;	
extern uc8	xdata uc_sfc_wrpage_flg0;	
extern uc8	xdata uc_sfc_wrpage_flg1;	
extern uc8	xdata uc_sfc_page_flag_val;	
extern uc8	xdata uc_sfc_bk_logblkaddr0;	
extern uc8	xdata uc_sfc_bk_logblkaddr1;	
extern uc8	xdata uc_sfc_bk_logblkaddr2;	
extern uc8	xdata uc_sfc_bk_logblkaddr3;	
extern uc8	xdata uc_sfc_bak_occ_buf_flg;	
extern uc8	xdata uc_sfc_bakbuf_offset0;	
extern uc8	xdata uc_sfc_bakbuf_offset1;	

extern uc8  idata uc_disktype_flg;
/*************************dbuf&cbuf**************************/
extern uc8  xdata uc_cbuf[];
extern ui16 xdata ui_cbuf[];  
extern ul32 xdata ul_cbuf[];
extern uc8  xdata uc_dbuf0[];
extern ui16 xdata ui_dbuf0[];
extern ul32 xdata ul_dbuf0[];
extern uc8  xdata uc_dbuf1[];
extern ui16 xdata ui_dbuf1[];
extern ul32 xdata ul_dbuf1[];
extern uc8  xdata uc_sfc_combuf[];
extern ui16 xdata ui_sfc_combuf[];
extern si16 xdata si_dbuf0[];
extern si16 xdata si_dbuf1[];

/************************DAC Register*************************/
extern uc8  xdata MR_DACI;
extern uc8  xdata MR_DACCLK;
extern uc8  xdata MR_DACSRST;
extern uc8  xdata MR_DACPCON;
extern uc8  xdata MR_AVCL;
extern uc8  xdata MR_AVCR;
extern uc8  xdata MR_MVCL;
extern uc8  xdata MR_MVCR;
extern uc8  xdata MR_DACSET;
extern uc8  xdata MR_DACSTA;
extern uc8  xdata MR_DACOPT1;
extern uc8  xdata MR_DACOPT2;
extern uc8  xdata MR_DACOPT3H;
extern uc8  xdata MR_DACOPT3L;
extern uc8  xdata MR_DACOPT4H;
extern uc8  xdata MR_DACOPT4L;
extern uc8  xdata MR_DACOPT5;


/*********************Communication Reg************************/
extern uc8  xdata CR_MCUIRQEN_H;
extern uc8  xdata CR_MCUIRQEN_L;
extern uc8  xdata CR_MPEGIRQEN_H;
extern uc8  xdata CR_MPEGIRQEN_L;
extern uc8  xdata CR_MCUIRQ_H;
extern uc8  xdata CR_MCUIRQ_L;
extern uc8  xdata CR_MPEGIRQ_H;
extern uc8  xdata CR_MPEGIRQ_L;
extern uc8  xdata CR_MTDCMD_H;
extern uc8  xdata CR_MTDCMD_L;
extern uc8  xdata CR_DTMCMD_H;
extern uc8  xdata CR_DTMCMD_L;
extern uc8  xdata CR_MTDDAT1_H;
extern uc8  xdata CR_MTDDAT1_L;
extern uc8  xdata CR_MTDDAT2_H;
extern uc8  xdata CR_MTDDAT2_L;
extern uc8  xdata CR_DTMDAT1_H;
extern uc8  xdata CR_DTMDAT1_L;
extern uc8  xdata CR_DTMDAT2_H;
extern uc8  xdata CR_DTMDAT2_L;
extern uc8  xdata CR_DTMDAT3_H;
extern uc8  xdata CR_DTMDAT3_L;
extern uc8  xdata CR_DTMDAT4_H;
extern uc8  xdata CR_DTMDAT4_L;


#endif