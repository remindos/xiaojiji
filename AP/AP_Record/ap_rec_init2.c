/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_record_init2.c
Creator: zj.zong				Date: 2009-06-02
Description: init record (init variable and get record parameter)
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_RECORD_INIT2_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"

extern bit  FS_Fopen_Appointed(ui16 file_idx);
extern void FS_EnterRootDir(void);
extern bit  FS_EnterSubDir(ui16 dir_index);
extern void FS_Get_83Name(void);
extern void FS_GetSpace(void);
extern void FS_Get_ItemNum_Indir(void);
extern void FS_Get_NextFile_InDir(void);
extern void FS_GetItemNum_InDir(void);
extern void Api_Delay_1ms(uc8 timer_cnt);

void Ap_Init_ADC(void);
extern void Ui_SetLcdFlag_0xfd(void);


void Ap_CalRec_RemainTime(void);
void Ap_Set_WavMap(void);
void Ap_Scan_Wav_File(void);
bit Ap_Get_NextWav_Idx(void);

/*******************************************************************************
Function: Ap_Rec_Init2()
description: init record
             1. init record variable
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Rec_Init2(void)
 {	uc_fs_wav_idx = 0;
	ui_rec_wav_total_num = 0;
 
	if(	b_rec_card_full_flg == 0)
	{
	    mSet_MCUClock_33M;					
		FS_EnterRootDir();				//enter "root"
		
		FS_EnterSubDir(ui_fs_wav_dir_idx);	//enter "voice" dir
		
		Ap_Scan_Wav_File();
		
		if(Ap_Get_NextWav_Idx() == 0)	//get next wav index
		{
			b_rec_wav_full_flg = 1;
			uc_fs_wav_idx = 0;
			ui_rec_wav_total_num = 255;	//wav file max number is 255
		}
		
		FS_GetSpace();				//according to system setting, wav ui_enc_sample rate
		Ap_CalRec_RemainTime();
		mSet_MCUClock_22M;			//calculate the remainder time
	}

	Ap_Init_ADC();

	uc_save_mode_idx = MODE_2_RECORD;
	uc_window_no = WINDOW_REC;			//set record window
	b_lcd_clrall_flg = 1;
	uc_lcd_dis_flg= 0xfd;					//bit 1 hold picture
	ui_sys_event |= EVT_DISPLAY;			//set the display event
}

/*******************************************************************************
scan WAV file, set corresponding bit in var uc_rec_wav_map_buf 
if one WAV file exists
********************************************************************************/
void Ap_Scan_Wav_File(void)
{
	ui16 i;
	
	for(i = 0; i < 32; i++)
	{
		uc_rec_wav_map_buf[i] = 0;
	}
	
	FS_GetItemNum_InDir();
	for(i = 0; i < ui_fs_special_file_num_indir; i++)
	{
		if(i == 0)
		{
			FS_Fopen_Appointed(i);
		}
		else
		{
			FS_Get_NextFile_InDir();
		}
		FS_Get_83Name();
		Ap_Set_WavMap();
	}
}



/*******************************************************************************
check if the file is the WAV file which matchs formatif be,
set corresponding bit in uc_rec_wav_map_buf,
the wav max number is 255
********************************************************************************/
void Ap_Set_WavMap(void)
{
	ui16 tmp_wav_idx;
	uc8 tmp_map_byte, tmp_map_bit, i = 1;

	if((uc_fs_item_83_name[8] == 'W') && (uc_fs_item_83_name[9] == 'A') && (uc_fs_item_83_name[10] == 'V'))
	{
		ui_rec_wav_total_num++;

		if((uc_fs_item_83_name[0] == 'M') && (uc_fs_item_83_name[1] == 'I') && (uc_fs_item_83_name[2] == 'C')
	 	&& (uc_fs_item_83_name[3] == 'R') && (uc_fs_item_83_name[4] == '0') && (uc_fs_item_83_name[5] > '/')
	 	&& (uc_fs_item_83_name[5]  < '3') && (uc_fs_item_83_name[6]  > '/') && (uc_fs_item_83_name[6] < ':')
	 	&& (uc_fs_item_83_name[7]  > '/') && (uc_fs_item_83_name[7]  < ':'))
	 	{
			tmp_wav_idx = (uc_fs_item_83_name[5] - '0') * 100 + (uc_fs_item_83_name[6] - '0') * 10
						+ (uc_fs_item_83_name[7] - '0');
			
			if((tmp_wav_idx > 0) && (tmp_wav_idx < 256))
			{
				tmp_wav_idx--;
				tmp_map_byte = (uc8)tmp_wav_idx / 8;
				tmp_map_bit = (uc8)tmp_wav_idx % 8;
				i = i << tmp_map_bit;
				uc_rec_wav_map_buf[tmp_map_byte] |= i;
			}
	 	}
	}
}



/*******************************************************************************
Function: Ap_Get_NextWav_Idx()
description: check uc_rec_wav_map_buf[32], get the blank wav index,
			 take it as next wav record file name
Calls:
Global:uc_fs_wav_idx
Input: Null
Output:1bit(1--success; 0--wav index is full)
Others:
********************************************************************************/
bit Ap_Get_NextWav_Idx(void)
{
	uc8 i, j, k;

	for(i = 0; i < 32; i++)
	{
		if(uc_rec_wav_map_buf[i] != 0xff)
		{
			for(j = 1, k = 1; j != 0; k++)
			{
				if((uc_rec_wav_map_buf[i] & j) == 0)
				{
					uc_fs_wav_idx = i * 8 + k;
					if(uc_fs_wav_idx == 0)
					{
						return 0;
					}
					uc_rec_sav_map_idx = i;
					uc_rec_sav_map_val = j;
					return 1;
				}
				
				j = j << 1;
			}
		}
	}
	
	return 0;
}


/*******************************************************************************
calcuate record remain time according to disk space	
********************************************************************************/
void Ap_CalRec_RemainTime(void)
{
	if(b_rec_samplerate_flg == 1)				//16KHz sample rate(collect 16*1024 byte per second)
	{
		ul_rec_remain_time = ul_fs_spare_space * 101 / 1600;
	}
	else										//8KHz sample rate(collect 8*1024 byte per second)
	{
		ul_rec_remain_time = ul_fs_spare_space * 101 / 800;
	}
}


/*******************************************************************************
Function:   Ap_Init_ADC()
Description: interface for initial module ADC
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_Init_ADC(void)
{
	MR_MODEN1 |= 0x04;				//set bit2, enable ADC
	
	if(b_rec_samplerate_flg == 1)	//16KHz sample rate
	{
		MR_SYSCLK |= 0x40;			//bit6: 1--256KHz, used for 16K sample rate
	}
	else
	{
		MR_SYSCLK &= 0xbf;			//bit6: 0--128KHz, used for 8K sample rate
	}
	
	MR_ADCCON = 0x84;				//bit7: 1--enable adc interrupt; bit2: 1--enable DC filter
	MR_ADCGAIN = 0xc0;				//bit7:	1--mic 23 dB amplifier; bit6: 1--PGA is powered-on;
									//bit2~0: PGA gain setting, 4--24dB
	MR_DACCLK &= 0xfb;				//clr bit2	
}
#endif