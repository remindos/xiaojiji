/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: record_var_ext.h
Creator: zj.zong					Date: 2009-06-09
Description: extern record variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2009-06-09		zj.zong
			1. build this module
********************************************************************************/
#ifndef RECORD_VAR_EXT
#define RECORD_VAR_EXT


extern bit b_rec_card_full_flg;
extern bit b_rec_wav_full_flg;
extern bit b_rec_samplerate_flg;
extern bit b_rec_state_flg;
extern bit b_rec_decbuf_flg;
extern bit b_rec_mode_flg;
extern bit b_rec_adcbuf_full_flg;
extern bit b_rec_have_rec_flg;
extern bit b_flicker_time_flg;
extern bit b_sd_exist_flg;

extern uc8 data uc_enc_step_idx;
extern uc8 data uc_enc_thecode;
extern uc8 data uc_enc_diff_buf[];
extern uc8 data uc_enc_effdiff_buf[];
extern uc8 data uc_enc_sample_expand;
extern uc8 data uc_rec_playkey_cnt_h;
extern uc8 data uc_rec_playkey_cnt_l;
extern uc8 data uc_rec_adcbuf_dph;
extern uc8 data uc_rec_adcbuf_dpl;

extern ui16 data ui_enc_indata;
extern ui16 data ui_enc_sample;
extern ui16 data ui_enc_step_sft1;
extern ui16 data ui_enc_step_sft2;

extern uc8	xdata tmp_h;
extern uc8	xdata tmp_m;
extern uc8	xdata tmp_s;
extern uc8  xdata uc_rec_time_cnt;
extern uc8  xdata uc_rec_sav_map_idx;
extern uc8  xdata uc_rec_sav_map_val;
extern uc8  xdata uc_rec_page_free_sec;
extern uc8  xdata uc_rec_wav_map_buf[];
extern uc8  xdata uc_fs_wav_idx;
extern uc8  xdata  uc_rec_status;
extern uc8  xdata uc_rec_encode_cnt;
extern uc8  xdata uc_refresh_pic_num;
extern uc8  xdata uc_refresh_rec_cnt;
extern uc8  xdata uc_menu_top_idx;
extern uc8  xdata uc_dynamicload_idx; //allen

extern uc8  xdata uc_refresh_time_cnt;
extern uc8  xdata uc_menu_cur_idx;
extern uc8  xdata uc_menu_total_num;
extern uc8  xdata uc_menu_exit_cnt;

extern ui16 xdata ui_rec_dptr_var;
extern ui16 xdata  ui_rec_wav_total_num;
extern ui16 xdata ui_rec_temp_var;
extern si16 xdata si_rec_adc_inbuf[4], si_rec_adc_outbuf[4];

extern ul32 xdata ul_rec_cur_time;
extern ul32 xdata ul_rec_remain_time;
extern ul32 xdata ul_rec_cur_sec_addr;
extern ul32 xdata ul_rec_cur_sec_addr_bk;
extern ul32 xdata ul_rec_next_sec_addr;
extern ul32 xdata ul_rec_temp_var;
extern ul32 xdata ul_rec_sample_total_num;

/**************************fs****************************/

extern ui16 data ui_fs_subdir_num;
extern ul32 data ul_fs_byte_offset;

extern ui16 idata ui_fs_item_num;

extern uc8 xdata uc_fs_assign_buf_flg;
extern uc8 xdata uc_fs_sav_clst1;
extern uc8 xdata uc_fs_sav_clst2;
extern uc8 xdata uc_fs_sav_clst3;
extern uc8 xdata uc_fs_sav_sector_no;
extern uc8 xdata uc_fs_item_83_name[];
extern uc8 xdata uc_fs_cr_bgn_cluster1;
extern uc8 xdata uc_fs_cr_bgn_cluster2;

extern ui16 xdata ui_fs_wav_dir_idx;
extern ui16 xdata ui_fs_special_file_num_indir;
extern ui16 xdata ui_fs_file_type;

extern ul32 xdata ul_fs_spare_space;
extern uc8 data uc_msi_status;

#endif