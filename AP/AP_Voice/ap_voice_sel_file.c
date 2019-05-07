/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_sel_file.c
Creator: zj.zong					Date: 2009-05-22
Description: 1. select next or previous file;
             2. file end and get next file
Others:
Version: V0.1
History:
	V0.1	2009-05-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOICE_SEL_FILE_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "..\..\UI\UI_Voice\ui_voice_header.h"
#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"
#include <stdlib.h>

extern bit FS_Fopen_Appointed(ui16 file_idx);
extern bit FS_Chk_FileType(void);
extern void FS_Get_NextFile_InDir(void);

void Ap_Sel_NextFile(void);
void Ap_Sel_PreviousFile(void);
void Ap_Get_Next_PlayFile(void);
ui16 Ap_GetRandom(void);

/*******************************************************************************
Function: Ap_Sel_File()
Description: select next file or previous file
Calls:
Global:uc_dynamicload_idx:1--sel next file;
						 	  2--sel previous file; 
							  3--file end; 
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Sel_File(void)
{
	switch(uc_dynamicload_idx)
	{
		case 35:					  	//sel next voice file
			Ap_Sel_NextFile();			   
			break;

		case 36:						//sel previous file
		   	Ap_Sel_PreviousFile();
			break;

		case 37:						//file end
			Ap_Get_Next_PlayFile();
			break;
		default:
		break;	
	}
	uc_fs_store_cluster1 = 0;		
	uc_fs_store_cluster2 = 0;
	uc_fs_store_length1 = 0;
	uc_fs_store_length2	= 0;

	uc_dynamicload_idx = 0;		 //clr bit1,bit2
}
/*******************************************************************************
Function: Ap_Sel_NextFile()
Description: select next file;
Calls:
Global:ui_voice_wav_idx, uc_voice_repeat_mode
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Sel_NextFile(void)
{
	b_voice_get_next_file = 1;

	if(uc_voice_repeat_mode != REPEAT_RANDOM)
	{
		ui_voice_wav_idx++;
		if(ui_voice_wav_idx > ui_fb_total_num)
		{
			ui_voice_wav_idx = 1;
		}
		
		FS_Get_NextFile_InDir();
	}
	else
	{
		ui_voice_wav_idx = Ap_GetRandom();
		FS_Fopen_Appointed(ui_voice_wav_idx - 1);
	}
}



/*******************************************************************************
Function: Ap_Sel_PreviousFile()
Description: select previous file;
Calls:
Global:ui_voice_wav_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Sel_PreviousFile(void)
{
	b_voice_get_next_file = 1;

	if(uc_voice_repeat_mode != REPEAT_RANDOM)
	{
		ui_voice_wav_idx--;
		if(ui_voice_wav_idx == 0)
		{
			ui_voice_wav_idx = ui_fb_total_num;
		}
	}
	else
	{
		ui_voice_wav_idx = Ap_GetRandom();
	}

	FS_Fopen_Appointed(ui_voice_wav_idx - 1);
}



/*******************************************************************************
Function: Ap_Get_Next_PlayFile()
Description: get next wav fine in voice dir when playing file is end
Calls:
Global:ui_voice_wav_idx, uc_voice_repeat_mode
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Get_Next_PlayFile(void)
{
 	b_voice_get_next_file = 1;

	switch(uc_voice_repeat_mode)
	{
		case REPEAT_FOLDER_NOR:
		case REPEAT_PREVIEW:
		
			Ap_Sel_NextFile();		//get next wav file
			
			if(ui_voice_wav_idx == 1)	//if next wav file is first, and stop play
			{
				b_voice_get_next_file = 0;
			}
			break;

	 	case REPEAT_FOLDER:
			Ap_Sel_NextFile();		//get next wav file
			break;

	 	case REPEAT_ONE:
			FS_Fopen_Appointed(ui_voice_wav_idx - 1);
			break;

	 	case REPEAT_RANDOM:
	 		ui_voice_wav_idx = Ap_GetRandom();
			FS_Fopen_Appointed(ui_voice_wav_idx - 1);
			break;
	}
}



ui16 Ap_GetRandom(void)
{
	ui16 i;
	
get_new_random:
	i = rand() % ui_fb_total_num + 1;
	if(ui_fb_total_num != 1)
	{
		if(i == ui_voice_random_idx_bk)
		{
			goto get_new_random;
		}
	}
	
	return(i);
}



#endif