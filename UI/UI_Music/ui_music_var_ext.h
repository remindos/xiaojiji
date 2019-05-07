/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_music_var_ext.h
Creator:zj.zong					Date: 2009-04-29
Description: extern music variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2009-04-29		zj.zong
			1. build this module
********************************************************************************/
#ifndef UI_MUSIC_VAR_EXT
#define UI_MUSIC_VAR_EXT

extern uc8 xdata PartitionEntryNo;
extern bit b_id3_overflow_flg;
extern bit b_id3_artist_flg;
extern bit b_id3_album_flg;
extern bit b_id3_title_flg;
extern bit b_lrc_labelready;
extern bit b_lrc_songgoing;
extern bit b_lrc_label_overflow;
extern bit b_lrc_rd_error;
extern bit b_lrc_valid_label;
extern bit b_lrc_file_exist;
extern bit b_dac_ws_flg;
extern bit b_mpeg_init_ok;
extern bit b_mpeg_dec5s_ok;
extern bit b_mp3_file_err;
extern bit b_mp3_file_end;
extern bit b_mp3_lrc_flg;
extern bit b_mp3_dis_ab_flicker;
extern bit b_mp3_dis_volume_flicker;
extern bit b_mp3_save_flg;
extern bit b_mp3_get_next_file;
extern bit b_mp3_next_last_flg;
extern bit b_mp3_replay_start;
extern bit b_mp3_replay_mode;
extern bit b_mp3_scroll_long_name;
extern bit b_mp3_continue_play;
extern bit b_fb_nofile_flg;
extern bit b_mp3_dec_reset_flg;
extern bit b_browser_fir_getname_flg;
extern bit b_mp3_dis_freq_flg;
extern bit b_cur_list_no_dir_file;
extern bit b_fs_subdir_pt_flg;
extern bit b_mp3_restore_file_flg;
extern bit b_lcd_dis_file_err;
extern bit b_fs_subdir_stk_pt;
extern bit b_headphone_plug_flg;
extern bit b_mp3_startplay_flg;
extern bit b_sd_exist_flg;
extern bit b_mp3_startplay_flg;

extern uc8   data uc_mp3_fill_dbuf_flg;
extern uc8   data uc_mp3_status;
extern uc8   data uc_menu_cur_idx;
extern uc8   data uc_menu_total_num;
extern ui16  data ui_mp3_cur_time;
extern ui16  data ui_mp3_total_time;
extern uc8   data uc_mpeg_state;
extern ui16  data ui_mp3_idx;
extern ui16  data ui_mp3_idx_indir;
extern ui16 xdata ui_fb_total_num;
extern uc8  xdata uc_mp3_status_bk;
extern ui16 xdata ui_mp3_random_idx_bk;

extern ul32 xdata ul_mp3_replay_a_byte;
extern ui16 xdata ui_mp3_replay_a_time;
extern ui16 xdata ui_mp3_replay_b_time;
extern uc8  xdata uc_mp3_replay_ab_flg;
extern uc8  xdata uc_menu_cur_idx_bk;
extern uc8  xdata uc_menu_title_idx;
extern uc8  xdata uc_menu_exit_cnt;
extern ui16 xdata ui_mp3_played_time;
extern uc8  xdata uc_mp3_eq_mode;
extern uc8  xdata uc_mp3_repeat_mode;
extern uc8  xdata uc_mp3_replay_time;
extern uc8  xdata uc_mp3_replay_gap;
extern uc8  xdata uc_mp3_replay_time_cnt;
extern uc8  xdata uc_mp3_replay_gap_cnt;
extern uc8  data uc_mp3_volume_level;
extern uc8  data uc_mp3_bitrate_idx;
extern ui16 xdata ui_mp3_bitrate;
extern ui16 xdata ui_fb_sel_item_idx;
extern ui16 xdata ui_fb_top_item_idx;
extern ui16 xdata ui_fb_cur_item_idx;
extern uc8  xdata uc_dynamicload_idx;
extern uc8  xdata uc_fb_tmp_dir_max_item;
extern uc8  xdata uc_fb_dis_cnt;
extern uc8  xdata uc_menu_top_idx;
extern uc8  xdata uc_mp3_refresh_freq_cnt;
extern uc8  xdata uc_refresh_pic_num;

extern uc8  xdata uc_mp3_time_cnt;
extern uc8  xdata uc_mp3_ab_fliker_cnt;
extern uc8  xdata uc_mp3_refresh_id3_cnt;
extern uc8  xdata uc_mp3_refresh_lrc_cnt;
extern ul32 xdata ul_mp3_vbr_total_frame;
extern ul32 xdata ul_mp3_byte_offset_bk;
extern uc8  data uc_mp3_dis_offset;
extern ui16  data ui_mp3_tmp_var;
extern uc8  xdata uc_fb_sel_item_type;
extern uc8	xdata uc_filldata_cnt;
extern uc8	xdata uc_mpegstate_cnt;

extern uc8  xdata uc_id3_length;
extern uc8  xdata uc_id3_artist_length;
extern uc8  xdata uc_id3_title_length;
extern uc8  xdata uc_id3_album_length;
extern uc8  xdata uc_id3_version;
extern uc8  xdata uc_id3_temp_input;
extern ui16 xdata ui_id3_byte_offset;
extern ul32 xdata ul_id3_size;
extern ul32 xdata ul_id3_tag_size;
extern ul32 xdata ul_id3_tmp_tag_size;
extern uc8  xdata uc_id3_tmp_length;
extern uc8  xdata uc_id3_tmp_var1;
extern uc8  xdata uc_id3_tmp_var2;
extern ui16 xdata ui_id3_tmp_offset;
extern uc8  xdata uc_id3_artist_buf[];
extern uc8  xdata uc_id3_title_buf[];
extern uc8  xdata uc_id3_album_buf[];
extern ui16 xdata ui_id3_buf[];
extern uc8  xdata uc_vbr_decbuf_backup_511;
extern uc8  xdata uc_vbr_offset;
extern ui16 xdata ui_lrc_minute;
extern ui16 xdata ui_lrc_second;
extern ui16 xdata ui_lrc_byte_offset;
extern ui16 xdata ui_lrc_buffer_offset;
extern uc8  xdata uc_lrc_label_idx;
extern uc8  xdata uc_lrc_label_num;
extern uc8  xdata uc_lrc_curlabel_idx;
extern ui16 xdata ui_lrc_value;
extern uc8  xdata uc_lrc_unicode_flg;
extern ui16 xdata ui_lrc_bytes_idx;
extern ui16 xdata ui_lrc_line_buf[];
extern ui16 xdata ui_lrc_time_buf[];

extern uc8  data uc_daci_sr;
extern uc8  xdata uc_headphone_short_time;
extern uc8  xdata uc_headphone_plug_cnt;
extern uc8  xdata uc_headphone_unplug_cnt;
extern uc8  xdata uc_mp3_startplay_cnt;

/**********fs*************/
extern uc8  xdata uc_fs_store_length1;
extern uc8  xdata uc_fs_store_length2;
extern uc8  xdata uc_fs_store_cluster1;
extern uc8  xdata uc_fs_store_cluster2;
extern uc8   data uc_fs_begin_cluster1;
extern uc8   data uc_fs_begin_cluster2;
extern uc8   data uc_fs_begin_cluster3;

extern uc8  xdata uc_fs_item_83_name[];
extern ui16 xdata ui_fs_item_name[];
extern uc8  xdata uc_fs_item_name_length;
extern ul32 idata ul_fs_file_length;
extern ul32  data ul_fs_byte_offset;

extern ui16 xdata ui_fs_special_file_num_indir;
extern ui16 xdata ui_fs_file_num_indir;
extern ui16  data ui_fs_subdir_num;
extern ui16  data ui_fs_item_num;
extern ui16 xdata ui_fs_dir_path_buf[];

extern si16 xdata si_fs_file_idx;

extern uc8   data uc_fs_subdir_stk_pt;
extern uc8  xdata uc_fs_store_subdir_stk_pt;

extern ui16 xdata ui_fs_file_type;
extern uc8  xdata uc_fs_assign_buf_flg;
extern uc8  xdata uc_fs_item_type;

extern bit b_rsv_mp3_flg;
extern ui16 xdata ui_rsv_mp3_sec_cnt;
extern ui16 xdata ui_rsv_mp3_total_sec;
extern ul32 xdata ul_rsv_mp3_addr;
extern ul32 xdata ul_rsv_mp3_addr_bk;
extern ui16 xdata ui_rsv_mp3_idx;
extern ui16 xdata ui_rsv_mp3_total_num;

extern uc8 xdata uc_next_action;

extern uc8 xdata	uc_rtc_init_flg;
extern uc8 xdata	uc_rtc_second;
extern uc8 xdata	uc_rtc_minute;
extern uc8 xdata	uc_rtc_hour;
extern uc8 xdata	uc_rtc_day;
extern uc8 xdata	uc_rtc_week;
extern uc8 xdata	uc_rtc_month;
extern uc8 xdata	uc_rtc_year;
extern uc8 xdata	uc_rtc_cnt;

extern uc8 xdata	uc_rtc_minute_adj;
extern uc8 xdata	uc_rtc_hour_adj;
extern uc8 xdata	uc_rtc_day_adj;
extern uc8 xdata	uc_rtc_week_adj;
extern uc8 xdata	uc_rtc_month_adj;
extern uc8 xdata	uc_rtc_year_adj;

extern uc8 xdata 	uc_rtc_hour_off;
extern uc8 xdata	uc_rtc_minute_off;

extern bit b_flicker_flg;
extern uc8 xdata uc_flicker_cnt;

extern bit b_num_flg;
extern ui16 xdata ui_num_key;
extern uc8 xdata uc_book_idx;
extern ui16 xdata ui_base_idx;

extern uc8 xdata uc_mp3_lrc_offset;

extern uc8 xdata uc_logo_idx;
extern uc8 xdata uc_logo_cnt;

extern uc8 xdata uc_lock_level;
extern ui16 xdata ui_lock_time;
extern bit b_lock_flg;

extern uc8 xdata uc_select_idx;

extern bit b_mute_flg;

extern uc8 xdata uc_loop_cnt;

extern ui16 xdata ui_root_dir_total;
extern ui16 xdata ui_sub_dir_total;

extern ui16 xdata sub_dir_idx;
extern ui16 xdata root_dir_idx;
extern uc8	xdata uc_seldir_flag;
extern ui16 xdata ui_root_idx;
extern ui16 xdata ui_sub_idx;

extern uc8 xdata uc_status_cnt;
extern bit b_status_flg;
//extern ui16 xdata ui_Teset_cnt_number;
extern uc8 xdata uc_sysgetlrc_flag;
extern uc8 xdata uc_sys_Teset_cnt;
extern uc8 xdata uc_sys_leddis_cnt;
extern uc8 xdata uc_sys_led_cunter;
extern uc8 xdata uc_Flashled_flg;
extern uc8 xdata uc_Setled_flg;
extern ui16 xdata ui_Flashled_Count;

extern uc8 idata uc_200us_timer_cnt;
extern uc8 xdata uc_pwmr_width;
extern uc8 xdata uc_pwmg_width;
extern uc8 xdata uc_pwmb_width;
extern uc8 xdata uc_pwmw_width;
extern uc8 xdata uc_pwmc_width;
extern uc8 xdata uc_pwm_step;
extern uc8 xdata uc_pwm_cnt;
extern uc8 xdata uc_pwm_mode;

extern bit b_led_pwm_flag;
extern bit b_led_flg;
extern bit b_led_pa3;
extern bit b_led_pa4;
extern bit b_led_pa5;
extern bit b_led_pa6;
extern bit b_bat_low_flag;

#endif