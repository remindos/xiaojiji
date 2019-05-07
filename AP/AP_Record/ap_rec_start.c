/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_rec_start.c
Creator: zj.zong				Date: 2009-06-02
Description: start record
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_RECORD_START_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"

extern bit FS_Fcreate(void);
extern void FS_Fwrite(void);
extern void _media_readsector(void);

void modify_wav_header(ul32 byte_offset_var);
void Ap_Clear_Dbuf(void);
void Ap_Init_ADPCM_Enc(void);
/*******************************************************************************
Function: Ap_StartRecord_Sub()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_StartRecord_Sub()
{
	if((b_rec_card_full_flg == 0) && (b_rec_wav_full_flg == 0))
	{
		if(ul_rec_remain_time == 0)				  //20091124
		{
			goto label_card_full;
		}
		if(FS_Fcreate() == 0)			//creat record file
		{
label_card_full:
			b_rec_card_full_flg = 1;
		}
		else
		{
			b_rec_card_full_flg = 0;
			b_flicker_time_flg = 0;

			uc_rec_wav_map_buf[uc_rec_sav_map_idx] |= uc_rec_sav_map_val;
			uc_rec_status = STATE_RECORD;
			
			Ap_Init_ADPCM_Enc();
			
			uc_fs_assign_buf_flg = SFC_APP_CBUF;//write cbuf0
			FS_Fwrite();
			b_rec_mode_flg = 1;			//set recording flag
			b_rec_have_rec_flg = 1;		//have recorded
		}
	}
}

void Ap_Init_ADPCM_Enc(void)
{
	uc8 i;
	
	b_rec_decbuf_flg = 0;	//0:fill dbuf0; 1:fill dbuf1
	uc_enc_step_idx = 0;
	uc_rec_time_cnt = 0;
	
	uc_rec_adcbuf_dph = (uc8)(X_ADDR_DBUF0 >> 8);
	uc_rec_adcbuf_dpl = (uc8)X_ADDR_DBUF0;
	ui_rec_dptr_var = 0x0000;
	b_rec_adcbuf_full_flg = 1;
	
	for(i = 0; i < 4; i++)
	{
		si_rec_adc_inbuf[i] = 0;
		si_rec_adc_outbuf[i] = 0;
	}	
	Ap_Clear_Dbuf();			//clr dbuf0 & dbuf1
}

void Ap_Clear_Dbuf(void)		//DBUF0 & DBUF1
{
	ui16 i;
	for(i = 0; i < 2304; i++)
	{
		uc_dbuf0[i] = 0;
		uc_dbuf1[i] = 0;
	}

	for(i = 0; i < 512; i++)
	{
		uc_cbuf[0x3000+i] = 0;
	}
}


#endif