/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_rec_encode.c
Creator: zj.zong					Date: 2009-06-02
Description: encode, compress 1010words into 254words
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_RECORD_ENCODE_PROCESS_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"


extern void Ap_Generate_Nibble();
extern void Api_DynamicLoad_MainMenu(void);
extern void Api_Read_SysPara_ToDbuf0(void);
extern void Api_DynamicLoad_Record_Target(uc8 load_code_idx);
extern uc8  FS_Sector_To_Cluster(void);

void Ap_SetStopRec(void);
void Ap_Exit_RecMode(void);
void Ap_PowerOff_MP3(void);
void Ap_SaveRec_Para(void);
bit  Ap_Get_NextWav_Idx(void);
void Ap_EZ_Cal_RemainTime(void);
void Ap_SwitchTo_MainMenu(void);
void Ap_Wr_ADC_CompressData(ui16 tmp_cur_val);
/*******************************************************************************
Function: Ap_Encode_Block()
Description: encode, compress dbuf data into cbuf, then fwrite
Calls:
Global:si_dbuf0[], si_dbuf1[], b_rec_decbuf_flg, ui_cbuf
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Encode_Block()
{
	ui16 tmp_enc_code, tmp_mic_sample, tmp_sample_inx;
	ui16 tmp_sample_buf[4];
	si16 tmp_raw_data;
	uc8  tmp_fill_samplebuf_idx;
	si16 tmp1, tmp2, tmp3, tmp4, tmp5;

	tmp_fill_samplebuf_idx = 0;
	ui_rec_dptr_var = 0x0000;			//combuf addr for compress adc data
	
	for( tmp_sample_inx = 0; tmp_sample_inx < 1010;  tmp_sample_inx++ )
	{
		if(b_rec_decbuf_flg == 1)		//b_rec_decbuf_flg = 1, meaning dbuf0 is full
		{
			tmp_raw_data = si_dbuf0[tmp_sample_inx];
		}
		else
		{
			tmp_raw_data = si_dbuf1[tmp_sample_inx];
		}

		if(b_rec_samplerate_flg == 1)
		{
			tmp1 = (tmp_raw_data >> 3) + (si_rec_adc_inbuf[0] >> 3);			//[x(n)+x(n-4)]*1/8
			tmp2 = ((si_rec_adc_inbuf[3] >> 3) + (si_rec_adc_inbuf[1] >> 3)) * 3;	//[x(n-1)+x(n-3)]*3/8
			tmp3 = si_rec_adc_inbuf[2] >> 1;								//x(n-2)*1/2
			tmp4 = si_rec_adc_outbuf[2] >> 1;								//y(n-2)*1/2
			tmp5 = si_rec_adc_outbuf[0] >> 6;								//y(n-4)*1/64
			
			si_rec_adc_inbuf[0] = si_rec_adc_inbuf[1];
			si_rec_adc_inbuf[1] = si_rec_adc_inbuf[2];
			si_rec_adc_inbuf[2] = si_rec_adc_inbuf[3];
			si_rec_adc_inbuf[3] = tmp_raw_data;
			
			tmp_raw_data = tmp1 - tmp4 + tmp2 - tmp5 + tmp3;
			
			si_rec_adc_outbuf[0] = si_rec_adc_outbuf[1];
			si_rec_adc_outbuf[1] = si_rec_adc_outbuf[2];
			si_rec_adc_outbuf[2] = si_rec_adc_outbuf[3];
			si_rec_adc_outbuf[3] = tmp_raw_data;
		}

		tmp_mic_sample = tmp_raw_data;
		
		if( tmp_sample_inx == 0 || tmp_sample_inx == 505)
		{
			ui_enc_sample = tmp_mic_sample;
			Ap_Wr_ADC_CompressData(tmp_mic_sample);
			Ap_Wr_ADC_CompressData(uc_enc_step_idx);
		}
		else
		{
			tmp_sample_buf[tmp_fill_samplebuf_idx] = tmp_mic_sample;
			tmp_fill_samplebuf_idx++;
			
			if(tmp_fill_samplebuf_idx == 4)
			{
				ui_enc_indata = tmp_sample_buf[0];
				Ap_Generate_Nibble();
				tmp_enc_code = uc_enc_thecode;

				ui_enc_indata = tmp_sample_buf[1];
				Ap_Generate_Nibble();
				tmp_enc_code |= (uc_enc_thecode << 4);
	
				ui_enc_indata = tmp_sample_buf[2];
				Ap_Generate_Nibble();
				tmp_enc_code |= (uc_enc_thecode << 8);
	
				ui_enc_indata = tmp_sample_buf[3];
				Ap_Generate_Nibble();
				tmp_enc_code |= (uc_enc_thecode << 12);

				Ap_Wr_ADC_CompressData(tmp_enc_code);
				tmp_fill_samplebuf_idx = 0;
			}
		}
	}
}

/*******************************************************************************
write copress data to cbuf
********************************************************************************/
void Ap_Wr_ADC_CompressData(ui16 tmp_cur_val)	//write compress 
{
	ui_cbuf[ui_rec_dptr_var + X_ADDR_REC_ENC_BUF] = tmp_cur_val;
	ui_rec_dptr_var++;							//addr + 2bytes
}


/*******************************************************************************
Function: Ap_SwitchTo_MainMenu()
Description: return to main menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SwitchTo_MainMenu(void)
{
	Ap_Exit_RecMode();						//stop record and save record parameter
	Api_DynamicLoad_MainMenu();			 	//enter top menu
}

/*******************************************************************************
exit record mode
********************************************************************************/
void Ap_Exit_RecMode(void)
{
	if(uc_rec_status != STATE_STOP)
	{
		Ap_SetStopRec();			  //stop record
	}
  	MR_DACCLK |= 0x04;				  //set VREFDAC is grounded
	Api_DynamicLoad_Record_Target(CODE_RECORD_6_SAVE_IDX);
}

/*******************************************************************************
Function: Ap_PowerOff_MP3()
Description: power off mp3
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_PowerOff_MP3(void)
{
	Ap_Exit_RecMode();				//stop record and save record parameter			  		
	Api_DynamicLoad_PowerOff();		//enter power off mp3 mode
}										

#endif