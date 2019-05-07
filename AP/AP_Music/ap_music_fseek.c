/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_fseek.c
Creator: zj.zong					Date: 2009-04-29
Description: point to the specifically position in file
Others:
Version: V0.1
History:
	V0.1	2009-04-29		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_FSEEK_DEF_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "sh86278_api_define.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"

extern bit FS_Fseek(ul32 offset);
extern bit FS_Fread();
extern bit FS_Fopen_Appointed(ui16 file_idx);
extern void _media_readsector();

bit Ap_Check_ContinuePlay(void);
void Ap_ReadId3_FromFlash(void);
void Ap_FseekFor_ABReplay(void);
void Ap_FseekFor_ForwardBackward(void);
void Ap_FseekFor_ContinuePlay(void);
void Ap_Fill_Decbuf_Firstly(void);

/*******************************************************************************
Function: Ap_Fseek_InFile()
Description: point to the appointed position in file
Calls:
Global:uc_mp3_fseek_flg:    1--(A-B) replay;  2--fast;  3--continue playing
       ul_mp3_replay_a_byte: A- byte offset
       uc_mp3_status:       bit7--1:file end; 0:file ok
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Fseek_InFile(void)
{
	switch(uc_dynamicload_idx)
	{
		case 36:		 	//A-B replay
	   		Ap_FseekFor_ABReplay();
			break;

		case 37:		 	//forward or backward
	   		Ap_FseekFor_ForwardBackward();
			break;

		case 38:			//continue playing
	   		Ap_FseekFor_ContinuePlay();
			/*
			if(b_lrc_file_exist == 1)
			{
				Ap_ReadId3_FromFlash();		//read id3 info from flash
			}*/
			Ap_Fill_Decbuf_Firstly();

			break;

		case 39:			//fs_fseek(0)					   
			FS_Fseek(ul_id3_size&0xfffffe00);	
			break;

		case 40:			//fs_fseek(0)					   
			FS_Fseek(ul_id3_size&0xfffffe00);	
			Ap_Fill_Decbuf_Firstly();
			break;
	}

	uc_dynamicload_idx = 0;						//clr flag
}


void Ap_FseekFor_ABReplay(void)
{
	ui_mp3_cur_time = ui_mp3_replay_a_time;	//init current time
		
	FS_Fseek(ul_mp3_replay_a_byte);			//point to the A- position
	uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf (0x5000)to fs
	FS_Fread();
}



void Ap_FseekFor_ForwardBackward(void)
{
	ul32 tmp_byte_offset;

	tmp_byte_offset = (ul32)ui_mp3_bitrate * 125 * ui_mp3_cur_time;

	if(FS_Fseek(tmp_byte_offset))			//fs_fseek ok
	{
		uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf 0x5000 to fs
		if(FS_Fread() == 1)					//1:read sector ok
		{
			b_mp3_file_end = 0;
		}
		else								//read sector error
		{
			b_mp3_file_end = 1;
		}
	}
	else									//fs_fseek error
	{
		b_mp3_file_end = 1;
	}
}



void Ap_FseekFor_ContinuePlay(void)
{
	if(Ap_Check_ContinuePlay() == 1)			//power on continue playing
	{
		if(ul_mp3_byte_offset_bk > (ul_fs_file_length - 3072))
		{
			ul_mp3_byte_offset_bk = ul_fs_file_length - 3072;
		}
		FS_Fseek(ul_mp3_byte_offset_bk);
		
		ui_mp3_cur_time = ui_mp3_played_time;
	}
	else
	{
		FS_Fseek(ul_id3_size&0xfffffe00);	
	}
}



void Ap_ReadId3_FromFlash(void)
{
	uc8 i;

	sfc_logblkaddr0 = 0x66;						//0x0000 0066 ~ 0x0000 0067
	sfc_logblkaddr1 = 0x00;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_reserved_area_flag = 1;					//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_FBUF;			//occupy decbuf
	buf_offset0 = (uc8)FCOMBUF_OFFSET_W;		//buffer offset
	buf_offset1 = (uc8)(FCOMBUF_OFFSET_W >> 8);
	_media_readsector();						//read one sector

	for(i = 0; i < 100; i++)
	{
		ui_id3_buf[i] = ui_sfc_combuf[i];
	}
	sfc_reserved_area_flag = 0;					//clr reserved area flag
}



bit Ap_Check_ContinuePlay(void)
{
	if((uc_fs_store_cluster1 == uc_fs_begin_cluster1) && (uc_fs_store_cluster2 == uc_fs_begin_cluster2))
	{
		if((uc_fs_store_length1 == (uc8)ul_fs_file_length) && (uc_fs_store_length2 == (uc8)(ul_fs_file_length>>8)))
		{
			return 1;
		}
	}
	
	uc_fs_store_cluster1 = uc_fs_begin_cluster1;	//backup current mp3 file parameter for continue playing
	uc_fs_store_cluster2 = uc_fs_begin_cluster2;
	uc_fs_store_length1 = (uc8)ul_fs_file_length;
	uc_fs_store_length2 = (uc8)(ul_fs_file_length >> 8);
	return 0;
}

void Ap_Fill_Decbuf_Firstly(void)
{
	mMPEG_Release_Dbuf1;						//mpeg release DBUF1 to MCU
	
	ui_sys_event &= ~EVT_MUSIC_FILL_DECBUF;
	uc_fs_assign_buf_flg = SFC_APP_DBUF1;		//assign dbuf(0x5000) to fs
	if(FS_Fread() == 0)
	{
		b_mp3_file_end = 1;
	}
	else
	{
		mMPEG_Release_Dbuf1;						//mpeg release DBUF1 to MCU
	}
}

#endif