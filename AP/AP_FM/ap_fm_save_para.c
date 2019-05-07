/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_fm_save_para.c
Creator: zj.zong				Date: 2009-05-25
Description: save fm parameter when exit fm mode
Others:
Version: V0.1
History:
	V0.1	2009-15-25		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_FM_SAVE_PARA

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_FM\ui_fm_header.h"
#include ".\UI\UI_FM\ui_fm_var_ext.h"
#include ".\DRV\DRV_FM\drv_fm.h"

extern void Api_Read_SysPara_ToDbuf0(void);
extern void FS_WrFlash_Interface(void);
/*******************************************************************************
Function: Ap_Save_FM_Para()
Description: save fm parameter when exit fm mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Save_FM_Para(void)
{	
 	uc8 j;
	Api_Read_SysPara_ToDbuf0();

	uc_dbuf0[339] = uc_save_mode_idx;			//save mode
	uc_dbuf0[210] = uc_fm_band;					//0--normal band; 1--Japan band

	if(uc_fm_band_bk == 0)						//Normal Band
	{
		uc_dbuf0[211] = uc_fm_ch_total_num;
		uc_dbuf0[212] = uc_fm_cur_ch_idx;
		uc_dbuf0[213] = (uc8)(ui_fm_cur_freq >> 8);
		uc_dbuf0[214] = (uc8)ui_fm_cur_freq;
		uc_dbuf0[215] = uc_fm_mode;
		
		for(j = 0; j < FM_CH_NUM_MAX; j++)
		{
			uc_dbuf0[90 + j * 2] = (uc8)(ui_fm_preset_buf[j] >> 8);
			uc_dbuf0[91 + j * 2] = (uc8)ui_fm_preset_buf[j];
		}
	}
	else										//Japan Band
	{
		uc_dbuf0[216] = uc_fm_ch_total_num;
		uc_dbuf0[217] = uc_fm_cur_ch_idx;
		uc_dbuf0[218] = (uc8)(ui_fm_cur_freq >> 8);
		uc_dbuf0[219] = (uc8)ui_fm_cur_freq;
		uc_dbuf0[220] = uc_fm_mode;
		
		for(j = 0; j < FM_CH_NUM_MAX; j++)
		{
			uc_dbuf0[150 + j * 2] = (uc8)(ui_fm_preset_buf[j] >> 8);
			uc_dbuf0[151 + j * 2] = (uc8)ui_fm_preset_buf[j];
		}
	}

	uc_dbuf0[221] = uc_fm_volume_level;			//fm volume level
	uc_dbuf0[223] = uc_fm_signal_strength_val;

	b_sfc_immediate_wr_flg = 1;
	sfc_reserved_area_flag = 1;
	FS_WrFlash_Interface();
	sfc_reserved_area_flag = 0;					//clr reserved area flag
}


			

			

#endif