/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_bootblock_fm.h
Creator: zj.zong				Date: 2009-08-23
Description: define Macro in fm api and driver
Others:
Version: V0.1
History:
	V0.1	2009-08-23		zj.zong
			1. build this module
********************************************************************************/
#ifndef BOOTBLOCK_FM_HEADER
#define BOOTBLOCK_FM_HEADER

/*****************************FM API Macro Define*******************************/
#define	FM_MODE_PRESET			0x1
#define	FM_MODE_TUNING			0x2
#define	FM_MODE_HALFSEARCH		0x10	//meaning halfsearch
#define	FM_MODE_HALFSEARCH_INC	0x14	//meaning halfsearch increase freq
#define	FM_MODE_HALFSEARCH_DEC	0x1c	//meaning halfsearch decrease freq
#define	FM_MODE_AUTOSEARCH		0x3


#define FM_CH_NUM_MAX		30		//fm saved channel number max

#define FM_AUTO_STEP		10		//0.1MHz	(unit is 10KHz)
#define FM_MANUAL_STEP		10		//0.1MHz

#define FM_LOW_LIMIT_JP		7600	//76MHz~90MHz
#define FM_HIGH_LIMIT_JP	9000

#define FM_LOW_LIMIT_NOR	8700	//87MHz~108MHz
#define FM_HIGH_LIMIT_NOR	10800

#define FM_LOW_LIMIT_ALL	7600	//76MHz~108MHz
#define FM_HIGH_LIMIT_ALL	10800


#define	FM_DATA_PORT		MR_PADATA
#define	FM_DATA_TYPE		MR_PATYPE
#define	FM_DATA_MASK		(1<<2)
#define	FM_CLK_PORT			MR_PADATA
#define	FM_CLK_TYPE			MR_PATYPE
#define	FM_CLK_MASK			(1<<3)

#define	CHIP_WR0_RD1_MASK 	(1<<0)
//Byte 1
#define	FM_REG_MUTE			(1<<7)
#define	FM_REG_SM			(1<<6)
//Byte 3
#define	FM_REG_HLSI			(1<<4)
#define	FM_REG_SUD			(1<<7)
#define	FM_REG_SSL1			(1<<6)
#define	FM_REG_SSL0			(1<<5)
#define	FM_REG_MS			(1<<3)
//Byte 4
#define	FM_REG_STBY			(1<<6)
#define	FM_REG_XTAL 		(1<<4)
#define	FM_REG_SMUTE		(1<<3)
#define	FM_REG_SNC			(1<<1)
/******************* AR1010 Register Macor Define**********************/
#define AR1010_I2C_WRITE_ADDR  0x20		

/******************* Si4702 Register Macor Define**********************/
#define FM_RESET_TYPE			MR_PBTYPE
#define FM_RESET_PORT			MR_PBDATA
#define FM_RESET_MASK			(1<<5)

#endif