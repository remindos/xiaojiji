/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_fm_enhance.c
Creator: zj.zong					Date: 2009-05-25
Description: subroutine with no decode for mcu
Others:
Version: V0.1
History:
	V0.1	2009-05-25		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_FM_DRV_FUNC_DEF

#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include ".\UI\UI_FM\ui_fm_header.h"
#include ".\UI\UI_FM\ui_fm_var_ext.h"
#include ".\DRV\DRV_FM\drv_fm.h"


extern void Api_Delay_1ms(uc8 time_cnt);
extern void Api_RadioSetFreq(ui16 cur_freq);
extern void Api_RadioOpen(uc8 radio_mode);
extern void Api_RadioClose(void);
extern void Api_RadioSetStereo(uc8 stereo_flg);
extern void Api_RadioSetMute(uc8 mute_flg);
extern void Api_RadioAutoSearch(uc8 search_flg, ui16 search_para);
void Ap_FM_CancelMute(void);
void Ap_FM_Turner_Init(void);


/*******************************************************************************
Function: Ap_FM_Manual_IncFreq()
Description: increase frequency manual by 0.1MHz step
Calls:
Global:ui_fm_cur_freq
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Manual_IncFreq(void)
{
    uc_fm_mode = FM_MODE_TUNING;
    ui_fm_cur_freq += FM_MANUAL_STEP;

    if(ui_fm_cur_freq > ui_fm_freq_limit_high)
    {
        ui_fm_cur_freq = ui_fm_freq_limit_low;
    }

    Api_RadioSetFreq(ui_fm_cur_freq);
}



/*******************************************************************************
Function: Ap_FM_Manual_DecFreq()
Description: decrease frequency manual by 0.1MHz step
Calls:
Global:ui_fm_cur_freq
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Manual_DecFreq(void)
{
    uc_fm_mode = FM_MODE_TUNING;
    ui_fm_cur_freq -= FM_MANUAL_STEP;

    if(ui_fm_cur_freq < ui_fm_freq_limit_low)
    {
        ui_fm_cur_freq = ui_fm_freq_limit_high;
    }

    Api_RadioSetFreq(ui_fm_cur_freq);
}



/*******************************************************************************
Function: Ap_FM_Save_CurFrequency()
Description: save current frequency while it does not existed in ui_fm_preset_buf 
             the frequency in ui_fm_preset_buf is sorted ascend
Calls:
Global:ui_fm_cur_freq, uc_fm_cur_ch_idx, uc_fm_ch_total_num
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Save_CurFrequency(void)
{
    uc8  xdata i, j;
    ui16 xdata tmp_frequency;

    for(i = 0; i < uc_fm_ch_total_num; i++)
    {
        if((ui_fm_preset_buf[i]&0x7fff) == ui_fm_cur_freq)
        {
            uc_fm_cur_ch_idx = i + 1;
            return;								//frequency already existed,get uc_fm_cur_ch_idx and return directly
        }
    }

    uc_fm_ch_total_num++;
    if(uc_fm_ch_total_num > FM_CH_NUM_MAX)
    {
        uc_fm_ch_total_num = FM_CH_NUM_MAX;
    }
    uc_fm_cur_ch_idx = uc_fm_ch_total_num;
    ui_fm_preset_buf[uc_fm_cur_ch_idx - 1] = ui_fm_cur_freq;

    for(i = 0; i < uc_fm_ch_total_num; i++)		//compare the neighboring data,and move the small data forward
    {
        for(j = uc_fm_ch_total_num-1; j > i; j--)	
        {
            if((ui_fm_preset_buf[j-1]&0x7fff) > (ui_fm_preset_buf[j]&0x7fff))
            {
                tmp_frequency = ui_fm_preset_buf[j-1];
                ui_fm_preset_buf[j-1] = ui_fm_preset_buf[j];
                ui_fm_preset_buf[j] = tmp_frequency;
            }
        }
    }

    for(i = 0; i < uc_fm_ch_total_num; i++)		//get cucrent channel index according to the current freq
    {
        if((ui_fm_preset_buf[i]&0x7fff) == ui_fm_cur_freq)
        {
            uc_fm_cur_ch_idx = i + 1;
        }
    }
}




/*******************************************************************************
Function: Ap_FM_Load_CurFrequency()
Description: save current frequency while it does not existed in ui_fm_preset_buf 
             the frequency in ui_fm_preset_buf is sorted ascend
Calls:
Global:ui_fm_cur_freq
       uc_fm_cur_ch_idx
       ui_fm_preset_buf[30]
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Load_CurFrequency(void)
{
    if(uc_fm_cur_ch_idx > uc_fm_ch_total_num)
    {
        uc_fm_cur_ch_idx = 1;
    }
	
	if(uc_fm_ch_total_num != 0)
    {
		ui_fm_cur_freq = ui_fm_preset_buf[uc_fm_cur_ch_idx-1]&0x7fff;
		Api_RadioSetFreq(ui_fm_preset_buf[uc_fm_cur_ch_idx-1]);
	}
}



/*******************************************************************************
Function: Ap_FM_Reset_PresetBuf()
Description: fill fm preset buffer with ui_fm_freq_limit_low
Calls:
Global:ui_fm_preset_buf[30]
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Reset_PresetBuf(void)
{
	uc8 i;

   	for(i = 0; i < FM_CH_NUM_MAX; i++)
   	{
       	ui_fm_preset_buf[i] = ui_fm_freq_limit_low;
   	}

   	uc_fm_cur_ch_idx = 0;
   	uc_fm_ch_total_num = 0;
}



/*******************************************************************************
Function: Ap_FM_AutoSearch()
Description: auto search
Calls:
Global:search_flg: 0--continue search; 1--end search
       search_para: search strength
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_AutoSearch(void)
{
	Api_RadioAutoSearch(0, (uc_fm_signal_strength_val<<8)|uc_fm_accumulative_val);
}


void Ap_FM_Stop_AutoSearch(void)
{
	Api_RadioAutoSearch(1, (uc_fm_signal_strength_val<<8)|uc_fm_accumulative_val);
}



/*******************************************************************************
Function: Ap_FM_Del_CurChannel()
Description: 
Calls:
Global:uc_fm_cur_ch_idx
       uc_fm_ch_total_num
       ui_fm_preset_buf[30]
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Del_CurChannel(void)
{
    uc8 xdata i, tmp;

    if(uc_fm_ch_total_num == 0)
    {
        return;
    }

    tmp = FM_CH_NUM_MAX - uc_fm_cur_ch_idx;
    i = uc_fm_cur_ch_idx;

    while(tmp > 0)
    {
        ui_fm_preset_buf[i-1] = ui_fm_preset_buf[i];
        tmp--;
        i++;
    }

    uc_fm_ch_total_num--;
}


/*******************************************************************************
Function: Ap_FM_Del_AllChannel()
Description: 
Calls:
Global:uc_fm_cur_ch_idx
       uc_fm_ch_total_num
       ui_fm_preset_buf[30]
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Del_AllChannel(void)
{
	Ap_FM_Reset_PresetBuf();
}


/*******************************************************************************
Function: Ap_FM_Turner_Init()
Description: initialize fm module
Calls:
Global:ui_fm_cur_freq
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Turner_Init(void)
{
	Api_RadioOpen(0);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
	Api_RadioSetFreq(ui_fm_cur_freq);
	Api_Delay_1ms(250);
	Api_RadioSetFreq(ui_fm_cur_freq);	//set fm freq secondly
	Ap_FM_CancelMute();
}


/*******************************************************************************
Function: Ap_FM_Turner_Close()
Description: set fm module standby
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Turner_Close(void)
{
	Api_RadioClose();
}



/*******************************************************************************
Function: Ap_FM_SetMute(),Ap_FM_CancelMute()
Description: set or cancel fm mute
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_SetMute(void)
{
	Api_RadioSetMute(1);			//1 mute on,0 mute off
}


void Ap_FM_CancelMute(void)
{
	Api_RadioSetMute(0);			//1 mute on,0 mute off
}


#ifdef AP_FM_SETSTEREO_DEF
/*******************************************************************************
Function: Ap_FM_SetStereo()
Description: set stereo
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_SetStereo(void)
{
	Api_RadioSetStereo(1);
}


/*******************************************************************************
Function: Ap_FM_SetMono()
Description: set mono
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_SetMono(void)
{
	Api_RadioSetStereo(0);
}
#endif

#endif