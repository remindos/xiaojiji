/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_usb_process_event.c
Creator: zj.zong					Date: 2009-06-09
Description: process usb events (process key, bat, etc.)as subroutine in usb mode	 
Others:
Version: V0.1
History:
	V0.1	2009-06-09		zj.zong
			1. build this module
*******************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_UDISK_PROCESS_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include ".\UI\UI_UDISK\ui_udisk_var_ext.h"

extern unsigned char xdata USBStatus;
extern void Ui_ProcessEvt_BatSub(void);
extern void Ui_PowerOn_Backlight(void);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void Api_Check_BklightOff(void);
extern void Api_Check_Battery(void);
extern void Api_Display_Picture(ui16 pic_idx);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Ui_Display_Battery(void);

void Ui_Init_USBVar(void);
void Ui_MsgManager(void);
void Ui_ProcessEvt_Key(void);
void Ui_Display_USBWindow(uc8 USBStatus);
void Ui_Load_PowerOn(void);
void Ui_Display_Charge(void);


/*******************************************************************************
Function: Ui_Init_USBVar()
Description: init 
Calls:
Global:Null
Input: Null
Output:Null
Others:
******************************************************************************/
void Ui_Init_USBVar(void)
{
	USBCharge_cnt = 0;
	USBCharge_idx = 0;
	USBConnect = 0;
	USBStatus = 0;
	USBStatus_bk = 0xff;
	b_usb_is_on = 1;
	b_bat_full_flg = 0;

	usb_startflag = 0;
	uc_timer_cnt = 0;
	uc_usb_timer_cnt1 = 0;
	uc_usb_timer_cnt2 = 0;
	uc_bat_level = 5;
	
	b_lcd_clrall_flg = 1;
	uc_lcd_dis_flg = 0x1;
/*** enable timer interrupt ***/	
//	MR_TMPLCL = 0x20;					//init timer interrupt 20ms once
//	MR_TMPLCH = 0x4e;
	MR_TMPLCL = 0x64;					//init timer interrupt 100us once
	MR_TMPLCH = 0x00;

	MR_TMCON = 0x02;					//enable time irq	
/*** enable ADC interrupt ***/
	MR_MODEN1 |= 0x04;					//set bit2, enable ADC
	MR_ADCCON = 0x80;					//enable ADC interrupt
	MR_IE1 |= 0x06;						//enable ADC interrupt and timer interrupt	

}

/*
#define PWM_STEP	1
#define PWM_START	1
#define PWM_FREQ	100
#define PWM_GAP		50

void PWM_LED(void)
{
  	if(uc_pwm_mode == 0)
	{
		
		b_led_pa3 = 1;
		uc_pwmr_width += PWM_STEP;
		if(uc_pwmr_width == PWM_FREQ-PWM_STEP)	//99
		{
		 	uc_pwm_mode ++;	
			uc_pwmg_width = PWM_START;
			b_led_pa4 = 1;			
		}

	//	uc_pwmc_width = PWM_FREQ-10;
		
	}
	else if(uc_pwm_mode == 1)
	{
		uc_pwmr_width -= PWM_STEP;
		if(uc_pwmr_width <= PWM_STEP)
		{
		 	uc_pwmr_width = PWM_STEP;
			//uc_pwm_mode ++;	
			b_led_pa3 = 0;
		}
		if(uc_pwmg_width<PWM_FREQ-2*PWM_STEP)
		uc_pwmg_width += PWM_STEP;
		if(b_led_pa3 == 0)
		{
		 	if(uc_pwm_cnt++ == PWM_GAP)
			{
				uc_pwm_cnt = 0;
				uc_pwm_mode ++;
			
			}
		}
			
	}
	else if(uc_pwm_mode == 2)
	{
		//b_led_pa2 = 1;
		uc_pwmg_width += PWM_STEP;
		if(uc_pwmg_width >=PWM_FREQ-PWM_STEP)	// 99
		{
		 	uc_pwm_mode ++;	
			uc_pwmb_width = PWM_START;
			b_led_pa5 = 1;			
		}
	//	uc_pwmc_width = PWM_FREQ-30;

	}
	else if(uc_pwm_mode == 3)
	{
		uc_pwmg_width -= PWM_STEP;
		if(uc_pwmg_width <=PWM_STEP)
		{
		 	//uc_pwm_mode ++;	
			b_led_pa4 = 0;
		}
		if(uc_pwmb_width<PWM_FREQ-2*PWM_STEP)
		uc_pwmb_width += PWM_STEP;
		if(b_led_pa4 == 0)
		{
		 	if(uc_pwm_cnt++ == PWM_GAP)
			{
				uc_pwm_cnt = 0;
				uc_pwm_mode ++;
			
			}
		}
	}
	else if(uc_pwm_mode == 4) 
	{
		uc_pwmb_width += PWM_STEP;			
		if(uc_pwmb_width >=PWM_FREQ-PWM_STEP)
		{
		 	uc_pwm_mode ++;	
			uc_pwmw_width = PWM_START;
			b_led_pa6 = 1;			
		}

	//	uc_pwmc_width = PWM_FREQ - 50;

	}
	else if(uc_pwm_mode == 5)
	{
		uc_pwmb_width -= PWM_STEP;
		if(uc_pwmb_width <=PWM_STEP)
		{
		 	//uc_pwm_mode ++;	
			b_led_pa5 = 0;
		}
		if(uc_pwmw_width<PWM_FREQ-2*PWM_STEP)
		uc_pwmw_width += PWM_STEP;
		if(b_led_pa5 == 0)
		{
		 	if(uc_pwm_cnt++ == PWM_GAP)
			{
				uc_pwm_cnt = 0;
				uc_pwm_mode ++;
			
			}
		}
	}
	else if(uc_pwm_mode == 6) 
	{
		uc_pwmw_width += PWM_STEP;			
		if(uc_pwmw_width >=PWM_FREQ-PWM_STEP)
		{
		 	uc_pwm_mode ++;	
			uc_pwmr_width = PWM_START;
			b_led_pa3 = 1;			
		}

	//	uc_pwmc_width = PWM_FREQ - 50;

	}
	else 
	{
		uc_pwmw_width -= PWM_STEP;
		if(uc_pwmw_width ==PWM_STEP)
		{
		 	//uc_pwm_mode = 0;	
			b_led_pa6 = 0;
		}
		if(uc_pwmr_width<PWM_FREQ-2*PWM_STEP)
		uc_pwmr_width += PWM_STEP;
		if(b_led_pa6 == 0)
		{
		 	if(uc_pwm_cnt++ == PWM_GAP)
			{
				uc_pwm_cnt = 0;
				uc_pwm_mode = 0;
			
			}
		}	
	}	
}

void check_led(void)
{
	if(b_led_pa3!=0)
	{
	 	if(uc_200us_timer_cnt>uc_pwmr_width) 
		{
				MR_PADATA |= 0x08;//pa3 = 1
								
		}
		else 
		{
			
		 	MR_PADATA &= 0xF7;	//pa3 = 0
			
		}	
	}
	else MR_PADATA |= 0x08;

	if(b_led_pa4!=0)
	{
	 	if(uc_200us_timer_cnt>uc_pwmg_width) 
		{
				MR_PADATA |= 0x10;//pa4 = 1
								
		}
		else 
		{
			
		 	MR_PADATA &= 0xEF;	//pa4 = 0
			
		}	
	}
	else MR_PADATA |= 0x10;

	if(b_led_pa5!=0)
	{
	 	if(uc_200us_timer_cnt>uc_pwmb_width) 
		{
				MR_PADATA |= 0x20;//pa5 = 1
								
		}
		else 
		{
			
		 	MR_PADATA &= 0xDF;	//pa5 = 0
			
		}	
	}
	else MR_PADATA |= 0x20;

	if(b_led_pa6!=0)
	{
	 	if(uc_200us_timer_cnt>uc_pwmw_width) 
		{
				MR_PADATA |= 0x40;//pa6 = 1
								
		}
		else 
		{
			
		 	MR_PADATA &= 0xBF;	//pa6 = 0
			
		}	
	}
	else MR_PADATA |= 0x40;
}

 */
/*******************************************************************************
Function: Ui_MsgManager()
Description: process events, include process event key, bat, timer
Calls:
Global:Null
Input: Null
Output:Null
Others:
******************************************************************************/
void Ui_MsgManager(void)
{
	if((ui_sys_event & EVT_KEY) != 0)
	{
		ui_sys_event &= ~EVT_KEY;
		Ui_ProcessEvt_Key();
	}
	
	if((ui_sys_event & EVT_BATTERY) != 0)
	{
		ui_sys_event &= ~EVT_BATTERY;

	//	Ui_ProcessEvt_BatSub();	
		uc_lcd_dis_flg |= 0x01;				
	}  

	if((ui_sys_event & EVT_TIMER) != 0)
	{
		ui_sys_event &= ~EVT_TIMER;
		
		if(uc_timer_cnt >= 10)				
		{
			uc_timer_cnt = 0;
	
			Api_Check_BklightOff();			
	
		/*	if(b_bat_full_flg == 0)
			{
				Api_Check_Battery();			
			}  */
			USBCharge_cnt++;
			if(USBCharge_cnt>5)
			{
			 	USBCharge_cnt = 0;
				USBCharge_idx++;
				if(USBCharge_idx>5)USBCharge_idx = 0; 
			//	if(b_bat_full_flg != 0)	USBCharge_idx = 5;
				uc_lcd_dis_flg |= 0x01;
			}
		}	
	}
}


/*******************************************************************************
Function: Ui_ProcessEvt_Key()
Description: process event key(play for eject USB and enter poweron)
Calls:
Global:Null
Input: Null
Output:Null
Others:
******************************************************************************/
void Ui_ProcessEvt_Key(void)
{	
	if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
	{
		Ui_PowerOn_Backlight();
		return;
	}
	Ui_PowerOn_Backlight();				//power  backlight
	switch(uc_key_value)
	{
		case KEY_PLAY_S:
			b_usb_is_on = 0;			//exit USB mode
			break; 

		default:
			break;
	}	
}


/*******************************************************************************
Function: Ui_Display_USBWindow()
Description: display USB window
Calls:
Global:USBStatus
		0x00:idle	0x10:up load	0x20:down load	0x30:charge
Input: Null
Output:Null
Others:
******************************************************************************/
void Ui_Display_USBWindow(uc8 USBStatus)
{
	if(b_lcd_clrall_flg== 1)
	{
		Api_LcdDriver_Clr_AllPage();
	}

	if((uc_lcd_dis_flg & BIT0_MASK) != 0)			//display battery is common func
	{
		uc_lcd_dis_flg &= ~BIT0_MASK;
	/*	if(USBConnect != 0)
		Ui_Display_Battery();
		else
			Ui_Display_Charge();
	*/		
	}

	uc_lcd_page_num = 0;
	uc_lcd_column_num = 0;
	if(USBStatus != USBStatus_bk)
	{
		switch(USBStatus)
		{
			case 0x00:		 // usb is idle
			//	if(USBConnect != 0)
			//   	Api_Display_Picture(PIC_USB_STATUS_IDEL_IDX);
				break;
		
			case 0x10:		// usb is up load
			//	USBConnect = 1;
			//   	Api_Display_Picture(PIC_USB_UP_LOAD_IDX);
				usb_startflag = 1;
				break;

			case 0x20:		// usb is down load
			//	USBConnect = 1;
			//   	Api_Display_Picture(PIC_USB_DOWN_LOAD_IDX);
				usb_startflag = 1;
				break;
		}	
		USBStatus_bk = USBStatus;
	}
 	uc_lcd_dis_flg = 0;
 	b_lcd_clrall_flg = 0;
}

void Ui_Display_Charge(void)
{
 	uc_lcd_page_num = 0;
	uc_lcd_column_num = 0;
	Api_Display_Picture(0x481E+USBCharge_idx);
}
/*******************************************************************************
Function: Ui_Load_PowerOn()
Description: load power on from usb
Calls:
Global:b_usb_poweron
Input: Null
Output:Null
Others:
******************************************************************************/
void Ui_Load_PowerOn(void)
{
	b_usb_poweron = 1;
	uc_load_code_idx = CODE_POWERON_MODE_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();
}

#endif