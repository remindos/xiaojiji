/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_fm_restore_para.c
Creator: zj.zong				Date: 2009-05-25
Description: restore fm parameter when exit fm mode
Others:
Version: V0.1
History:
	V0.1	2009-15-25		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_FM_RESTORE_PARA

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_FM\ui_fm_header.h"
#include "..\..\UI\UI_FM\ui_fm_var_ext.h"
#include ".\DRV\DRV_FM\drv_fm.h"

extern void Api_Read_SysPara_ToDbuf0(void);

void Ap_Restore_FM_Para(void);
/*******************************************************************************
Function: Ap_Restore_FM_Para()
Description: save fm parameter when exit fm mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Restore_FM_Para(void)
{
	uc8 j;
	Api_Read_SysPara_ToDbuf0();
	if((uc_dbuf0[0] == 'S') && (uc_dbuf0[1] == 'I') && (uc_dbuf0[2] == 'N') && (uc_dbuf0[3] == 'O'))
	{
		uc_fm_band = uc_dbuf0[210];			//0--normal band; 1--Japan band
		
		if(uc_fm_band == FM_BAND_NORMAL)	//Normal Band
		{
			uc_fm_ch_total_num = uc_dbuf0[211];
			uc_fm_cur_ch_idx = uc_dbuf0[212];
	
			ui_fm_cur_freq = ((ui16)uc_dbuf0[213]) << 8;
			ui_fm_cur_freq |= (ui16)uc_dbuf0[214];		
			uc_fm_mode = uc_dbuf0[215];
				
			for(j = 0; j < FM_CH_NUM_MAX; j++)
			{
				ui_fm_preset_buf[j] = ((ui16)uc_dbuf0[90 + j * 2]) << 8;
				ui_fm_preset_buf[j] |= (ui16)uc_dbuf0[91 + j * 2];
			}
		}
		else								//Japan Band
		{
			uc_fm_ch_total_num = uc_dbuf0[216];
			uc_fm_cur_ch_idx = uc_dbuf0[217];
			ui_fm_cur_freq = ((ui16)uc_dbuf0[218]) << 8;
			ui_fm_cur_freq |= (ui16)uc_dbuf0[219];
			uc_fm_mode = uc_dbuf0[220];
			
			for(j = 0; j < FM_CH_NUM_MAX; j++)
			{
				ui_fm_preset_buf[j] = ((ui16)uc_dbuf0[150 + j * 2]) << 8;
				ui_fm_preset_buf[j] |= (ui16)uc_dbuf0[151 + j * 2];
			}
		}
			
		uc_fm_volume_level = uc_dbuf0[221];	//fm volume level
			
		uc_fm_signal_strength_val = uc_dbuf0[223];	//fm single strength val
		uc_fm_accumulative_val = uc_dbuf0[224];		//fm single strength val
	}
	else
	{
		uc_fm_band = FM_BAND_NORMAL;
		uc_fm_ch_total_num = 0;
		uc_fm_cur_ch_idx = 0;
		ui_fm_cur_freq = 8700;					//87MHz
		uc_fm_mode = FM_MODE_TUNING;
		
		for(j = 0; j < FM_CH_NUM_MAX; j++)
		{
			ui_fm_preset_buf[j] = 8700;			//87MHz
		}
		
		uc_fm_volume_level = 16;
		uc_fm_signal_strength_val = 5;			//fm single strength val
		uc_fm_accumulative_val = 100;			//fm single strength val
	}

/*init these variable according to the fm band*/
	if(uc_fm_band == FM_BAND_NORMAL)
	{
		ui_fm_freq_limit_low = FM_LOW_LIMIT_NOR;
		ui_fm_freq_limit_high = FM_HIGH_LIMIT_NOR;
	}
	else
	{
		ui_fm_freq_limit_low = FM_LOW_LIMIT_JP;
		ui_fm_freq_limit_high = FM_HIGH_LIMIT_JP;
	}

	if((uc_fm_mode != FM_MODE_TUNING) && (uc_fm_mode != FM_MODE_PRESET))
	{
		uc_fm_mode = FM_MODE_TUNING;
	}	
}



			

			

#endif