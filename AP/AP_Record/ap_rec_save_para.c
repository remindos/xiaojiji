/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_rec_save_para.c
Creator: zj.zong					Date: 2009-05-21
Description: 1. save rec parameter into flash system area
Others:
Version: V0.1
History:
	V0.1	2009-05-21		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_REC_SAVE_PARA_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"

extern void Api_Read_SysPara_ToDbuf0(void);
extern void FS_WrFlash_Interface(void);
extern void _media_readsector(void);

/*******************************************************************************
Function: Ap_SaveRec_Para()
Description: save rec parameter into flash when exited rec mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/

void Ap_SaveRec_Para(void)
{
//	MR_PCDATA &= 0xef;								//Clr PC4, 
	MR_ADCGAIN &= 0xc7;								//disable PGAEN

	Api_Read_SysPara_ToDbuf0();
	uc_dbuf0[339] = uc_save_mode_idx;					//save mode 
	uc_dbuf0[78] = b_sd_flash_flg;

	if(b_rec_samplerate_flg == 0)						//1--16KHz; 0--8KHz
	{
		uc_dbuf0[81] = 0;
	}
	else
	{
		uc_dbuf0[81] = 1;
	}

	if(b_rec_have_rec_flg == 1)
	{
		if(b_sd_flash_flg == 0)
		{
			uc_dbuf0[70] = uc_fs_cr_bgn_cluster1;			//low byte
			uc_dbuf0[71] = uc_fs_cr_bgn_cluster2;			//high byte
			uc_dbuf0[72] = (uc8)ul_fs_byte_offset;			//low byte (ul_fs_byte_offset + 3)
			uc_dbuf0[73] = (uc8)(ul_fs_byte_offset >> 8);	//high byte(ul_fs_byte_offset + 2)
		}
		else
		{
			uc_dbuf0[74] = uc_fs_cr_bgn_cluster1;			//low byte
			uc_dbuf0[75] = uc_fs_cr_bgn_cluster2;			//high byte
			uc_dbuf0[76] = (uc8)ul_fs_byte_offset;			//low byte (ul_fs_byte_offset + 3)
			uc_dbuf0[77] = (uc8)(ul_fs_byte_offset >> 8);	//high byte(ul_fs_byte_offset + 2)	
		}
	}

    b_sfc_immediate_wr_flg = 1;        					//write to flash reserved area
	FS_WrFlash_Interface();
	sfc_reserved_area_flag = 0;

}

#endif