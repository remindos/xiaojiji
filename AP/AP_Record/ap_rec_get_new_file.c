/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_rec_get_new_file.c
Creator: zj.zong					Date: 2009-06-02
Description:
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_RECORD_GET_NEW_FILE_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"


extern uc8  FS_Sector_To_Cluster(void);

void Ap_Get_NewRecFile(void);
bit  Ap_Get_NextWav_Idx(void);
void Ap_EZ_Cal_RemainTime(void);
/*******************************************************************************
Function: Ap_Get_NewRecFile()
Description: get new record file index, and calculate the remaining time
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Get_NewRecFile(void)
{
	if(Ap_Get_NextWav_Idx() == 0)			//create new file(*.wav)
	{
		b_rec_wav_full_flg = 1;
	}

	Ap_EZ_Cal_RemainTime();
	ul_rec_cur_time=0;
	b_flicker_time_flg=0;
}

/*******************************************************************************
get the next wav record file name(1--success; 0--wav index is full)
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

/****************************************************************
calculate record remaining time according to disk space	
****************************************************************/
void Ap_EZ_Cal_RemainTime(void)
{
	uc8 tmp_exp;
	ul32 tmp_capacity;

	tmp_exp = FS_Sector_To_Cluster();

	tmp_capacity = (((ul_fs_byte_offset >> (9 + tmp_exp)) + 1) << tmp_exp);	 //20091124
	if(ul_fs_spare_space < tmp_capacity)
	{
		ul_fs_spare_space = 0;
	}
	else
	{
		ul_fs_spare_space -= tmp_capacity;
	}

	if(b_rec_samplerate_flg == 1)
	{
		ul_rec_remain_time = ul_fs_spare_space * 101 / 1600;
	}
	else
	{
		ul_rec_remain_time = ul_fs_spare_space * 101 / 800;
	}
}

#endif