/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_rec_stop.c
Creator: zj.zong				Date: 2009-06-02
Description: stop record, write file header and fatbuf, etc.
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_RECORD_STOP_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"

extern void FS_Fwrite_Header(void);
extern void FS_Fwrite_Over(void);
extern void _media_readsector(void);

void Ap_Modify_WavHeader(ul32 byte_offset_var);

/*******************************************************************************
Function: Ap_Stop_RecSub()
Description: stop record, write file header and fatbuf, etc.
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Stop_RecSub(void)
{
	ul_fs_byte_offset -= 512;
	ul_rec_sample_total_num = (ul_fs_byte_offset >> 9) * 1010;	//sector num * 1010
	Ap_Modify_WavHeader(ul_fs_byte_offset + 504);	//504=512-8;

	FS_Fwrite_Header();				//write header (ef common buffer)
  	ul_fs_byte_offset += 512;
	FS_Fwrite_Over();				//write fat bufer
	ui_rec_wav_total_num++;
}



void Ap_Modify_WavHeader(ul32 byte_offset_var)
{
	if(b_rec_samplerate_flg == 1)							//16KHz sample rate
	{
		sfc_logblkaddr0 = 0x07 + uc_reserve_addr0;
	}
	else									   				//8KHz sample rate
	{
		sfc_logblkaddr0 = 0x06 + uc_reserve_addr0;
	}
	sfc_logblkaddr1 = uc_reserve_addr1;
	sfc_logblkaddr2 = uc_reserve_addr2;
	sfc_logblkaddr3 = uc_reserve_addr3;
	sfc_occ_buf_flag = SFC_APP_FBUF;						//occupy FBUF
	sfc_reserved_area_flag = 1;								//read flash reserved area
	buf_offset0 = (uc8)FCOMBUF_OFFSET_W;						//buffer offset :0x0000
	buf_offset1 = (uc8)(FCOMBUF_OFFSET_W >> 8);
	_media_readsector();										//read one sector

	sfc_reserved_area_flag = 0;					//clr reserved area flag
	
	uc_sfc_combuf[4] = (uc8)(byte_offset_var >> 8);				//byte_offset_var + 2
	uc_sfc_combuf[5] = (uc8)byte_offset_var;					//byte_offset_var + 3
	uc_sfc_combuf[6] = (uc8)(byte_offset_var >> 24);			//byte_offset_var + 0
	uc_sfc_combuf[7] = (uc8)(byte_offset_var >> 16);			//byte_offset_var + 1

	uc_sfc_combuf[48] = (uc8)(ul_rec_sample_total_num >> 8);	//ul_rec_sample_total_num + 2
	uc_sfc_combuf[49] = (uc8)ul_rec_sample_total_num;			//ul_rec_sample_total_num + 3
	uc_sfc_combuf[50] = (uc8)(ul_rec_sample_total_num >> 24);	//ul_rec_sample_total_num + 0
	uc_sfc_combuf[51] = (uc8)(ul_rec_sample_total_num >> 16);	//ul_rec_sample_total_num + 1

	uc_sfc_combuf[508] = (uc8)(ul_fs_byte_offset >> 8);			//ul_fs_byte_offset + 2
	uc_sfc_combuf[509] = (uc8)ul_fs_byte_offset;				//ul_fs_byte_offset + 3
	uc_sfc_combuf[510] = (uc8)(ul_fs_byte_offset >> 24);		//ul_fs_byte_offset + 0
	uc_sfc_combuf[511] = (uc8)(ul_fs_byte_offset >> 16);		//ul_fs_byte_offset + 1
}


#endif