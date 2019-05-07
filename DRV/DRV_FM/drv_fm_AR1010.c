/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_fm_AR1010.c
Creator: 	zj.zong			Date: 2009-08-10
Description: fm driver function, fm module is AR1010
Others:
Version: V0.1
History:
	V0.1	2009-08-10		zj.zong	
		1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef DRV_FM_AR1010
#include "sh86278_sfr_reg.h"
#include "drv_fm.h"
#include "drv_fm_var_ext.h"

extern void _delay_1ms(uc8 cnt);
extern void _fm_i2c_write(uc8 wr_cnt);
extern void _fm_i2c_read(uc8 rd_cnt);

void Api_RadioOpen(uc8 freq_mode);
uc8 fm_search_one_freq(uc8 searchmode);
void Api_RadioClose(void);
void fm_tuning(void);
void reset_fm_preset_buffer(void);

ui16 AR1000reg[18];
code ui16 AR1000reg_dai[18]={ //using  AR1000 XO function
	0xFFFB,		// R0 -- the first writable register .
	0x5B15,		// R1.
	0xD0B9,		// R2.
	0xA010,		// R3, seekTHD = 16
	0x0780,		// R4
	0x28AB,		// R5
	0x6400,		// R6
	0x1EE7,		// R7
	0x7141,		// R8
	0x007D,		// R9
	0x82CE,		// R10  disable wrap
	0x4E55,		// R11. <--- 
	0x970C,		// R12.
	0xB845,		// R13
	0xFC2D,		// R14
	0x8097,		// R15
	0x04A1,		// R16
	0xDF6A		// R17
};

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
void Api_RadioOpen(uc8 freq_mode)
{
	uc8 Cnt1;

	uc_fm_read_bytes[0] = 0;
	uc_fm_read_bytes[1] = 0;
	uc_fm_write_bytes[0] = 0;
	uc_fm_write_bytes[1] = 0;

	_delay_1ms(20);

   	for(Cnt1=0;Cnt1<18;Cnt1++)
	{
		AR1000reg[Cnt1]	=AR1000reg_dai[Cnt1];
	}

	uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[0]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[0]&0xfe);
 	_fm_i2c_write(0x00);

	_delay_1ms(40);

	uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[0x01]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[0x01]);
  	_fm_i2c_write(0x01);

	uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[0x02]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[0x02]);
	_fm_i2c_write(0x02);

	AR1000reg[0x03]	=AR1000reg_dai[0x03];
	AR1000reg[0x03] &= 0xE7FF;
	switch(freq_mode)
	{	
		case 0:
			break;
		case 1:
			AR1000reg[3] |= 0x1000;
			break;
		case 2:
			AR1000reg[3] |= 0x1800;
			break;
	}
	uc_fm_write_bytes[0] = (uc8)(AR1000reg[0x03]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg[0x03]);
  	_fm_i2c_write(0x03);

	for(Cnt1=4;Cnt1<18;Cnt1++)
  	{
  		uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[Cnt1]>>8);
		uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[Cnt1]);
  		_fm_i2c_write(Cnt1);
	}
	uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[0]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[0]);
 	_fm_i2c_write(0x00);

	MR_WDT = 0xff;
    _fm_i2c_read(0x13);
   
	while( (uc_fm_read_bytes[1]&0x20) == 0)
	{
		MR_WDT = 0xff;
		_delay_1ms(5);
		_fm_i2c_read(0x13);	
	}
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
void Api_RadioClose()
{
	uc_fm_write_bytes[0] = 0xff;
	uc_fm_write_bytes[1] = 0xfa;
 	_fm_i2c_write(0x00);
	
	_delay_1ms(100); 		// delay 100ms
	
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
void Api_RadioSetFreq(ui16 ui_fm_cur_freq)
{
	ui16 frequency;
	frequency =	 ui_fm_cur_freq/10;

	uc_fm_write_bytes[1] = (uc8)(AR1000reg[1]|0x0002);
	uc_fm_write_bytes[0] = (uc8)(AR1000reg[1]>>8);	 
	_fm_i2c_write(1);

	uc_fm_write_bytes[1] = (uc8)(AR1000reg[2]);		    //tune off
   	uc_fm_write_bytes[0] = (uc8)((AR1000reg[2]&0xfd00)>>8);	 
	_fm_i2c_write(2);

	if(ui_fm_cur_freq<ui_fm_freq_limit_low)					
 	{
		AR1000reg[3] |= 0x1000;	
	}
	else
	{
		AR1000reg[3] &= ~0x1000;
	}

  	uc_fm_write_bytes[1] = (uc8)(AR1000reg[3]);
	AR1000reg[3] = AR1000reg[3]|0x2000;   //space
   	uc_fm_write_bytes[0] = (uc8)((AR1000reg[3]&0x3000)>>8);//seek off 	 
	_fm_i2c_write(3);
    
  	AR1000reg[2] =AR1000reg[2]&0xfe00;
	AR1000reg[2] |=(frequency-690);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg[2]);		   //chan
   	uc_fm_write_bytes[0] = (uc8)(AR1000reg[2]>>8);	 
	_fm_i2c_write(2);

	uc_fm_write_bytes[1] = (uc8)(AR1000reg[2]);
   	uc_fm_write_bytes[0] = (uc8)((AR1000reg[2]|0x0200)>>8);	 //tune on
	_fm_i2c_write(2);

	MR_WDT = 0xff;						   
	_fm_i2c_read(0x13);
	while(( uc_fm_read_bytes[1] &0x20)==0) 
	{
		MR_WDT = 0xff;
	 	_fm_i2c_read(0x13);
	}
	uc_fm_write_bytes[1] = (uc8)(AR1000reg[1]&0xfffd);		 //hmute off
	uc_fm_write_bytes[0] = (uc8)(AR1000reg[1]>>8);	 //BAND
	_fm_i2c_write(1);	

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
ui16 Api_RadioGetFreq()
{
	_fm_i2c_read(0x13);
	ui_fm_cur_freq = uc_fm_read_bytes[0]*2;
	ui_fm_cur_freq = ((uc_fm_read_bytes[1] >> 7)&0x01)+ui_fm_cur_freq + 690;
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
	_fm_i2c_read(0x13);
	if(AR1000reg[1] &0x08)
	{
		uc_radio_status |= 0x40;
	}
	else
	{
		uc_radio_status &= ~0x40;
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
		uc_fm_write_bytes[1] = (uc8)(AR1000reg[1]|0x0002);	 		//mute on
	}
	else
	{
		uc_fm_write_bytes[1] = (uc8)(AR1000reg[1]&0xfffd);		 	// mute off
	}
	uc_fm_write_bytes[0] = (uc8)(AR1000reg[1]>>8);
	_fm_i2c_write(1);
}
/*******************************************************************************
Function: Api_RadioSetStereo()
Description: set fm stereo or mono
Calls:
Global:
Input: stereo_flag: 1 is force stereo, 0 is force mono
Output:Null
Others:
********************************************************************************/
void Api_RadioSetStereo(uc8 stereo_flag)
{
	if(stereo_flag)	//force stereo
	{
		uc_fm_write_bytes[1] = (uc8)(AR1000reg[1]|0xff7f);	 		//stereo 

	}
	else			//force mono
	{
		uc_fm_write_bytes[1] = (uc8)(AR1000reg[1]|0x0080);	 		//mono
	}
		_fm_i2c_write(1);
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
//	search_para = 0 ;
	uc_fm_signal_strength_val = (uc8)(search_para>>8);
	AR1000reg[0x03] &= 0xff80;	
	AR1000reg[0x03] |= (0x007f&(uc_fm_signal_strength_val*2+0x06));		//set threshold	
	uc_fm_write_bytes[0] = (uc8)(AR1000reg[0x03]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg[0x03]);
  	_fm_i2c_write(0x03);
	_delay_1ms(20); 	
	
	if(search_flg == 0)
	{	switch(uc_fm_mode)
		{
			case FM_MODE_HALFSEARCH_INC:	//halfsearch inc freq
			case FM_MODE_HALFSEARCH_DEC:	//halfsearch dec freq
				fm_tuning();
				break;
					 
			default:
		  		if(uc_fm_mode != FM_MODE_AUTOSEARCH)//initial variants while first search start
				{
    				uc_fm_mode = FM_MODE_AUTOSEARCH; //auto search mode
			
					ui_fm_cur_freq = ui_fm_freq_limit_low;
				
					Api_RadioSetFreq(ui_fm_cur_freq);
					_delay_1ms(200); 		// delay 100ms
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
	ui16 xdata tmp_fm_freq;
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
					ui_fm_cur_freq = ui_fm_freq_limit_high;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				#if 0
				else if(tmp_fm_freq > ui_fm_freq_limit_high)
				{
					ui_fm_cur_freq = ui_fm_freq_limit_low;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				#endif
				else
				{
					goto auto_search_one_tune;
				}

			}
			else	//half search +0.1MHz(bit3 = 0)
			{
				if(tmp_fm_freq > ui_fm_freq_limit_high)
				{
					ui_fm_cur_freq = ui_fm_freq_limit_low;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				#if 0
				else if(tmp_fm_freq < ui_fm_freq_limit_low)
				{
					ui_fm_cur_freq = ui_fm_freq_limit_high;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				#endif
				else
				{
					goto auto_search_one_tune;
				}

			//	delay_1ms(10);			//delay 100ms so that FM_LIMIT can display for a while
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

	 if(uc_fm_mode == FM_MODE_HALFSEARCH_DEC)
	 {
		uc_fm_valid_station_flg = fm_search_one_freq(0);
	 }
	 else
	 {		
		uc_fm_valid_station_flg = fm_search_one_freq(1);
	 }
	
	if((uc_fm_band == 0)&&(ui_fm_cur_freq>=8700)&&(ui_fm_cur_freq<8750))
	{
		uc_fm_valid_station_flg = 0;
	}
		   
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
}


uc8 fm_search_one_freq(uc8 searchmode)
{
	uc8	Freq_tune_ok, fm_flag2, stc_flag, sf_flag;	
	ui16 SaveSeekOkFreq, if_cnt;
	Freq_tune_ok = 0;
	//mute on
	uc_fm_write_bytes[1] = (uc8)(AR1000reg[1]|0x0002);
	uc_fm_write_bytes[0] = (uc8)(AR1000reg[1]>>8);	 
	_fm_i2c_write(1);

	_fm_i2c_read(0x13);
	SaveSeekOkFreq = uc_fm_read_bytes[0]*2;
	SaveSeekOkFreq = ((uc_fm_read_bytes[1] >> 7)&0x01)+SaveSeekOkFreq + 690;

	fm_flag2 = 0;
	if(uc_fm_read_bytes[1] & 0x20)
	{
		if(uc_fm_mode == FM_MODE_HALFSEARCH_DEC)
		{
			if(SaveSeekOkFreq == (ui_fm_cur_freq/10+1))
			{
				fm_flag2 = 1;			
			}
		}
		else
		{
			if(SaveSeekOkFreq ==(ui_fm_cur_freq/10-1))
			{
				fm_flag2 = 1;
			}
		}
	}

	if(fm_flag2)
	{ 	
		AR1000reg[2] =AR1000reg[2]&0xfe00;	  	//tune off set chan
		if(!searchmode)
		{
			if(ui_fm_cur_freq/10 >= ui_fm_freq_limit_high)
			{
			    AR1000reg[2] |= (ui_fm_freq_limit_high - 690);
			}
			else
			{
		        AR1000reg[2] |= ((ui_fm_cur_freq/10 + 1) -690);
			}
 		}
		else
		{
			if((ui_fm_cur_freq/10 -1) <= (ui_fm_freq_limit_low/10))
			{
			    AR1000reg[2] |= ((ui_fm_freq_limit_low/10)-690);
			}
			else
			{
		        AR1000reg[2] |= ((ui_fm_cur_freq /10 -1)-690);
			}
		}
		
		uc_fm_write_bytes[1] = (uc8)(AR1000reg[2]);
   		uc_fm_write_bytes[0] = (uc8)(AR1000reg[2]>>8);
   		uc_fm_write_bytes[0] &= 0xfd;	 
		_fm_i2c_write(2);
  	
  		uc_fm_write_bytes[1] = (uc8)(AR1000reg[3]);			  	//seeek off	  
   		uc_fm_write_bytes[0] = (uc8)((AR1000reg[3]&0xb000)>>8);	 
		_fm_i2c_write(3);

	 	if(!searchmode)
		{
	    	AR1000reg[3] &= ~0x8000;		//seekdown
	   	}
		else
 		{
			AR1000reg[3] |= 0x8000;			  //seekup
		}
	    _fm_i2c_write(3);
 		
		AR1000reg[0x11] &= 0xC3FF;					  // Setting before seek 
		AR1000reg[0x11] |= 0x2000;
		uc_fm_write_bytes[1] = (uc8)(AR1000reg[0x11]);
   		uc_fm_write_bytes[0] = (uc8)(AR1000reg[0x11]>>8);	 
		_fm_i2c_write(0x11);

	 	AR1000reg[3] = AR1000reg[3]|0x4000;   //enable seek
	 	uc_fm_write_bytes[1] = (uc8)(AR1000reg[3]);
   		uc_fm_write_bytes[0] = (uc8)(AR1000reg[3]>>8);	 
		_fm_i2c_write(3);
     
	}

	_delay_1ms(20);

	uc_fm_valid_station_flg = 0;
	do
    {
		MR_WDT = 0xff;					//clr watchdog
		_delay_1ms(20);
        _fm_i2c_read(0x13);
		_delay_1ms(60);
        sf_flag = uc_fm_read_bytes[1] & 0x10;
        stc_flag = uc_fm_read_bytes[1] & 0x20;
        SaveSeekOkFreq = uc_fm_read_bytes[0]*2;
        SaveSeekOkFreq = ((uc_fm_read_bytes[1] >> 7)&0x01)+SaveSeekOkFreq + 690;
    
        if(ui_fm_cur_freq/10 == SaveSeekOkFreq)
        {
			if(sf_flag)
			{
				Freq_tune_ok = 0;
			}
			else
				Freq_tune_ok = 1;                       
        }
        else
        {
            Freq_tune_ok = 0;
            	break;
        }
    }while(stc_flag==0x00);


	if(stc_flag)
	{
		AR1000reg[0x11] = 0xdf6a;
		uc_fm_write_bytes[1] = (uc8)(AR1000reg[0x11]);
		uc_fm_write_bytes[0] = (uc8)(AR1000reg[0x11]>>8);	
		_fm_i2c_write(0x11);
	}

	if(Freq_tune_ok == 1)
	{	
		_fm_i2c_read(0x12);
		if_cnt = (uc_fm_read_bytes[0]&0x01)*256+ uc_fm_read_bytes[1];
		
		if(if_cnt<260&&if_cnt>240)
		{
			Freq_tune_ok = 1;
		}
		else
		{
			Freq_tune_ok = 0;
		}
	}
	return Freq_tune_ok;
}


#endif
