/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: voice_var_ext.h
Creator: zj.zong					Date: 2007-12-18
Description: extern voice variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2007-12-18		zj.zong
			1. build this module
********************************************************************************/
#ifndef VOICE_VAR_EXT
#define VOICE_VAR_EXT

extern bit b_mpeg_init_ok;
extern bit b_voice_get_next_file;
extern bit b_voice_set_volume;
extern bit b_voice_dis_ab_flicker;
extern bit b_voice_dec_reset_flg;
extern bit b_dac_mute_flg;
extern bit b_voice_check_time_flg;
extern bit b_voice_file_end;
extern bit b_voice_file_err;								    
extern bit b_voice_folder_exist;
extern bit b_headphone_plug_flg;
extern bit b_voice_startplay_flg;
extern bit b_sd_exist_flg;
extern bit b_fb_nofile_flg;

extern uc8 data uc_voice_fill_dbuf_flg;
extern uc8 data uc_voice_status;
extern uc8 data uc_mpeg_state;
extern uc8 data uc_menu_cur_idx;
extern uc8 data uc_menu_total_num;
extern uc8 data uc_voice_volume_level;
extern uc8  data uc_daci_sr;

extern ui16	data ui_voice_cur_time;
extern ui16 data ui_voice_total_time;
extern ui16 data ui_fb_total_num;
extern ui16 data ui_voice_wav_idx;

extern uc8  xdata uc_voice_repeat_mode;
extern uc8  xdata uc_voice_status_bk;
extern uc8  xdata uc_menu_cur_idx_bk;
extern uc8  xdata uc_refresh_pic_num;
extern uc8  xdata uc_menu_top_idx;
extern uc8  xdata uc_mpegstate_cnt;
extern uc8  xdata uc_filldata_cnt;

extern uc8  xdata uc_refresh_voice_cnt;
extern uc8  xdata uc_voice_ab_fliker_cnt;
extern uc8  xdata uc_menu_exit_cnt;
extern uc8  xdata uc_voice_time_cnt;
extern uc8  xdata uc_voice_replay_ab_flg;
extern uc8	xdata uc_dynamicload_idx;
extern uc8  xdata uc_headphone_short_time;
extern uc8  xdata uc_headphone_plug_cnt;
extern uc8  xdata uc_headphone_unplug_cnt;
extern uc8  xdata uc_voice_startplay_cnt;

extern ui16 xdata ui_voice_replay_a_time;
extern ui16 xdata ui_voice_replay_b_time;
extern ui16 xdata ui_voice_audioformat;
extern ui16 xdata ui_voice_temp_var;
extern ui16 xdata ui_voice_random_idx_bk;
extern ui16 xdata ui_voice_dir_idx;

extern ul32 xdata ul_voice_replay_a_byte;
extern ul32 xdata ul_voice_byterate;
extern ul32 xdata ul_voice_wavdata_len;
extern ul32 xdata ul_voice_samplerate;
extern uc8  xdata uc_mp3_startplay_cnt;

/*************file system***************/
extern uc8 data uc_fs_begin_cluster2;
extern uc8 data uc_fs_begin_cluster1;

extern ui16 data ui_fs_item_num;
extern ui16 data ui_fs_subdir_num;

extern ul32 data ul_fs_byte_offset;

extern uc8  xdata uc_fs_store_length1;
extern uc8  xdata uc_fs_store_length2;
extern uc8  xdata uc_fs_store_cluster1;
extern uc8  xdata uc_fs_store_cluster2;

extern ul32 idata ul_fs_file_length;

extern uc8  xdata uc_fs_item_name_length;
extern uc8  xdata uc_fs_item_83_name[];
extern uc8  xdata uc_fs_assign_buf_flg;
 
extern ui16 xdata ui_fs_item_name[];
extern ui16 xdata ui_fs_file_num_indir;
extern ui16 xdata ui_fs_wav_dir_idx;
extern ui16 xdata ui_fs_special_file_num_indir;
extern ui16 xdata ui_fs_file_type;

extern si16 xdata si_fs_file_idx;

#endif