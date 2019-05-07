/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_fm_CL6017S.c
Creator: zj.zong				Date: 2009-08-12
Description: fm driver function, fm module is SI6017S
Others:
Version: V0.1
History:
	V0.1	2009-08-12		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef DRV_FM_CL6017S
#include "sh86278_sfr_reg.h"
#include "drv_fm.h"
#include "drv_fm_var_ext.h"

extern void _fm_i2c_enable(void);
extern void _fm_i2c_disable(void);
extern void _fm_i2c_write(uc8 wr_cnt);
extern void _fm_i2c_read(uc8 rd_cnt);

void Api_RadioSetFreq(ui16 radio_freq);
void fm_search_one_freq(void);
void _delay_1ms(uc8 timer_cnt);
void Api_RadioOpen(uc8 radio_mode);
void Api_RadioClose(void);
void Api_RadioSetMute(uc8 mute_flag);
void Api_RadioSetStereo(uc8 stereo_flag);
void fm_tuning(void);
uc8 Api_SeekTH_Value(void);
uc8 Api_EnvelopTH_Value(void);
code ui16 Api_SeekTH_table[]={170,180,180,190,190,200,200};
code ui16 Api_EnvelopTH_table[]={38,38,34,34,30,30,26};

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
	ui16 tmp_pll;
	tmp_pll = 0;				// initial default value of CL6017S READ/WRITE registers 03h~0Eh
	
   	uc_fm_write_bytes[0] = 0xe4;  //e4
   	uc_fm_write_bytes[1] = 0x81;  //81-->83(auto HLSI)
   	uc_fm_write_bytes[2] = 0x3e;
   	uc_fm_write_bytes[3] = 0xf8;
	uc_fm_write_bytes[4] = 0x1f;  //	1f-1d		
	uc_fm_write_bytes[5] = 0x40; 
	uc_fm_write_bytes[6] = 0x38;
	uc_fm_write_bytes[7] = 0x5a;
	uc_fm_write_bytes[8] = 0xe8;
	uc_fm_write_bytes[9] = 0xac;
	uc_fm_write_bytes[10] = 0xb0;//0xb0;
	uc_fm_write_bytes[11] = 0xf1;//0xf1;  
	uc_fm_write_bytes[12] = 0x8b;
	uc_fm_write_bytes[13] = 0xaa;   
	uc_fm_write_bytes[14] = 0xc6;
	uc_fm_write_bytes[15] = 0x04;
	uc_fm_write_bytes[16] = 0x6d;
	uc_fm_write_bytes[17] = 0x25;
	uc_fm_write_bytes[18] = 0xff;
	uc_fm_write_bytes[19] = 0xfd;
	uc_fm_write_bytes[20] = 0x12; 
	uc_fm_write_bytes[21] = 0x0f;
	uc_fm_write_bytes[22] = 0x45;
	uc_fm_write_bytes[23] = 0x1d;

	// frequency transfer to channel number,  channel=(frequencyMhz-70)/0.05, e.g. 87.5Mhz->350, 108Mhz->760
	tmp_pll = (ui_fm_cur_freq - 7000)/5; 

	if(ui_fm_cur_freq>10000)			   	// set channel number 
	{
		uc_fm_write_bytes[1] &= 0xfe;
	}
	else
	{
		uc_fm_write_bytes[1] |= 0x01;
	}
	uc_fm_write_bytes[2] &= 0xfc;
	uc_fm_write_bytes[2] |= (tmp_pll & 0x0300)>>8;
	uc_fm_write_bytes[3] =	tmp_pll & 0xff;

	uc_fm_write_bytes[0] &= 0x7f; //power up

	MR_WDT = 0xff;
	switch(radio_mode)
	{
		case 0:
			uc_fm_write_bytes[4] &= ~0xc0;

		case 1:
			uc_fm_write_bytes[4] &= ~0xc0;
			uc_fm_write_bytes[4] |= 0x80;

		case 2:
			uc_fm_write_bytes[4] |= 0xc0;
			break;
	}

	_fm_i2c_write(24);

	_delay_1ms(200);
	_delay_1ms(200);
	_delay_1ms(200);
	_delay_1ms(200);

	uc_fm_write_bytes[0] |= 0x02; 	// TUNE=1, begin tune operation
	_fm_i2c_write(1);
	_delay_1ms(100); 				// delay 100ms

	uc_fm_write_bytes[0] &= 0xdf;   // mute off
	_fm_i2c_write(1);

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

	uc_fm_write_bytes[0] |= 0x20;  // mute on
	_fm_i2c_write(1);

	uc_fm_write_bytes[0] |= 0x80; // set DISABLE=1 to power down
	_fm_i2c_write(1);
	
	FM_CLK_TYPE &= ~FM_CLK_MASK; 	//set the fm clk&data as input
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
	ui16 chan;
	chan = 0;
	ui_fm_cur_freq = radio_freq;
	chan = (ui_fm_cur_freq - 7000)/5; 

	if(ui_fm_cur_freq>10000)			   	// set channel number 
	{
		uc_fm_write_bytes[1] &= 0xfe;
	}
	else
	{
		uc_fm_write_bytes[1] |= 0x01;
	}
	uc_fm_write_bytes[2] &= 0xfc;
	uc_fm_write_bytes[2] |= (chan & 0x0300)>>8;
	uc_fm_write_bytes[3] =	chan & 0xff;

	uc_fm_write_bytes[0] &= 0xfe;// seek off
	uc_fm_write_bytes[0] &= 0xdf;// mute off
	uc_fm_write_bytes[0] &= 0xfd;// tune=0
	_fm_i2c_write(4);

	uc_fm_write_bytes[0] |= 0x02;//tune=1, begin tune operaton
	_fm_i2c_write(1);
	_delay_1ms(200);			 // wait for 50ms	
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
	_fm_i2c_read(6);
	ui_fm_cur_freq=(uc_fm_read_bytes[2]&0x03)*256+uc_fm_read_bytes[3];
   	ui_fm_cur_freq=ui_fm_cur_freq*5+7000;
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
	_fm_i2c_read(6);

	if(uc_fm_read_bytes[1]&0x20)
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
	if(mute_flag&0x20)
	{
		uc_fm_read_bytes[0] |= FM_REG_MUTE; // mute on
		uc_radio_status |= FM_REG_MUTE;
	}
	else
	{
		uc_fm_read_bytes[0] &= ~FM_REG_MUTE; //cancel mute
		uc_radio_status &= ~FM_REG_MUTE;
	}
	_fm_i2c_write(2);	
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
		uc_fm_write_bytes[1] |= FM_REG_S;		  //set bit5
		uc_fm_write_bytes[1] &= ~FM_REG_M;		  //clr bit6
	}
	else			//force mono
	{
		uc_fm_write_bytes[1] &= ~FM_REG_S;		  //clr bit5
		uc_fm_write_bytes[1] |= FM_REG_M;		  //set bit6
	}	
	_fm_i2c_write(2);
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

	ui_seek_value =	Api_SeekTH_table[(uc8)(search_para>>8) - 1];
	ui_envelop_value =	Api_EnvelopTH_table[(uc8)(search_para>>8) - 1];

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
			
					ui_fm_cur_freq = ui_fm_freq_limit_low;
					Api_RadioSetFreq(ui_fm_cur_freq);
					ui_fm_cur_freq = ui_fm_freq_limit_low - FM_AUTO_STEP;
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
	ui16 tmp_fm_freq;

	if((uc_fm_mode == FM_MODE_PRESET) || (uc_fm_mode == FM_MODE_TUNING))	//not auto&half search
	{
		return;
	}

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

			goto end_tuner_search;
		}
		else
		{
			goto auto_search_one_tune;
		}
	}
	else		//half search
	{
		if((uc_fm_mode & FM_MODE_HALFSEARCH) == 0)
		{
			return;	//half search finished
		}
		else
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
			}

half_auto_one_time_finish:					//half searching end
			uc_fm_valid_cnt=0;	
			uc_fm_valid_station_flg=0;
			uc_fm_mode = FM_MODE_TUNING;	//frequency modulation mode
			goto end_tuner_search;
		}
	}
	
	return;

end_tuner_search:
	Api_RadioSetFreq(ui_fm_cur_freq);
	return;

auto_search_one_tune:
	 ui_fm_cur_freq = tmp_fm_freq;
	 MR_WDT = 0xff;					//clr watchdog
	 fm_search_one_freq();
 		   
	if(uc_fm_valid_station_flg == 1)
	{	
		if(uc_fm_mode == FM_MODE_AUTOSEARCH)			//auto search
		{	
			uc_fm_valid_cnt=0;	
			uc_fm_valid_station_flg=0;
	       	if((uc_fm_cur_ch_idx + 1) <= FM_CH_NUM_MAX)
	       	{
	           	uc_fm_cur_ch_idx++;
	
	           	uc_fm_ch_total_num = uc_fm_cur_ch_idx;
	    	 }
			ui_fm_preset_buf[uc_fm_cur_ch_idx - 1] = ui_fm_cur_freq;
		}
		else
		{
		 	goto half_auto_one_time_finish;
		}
	}
	return;	
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
	ui16 ch;
  	ui16 loop;
	uc8  stcflag;
	ui16  rssi, envelope,fd;
	ch = 0;

	// frequency transfer to channel number,  channel=(frequencyMhz-70)/0.05, e.g. 87.5Mhz->350, 108Mhz->760
	ch = (ui_fm_cur_freq - 7000)/5; 
	
	if(ui_fm_cur_freq>10000)	  // set channel number 
	{
		uc_fm_write_bytes[1] &= 0xfe;
	}
	else
	{
		uc_fm_write_bytes[1] |= 0x01;
	}
	uc_fm_write_bytes[2] &= 0xfc;
	uc_fm_write_bytes[2] |= (ch & 0x0300)>>8;
	uc_fm_write_bytes[3] =	ch & 0xff;
  
	uc_fm_write_bytes[0] |= 0x20; // mute on
	uc_fm_write_bytes[0] &= 0xfd; //tune=0

	_fm_i2c_write(4);

	uc_fm_write_bytes[0] |= 0x02; //tune=1, 
	_fm_i2c_write(1);
	_delay_1ms(150); // wait for 50ms

	stcflag=0;
	loop=0;
	rssi = 0;
	envelope = 0;
	fd = 0;
	do
	{	
		_delay_1ms(1); 
		_fm_i2c_read(6);		  
	    stcflag=uc_fm_read_bytes[0]&0x04; // STC indicator			
		
	  	loop++;
	}while((!stcflag)&&(loop!=0xff));

 	if(loop == 0xff)
	{
		uc_fm_valid_station_flg = 0;
		return;
	}

	rssi = uc_fm_read_bytes[1]&0xff;
	envelope = (uc_fm_read_bytes[2]&0xff)/4;
	fd = uc_fm_read_bytes[5]&0xff;
	_delay_1ms(30); 			// wait for 30ms

	if(fd>125)
	{
		fd = 256 - fd;
	}

	if((rssi >= ui_seek_value)&&(envelope<= ui_envelop_value)&&(fd<=12))
	{
		uc_fm_valid_station_flg = 1;
	}
	else
	{
		uc_fm_valid_station_flg = 0;
	}

	if(ui_fm_cur_freq == ui_fm_freq_limit_high)
	{
		uc_fm_valid_station_flg = 0;
	} 
}


#endif