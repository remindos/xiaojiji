/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_save_para.c
Creator: zj.zong					Date: 2009-05-21
Description: 1. save voice parameter into flash system area
Others:
Version: V0.1
History:
	V0.1	2009-05-21		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOICE_SAVE_PARA_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"
#include "..\..\UI\UI_Voice\ui_voice_header.h"

extern void FS_WrFlash_Interface(void);
extern void _media_readsector(void);
extern void Ap_SaveVoice_Para(void);
/*******************************************************************************
Function: Ap_SaveVoice_Para()
Description: save voice parameter into flash when exited voice mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SaveVoice_Para(void)
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

	uc_dbuf0[339] = uc_save_mode_idx;					//save mode 
	uc_dbuf0[78] = (uc8)b_sd_flash_flg;			//flash or SD/MMC card

	if(b_sd_flash_flg == 0)						//flash
	{
		uc_dbuf0[70] = uc_fs_begin_cluster1;			//the saved file addr
		uc_dbuf0[71] = uc_fs_begin_cluster2;
		uc_dbuf0[72] = (uc8)ul_fs_file_length;			//the saved file length(7~0)
		uc_dbuf0[73] = (uc8)(ul_fs_file_length >> 8);	//the saved file length(15~8)
	}
	else												//SD/MMC card
	{
		uc_dbuf0[74] = uc_fs_begin_cluster1;			//the saved file addr
		uc_dbuf0[75] = uc_fs_begin_cluster2;
		uc_dbuf0[76] = (uc8)ul_fs_file_length;			//the saved file length(7~0)
		uc_dbuf0[77] = (uc8)(ul_fs_file_length >> 8);	//the saved file length(15~8)	
	}

	uc_dbuf0[79] = uc_voice_repeat_mode;				//voice repeat
	uc_dbuf0[80] = uc_voice_volume_level;				//voice volume level

	b_sfc_immediate_wr_flg = 1;							
	FS_WrFlash_Interface();
	sfc_reserved_area_flag = 0;
}


#endif