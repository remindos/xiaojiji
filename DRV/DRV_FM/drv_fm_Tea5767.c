/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_fm_Tea5767.c
Creator: andyliu				Date: 2009-07-18
Description: fm driver function, fm module is philips TEA5767
Others:
Version: V0.1
History:
	V0.1	2009-07-18		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef DRV_FM_TEA5767
#include "sh86278_sfr_reg.h"
#include "drv_fm.h"
#include "drv_fm_var_ext.h"

extern void _fm_i2c_enable(void);
extern void _fm_i2c_disable(void);
extern void _fm_i2c_write(uc8 wr_cnt);
extern void _fm_i2c_read(uc8 rd_cnt);

void fm_i2c_reset_hlsi(void);
void fm_i2c_tune_over(void);
void Api_RadioSetFreq(ui16 radio_freq);
void fm_search_one_freq(void);
void _delay_1ms(uc8 timer_cnt);
void fm_i2c_decide_hlsi(void);
void Api_RadioOpen(uc8 radio_mode);
void Api_RadioClose(void);
void Api_RadioSetMute(uc8 mute_flag);
void Api_RadioSetStereo(uc8 stereo_flag);
void fm_i2c_calculate_freq(void);
void fm_tuning(void);

/*******************************************************************************
Function: Api_RadioInit()
Description: initialize and enter standby mode;
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioInit(void)
{
	Api_RadioOpen(0);
	Api_RadioClose();
}



/*******************************************************************************
Function: Api_RadioOpen()
Description: initialize fm module
Calls:
Global:radio_mode:0--FM; 1--AM
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioOpen(uc8 radio_mode)
{
	if(radio_mode == 1)		//1 is AM mode
	{
		return;
	}

	uc_fm_high_injection_flg = 0;
	uc_fm_valid_cnt = 0;
	uc_fm_read_bytes[0] = 0;
	uc_fm_read_bytes[1] = 0;
	uc_fm_read_bytes[2] = 0;
	uc_fm_read_bytes[3] = 0;
	uc_fm_read_bytes[4] = 0;
	
	uc_fm_write_bytes[0] = FM_REG_MUTE & (~FM_REG_SM); 	//CLR "SM", set mute
	uc_fm_write_bytes[1] = 0;
	uc_fm_write_bytes[2] = (FM_REG_SUD | FM_REG_SSL1) & (~(FM_REG_SSL0 | FM_REG_MS));
	uc_fm_write_bytes[3] = FM_REG_XTAL | FM_REG_SMUTE | FM_REG_SNC;
	uc_fm_write_bytes[4] = 0;
		
	fm_i2c_calculate_freq();
	_fm_i2c_write(5);
}



/*******************************************************************************
Function: Api_RadioClose()
Description: mute the tuner and set it standby
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioClose(void)
{
	uc_fm_write_bytes[0] |= FM_REG_MUTE;
	uc_fm_write_bytes[3] |= FM_REG_STBY;
	_fm_i2c_write(4);
	
	FM_CLK_TYPE &= ~FM_CLK_MASK; 		//set the fm clk&data as input
	FM_DATA_TYPE &= ~FM_DATA_MASK;
}



/*******************************************************************************
Function: Api_RadioSetFreq()
Description: set as normal mode first, write PLL(get from ui_fm_cur_freq), 
             and then set mute off
Calls:
Global:ui_fm_cur_freq
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioSetFreq(ui16 radio_freq)
{
	ui_fm_cur_freq = radio_freq;
	
	uc_fm_write_bytes[0] &= ~FM_REG_SM;		//set fm normal mode
	
	fm_i2c_decide_hlsi();
	
	fm_i2c_calculate_freq();
	_fm_i2c_write(3);
	
	_delay_1ms(10);
	
	fm_i2c_tune_over();						//get current frequency uc_fm_signal_strength

	ui_fm_cur_freq &= 0x7fff;				//clr bit15
}




/*******************************************************************************
Function: Api_RadioGetFreq()
Description: calculate frequency from the read PLL,store in ui_fm_cur_freq
Calls:
Global:ui_fm_cur_freq
Input: Null
Output:Null
Others:
********************************************************************************/
int Api_RadioGetFreq(void)
{
	uc8  xdata tmp_pll_l, tmp_pll_h;
	ui16 xdata tmp_pll;
	ul32 xdata tmp_freq;

	_fm_i2c_read(4);
	tmp_pll_l = uc_fm_read_bytes[1];
	tmp_pll_h = uc_fm_read_bytes[0];
	tmp_pll_h &= 0x3f;
	tmp_pll = tmp_pll_h * 256 + tmp_pll_l;

	if(uc_fm_high_injection_flg)
	{
		tmp_freq = ((ul32)tmp_pll) * 8192 / 1000 - 225;	//unit:1KHz
	}
	else
	{
		tmp_freq = ((ul32)tmp_pll) * 8192 / 1000 + 225;	//unit:1KHz
	}

	ui_fm_cur_freq = (ui16)(tmp_freq / 10);				//unit:10KHz
	
	return ui_fm_cur_freq;
}



/*******************************************************************************
Function: Api_RadioGetStereoFlag()
Description: get fm stereo or mono
Calls:
Global:uc_radio_status: bit6=1 is stereo, 0 is mono
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioGetStereoFlag(void)
{
	_fm_i2c_read(4);
	if(uc_fm_read_bytes[2]&0x80)
	{
		uc_radio_status |= 0x40;		//stereo
	}
	else
	{
		uc_radio_status &= ~0x40;		//mono
	}
}




/*******************************************************************************
Function: Api_RadioSetMute()
Description: set fm mute or cancel fm mute
Calls:
Global:uc_radio_status: bit5=1 is mute, 0 is mute off
Input: mute_flag: 1--mute on, 0--cancel mute
Output:Null
Others:
********************************************************************************/
void Api_RadioSetMute(uc8 mute_flag)
{
	if(mute_flag)		//mute
	{
		uc_fm_write_bytes[0] |= FM_REG_MUTE;
		uc_radio_status |= 0x20;
	}
	else				//cancel mute
	{
		uc_fm_write_bytes[0] &= ~FM_REG_MUTE;
		uc_radio_status &= ~0x20;
	}
	
	_fm_i2c_write(1);
	
}



/*******************************************************************************
Function: Api_RadioSetStereo()
Description: set fm stereo or mono
Calls:
Global:
Input: stereo_flag: 1 is force strreo, 0 is force mono
Output:Null
Others:
********************************************************************************/
void Api_RadioSetStereo(uc8 stereo_flag)
{
	if(stereo_flag)	//force stereo
	{
		uc_fm_write_bytes[2] &= ~FM_REG_MS;
	}
	else			//force mono
	{
		uc_fm_write_bytes[2] |= FM_REG_MS;
	}
	
	_fm_i2c_write(3);
}



/*******************************************************************************
Function: Api_RadioAutoSearch()
Description: check the high/low side injection, and set uc_fm_write_bytes[2] bit4 "HLSI"
Calls:
Global:ui_fm_cur_freq
       uc_fm_mode :0x14 means halfsearch inc
                   0x1c means halfsearch dec
                   0x03 means autosearch
                   0x01 means autosearch finished
                   0x02 means halfsearch finished or reach band limit
       uc_fm_ch_total_num, uc_fm_cur_ch_idx: valid while autosearch
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioAutoSearch(uc8 search_flg, ui16 search_para)
{
	uc8 i;

	uc_fm_signal_strength_val = (uc8)(search_para>>8);
	uc_fm_accumulative_val = (uc8)search_para;
	if(search_flg == 0)						//continue search
	{
		switch(uc_fm_mode)
		{
			case FM_MODE_HALFSEARCH_INC:	//halfsearch inc freq
			case FM_MODE_HALFSEARCH_DEC:	//halfsearch dec freq
				fm_tuning();
				break;
			
			default:						//autosearch from low limit to high limit 
				if(uc_fm_mode != FM_MODE_AUTOSEARCH)//initial variants while first search start
				{
    				uc_fm_mode = FM_MODE_AUTOSEARCH; //auto search mode
					ui_fm_cur_freq = ui_fm_freq_limit_low - FM_AUTO_STEP;
					uc_fm_valid_cnt = 0;
					ui_fm_first_valid_freq = ui_fm_freq_limit_low;
					ui_fm_cur_freq_bk = ui_fm_freq_limit_low;
  	 				for(i = 0; i < FM_CH_NUM_MAX; i++)
  	 				{
      	 				ui_fm_preset_buf[i] = ui_fm_freq_limit_low;
  	 				}
  	 				uc_fm_cur_ch_idx = 0;
  	 				uc_fm_ch_total_num = 0;
				}
				fm_tuning();
				break;
		}
	}
	else		//end search
	{
		uc_fm_valid_cnt = 0;
		uc_fm_mode = FM_MODE_TUNING;
		Api_RadioSetFreq(ui_fm_cur_freq);
	}
}




void fm_tuning(void)
{
	uc8  xdata tmp_fm_valid_counter;
	ui16 xdata tmp_fm_freq;

	tmp_fm_freq = ui_fm_cur_freq + FM_AUTO_STEP;	//default is increase fm step

	if(uc_fm_mode == FM_MODE_AUTOSEARCH)			//auto search
	{
		if(tmp_fm_freq > ui_fm_freq_limit_high)
		{
			if(uc_fm_ch_total_num == 0)	//have not searched valid channel
			{
				uc_fm_mode = FM_MODE_TUNING;		//tuning mode
				uc_fm_cur_ch_idx = 0;	//0: means no station after auto search finished
				ui_fm_cur_freq = ui_fm_freq_limit_low;
			}
			else
			{
				uc_fm_mode = FM_MODE_PRESET;		//preset mode
				uc_fm_cur_ch_idx = 1;	//1: means the first channel number
				ui_fm_cur_freq = ui_fm_preset_buf[0]; //after search finished,set the first station as preset
			}
			
			uc_fm_valid_cnt = 0;		//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
			_delay_1ms(100);			//delay 100ms so that ui_fm_freq_limit_high can display for a while
			goto end_tuner_search;
		}
		else
		{
			goto auto_search_one_tune;
		}
	}
	else		//half search
	{
		if((uc_fm_mode & FM_MODE_HALFSEARCH) != 0)
		{
			if(uc_fm_mode == FM_MODE_HALFSEARCH_DEC)	//half search -0.1MHz
			{
				tmp_fm_freq = ui_fm_cur_freq - FM_AUTO_STEP;
				if(tmp_fm_freq < ui_fm_freq_limit_low)
				{
					tmp_fm_freq = ui_fm_freq_limit_high;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				else if(tmp_fm_freq > ui_fm_freq_limit_high)
				{
					tmp_fm_freq = ui_fm_freq_limit_low;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				else
				{
					goto auto_search_one_tune;
				}

				_delay_1ms(100);	//delay 100ms so that FM_LIMIT can display for a while
			}
			else	//half search +0.1MHz(bit3 = 0)
			{
				if(tmp_fm_freq > ui_fm_freq_limit_high)
				{
					tmp_fm_freq = ui_fm_freq_limit_low;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				else if(tmp_fm_freq < ui_fm_freq_limit_low)
				{
					tmp_fm_freq = ui_fm_freq_limit_high;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				else
				{
					goto auto_search_one_tune;
				}

				_delay_1ms(100);			//delay 100ms so that FM_LIMIT can display for a while
			}
half_auto_one_time_finish:					//half searching end
			ui_fm_cur_freq = tmp_fm_freq;
			uc_fm_mode = FM_MODE_TUNING;	//frequency modulation mode
			goto end_tuner_search;
		}
	}
	return;

end_tuner_search:
	Api_RadioSetFreq(ui_fm_cur_freq);
	Api_RadioSetMute(0);	//cancel fm mute
	return;

auto_search_one_tune:
	ui_fm_cur_freq = tmp_fm_freq;
	fm_search_one_freq();	//output a flag to indicator the station is valid or invalid
	
	if(uc_fm_valid_station_flg == 1)	//valid
	{
		uc_fm_valid_cnt++;

		if((uc_fm_valid_cnt - 1) == 0)
		{
			ui_fm_first_valid_freq = tmp_fm_freq;
			
			ui_fm_cur_freq_bk = tmp_fm_freq;
			uc_fm_signal_th_acc_max = uc_fm_signal_th_acc;
		}
		else
		{
			if(uc_fm_signal_th_acc_max < uc_fm_signal_th_acc)
			{
				ui_fm_cur_freq_bk = tmp_fm_freq;
				uc_fm_signal_th_acc_max = uc_fm_signal_th_acc;
			}

			if(uc_fm_mode != FM_MODE_AUTOSEARCH)
			{
				tmp_fm_valid_counter = (uc_fm_valid_cnt - 1) / 2; 

				if(uc_fm_mode == FM_MODE_HALFSEARCH_DEC)	//half search -0.1MHz
				{
					tmp_fm_freq = ui_fm_first_valid_freq - tmp_fm_valid_counter * FM_AUTO_STEP;  
				}
				else	//bit3 = 0,half search +0.1MHz
				{
					tmp_fm_freq = ui_fm_first_valid_freq + tmp_fm_valid_counter * FM_AUTO_STEP;                        
				}
			}
		}

		return;	//valid end
	}
	else		//invalid
	{
		if(uc_fm_valid_cnt == 0)
		{
			return;
		}
		else
		{
			tmp_fm_valid_counter = (uc_fm_valid_cnt - 1) / 2; 
			uc_fm_valid_cnt = 0;

			if(uc_fm_mode == FM_MODE_AUTOSEARCH)
			{	
				tmp_fm_freq = ui_fm_cur_freq_bk;
				
				//auto search good channel finished,auto search save current tune
				if((uc_fm_cur_ch_idx + 1) <= FM_CH_NUM_MAX)
				{
					uc_fm_cur_ch_idx++;
					uc_fm_ch_total_num = uc_fm_cur_ch_idx;
				}
				
				if(tmp_fm_freq < ui_fm_freq_limit_low)
				{
					tmp_fm_freq = ui_fm_freq_limit_high; 
				}
				
				if(tmp_fm_freq > ui_fm_freq_limit_high)
				{
					tmp_fm_freq = ui_fm_freq_limit_low; 
				}
				
				if(uc_fm_high_injection_flg == 1)
				{
					ui_fm_preset_buf[uc_fm_cur_ch_idx - 1] = tmp_fm_freq | 0x8000;
				}
				else
				{
					ui_fm_preset_buf[uc_fm_cur_ch_idx - 1] = tmp_fm_freq;
				}
			}
			else
			{
				tmp_fm_freq = ui_fm_cur_freq_bk;
				goto half_auto_one_time_finish;
			}
		}
	}
}


/*******************************************************************************
Function: fm_search_one_freq()
Description: set as normal mode first, write PLL(get from ui_fm_cur_freq), 
             and then set mute off
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void fm_search_one_freq(void)
{
	uc8 i, tmp_if_cnt, tmp_adc_lev;
		
	Api_RadioSetMute(1);					//set mute
	uc_fm_write_bytes[0] &= ~FM_REG_SM;		//while SM = 1,find few station,but SM = 0,could find many station
	
	fm_i2c_decide_hlsi();
	
	uc_fm_signal_strength = 0;
	uc_fm_signal_th_acc = 0;
	uc_fm_signal_er_cnt = 0;
	uc_fm_ifc_er_cnt = 0;

	fm_i2c_calculate_freq();
	_fm_i2c_write(3);
	_delay_1ms(20);
	_fm_i2c_read(4);
	if(((uc_fm_read_bytes[3]>>4) + 2) < uc_fm_signal_strength_val)
	{
		uc_fm_valid_station_flg = 0;	//invalid station
		return;
	}
	
	for(i = 0; i < 16; i++)
	{
		fm_i2c_calculate_freq();
		_fm_i2c_write(3);
		_delay_1ms(30);
		
		_fm_i2c_read(4);
		tmp_if_cnt = uc_fm_read_bytes[2]&0x7f;
		tmp_adc_lev = (uc_fm_read_bytes[3]>>4);
		uc_fm_signal_strength = tmp_adc_lev;
		
		if((tmp_if_cnt<0x34) || (tmp_if_cnt>=0x3b))
		{
			if(tmp_adc_lev < (uc_fm_signal_strength_val-1))
			{
				uc_fm_valid_station_flg = 0;	//invalid station
				return;
			}
			else
			{
				goto continue_check1;
			}
		}
		else
		{
continue_check1:
			if((tmp_if_cnt>=0x32) && (tmp_if_cnt<0x3d))
			{
				if((tmp_adc_lev + 2) < uc_fm_signal_strength_val)
				{
					uc_fm_valid_station_flg = 0;	//invalid station
				  	return;
				}
				else
				{
					goto continue_check2;
				}
			}
			else
			{
continue_check2:
				if((tmp_if_cnt<0x32) || (tmp_if_cnt>=0x3d))
				{
					uc_fm_ifc_er_cnt++;
				}
			}
		}
		
		uc_fm_signal_th_acc += uc_fm_signal_strength;
		if(uc_fm_signal_strength < uc_fm_signal_strength_val)
		{
			uc_fm_signal_er_cnt++;
		}
		
		if((uc_fm_ifc_er_cnt!=0) || (uc_fm_signal_er_cnt>=8))
		{
			uc_fm_valid_station_flg = 0;	//invalid station
		  	return;
		}
	}
	
	if(uc_fm_signal_th_acc < uc_fm_accumulative_val)
	{
		uc_fm_valid_station_flg = 0;	//invalid station
	}
	else
	{
		uc_fm_valid_station_flg = 1;	//valid station
	}
}




/*******************************************************************************
Function: fm_i2c_tune_over()
Description: get current frequency uc_fm_signal_strength
Calls:
Global: uc_fm_signal_strength
Input: Null
Output:Null
Others:
********************************************************************************/
void fm_i2c_tune_over(void)
{
	uc8 i;
	
	uc_fm_signal_strength = 0;
	for(i = 0; i < 2; i++)		//calculate twice
	{
		_fm_i2c_read(4);
		uc_fm_signal_strength += (uc_fm_read_bytes[3]>>4);
		_delay_1ms(10);
	}
	
	uc_fm_signal_strength = (uc_fm_signal_strength>>1);	//uc_fm_signal_strength = uc_fm_signal_strength/2 because add twice above
}




/*******************************************************************************
Function: fm_i2c_reset_hlsi()
Description: set HLSI high or low injection
Calls:
Global:uc_fm_high_injection_flg: 1--High, 0--Low
Input: Null
Output:Null
Others:
********************************************************************************/
void fm_i2c_reset_hlsi(void)
{
	if(uc_fm_high_injection_flg == 1)
	{
		uc_fm_write_bytes[2] |= FM_REG_HLSI;
	}
	else
	{
		uc_fm_write_bytes[2] &= ~FM_REG_HLSI;
	}
}



/*******************************************************************************
Function: fm_i2c_decide_hlsi()
Description: check the high/low side injection, and set uc_fm_write_bytes[2] bit4 "HLSI"
Calls:
Global:uc_fm_mode
       uc_fm_write_bytes[5]
Input: Null
Output:Null
Others:
********************************************************************************/
void fm_i2c_decide_hlsi(void)
{
	uc8  xdata tmp_fm_strength;
	ui16 xdata tmp_pll;

	if(uc_fm_mode == FM_MODE_PRESET)//Preset Mode
	{
		if(ui_fm_cur_freq & 0x8000)
		{
			uc_fm_high_injection_flg = 1;
		}
		else
		{
			uc_fm_high_injection_flg = 0;
		}

		fm_i2c_reset_hlsi();
	}
	else if(uc_fm_valid_cnt == 0)	//auto search mode
	{
		if(uc_fm_mode == FM_MODE_HALFSEARCH_DEC)	//fm_halfsearch_dec
		{
			uc_fm_high_injection_flg = 0;
		}
		else
		{
			uc_fm_high_injection_flg = 1;
		}
		
		fm_i2c_reset_hlsi();
	}
	else							//manual
	{
		uc_fm_high_injection_flg = 1;
		fm_i2c_reset_hlsi();
		
		tmp_pll = (ui16)(((ul32)(ui_fm_cur_freq) * 10000 + 450000) / 8192);    
		uc_fm_write_bytes[0] = (uc_fm_write_bytes[0] & 0xC0) | (tmp_pll / 256);
		uc_fm_write_bytes[1] = tmp_pll % 256;
		
		_fm_i2c_write(3);
		fm_i2c_tune_over();
		tmp_fm_strength = uc_fm_signal_strength;	//LevelHigh

		uc_fm_high_injection_flg = 1;
		fm_i2c_reset_hlsi();
		
		tmp_pll = (ui16)(((ul32)(ui_fm_cur_freq) * 10000 - 450000) / 8192);    
		uc_fm_write_bytes[0] = (uc_fm_write_bytes[0] & 0xC0) | (tmp_pll / 256);
		uc_fm_write_bytes[1] = tmp_pll % 256;
		
		_fm_i2c_write(3);
		fm_i2c_tune_over();
		
		if(tmp_fm_strength < uc_fm_signal_strength)	//LevelHigh < LevelLow
		{
			uc_fm_high_injection_flg = 1;
		}
		else
		{
			uc_fm_high_injection_flg = 0;
		}
		
		fm_i2c_reset_hlsi();
	}
}


void fm_i2c_calculate_freq(void)
{
	ui16 xdata tmp_pll;
	ul32 xdata tmp_freq;

	ui_fm_cur_freq = ui_fm_cur_freq & 0x7fff;

	tmp_freq = (ul32)(ui_fm_cur_freq) * 10000;			//unit:10KHz->1Hz

	if(uc_fm_high_injection_flg)
	{
		tmp_pll = (ui16)((tmp_freq + 225000) / 8192);	//unit:1Hz
	}
	else
	{
		tmp_pll = (ui16)((tmp_freq - 225000) / 8192);	//unit:1Hz
	}

	uc_fm_write_bytes[0] = (uc_fm_write_bytes[0] & 0xC0) | (tmp_pll / 256);
	uc_fm_write_bytes[1] = tmp_pll % 256;
}

#endif