/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_init.c
Creator: zj.zong					Date: 2009-05-21
Description: init voice(init variable and get voice parameter)
Others:
Version: V0.1
History:
	V0.1	2009-05-21		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOICE_INIT_PLAY_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Voice\ui_voice_header.h"
#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"


extern bit	FS_Fread(void);
extern void FS_Get_83Name(void);
extern ui16 Api_Isn_ConvertTo_Unicode(ui16 isn_index);
extern ui16 Api_Merge_2Byte_Sub(uc8 lo,uc8 hi);

bit Ap_Parse_WavHeader(void);
void Ap_Process_83Name(void);
void Ap_GetWav_SampleRate(void);
void Ap_GetWav_Infomation(void);

/*******************************************************************************
Description: init play,Get file name...	
uc_voice_status bit7:	file end flag
				bit6:	file error flag							
********************************************************************************/
void Ap_Init_Play(void)
{
	b_voice_file_err = 0;
	b_voice_file_end = 0;
	b_voice_dec_reset_flg = 0;
	
	uc_filldata_cnt = 0;	
	uc_voice_time_cnt = 0;
	ui_voice_cur_time = 0;
	ui_voice_total_time = 0;
	uc_voice_replay_ab_flg = 0;					//0-normal, 1:set A, 2:set B, 3:A-B
	uc_voice_fill_dbuf_flg = 0;

	ui_sys_event &= ~EVT_MOVIE_FILE_ERR;
	ui_sys_event &= ~EVT_MOVIE_FILE_END;
	ui_voice_random_idx_bk = ui_voice_wav_idx;

	if(ul_fs_file_length == 0)
	{
	    b_voice_file_err = 1;
		ui_sys_event |= EVT_VOICE_FILE_ERR;	//wav file format is error or nonsupport
		return;	
	}

	if((uc_voice_fill_dbuf_flg & 0x02) == 0)	//uc_voice_fill_dbuf_flg (bit1: 0--empty; 1--full)
	{
		uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf(0x5000) to filesystem
		if(FS_Fread() == 0)						//get one sector data to fill dbuf1
		{
			b_voice_file_end = 1;				// file end flag
			ui_sys_event |= EVT_VOICE_FILE_END;	//read file end
		}
		else
		{
			Ap_GetWav_Infomation();				//get wav header info and file name

			if(b_voice_file_err == 1)			// file error flag
			{
				ui_sys_event |= EVT_VOICE_FILE_ERR;	//wav file format is error or nonsupport
			}
		}
		
		uc_voice_fill_dbuf_flg |= 2;
	}

}


void Ap_GetWav_Infomation(void)
{
	if(Ap_Parse_WavHeader() == 1)		//rts 1bit(1--ok; 0--error)
	{
		b_voice_file_err = 0;	    
	}
	else
	{
		b_voice_file_err = 1;		 //set bit6
	}
		
	if(b_voice_file_err == 0)	 //if file right and get total time
	{
		if(ul_voice_byterate != 0)
		{
			ui_voice_total_time = ul_voice_wavdata_len / ul_voice_byterate;
		}
	}
	
	FS_Get_83Name();
	Ap_Process_83Name();
	
	Ap_GetWav_SampleRate();
}


/*******************************************************************************
parse wav header, get total time and sample rate
return: 1bit(1--parse header ok; 0--error)
********************************************************************************/
bit Ap_Parse_WavHeader(void)
{
	ui16 tmp_subchunk_id_idx;
	ul32 tmp_subchunk_size, tmp_size;
	
	if((ui_dbuf1[0] == 0x4952) && (ui_dbuf1[1] == 0x4646) && (ui_dbuf1[4] == 0x4157) && (ui_dbuf1[5] == 0x4556))//"RIFF" ""WAVE
	{
		tmp_subchunk_id_idx = 6; 	//point to the "subchunk1 ID"
		while(1)
		{
			tmp_size = (ul32)ui_dbuf1[tmp_subchunk_id_idx + 3];
			tmp_size = tmp_size << 16;
			tmp_subchunk_size = tmp_size + ui_dbuf1[tmp_subchunk_id_idx+2];
  			if((ui_dbuf1[tmp_subchunk_id_idx] == 0x6D66) && (ui_dbuf1[tmp_subchunk_id_idx + 1] == 0x2074)) 	//"fmt"
	  		{
				ui_voice_audioformat = ui_dbuf1[tmp_subchunk_id_idx + 4];			//wav format

				ul_voice_samplerate  = ui_dbuf1[tmp_subchunk_id_idx + 6];			//wav sample rate for setting DAC

				tmp_size = (ul32)ui_dbuf1[tmp_subchunk_id_idx + 9];			
				tmp_size = tmp_size << 16;
				ul_voice_byterate = tmp_size + ui_dbuf1[tmp_subchunk_id_idx + 8];	//wav byterate for calculating total time
			}
			else if( ui_dbuf1[tmp_subchunk_id_idx] == 0x6164 && ui_dbuf1[tmp_subchunk_id_idx+1]  == 0x6174  )//"data"
			{
				ul_voice_wavdata_len = tmp_subchunk_size;							//wav file length for calculating total time
				if(ul_voice_wavdata_len == 0)
				{
					ul_voice_wavdata_len = ul_fs_file_length - 512;
				}
				break;
			}

			tmp_subchunk_id_idx = tmp_subchunk_id_idx + 4 + (tmp_subchunk_size >> 1);
			if(tmp_subchunk_id_idx >= 256)
			{
				return 0;	//wav format error or nonsupport
			}
		}
		
		if(ui_voice_audioformat == 0x0001)		//PCM
		{
			return 1;
		}
		else if(ui_voice_audioformat == 0x0011) //ADPCM(Sinowealth)
		{
			return 1;
		}
		else
		{
			return 0;		//wav format error or nonsupport
		}
	}
	else
	{
	    return 0;			//wav format error or nonsupport
	}
}



/*******************************************************************************
Description: get wav sample rate for setting DACI
********************************************************************************/
void Ap_GetWav_SampleRate(void)
{
	if(ul_voice_samplerate == 48000)	
	{
		uc_daci_sr = 4;
	}
	else if(ul_voice_samplerate == 44100)
	{
		uc_daci_sr = 5;
	}
	else if(ul_voice_samplerate == 32000)
	{
		uc_daci_sr = 6;
	}
	else if(ul_voice_samplerate == 24000)
	{
		uc_daci_sr = 8;
	}
	else if(ul_voice_samplerate == 22050)
	{
		uc_daci_sr = 9;
	}
	else if(ul_voice_samplerate == 16000)
	{
		uc_daci_sr = 0xa;
	}
	else if(ul_voice_samplerate == 12000)
	{
		uc_daci_sr = 0xc;
	}
	else if(ul_voice_samplerate == 11025)
	{
		uc_daci_sr = 0xd;	
	}
	else if(ul_voice_samplerate == 8000)
	{
		uc_daci_sr = 0xe;
	}
	else
	{
		b_voice_file_err = 1;	  //set  file error
	}
}



/*******************************************************************************
Function: Ap_Process_83name()
Description: process 8.3 format item name, and convert isn into unicode
Calls:
Global:uc_fs_item_83_name[]
       ui_fs_item_name[]:  current item name(unicode)
       uc_fs_item_name_length: current item name length
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Process_83name(void)
{
	uc8 i;
	ui16 tmp_unicode;

	uc_fs_item_name_length = 0;				//init the file length = 0

	if(uc_fs_item_83_name[8] != 0x20)		//check mp3 or wav file
	{
		uc_fs_item_83_name[11] = uc_fs_item_83_name[10];
		uc_fs_item_83_name[10] = uc_fs_item_83_name[9];
		uc_fs_item_83_name[9] = uc_fs_item_83_name[8];
		uc_fs_item_83_name[8] = '.';		//add "."
	}
	
	for(i = 0; i < 12; i++)
	{
		if(uc_fs_item_83_name[i] != 0x20)	//check is "space"
		{
			tmp_unicode = Api_Isn_ConvertTo_Unicode(Api_Merge_2Byte_Sub(uc_fs_item_83_name[i], 0x00));
			if(tmp_unicode == 0)
			{
				tmp_unicode = Api_Isn_ConvertTo_Unicode(Api_Merge_2Byte_Sub(uc_fs_item_83_name[i+1], uc_fs_item_83_name[i]));
				i++;
			}

			ui_fs_item_name[uc_fs_item_name_length] = tmp_unicode;
			uc_fs_item_name_length++;
		}
	}
}

#endif