/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_lrc.c
Creator: zj.zong					Date: 2009-04-27
Description: check lrc file existed and process lrc file
Others:
Version: V0.1
History:
	V0.1	2009-04-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_LRC_PROCESS_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"
extern void Ap_Process_83Name(void);
extern ui16 Api_Isn_ConvertTo_Unicode(ui16 isn_index);

extern void FS_Get_83Name(void);
extern void FS_Init_OpenVar(void);
extern bit FS_Fopen(ui16 file_idx);
extern bit FS_Get_LongName(void);
extern bit FS_Fread(void);
extern bit FS_Chk_FileType(void);
extern bit FS_Fseek(ul32 offset);
extern void FS_WrFlash_Interface(void);
extern ui16 Api_Merge_2Byte_Sub(uc8 lo,uc8 hi);

void Ap_WriteLrc_LabelBuf(void);
void Ap_Modify_LrcBuf(void);
void Ap_WritLlrc_ToFlash(void);
void Ap_ProcessLrc_TimeLabel(void);
void Ap_Exchange_HighLow_Byte(void);
void Ap_Init_LrcVar(void);
void Ap_WriteId3_ToFlash(void);
void Ap_Init_Flash_Input(void);
bit  Ap_ReadLrc_OneByte(void);

void Ap_Writelrc_timebuf(void);

#define LRC_TIME_W		0x600		//0xE00
#define LRC_OFFSET_W	0x700		//0xF00

#ifdef AP_CHECK_LRC_FILE_EXIST
/*******************************************************************************
Function: Ap_Check_LrcFile_Existed()
description: check lrc file existed, compare lrc file name with the mp3 file name
Calls:
Global:ui_fs_item_name--current mp3 file name
       b_lrc_file_exist--1:have lrc; 0:no lrc
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Check_LrcFile_Existed(void)
{
	uc8 i,tmp_length;
	ui16 tmp_idx;

	for(i = 0; i < 32; i++)							//save the ui_fs_item_name
	{
		ui_lrc_time_buf[i] = ui_fs_item_name[i];
	}
	tmp_length = uc_fs_item_name_length;			//save length of current file name 
	
	b_lrc_file_exist = 0;							//clr b_lrc_file_exist

	ui_fs_file_type = FS_FILE_TYPE_LRC;				//set bit7, file type is *.lrc
	FS_Init_OpenVar();								//appointed  the first file
	for(tmp_idx = 0; tmp_idx < ui_fs_file_num_indir; tmp_idx++)
	{
		if(b_lrc_file_exist == 1)
		{
			break;
		}
		
		FS_Fopen(tmp_idx);
		if(FS_Chk_FileType() == 1)
		{
			if(FS_Get_LongName() == 0)				//get long file name. rts is ui_fs_item_name(unicode)
			{
				FS_Get_83Name();					//rts is uc_fs_item_83_name[12]
				Ap_Process_83Name();					//rts is ui_fs_item_name(unicode)
			}
        	
			if(tmp_length == uc_fs_item_name_length)			//firstly, compare file name length
			{
				for(i = 0; i < uc_fs_item_name_length - 4; i++)	//secondly, compare file name 
				{
					if(ui_lrc_time_buf[i] != ui_fs_item_name[i])
					{
						b_lrc_file_exist = 0;
						break;
					}
					b_lrc_file_exist = 1;
				}
			}
		}
	}
	ui_fs_file_type = FS_FILE_TYPE_MP3;				//set bit0, file type is *.mp3
	
//	for(i = 0; i < 32; i++)							//restore the ui_fs_item_name
//	{
//		ui_fs_item_name[i] = ui_lrc_time_buf[i];
//	}
	uc_fs_item_name_length = tmp_length;			//restore length of current file name
}
#endif



#ifdef AP_GET_LRC_FILE_DATA
/*******************************************************************************
Function: Ap_Read_LrcFile()
description: 1. read lrc file
             2. convert ISN into unicode
             3. write time label and lrc buffer to flash
Calls:
Global:ui_lrc_time_buf--lrc time lable buffer
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Read_LrcFile(void)
{
	uc8 i, tmp_cnt=0;
	
	Ap_WriteId3_ToFlash();
	
	Ap_Init_LrcVar();

	uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf(0x5000) to fs
	if(FS_Fread() == 0)						//read 512bytes into dbuf1
	{
		return;								//lrc file end
	}
	if(ui_dbuf1[0] == 0xfeff)
	{
		uc_lrc_unicode_flg = 1;				//unicode little endian lrc
	}
	else if(ui_dbuf1[0] == 0xfffe)
	{
		uc_lrc_unicode_flg = 2;				//unicode big endian lrc
		Ap_Exchange_HighLow_Byte();
	}
	else
	{
		Ap_Exchange_HighLow_Byte();
	}

	while(1)
	{
		if(b_lrc_label_overflow == 1)
		{
	 		break;								//lrc file end
	 	}
	 	
		b_lrc_rd_error = Ap_ReadLrc_OneByte();	//rts "1" is ok
		
		if(b_lrc_rd_error == 0)					//error or file end
		{
			break;
		}
		
		if(ui_lrc_value == '[')
		{
			Ap_ProcessLrc_TimeLabel();	//check lrc time label is valid?

			if(b_lrc_valid_label == 1)
			{
				Ap_WriteLrc_LabelBuf();	//write lrc time label to buffer
				tmp_cnt++;
			}
		}
		else
		{
			if(tmp_cnt == 1)
			{
				tmp_cnt = 0;
			}
			else
			{
				for(i = 0; i < tmp_cnt; i++)
				{
					//ui_id3_buf[uc_lrc_label_idx + i - tmp_cnt] = ui_id3_buf[uc_lrc_label_idx - 1];
					ui_dbuf1[LRC_OFFSET_W+uc_lrc_label_idx + i - tmp_cnt] = ui_dbuf1[LRC_OFFSET_W+uc_lrc_label_idx - 1];
				}
				tmp_cnt = 0;
			}
		}
	}
	uc_lrc_label_num = uc_lrc_label_idx;		//index(0~0x??)	, number(1~0x??)
	
	Ap_Writelrc_timebuf();		//wang
	
	Ap_Modify_LrcBuf();							//modify timer label from 00:00 to total time
	uc_lrc_curlabel_idx = -1;					//init the uc_lrc_curlabel_idx

	Ap_WritLlrc_ToFlash();						//write lrc song buffet to flash system area
}



/*******************************************************************************
init lrc variable
********************************************************************************/
void Ap_Init_LrcVar(void)
{
	uc8 i;

	b_lrc_labelready = 0;				//1--label read, 0--not
	b_lrc_songgoing = 0;				//have start to read lrc
	b_lrc_label_overflow = 0;			//1--overflow, 0--not
	b_lrc_rd_error = 0;					//0--read one byte error, 1--ok
	b_lrc_valid_label = 0;
	
	uc_lrc_label_num = 0;				//lrc label total number
	uc_lrc_curlabel_idx = 0;

	uc_lrc_label_idx = 0;
	ui_lrc_bytes_idx = 0;
	ui_lrc_byte_offset = 0;
	
/*	for(i = 0; i < LRC_ONE_LABEL_MAX; i++)
	{
		ui_lrc_line_buf[i] = 0;
	}*/

	for(i=0; i<255; i++)
	{
		ui_dbuf1[i+LRC_TIME_W] = 0;
		ui_dbuf1[i+LRC_OFFSET_W] = 0;
	}
}



/*******************************************************************************
process lrc timer label ---- [00:16.35]
ouput:b_lrc_valid_label,  1---lrc timer label is valid;
********************************************************************************/
void Ap_ProcessLrc_TimeLabel(void)
{
	b_lrc_valid_label = 0;

	Ap_ReadLrc_OneByte();				//rts is "ui_lrc_value"
	if((ui_lrc_value < '0') || (ui_lrc_value > '9'))//0~9
	{
		return;
	}
	ui_lrc_minute = ui_lrc_value;		//minute high byte
	
	Ap_ReadLrc_OneByte();				//rts is "ui_lrc_value"
	if((ui_lrc_value >= '0') && (ui_lrc_value <= '9'))
	{
		ui_lrc_minute = (ui_lrc_minute<<8) + ui_lrc_value;//minute low byte
		Ap_ReadLrc_OneByte();			//rts is "ui_lrc_value"
	}
		
	if( ui_lrc_value != ':')			//00:31	timer
	{
		return;
	}
	
	Ap_ReadLrc_OneByte();				//rts is "ui_lrc_value"
	if((ui_lrc_value < '0') || (ui_lrc_value > '9'))
	{
		return;
	}
	ui_lrc_second = ui_lrc_value;		//second high byte

	Ap_ReadLrc_OneByte();				//rts is "ui_lrc_value"
	if((ui_lrc_value >= '0') && (ui_lrc_value <= '9'))
	{
		ui_lrc_second = (ui_lrc_second<<8) + ui_lrc_value;//second low byte
		Ap_ReadLrc_OneByte();			//rts is "ui_lrc_value"
	}

	if(ui_lrc_value == '.')				//cancel "ms"
	{
		while(1)
		{
			Ap_ReadLrc_OneByte();		//rts is "ui_lrc_value"
			if((ui_lrc_value  < '0') || (ui_lrc_value > '9'))
			{
				break;
			}
		}
	}

	if( ui_lrc_value != ']')
	{
		return;
	}

	b_lrc_valid_label = 1;
}




/*******************************************************************************
write lrc label timer and start address into buffer
********************************************************************************/
void Ap_WriteLrc_LabelBuf(void)
{
	if(uc_lrc_label_idx >= LRC_LABEL_NUM_MAX)		//support 100 time label
	{
		b_lrc_label_overflow = 1;
		return;
	}
/*
	ui_lrc_time_buf[uc_lrc_label_idx] = (((ui_lrc_minute>>8)-0x30)*10 + ((ui_lrc_minute&0xff)-0x30))*60
					+ (((ui_lrc_second>>8)-0x30)*10 + ((ui_lrc_second&0xff)-0x30));

	ui_id3_buf[uc_lrc_label_idx] = ui_lrc_bytes_idx;	//offset + buffer address
*/
	ui_dbuf1[LRC_TIME_W+uc_lrc_label_idx] = (((ui_lrc_minute>>8)-0x30)*10 + ((ui_lrc_minute&0xff)-0x30))*60
					+ (((ui_lrc_second>>8)-0x30)*10 + ((ui_lrc_second&0xff)-0x30));

	ui_dbuf1[LRC_OFFSET_W+uc_lrc_label_idx] = ui_lrc_bytes_idx;	//offset + buffer address

	uc_lrc_label_idx++;
}



/*******************************************************************************
modify lrc timer buffer and lrc address buffer
according to the timer label
from timer 00:00 to total timer xx:xx
********************************************************************************/
void Ap_Modify_LrcBuf(void)
{
	uc8 i, j;
	ui16 ui_lrc_tmep;
	
	for(i = 0; i < uc_lrc_label_idx - 1; i++)
	{
		for(j = i + 1; j < uc_lrc_label_idx; j++)
		{
			/*if( ui_lrc_time_buf[i]> ui_lrc_time_buf[j] )
			{
				ui_lrc_tmep = ui_lrc_time_buf[i];	//lrc time label buffer
				ui_lrc_time_buf[i] = ui_lrc_time_buf[j];	
				ui_lrc_time_buf[j] = ui_lrc_tmep;
				
				ui_lrc_tmep = ui_id3_buf[i];
				ui_id3_buf[i] = ui_id3_buf[j];//lrc line address buffer
				ui_id3_buf[j] = ui_lrc_tmep;
			}*/
			if( ui_dbuf1[LRC_TIME_W+i]> ui_dbuf1[LRC_TIME_W+j] )
			{
				ui_lrc_tmep = ui_dbuf1[LRC_TIME_W+i];	//lrc time label buffer
				ui_dbuf1[LRC_TIME_W+i] = ui_dbuf1[LRC_TIME_W+j];	
				ui_dbuf1[LRC_TIME_W+j] = ui_lrc_tmep;
				
				ui_lrc_tmep = ui_dbuf1[LRC_OFFSET_W+i];
				ui_dbuf1[LRC_OFFSET_W+i] = ui_dbuf1[LRC_OFFSET_W+j];//lrc line address buffer
				ui_dbuf1[LRC_OFFSET_W+j] = ui_lrc_tmep;
			}
		}
	}
}



/*******************************************************************************
write processed lrc data into flsh(0xa500 0004 ~ 0xa500 0067)
********************************************************************************/
void Ap_WritLlrc_ToFlash(void)
{
	uc8 i, j;
	ui16 tmp_lrc_value, tmp_unicode;
	
	for(i = 0; i < uc_lrc_label_num; i++)
	{
		for(j = 0; j < 255; j++)			//clr dbuf(0x4000) 510bytes
		{
			ui_dbuf0[j] = 0;
		}
		
		//FS_Fseek(ui_id3_buf[i] & 0xfe00);
		FS_Fseek(ui_dbuf1[LRC_OFFSET_W+i] & 0xfe00);
		FS_Fread();
		if(uc_lrc_unicode_flg != 1)
		{
			Ap_Exchange_HighLow_Byte();				//if it's unicode little endian, and do not exchange high with low
		}
		//ui_lrc_byte_offset = ui_id3_buf[i] % 512;
		ui_lrc_byte_offset = ui_dbuf1[LRC_OFFSET_W+i] % 512;
		
		j = 0;
		while(1)
		{
			if(Ap_ReadLrc_OneByte() == 1)
			{
				if((ui_lrc_value == 0x0d) || (ui_lrc_value == '['))
				{
					break;
				}
				else
				{
					if(uc_lrc_unicode_flg == 0)
					{
						tmp_lrc_value = ui_lrc_value;	//read high byte
						tmp_unicode = Api_Isn_ConvertTo_Unicode(tmp_lrc_value);
						if(tmp_unicode == 0)			//check isn is 16*16 or 16*8
						{
							Ap_ReadLrc_OneByte();		//rts is "ui_lrc_value"
							tmp_lrc_value = (tmp_lrc_value<<8) + ui_lrc_value;
							tmp_unicode = Api_Isn_ConvertTo_Unicode(tmp_lrc_value);
						}
						ui_dbuf0[j] = tmp_unicode;
					}
					else
					{
						ui_dbuf0[j] = ui_lrc_value;		//unicode lrc
					}
					j++;
				}
			}
			else
			{
				break;
			}
		}

		if(i == uc_lrc_label_num - 1)			//get b_sfc_immediate_wr_flg
		{
			b_sfc_immediate_wr_flg = 1;			//last write
		}
		else
		{
			b_sfc_immediate_wr_flg = 0;
		}
		//sfc_logblkaddr0 = i + LRC_FLASH_ADDRESS;
		sfc_logblkaddr0 = (uc8)((ui16)i + LRC_FLASH_ADDRESS);
		sfc_logblkaddr1 = (uc8)(((ui16)i + LRC_FLASH_ADDRESS)>>8);

		Ap_Init_Flash_Input();
	}
}

//wang
void Ap_Writelrc_timebuf(void)
{
	uc8 i;
	mReset_WatchDog;
	for(i = 0; i < 100; i++)
	{
		ui_lrc_time_buf[i] = ui_dbuf1[i+LRC_TIME_W];
	}

	for(i = 0; i < LRC_LABEL_NUM_MAX; i++)
	{
		ui_dbuf0[i] = ui_dbuf1[i+LRC_TIME_W];
		
	}
	
	sfc_logblkaddr0 = 0x64;				//0x0000 0064 ~ 0x0000 0065
	sfc_logblkaddr1 = 0x00;
	b_sfc_immediate_wr_flg = 1;
	Ap_Init_Flash_Input();
}

void Ap_Init_Flash_Input(void)
{
	mReset_WatchDog;
//	sfc_logblkaddr1 = 0x00;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_reserved_area_flag = 1;					//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_DBUF0;		//accupy dbuf(0x4000)
	buf_offset0 = 0x00;						//buffer offset :0x0000
	buf_offset1 = 0x00;
	FS_WrFlash_Interface();
	sfc_reserved_area_flag = 0;					//clr reserved area flag
}



/*******************************************************************************
read one byte from lrc file
output: ui_lrc_value
return: 1--read ok; 0--read error
********************************************************************************/
bit Ap_ReadLrc_OneByte(void)
{
	if(ui_lrc_byte_offset >= 512)
	{
		ui_lrc_byte_offset = 0;
		uc_fs_assign_buf_flg = SFC_APP_DBUF1;		//assign dbuf(0x5000) to fs
		if(FS_Fread() == 0)
		{
			return 0;								//lrc file end
		}
		if(uc_lrc_unicode_flg != 1)
		{
			Ap_Exchange_HighLow_Byte();				//if it's unicode little endian, and do not exchange high with low
		}
	}

	if(uc_lrc_unicode_flg == 0)
	{
		ui_lrc_value = (ui16)uc_dbuf1[ui_lrc_byte_offset];	//get the one byte value
	}
	else
	{
		ui_lrc_value = Api_Merge_2Byte_Sub(uc_dbuf1[ui_lrc_byte_offset+1],uc_dbuf1[ui_lrc_byte_offset]);
	}
	
	if(ui_lrc_value == 0)
	{
		return 0;									//lrc file end
	}
	ui_lrc_byte_offset++;							//offset + 1
	ui_lrc_bytes_idx++;

	if(uc_lrc_unicode_flg != 0)
	{
		ui_lrc_byte_offset++;						//unicode lrc, +2bytes
		ui_lrc_bytes_idx++;
	}
	
	return 1;										//read ok
}



/*******************************************************************************
exchange high with low byte
********************************************************************************/
void Ap_Exchange_HighLow_Byte(void)
{
	ui16 i;
	uc8 tmp_var;
	for(i = 0; i < 256; i++)
	{
		tmp_var = uc_dbuf1[i * 2];
		uc_dbuf1[i * 2] = uc_dbuf1[i * 2 + 1];
		uc_dbuf1[i * 2 + 1] = tmp_var;
	}
}



/*******************************************************************************
write id3 buffer into flash
********************************************************************************/
void Ap_WriteId3_ToFlash(void)
{
	uc8 i;

	for(i = 0; i < 100; i++)
	{
		ui_dbuf0[i] = ui_id3_buf[i];
		//ui_id3_buf[i] = 0;
		ui_lrc_time_buf[i] = 0;			//ui_lrc_time_buf[100] = 200bytes
	}
	
	sfc_logblkaddr0 = 0x66;				//0x0000 0066 ~ 0x0000 0067
	sfc_logblkaddr1 = 0x00;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	b_sfc_immediate_wr_flg = 1;
	Ap_Init_Flash_Input();
}


#endif
#endif