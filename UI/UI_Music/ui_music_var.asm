/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: music_var.asm
Creator: zj.zong				Date: 2009-04-29
Description: define variable be used in music mode
Others: 
Version: V0.1
History:
	V0.1	2009-04-29		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_MUSIC_VAR_DEF

/*****************************************/
#ifndef UI_MUSIC_VAR
#define UI_MUSIC_VAR

PUBLIC	b_id3_overflow_flg
PUBLIC	b_id3_artist_flg
PUBLIC	b_id3_album_flg
PUBLIC	b_id3_title_flg
PUBLIC	b_lrc_labelready
PUBLIC	b_lrc_songgoing
PUBLIC	b_lrc_label_overflow
PUBLIC	b_lrc_rd_error
PUBLIC	b_lrc_valid_label
PUBLIC	b_lrc_file_exist
PUBLIC	b_dac_ws_flg
PUBLIC	b_mpeg_init_ok
PUBLIC	b_mpeg_dec5s_ok
PUBLIC	b_mp3_file_err
PUBLIC	b_mp3_file_end
PUBLIC	b_mp3_lrc_flg
PUBLIC	b_mp3_dis_ab_flicker
PUBLIC	b_mp3_dis_volume_flicker
PUBLIC	b_mp3_save_flg
PUBLIC	b_mp3_get_next_file
PUBLIC	b_mp3_next_last_flg
PUBLIC	b_mp3_replay_start
PUBLIC	b_mp3_replay_mode
PUBLIC	b_mp3_scroll_long_name
PUBLIC	b_mp3_continue_play
PUBLIC	b_fb_nofile_flg
PUBLIC	b_mp3_dec_reset_flg
PUBLIC	b_fs_subdir_pt_flg
PUBLIC	b_browser_fir_getname_flg
PUBLIC  b_mp3_dis_freq_flg
PUBLIC  b_cur_list_no_dir_file
PUBLIC	b_mp3_restore_file_flg
PUBLIC	b_lcd_dis_file_err
PUBLIC	b_fs_subdir_stk_pt
PUBLIC	b_headphone_plug_flg
PUBLIC	b_mp3_startplay_flg
;PUBLIC	b_sd_flash_flg


PUBLIC	uc_mp3_fill_dbuf_flg
PUBLIC	uc_mpeg_state
PUBLIC	ul_mp3_replay_a_byte
PUBLIC	ui_mp3_replay_a_time
PUBLIC	ui_mp3_replay_b_time
PUBLIC	uc_mp3_replay_ab_flg
PUBLIC	uc_menu_cur_idx
PUBLIC	uc_menu_cur_idx_bk

PUBLIC	uc_menu_title_idx
PUBLIC	uc_menu_total_num
PUBLIC	uc_menu_exit_cnt
PUBLIC	ui_mp3_cur_time
PUBLIC	ui_mp3_played_time
PUBLIC	ui_mp3_total_time
PUBLIC	uc_mp3_eq_mode
PUBLIC	uc_mp3_repeat_mode
PUBLIC	uc_mp3_replay_time
PUBLIC	uc_mp3_replay_gap
PUBLIC	uc_mp3_replay_time_cnt
PUBLIC	uc_mp3_replay_gap_cnt
PUBLIC	uc_mp3_volume_level
PUBLIC	uc_mp3_bitrate_idx
PUBLIC	ui_mp3_bitrate
PUBLIC	ui_mp3_idx
PUBLIC	ui_mp3_idx_indir
PUBLIC	ui_fb_total_num
PUBLIC	uc_menu_top_idx
PUBLIC	uc_mp3_refresh_freq_cnt
PUBLIC	uc_refresh_pic_num
PUBLIC	uc_headphone_plug_cnt
PUBLIC	uc_headphone_unplug_cnt

PUBLIC	uc_mp3_status
PUBLIC	uc_mp3_status_bk
PUBLIC	ui_mp3_random_idx_bk
PUBLIC	ui_fb_sel_item_idx
PUBLIC	ui_fb_top_item_idx
PUBLIC	ui_fb_cur_item_idx
PUBLIC	uc_dynamicload_idx
PUBLIC	uc_fb_tmp_dir_max_item
PUBLIC	uc_fb_dis_cnt
PUBLIC	uc_fb_sel_item_type
PUBLIC	uc_mp3_time_cnt
PUBLIC	uc_mp3_ab_fliker_cnt
PUBLIC	uc_mp3_refresh_id3_cnt
PUBLIC	uc_mp3_refresh_lrc_cnt
PUBLIC	ul_mp3_vbr_total_frame
PUBLIC	ul_mp3_byte_offset_bk
PUBLIC	uc_mp3_dis_offset
PUBLIC	ui_mp3_tmp_var
PUBLIC	uc_mp3_startplay_cnt

PUBLIC	uc_id3_length
PUBLIC	uc_id3_artist_length
PUBLIC	uc_id3_title_length
PUBLIC	uc_id3_album_length
PUBLIC	uc_id3_version
PUBLIC	uc_id3_temp_input
PUBLIC	ui_id3_byte_offset
PUBLIC	ul_id3_size
PUBLIC	ul_id3_tag_size
PUBLIC	ul_id3_tmp_tag_size
PUBLIC	uc_id3_tmp_length
PUBLIC	uc_id3_tmp_var1
PUBLIC	uc_id3_tmp_var2
PUBLIC	ui_id3_tmp_offset
PUBLIC	uc_id3_artist_buf
PUBLIC	uc_id3_title_buf
PUBLIC	uc_id3_album_buf
PUBLIC	ui_id3_buf
PUBLIC	uc_vbr_decbuf_backup_511
PUBLIC	uc_vbr_offset
PUBLIC	ui_lrc_minute
PUBLIC	ui_lrc_second
PUBLIC	ui_lrc_byte_offset
PUBLIC	ui_lrc_buffer_offset
PUBLIC	uc_lrc_label_idx
PUBLIC	uc_lrc_label_num
PUBLIC	uc_lrc_curlabel_idx
PUBLIC	ui_lrc_value
PUBLIC	uc_lrc_unicode_flg
PUBLIC	ui_lrc_bytes_idx
PUBLIC	ui_lrc_line_buf
PUBLIC	ui_lrc_time_buf
PUBLIC  uc_filldata_cnt
PUBLIC  uc_mpegstate_cnt


PUBLIC	uc_daci_sr
PUBLIC	uc_headphone_short_time

PUBLIC	uc_fs_store_cluster1
PUBLIC	uc_fs_store_cluster2
PUBLIC	uc_fs_store_length1
PUBLIC	uc_fs_store_length2
PUBLIC	b_rsv_mp3_flg
PUBLIC	ui_rsv_mp3_total_sec
PUBLIC	ui_rsv_mp3_sec_cnt
PUBLIC 	ul_rsv_mp3_addr
PUBLIC	ul_rsv_mp3_addr_bk
PUBLIC	ui_rsv_mp3_idx
PUBLIC	ui_rsv_mp3_total_num

PUBLIC	uc_next_action
/*
PUBLIC	uc_rtc_init_flg
PUBLIC	uc_rtc_second
PUBLIC	uc_rtc_minute
PUBLIC	uc_rtc_hour
PUBLIC	uc_rtc_day
PUBLIC	uc_rtc_week
PUBLIC	uc_rtc_month
PUBLIC	uc_rtc_year
PUBLIC	uc_rtc_cnt

PUBLIC	uc_rtc_minute_adj
PUBLIC	uc_rtc_hour_adj
PUBLIC	uc_rtc_day_adj
PUBLIC	uc_rtc_week_adj
PUBLIC	uc_rtc_month_adj
PUBLIC	uc_rtc_year_adj

PUBLIC	uc_rtc_hour_off
PUBLIC	uc_rtc_minute_off
*/
PUBLIC	b_flicker_flg
PUBLIC	uc_flicker_cnt

PUBLIC	b_num_flg
PUBLIC	ui_num_key
PUBLIC	uc_book_idx
PUBLIC	ui_base_idx

PUBLIC	uc_mp3_lrc_offset

PUBLIC	uc_logo_idx
PUBLIC	uc_logo_cnt

PUBLIC	uc_lock_level
PUBLIC	ui_lock_time
PUBLIC	b_lock_flg

PUBLIC	uc_select_idx

PUBLIC	b_mute_flg

PUBLIC	uc_loop_cnt


PUBLIC	ui_root_dir_total
PUBLIC	ui_sub_dir_total
PUBLIC	uc_seldir_flag
PUBLIC	sub_dir_idx
PUBLIC	root_dir_idx
PUBLIC	ui_root_idx
PUBLIC	ui_sub_idx

PUBLIC	b_status_flg
PUBLIC	uc_status_cnt
//PUBLIC	ui_Teset_cnt_number
PUBLIC	uc_sysgetlrc_flag
PUBLIC	uc_sys_Teset_cnt
PUBLIC	uc_sys_leddis_cnt
PUBLIC	uc_sys_led_cunter
PUBLIC	uc_Flashled_flg
PUBLIC	uc_Setled_flg
PUBLIC	ui_Flashled_Count

PUBLIC	uc_pwmr_width
PUBLIC	uc_pwmg_width
PUBLIC	uc_pwmb_width
PUBLIC	uc_pwmw_width
PUBLIC	uc_pwmc_width
PUBLIC	uc_pwm_step
PUBLIC	uc_pwm_cnt
PUBLIC	uc_pwm_mode


   	BSEG    AT      25H.0
;b_sd_flash_flg:		    DBIT    1
	BSEG	AT		29H.0
b_id3_overflow_flg:		DBIT	1
b_id3_artist_flg:		DBIT	1
b_id3_album_flg:		DBIT	1
b_id3_title_flg:		DBIT	1

b_lrc_labelready:		DBIT	1
b_lrc_songgoing:		DBIT	1
b_lrc_label_overflow:	DBIT	1
b_lrc_rd_error:			DBIT	1
b_lrc_valid_label:		DBIT	1
b_lrc_file_exist:		DBIT	1
;b_sd_exist_flg:		    DBIT    1
b_headphone_plug_flg:	DBIT	1

b_dac_ws_flg:			DBIT	1
b_mpeg_init_ok:			DBIT	1
b_mpeg_dec5s_ok:		DBIT	1
b_mp3_file_err:			DBIT	1
b_mp3_file_end:			DBIT	1
b_mp3_lrc_flg:			DBIT	1
b_mp3_dis_ab_flicker:	DBIT	1
b_mp3_dis_volume_flicker:DBIT	1
b_mp3_save_flg:			DBIT	1
b_mp3_get_next_file:;	DBIT	1
b_mp3_next_last_flg:	DBIT	1
b_mp3_replay_start:;		DBIT	1
b_mp3_replay_mode:		DBIT	1
b_fb_nofile_flg:		DBIT	1
b_mp3_scroll_long_name:	DBIT	1
b_mp3_continue_play:	DBIT	1
b_mp3_dec_reset_flg:	DBIT	1
b_mp3_dis_freq_flg:		DBIT	1
b_fs_subdir_pt_flg:			DBIT	1
b_browser_fir_getname_flg:	DBIT	1
b_cur_list_no_dir_file:		DBIT	1
b_mp3_restore_file_flg:		DBIT	1
b_lcd_dis_file_err:		DBIT	1
b_fs_subdir_stk_pt:		DBIT	1
b_mp3_startplay_flg:		DBIT	1
b_rsv_mp3_flg:			DBIT	1

b_flicker_flg:			DBIT	1
b_num_flg:				DBIT	1

b_lock_flg:				DBIT	1

b_mute_flg:				DBIT	1
b_status_flg:			DBIT	1


	DSEG	AT		12H
uc_mp3_fill_dbuf_flg:		DS	1
uc_mp3_status:				DS	1
ui_mp3_cur_time:			DS	2
ui_mp3_total_time:			DS	2
ui_mp3_idx:					DS	2
ui_mp3_idx_indir:			DS	2
uc_daci_sr:					DS	1
uc_mp3_bitrate_idx:			DS	1
uc_mpeg_state:				DS	1

	DSEG	AT		1FH
uc_menu_total_num:			DS	1
	DSEG	AT		2EH
ui_mp3_tmp_var:				DS	2
	


 	DSEG	AT		7BH
uc_mp3_dis_offset:			DS	1
uc_mp3_volume_level:		DS	1
uc_menu_cur_idx:			DS	1



	XSEG	AT		6010H

uc_mp3_status_bk:			DS	1
ul_mp3_vbr_total_frame:		DS	4
ul_mp3_replay_a_byte:		DS	4
ui_mp3_played_time:			DS	2
ui_mp3_replay_a_time:		DS	2
ui_mp3_replay_b_time:		DS	2
uc_mp3_replay_ab_flg:		DS	1		;0-normal, 1:set A, 2:set B, 3:A-B

ui_fb_total_num:			DS	2
uc_menu_cur_idx_bk:			DS	1
uc_menu_top_idx:			DS	1

uc_menu_title_idx:			DS	1
uc_mp3_refresh_freq_cnt:	DS	1
uc_refresh_pic_num:			DS	1

uc_headphone_plug_cnt:		DS	1
uc_headphone_unplug_cnt:	DS	1
uc_menu_exit_cnt:			DS	1
uc_mp3_refresh_id3_cnt:		DS  1
uc_mp3_refresh_lrc_cnt:		DS	1
uc_mp3_eq_mode:				DS	1
uc_mp3_repeat_mode:			DS	1
uc_mp3_replay_time:			DS	1
uc_mp3_replay_gap:			DS	1
uc_mp3_replay_time_cnt:		DS	1
uc_mp3_replay_gap_cnt:		DS	1
ui_mp3_bitrate:				DS	2
uc_mp3_time_cnt:			DS	1
uc_mp3_ab_fliker_cnt:		DS	1
uc_filldata_cnt:			DS	1
uc_mpegstate_cnt:			DS	1

ul_mp3_byte_offset_bk:		DS	4
uc_mp3_startplay_cnt:			DS	1
uc_fs_store_length1:		DS		1
uc_fs_store_length2:		DS		1
uc_fs_store_cluster1:		DS		1
uc_fs_store_cluster2:		DS		1

ui_rsv_mp3_total_sec:		DS		2
ui_rsv_mp3_sec_cnt:			DS		2
ul_rsv_mp3_addr:			DS		4
ul_rsv_mp3_addr_bk:			DS		4
ui_rsv_mp3_idx:				DS		2
ui_rsv_mp3_total_num:		DS		2
uc_next_action:				DS	1
/*
uc_rtc_init_flg:			DS	1

uc_rtc_second:				DS	1
uc_rtc_minute:				DS	1
uc_rtc_hour:				DS	1
uc_rtc_day:					DS	1
uc_rtc_week:				DS	1
uc_rtc_month:				DS	1
uc_rtc_year:				DS	1
uc_rtc_cnt:					DS	1

uc_rtc_minute_adj:				DS	1
uc_rtc_hour_adj:				DS	1
uc_rtc_day_adj:					DS	1
uc_rtc_week_adj:				DS	1
uc_rtc_month_adj:				DS	1
uc_rtc_year_adj:				DS	1

uc_rtc_hour_off:				DS	1
uc_rtc_minute_off:				DS	1
*/
uc_flicker_cnt:					DS	1

ui_num_key:						DS	1

uc_book_idx:					DS	1
ui_base_idx:					DS	2

uc_mp3_lrc_offset:				DS	1

uc_logo_idx:				DS	1
uc_logo_cnt:				DS	1

uc_lock_level:				DS	1
uI_lock_time:				DS	2

uc_select_idx:				DS	1
uc_loop_cnt:				DS	1

ui_root_dir_total:			DS	2
ui_sub_dir_total:			DS	2
uc_seldir_flag:				DS	1
sub_dir_idx:				DS	2
root_dir_idx:				DS	2
ui_root_idx:				DS	2
ui_sub_idx:					DS	2

uc_status_cnt:				DS	1



	XSEG	AT		7200H


uc_id3_length:				DS	1
ui_id3_buf:					DS	200
uc_lrc_curlabel_idx:		DS	1
uc_lrc_label_num:			DS	1
ui_lrc_time_buf:			DS	100

uc_headphone_short_time:	DS	1
ui_mp3_random_idx_bk:		DS	2

uc_fb_sel_item_type:		DS	1
ul_id3_size:				DS	4

ui_lrc_value:
ui_id3_byte_offset:			DS	2



/*xdata memory overlap						  */
ui_lrc_line_buf:			/*DS	140 XDATA Overlap */
uc_id3_artist_buf:			DS	30
uc_id3_title_buf:			DS	30
uc_id3_album_buf:			DS	30
ul_id3_tag_size:			DS	4
//ui_id3_byte_offset:		DS	2
uc_id3_temp_input:			DS	1
uc_id3_version:				DS	1
uc_id3_artist_length:		DS	1
uc_id3_title_length:		DS	1
uc_id3_album_length:		DS	1
ul_id3_tmp_tag_size:		DS	4
uc_id3_tmp_length:			DS	1
uc_id3_tmp_var1:			DS	1
uc_id3_tmp_var2:			DS	1
ui_id3_tmp_offset:			DS	2
uc_vbr_decbuf_backup_511:	DS	1
uc_vbr_offset:				DS	1
ui_lrc_minute:				DS	2
ui_lrc_second:				DS	2
ui_lrc_byte_offset:			DS	2
ui_lrc_buffer_offset:		DS	2
uc_lrc_label_idx:			DS	1
uc_lrc_unicode_flg:			DS	1
ui_lrc_bytes_idx:
uc_dynamicload_idx:			DS	1
ui_fb_sel_item_idx:			DS	2
ui_fb_top_item_idx:			DS	2
ui_fb_cur_item_idx:			DS	2
uc_fb_tmp_dir_max_item:		DS	1
uc_fb_dis_cnt:				DS	1
//ui_Teset_cnt_number:		DS  2
uc_sysgetlrc_flag:			DS	1
uc_sys_Teset_cnt:			DS	1
uc_sys_leddis_cnt:			DS	1
uc_sys_led_cunter:			DS	1
uc_Flashled_flg:			DS	1
uc_Setled_flg:				DS	1
ui_Flashled_Count:			DS	2

uc_pwmr_width:				DS	1
uc_pwmg_width:				DS	1
uc_pwmb_width:				DS	1
uc_pwmw_width:				DS	1
uc_pwmc_width:				DS	1
uc_pwm_step:				DS	1
uc_pwm_cnt:					DS	1
uc_pwm_mode:				DS	1

#endif
#endif
	END
