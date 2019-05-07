/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_init.c
Creator: aj.zhao					Date: 2009-04-27
Description: initial music (include variable and get music parameter)
Others:
Version: V0.1
History:
	V0.1	2009-04-27		aj.zhao
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef SP_PLAY_RESERVEDMP3_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include "hwdriver_resident.h"

#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"

extern Api_Cmd_Dbuf1_Filled_ValidData(void);
extern Api_Cmd_Dbuf1_Filled_InvalidData(void);

extern Read_Ctra(void);
extern _media_readsector(void);
extern Ap_Get_BitRate(void);
void ef_Read(void);


/*******************************************************************************
Ap functions for play reserved mp3
1. Ap_Rsv_Get_MP3Number(): get mp3 number in reserved area
   output: ui_rsv_mp3_total_num---mp3 number
           b_rsv_mp3_flg---flag
2. Ap_Rsv_Fill_Debuf_Firstly()
   Ap_Rsv_InitData()
   Ap_Rsv_FillMp3data()
********************************************************************************/


#ifdef SP_PLAY_RSV_INIT
void Ap_Rsv_Get_MP3Number(void)
{
	MR_LCMNUMH = 0x08; 							//segment name 'MR_LCMNUMH + 2' (code infomation is lie in segment 10)
	Read_Ctra();
	b_rsv_mp3_flg = 0;
	ui_rsv_mp3_idx = 0;
	sfc_reserved_area_flag = 1;					//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_DBUF0;			//occupy dbuf 0x4000	buf_offset0 = 0x00;					//buffer offset :0x0000
	buf_offset0 = 0x00;							//buffer offset :0x0000
	buf_offset1 = 0x00;
	_media_readsector();//read one sector
	sfc_reserved_area_flag = 0;					//read flash reserved area

	if((uc_dbuf0[4] == 'U') && (uc_dbuf0[5] == 'S') && (uc_dbuf0[6] == 'E') && (uc_dbuf0[7] == 'R'))
	{	
		ui_rsv_mp3_total_num = ui_dbuf0[1];
		b_rsv_mp3_flg = 1;
		ui_rsv_mp3_idx = 1;
	}		
}
#else



void Ap_Rsv_Fill_Debuf_Firstly(void)
{
	mMPEG_Release_Dbuf1;						//mpeg release DBUF to MCU
  	MR_LCMNUMH = 0x08; 		//segment name 'MR_LCMNUMH + 2' (code infomation is lie in segment 10)
	Read_Ctra();

	ul_rsv_mp3_addr = ((ul32)sfc_logblkaddr3) << 24;	//high
	ul_rsv_mp3_addr |= ((ul32)sfc_logblkaddr2) << 16;
	ul_rsv_mp3_addr |= ((ul32)sfc_logblkaddr1) << 8;
	ul_rsv_mp3_addr |= (ul32)sfc_logblkaddr0;		//low

	ul_rsv_mp3_addr +=  ((ul32)ui_rsv_mp3_idx) >> 6 ;

	ef_Read();
	ul_rsv_mp3_addr_bk = ul_dbuf1[(ui_rsv_mp3_idx & 0x003f)*0x02];
	ui_rsv_mp3_total_sec = (ui16)uc_dbuf1[(ui_rsv_mp3_idx & 0x003f)*0x08+0x05]<<8;	   //max 32M
	ui_rsv_mp3_total_sec |= (ui16)uc_dbuf1[(ui_rsv_mp3_idx & 0x003f)*0x08+0x06];	   //max 32M

	ul_rsv_mp3_addr = ul_rsv_mp3_addr_bk;
	ef_Read();
  
}


void ef_Read(void)
{
  	sfc_logblkaddr3 = (char)(ul_rsv_mp3_addr>>24);		   
	sfc_logblkaddr2 = (char)(ul_rsv_mp3_addr>>16);		   
	sfc_logblkaddr1 = (char)(ul_rsv_mp3_addr>>8);
	sfc_logblkaddr0 = (char)(ul_rsv_mp3_addr);

	sfc_reserved_area_flag = 1;					//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_DBUF1;			//occupy dbuf 0x5000	buf_offset0 = 0x00;					//buffer offset :0x0000
	buf_offset0 = 0x00;							//buffer offset :0x0800
	buf_offset1 = 0x00;
	_media_readsector();//read one sector
	sfc_reserved_area_flag = 0;					//read flash reserved area
}



void Ap_Cal_TotalTime_ef(void)
{
	ui_mp3_bitrate = Ap_Get_BitRate();
	ui_mp3_total_time = (int)(((ul32)ui_rsv_mp3_total_sec*512)/(ui_mp3_bitrate *125));
}


void Ap_Rsv_InitData(void)
{
	Ap_Cal_TotalTime_ef();
	mMPEG_Release_Dbuf1;						//mpeg release DBUF to MCU

	ui_rsv_mp3_sec_cnt = 0;
	ul_rsv_mp3_addr = ul_rsv_mp3_addr_bk;
	ef_Read();
	mMPEG_Occupy_Dbuf1;
}


void Ap_Rsv_FillMp3data(void)
{
	ui_rsv_mp3_sec_cnt += 1;//no sector number + 1	shuwei
	ul_rsv_mp3_addr += 1;//sector address + 1		shuwei
		
	ef_Read();

	if(ui_rsv_mp3_sec_cnt < ui_rsv_mp3_total_sec)//file data over 
	{
		mMPEG_Occupy_Dbuf1;			//mpeg occupy DBUF1
		Api_Cmd_Dbuf1_Filled_ValidData();	//dbuf1 be filled with valid data
	}
	else
	{
		ui_rsv_mp3_sec_cnt = 0;		//file over sector	number is 0		shuwei
		b_mp3_file_end = 1;					
		ui_sys_event |= EVT_MUSIC_FILE_END;
		mMPEG_Occupy_Dbuf1;			//mpeg occupy DBUF1
		Api_Cmd_Dbuf1_Filled_InvalidData();//dbuf1 be filled with invalid data 
	}
}



#endif
#endif