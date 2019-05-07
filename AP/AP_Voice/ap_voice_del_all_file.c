/*/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_del_file.c
Creator: zj.zong				Date: 2009-05-21
Description: 1. delete all file
Others:
Version: V0.1
History:
	V0.1	2009-05-21		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOECE_DELETE_ALL_FILE_DEF

#include "voice_header.h"
#include "voice_var_ext.h"

extern bit FS_Fopen_Appointed(ui16 file_idx);
extern void FS_Fdelete();

void Ap_Del_all_wav_file(void)
/*******************************************************************************
Function: Ap_Del_all_wav_file()
Description: delete all wav file
Calls:
Global:ui_fb_total_num, ui_voice_wav_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Del_all_wav_file(void)
{
	while(ui_fs_special_file_num_indir != 0)
	{
		FS_Fopen_Appointed(0);
		FS_Fdelete();
		ui_fs_file_num_indir--;
		ui_fs_special_file_num_indir--;
	}
	
	ui_fb_total_num = 0;
	ui_voice_wav_idx = 0;
}


#endif