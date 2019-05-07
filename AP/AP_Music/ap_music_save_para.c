/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_save_para.c
Creator: zj.zong					Date: 2009-04-27
Description: save music parameter when enter file browser or exit from music mode
Others:
Version: V0.1
History:
	V0.1	2009-04-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_SAVE_PARA_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"

extern void FS_WrFlash_Interface(void);
extern void _media_readsector(void);
extern void Ap_Save_Music_Para(void);
/*******************************************************************************
Function: Ap_Save_Music_Para()
Description: save music parameter when enter file browser or exit music mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Save_Music_Para()
{
	sfc_logblkaddr0 = 0x02;							//0x0000 0202
	sfc_logblkaddr1 = 0x02;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_reserved_area_flag = 1;						//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_DBUF0;				//occupy dbuf(0x4000)
	buf_offset0 = 0x00;								//buffer offset :0x0000
	buf_offset1 = 0x00;
	_media_readsector();							//read one sector

	uc_dbuf0[339] = uc_save_mode_idx;				//save mode 
	uc_dbuf0[31] = b_mp3_replay_mode;				//common parameter
	uc_dbuf0[24] = b_sd_flash_flg;	

	if(b_sd_flash_flg == 0)
	{
		uc_dbuf0[4] = uc_fs_store_cluster1;				//the saved file addr
		uc_dbuf0[5] = uc_fs_store_cluster2;
		uc_dbuf0[6] = uc_fs_store_length1;				//the saved file length
		uc_dbuf0[7] = uc_fs_store_length2;

		uc_dbuf0[8] = (char)(ul_mp3_byte_offset_bk >> 24);	//high
		uc_dbuf0[9] = (char)(ul_mp3_byte_offset_bk >> 16);
		uc_dbuf0[10] = (char)(ul_mp3_byte_offset_bk >> 8);
		uc_dbuf0[11] = (char)ul_mp3_byte_offset_bk;			//low

		uc_dbuf0[12] = (char)(ui_mp3_cur_time >> 8);	//the last saved time
		uc_dbuf0[13] = (char)ui_mp3_cur_time;			//the last saved time
	}
	else
	{
		uc_dbuf0[14] = uc_fs_store_cluster1;				//the saved file addr
		uc_dbuf0[15] = uc_fs_store_cluster2;
		uc_dbuf0[16] = uc_fs_store_length1;				//the saved file length
		uc_dbuf0[17] = uc_fs_store_length2;

		uc_dbuf0[18] = (char)(ul_mp3_byte_offset_bk >> 24);	//high
		uc_dbuf0[19] = (char)(ul_mp3_byte_offset_bk >> 16);
		uc_dbuf0[20] = (char)(ul_mp3_byte_offset_bk >> 8);
		uc_dbuf0[21] = (char)ul_mp3_byte_offset_bk;			//low

		uc_dbuf0[22] = (char)(ui_mp3_cur_time >> 8);	//the last saved time
		uc_dbuf0[23] = (char)ui_mp3_cur_time;			//the last saved time	
	}

	uc_dbuf0[25] = uc_mp3_eq_mode;					//eq
	uc_dbuf0[26] = uc_mp3_repeat_mode;				//music repeat

	uc_dbuf0[28] = uc_mp3_replay_time;				//A-B replay times
	uc_dbuf0[29] = uc_mp3_replay_gap;				//A-B replay gap
	uc_dbuf0[30] = uc_mp3_volume_level;				//music volume level

//	uc_dbuf0[260] = uc_rtc_init_flg;
	uc_dbuf0[261] = uc_lock_level;
	uc_dbuf0[341] = uc_bklight_time_level;
	uc_dbuf0[343] = uc_bklight_grade;

	uc_dbuf0[250] = uc_disktype_flg;

    b_sfc_immediate_wr_flg = 1;
	FS_WrFlash_Interface();
	sfc_reserved_area_flag = 0;
}

#endif