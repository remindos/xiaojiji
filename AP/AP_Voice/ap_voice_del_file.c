/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_del_file.c
Creator: zj.zong				Date: 2009-05-21
Description: 1. delete one file, and get next wav file
Others:
Version: V0.1
History:
	V0.1	2009-05-21		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOECE_DELETE_FILE_DEF

#include "voice_header.h"
#include "voice_var_ext.h"

extern bit FS_Fopen_Appointed(ui16 file_idx);
extern void FS_Fdelete();


void Ap_Del_OneWav_File(void)

/*******************************************************************************
Function: Ap_Del_OneWav_File()
Description: delete one file, and get next wav file
Calls:
Global:ui_fb_total_num, ui_voice_wav_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Del_OneWav_File(void)
{
	FS_Fdelete();									//delete one file

	ui_fs_file_num_indir--;
	ui_fb_total_num--;
	ui_fs_special_file_num_indir--;
	if(ui_fb_total_num == 0)
	{
		ui_voice_wav_idx = 0;						//no wav file
		return;
	}
	
	if(ui_voice_wav_idx > ui_fb_total_num)	//if deleted file is the last wav
	{
		ui_voice_wav_idx = ui_fb_total_num;
	}
	
	FS_Fopen_Appointed(ui_voice_wav_idx - 1);
}

#endif