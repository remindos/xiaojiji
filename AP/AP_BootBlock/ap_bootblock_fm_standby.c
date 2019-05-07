/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_bootblock_fm_init.c
Creator: zj.zong		Date: 2009-08-19
Description: fm driver init
Others:
Version: V0.1
History:
	V0.1	2009-08-19		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_BOOTBLOCK_FM_STANDBY
#include "sh86278_sfr_reg.h"
#include "ap_bootblock_fm.h"
#include "ap_bootblock_fm_var_ext.h"
#include "ui_resident_var_ext.h"

extern void _fm_i2c_enable(void);
extern void _fm_i2c_disable(void);
extern void _i2c_set_sda_out(void);
extern void _i2c_clr_sda(void);
extern void _fm_i2c_write(uc8 wr_cnt);
extern void _fm_i2c_ar1010_write(uc8 wr_cnt);
extern void _fm_i2c_bk1080_write(uc8 wr_cnt);

extern void Api_Delay_1ms(uc8 timer_cnt);

void Ap_Tea5767_RadioInit(void);
void Ap_Tea5767_RadioOpen(uc8 radio_mode);
void Ap_Tea5767_RadioClose(void);

void Ap_AR1010_RadioInit(void);
void Ap_AR1010_RadioOpen(uc8 radio_mode);
void Ap_AR1010_RadioClose(void);

void Ap_Si4702_RadioInit(void);
void Ap_Si4702_RadioOpen(uc8 radio_mode);
void Ap_Si4702_RadioClose(void);

void Ap_CL6017S_RadioInit(void);
void Ap_CL6017S_RadioOpen(uc8 radio_mode);
void Ap_CL6017S_RadioClose(void);

void Ap_Bk1080_RadioInit(void);
void Ap_Bk1080_RadioOpen(uc8 radio_mode);
void Ap_Bk1080_RadioClose(void);


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

code uc8 Si4702Reg[] ={
	0x40,
	0x01,
	0x00,
	0x00,
	0x90,
	0x04,
	0x0c,
	0x1f,
	0x00,
	0x48
};

code ui16 BK1080reg[34]={ //using  BK1080  function
	0x0008,	//REG0
	0x1080,	//REG1
	0x1201,	//REG2
	0x0000,	//REG3
	0x40C0,	//REG4
	0x0A1F,	//REG5
	0x002E,	//REG6
	0x02FF,	//REG7
	0x5B11,	//REG8
	0x0000,	//REG9
	0x411E,	//REG10
	0x0000,	//REG11
	0xCE00,	//REG12
	0x0000,	//REG13
	0x0000,	//REG14
	0x1000,	//REG15
	0x0010,	//REG16
	0x0000,	//REG17
	0x13FF,	//REG18
	0x9852,	//REG19
	0x0000,	//REG20
	0x0000,	//REG21
	0x0008,	//REG22
	0x0000,	//REG23
	0x51E0,	//REG24
	0x28DC,	//REG25
	0x2645,	//REG26
	0x00E4,	//REG27
	0x1CD8,	//REG28
	0x3A50,	//REG29
	0xEAF0,	//REG30
	0x3000,	//REG31
	0x0000,	//REG32
	0x0000,	//REG33
}; 
/*******************************************************************************
Function: Ap_Init_FM()
Description: initialize fm 
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Init_FM(void)
{
	if((uc_user_option1&0x01)== 0)		// have FM
	 {
		switch((uc_user_option1&0xe0))
		{
			case 0x00:					//bit5,6,7=0x00:tea5767 
				uc_chip_address = 0xc0;
				Ap_Tea5767_RadioInit();
				break;
		
				case 0x20:					//bit5,6,7=0x20:5800C 
				uc_chip_address = 0x20;
			
				break;
		
			case 0x40:				   	//bit5,6,7=0x40:AR1010 
				uc_chip_address = 0x20;
				Ap_AR1010_RadioInit();
				break;
			
			case 0x60:				   	//bit5,6,7=0x60:CL6017S 
				uc_chip_address = 0x20;
				Ap_CL6017S_RadioInit();
				break;	  
		
			case 0x80:				   	//bit5,6,7=0x80:SI4702 
				uc_chip_address = 0x20;
				Ap_Si4702_RadioInit();
				break;
	
			case 0xa0:				   	//bit5,6,7=0xA0:Rda5807SP
				uc_chip_address = 0x20;				 
				Ap_Tea5767_RadioInit();
				break;

			case 0xc0:				   	//bit5,6,7=0xC0:BK1080
				uc_chip_address = 0x80;				 
				Ap_Bk1080_RadioInit();
				break;
		} 
	}
}

/*******************************************************************************
Function: Ap_Tea5767_RadioInit()
Description: initialize and enter standby mode;
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Tea5767_RadioInit(void)
{
	Ap_Tea5767_RadioOpen(0);
	Ap_Tea5767_RadioClose();
}

/*******************************************************************************
tea5767 radio open
********************************************************************************/
void Ap_Tea5767_RadioOpen(uc8 radio_mode)
{
	if(radio_mode == 1)		//1 is AM mode
	{
		return;
	}
	uc_fm_write_bytes[0] = FM_REG_MUTE & (~FM_REG_SM); 	//CLR "SM", set mute
	uc_fm_write_bytes[1] = 0;
	uc_fm_write_bytes[2] = (FM_REG_SUD | FM_REG_SSL1) & (~(FM_REG_SSL0 | FM_REG_MS));
	uc_fm_write_bytes[3] = FM_REG_XTAL | FM_REG_SMUTE | FM_REG_SNC;
	uc_fm_write_bytes[4] = 0;

	uc_fm_write_bytes[0] = (uc_fm_write_bytes[0] & 0xC0) | (0x29);
	uc_fm_write_bytes[1] = 0x97;
	_fm_i2c_write(5);
}

/*******************************************************************************
tea5767 radio close
********************************************************************************/
void Ap_Tea5767_RadioClose(void)
{
	uc_fm_write_bytes[0] |= FM_REG_MUTE;
	uc_fm_write_bytes[3] |= FM_REG_STBY;
	_fm_i2c_write(4);
	
	FM_CLK_TYPE &= ~FM_CLK_MASK; 		//set the fm clk&data as input
	FM_DATA_TYPE &= ~FM_DATA_MASK;
}

/*******************************************************************************
Function: Ap_AR1010_RadioInit()
Description: initialize and enter standby mode;
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_AR1010_RadioInit(void)
{
	Ap_AR1010_RadioOpen(0);
	Ap_AR1010_RadioClose();	 
}

/*******************************************************************************
AR1010 radio open
********************************************************************************/
void Ap_AR1010_RadioOpen(uc8 freq_mode)
{
	uc8 Cnt1;

	uc_fm_read_bytes[0] = 0;
	uc_fm_read_bytes[1] = 0;
	uc_fm_write_bytes[0] = 0;
	uc_fm_write_bytes[1] = 0;

	Api_Delay_1ms(20);

   	for(Cnt1=0;Cnt1<18;Cnt1++)
	{
		AR1000reg[Cnt1]	=AR1000reg_dai[Cnt1];
	}

	uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[0]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[0]&0xfe);
 	_fm_i2c_ar1010_write(0x00);

	Api_Delay_1ms(40);

	uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[0x01]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[0x01]);
  	_fm_i2c_ar1010_write(0x01);

	uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[0x02]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[0x02]);
	_fm_i2c_ar1010_write(0x02);

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
  	_fm_i2c_ar1010_write(0x03);

	for(Cnt1=4;Cnt1<18;Cnt1++)
  	{
  		uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[Cnt1]>>8);
		uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[Cnt1]);
  		_fm_i2c_ar1010_write(Cnt1);
	}
	uc_fm_write_bytes[0] = (uc8)(AR1000reg_dai[0]>>8);
	uc_fm_write_bytes[1] = (uc8)(AR1000reg_dai[0]);
 	_fm_i2c_ar1010_write(0x00);
}

/*******************************************************************************
AR1010 radio close
********************************************************************************/
void Ap_AR1010_RadioClose(void)
{
	uc_fm_write_bytes[0] = 0xff;
	uc_fm_write_bytes[1] = 0xfa;
 	_fm_i2c_ar1010_write(0x00);
	
	Api_Delay_1ms(100); 		// delay 100ms
}

/*******************************************************************************
Function: Ap_CL6017S_RadioInit()
Description: initialize and enter standby mode;
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_CL6017S_RadioInit(void)
{
	Ap_CL6017S_RadioOpen(0);
	Ap_CL6017S_RadioClose();
}

/*******************************************************************************
cl6017s radio open
********************************************************************************/
void Ap_CL6017S_RadioOpen(uc8 radio_mode)
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
	tmp_pll = (8700 - 7000)/5; 

	uc_fm_write_bytes[1] |= 0x01;
	
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

	Api_Delay_1ms(200);
	Api_Delay_1ms(200);
	Api_Delay_1ms(200);
	Api_Delay_1ms(200);

	uc_fm_write_bytes[0] |= 0x02; 	// TUNE=1, begin tune operation
	_fm_i2c_write(1);
	Api_Delay_1ms(100); 				// delay 100ms

	uc_fm_write_bytes[0] &= 0xdf;   // mute off
	_fm_i2c_write(1);

} 

/*******************************************************************************
cl6017s radio close
********************************************************************************/
void Ap_CL6017S_RadioClose(void)
{

	uc_fm_write_bytes[0] |= 0x20;  // mute on
	_fm_i2c_write(1);

	uc_fm_write_bytes[0] |= 0x80; // set DISABLE=1 to power down
	_fm_i2c_write(1);
}


/*******************************************************************************
Function: Ap_Bk1080_RadioInit()
Description: initialize and enter standby mode;
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Bk1080_RadioInit(void)
{
	Ap_Bk1080_RadioOpen(0);
	Ap_Bk1080_RadioClose();
}

/*******************************************************************************
cl6017s radio open
********************************************************************************/
void Ap_Bk1080_RadioOpen(uc8 radio_mode)
{
	uc8 Cnt1;
	radio_mode = 0;
	for(Cnt1=0;Cnt1<34;Cnt1++)
  	{
  		uc_fm_write_bytes[0] = (uc8)(BK1080reg[Cnt1]>>8);
		uc_fm_write_bytes[1] = (uc8)(BK1080reg[Cnt1]);
  		_fm_i2c_bk1080_write(Cnt1);
	}

} 

/*******************************************************************************
cl6017s radio close
********************************************************************************/
void Ap_Bk1080_RadioClose(void)
{
	uc_fm_write_bytes[0] = (uc8)(BK1080reg[2]>>8);
	uc_fm_write_bytes[1] = (uc8)(BK1080reg[2]);
	
	uc_fm_write_bytes[0] |= 0x41;  // power down
	uc_fm_write_bytes[1] |= 0x40; // mute on
	_fm_i2c_bk1080_write(2);
}

/*******************************************************************************
Si4702 radio standby
*******************************************************************************/
void Ap_Si4702_RadioInit(void)
{
	Ap_Si4702_RadioOpen(0);
	Ap_Si4702_RadioClose();	
}

void Ap_Si4702_RadioOpen(uc8 radio_mode)
{
	uc8 RegNum;
	radio_mode = 0;
	/*** read reg ***/
	/*** read reg begin from R(0A)~ R(0B) 2 bytes ***/
	for(RegNum = 0; RegNum < 4; RegNum++)
	{
		uc_fm_read_bytes[RegNum] = 0;
	} 
	/*** write reg ***/
	/*** write reg begin from R(02)~ R(06) 5 bytes ***/
	for(RegNum = 0; RegNum < 10; RegNum++)
	{
		uc_fm_write_bytes[RegNum] = 0;
	}
	uc_fm_write_bytes[10] = 0x81;

	_fm_i2c_enable();
	_i2c_set_sda_out();
	_i2c_clr_sda();
	Api_Delay_1ms(10); 		// delay 10ms

	/*** RESET PIN ***/
	FM_RESET_TYPE |= FM_RESET_MASK;
	FM_RESET_PORT |= FM_RESET_MASK;
	Api_Delay_1ms(50); 		// delay 50ms
	FM_RESET_PORT &= ~FM_RESET_MASK;
	Api_Delay_1ms(200); 		// delay 200ms
	FM_RESET_PORT |= FM_RESET_MASK;

	Api_Delay_1ms(200);
	Api_Delay_1ms(200);
	Api_Delay_1ms(200);
	Api_Delay_1ms(200);
	_fm_i2c_write(11);		//include write_byte[10]
	Api_Delay_1ms(200);
	Api_Delay_1ms(200);
	Api_Delay_1ms(200);
	Api_Delay_1ms(200);
	_fm_i2c_write(11);		//include write_byte[10]

	/*** Init WriteReg ***/
	for(RegNum = 0; RegNum < 10; RegNum++)
	{
		uc_fm_write_bytes[RegNum] = Si4702Reg[RegNum];
	}

	_fm_i2c_write(10);
	Api_Delay_1ms(250);
}


void Ap_Si4702_RadioClose(void)
{
	uc_fm_write_bytes[0] = 0x40;
	uc_fm_write_bytes[1] = 0x01;
	uc_fm_write_bytes[2] = 0x00;
	uc_fm_write_bytes[3] = 0x00;
	uc_fm_write_bytes[4] = 0x00;
	_fm_i2c_write(5);
	
	uc_fm_write_bytes[0] = 0x00;
	uc_fm_write_bytes[1] = 0x41;		
 	_fm_i2c_write(2);
	Api_Delay_1ms(100);
	/*** CLR RESET ***/ 
	FM_RESET_PORT &= ~FM_RESET_MASK;
	Api_Delay_1ms(200);	
}







#endif