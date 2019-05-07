/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_rec_var.asm
Creator: zj.zong			Date: 2009-06-09
Description: define variable be used in record mode
Others: 
Version: V0.1
History:
	V0.1	2009-06-09		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#include "sh86278_memory_assign.h"
#ifdef UI_RECORD_VAR_DEF

/*****************************************/
#ifndef RECORD_VAR
#define RECORD_VAR


PUBLIC	b_rec_card_full_flg
PUBLIC	b_rec_wav_full_flg
PUBLIC	b_rec_samplerate_flg
PUBLIC	b_rec_decbuf_flg
PUBLIC	b_rec_mode_flg
PUBLIC	b_rec_adcbuf_full_flg
PUBLIC	b_rec_have_rec_flg
PUBLIC	b_flicker_time_flg

PUBLIC	uc_enc_step_idx 
PUBLIC	uc_enc_thecode   
PUBLIC	uc_enc_sample_expand
PUBLIC	uc_enc_diff_buf 
PUBLIC	uc_enc_effdiff_buf 
PUBLIC	uc_refresh_rec_cnt
PUBLIC	uc_refresh_pic_num
PUBLIC	uc_refresh_time_cnt
PUBLIC	uc_menu_top_idx

PUBLIC	uc_menu_cur_idx
PUBLIC	uc_menu_total_num
PUBLIC	uc_menu_exit_cnt
PUBLIC	uc_rec_playkey_cnt_h
PUBLIC	uc_rec_playkey_cnt_l
PUBLIC	uc_rec_status 
PUBLIC	uc_rec_time_cnt
PUBLIC	uc_rec_sav_map_idx
PUBLIC	uc_rec_sav_map_val
PUBLIC	uc_fm_write_bytes
PUBLIC	uc_rec_adcbuf_dph
PUBLIC	uc_rec_adcbuf_dpl

PUBLIC	ui_enc_indata    
PUBLIC	ui_enc_sample    
PUBLIC	ui_enc_step_sft2
PUBLIC	ui_enc_step 
PUBLIC	ui_enc_step_sft1 
PUBLIC	ui_rec_temp_var
PUBLIC	ui_rec_dptr_var
PUBLIC	ui_rec_wav_total_num
PUBLIC  uc_dynamicload_idx	   //allen

PUBLIC	si_rec_adc_inbuf
PUBLIC	si_rec_adc_outbuf

PUBLIC	ul_rec_cur_time
PUBLIC	ul_rec_remain_time
PUBLIC	ul_rec_temp_var
PUBLIC	ul_rec_sample_total_num

PUBLIC	tmp_h
PUBLIC	tmp_m
PUBLIC	tmp_s


	BSEG	AT		28H.5
b_rec_card_full_flg:		DBIT	1		;1--card full
b_rec_wav_full_flg:			DBIT	1		;1--wav number is full
b_rec_samplerate_flg:		DBIT	1		;1--16KHz ui_enc_sample rate; 0--8KHz ui_enc_sample rate
b_rec_decbuf_flg:			DBIT	1
b_rec_mode_flg:				DBIT	1
b_rec_adcbuf_full_flg:		DBIT	1
b_rec_have_rec_flg:			DBIT	1		;1--have recorded; 0--not rec
b_flicker_time_flg:			DBIT	1


	DSEG	AT		12H

uc_enc_step_idx:			DS	1
uc_enc_thecode:				DS	1
uc_rec_playkey_cnt_h:		DS	1
uc_rec_playkey_cnt_l:		DS	1
ui_enc_indata:				DS	2
ui_enc_sample:				DS	2
ui_enc_step:				DS	2
ui_enc_step_sft1:			DS	2
uc_rec_adcbuf_dpl:			DS	1
uc_rec_adcbuf_dph:			DS	1


	DSEG	AT		2aH	

uc_enc_diff_buf:			DS	3
uc_enc_effdiff_buf:			DS	3


   	DSEG	AT		7bH

uc_enc_sample_expand:		DS	1
ui_enc_step_sft2:			DS	2

   	XSEG	AT		6000H

uc_refresh_pic_num:			DS	1
uc_refresh_rec_cnt:			DS	1
uc_refresh_time_cnt:		DS	1
uc_menu_top_idx:			DS	1
uc_dynamicload_idx:         DS  1   //allen

uc_menu_cur_idx:			DS	1
uc_menu_total_num:			DS	1
uc_rec_time_cnt:			DS	1
uc_rec_sav_map_idx:			DS	1
uc_rec_sav_map_val:			DS	1
uc_rec_status:				DS	1
tmp_h:						DS	1
tmp_m:
uc_menu_exit_cnt:			DS	1
ui_rec_temp_var:			DS	2
ui_rec_wav_total_num:		DS	2
ui_rec_dptr_var:			DS	2

ul_rec_sample_total_num:	DS	4
ul_rec_temp_var:			DS	4
ul_rec_cur_time:			DS	4
ul_rec_remain_time:			DS	4
tmp_s:
uc_fm_write_bytes:
si_rec_adc_inbuf:			DS	8
si_rec_adc_outbuf:			DS	8



/**************************fs****************************/

XSEG		AT		X_ADDR_FS_FATBUF
uc_fs_fatbuf:	DS		512

#endif
#endif
	END