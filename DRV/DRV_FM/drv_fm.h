/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fm_driver.h
Creator: andyliu					Date: 2007-12-25
Description: define Macro in fm api and driver
Others:
Version: V0.1
History:
	V0.1	2007-12-25		andyliu
			1. build this module
********************************************************************************/
#ifndef DRV_FM_HEADER
#define DRV_FM_HEADER


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


/*******************Philips TEA5767 Register Macor Define**********************/

#ifdef DRV_FM_TEA5767
#define	CHIP_ADDRESS 		11000000B
	#define	CHIP_WR0_RD1_MASK 		(1<<0)
#define FM_READ_BYTES_NUM	 5
#define FM_WRITE_BYTES_NUM	 5

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

#endif
/******************* AR1010 Register Macor Define**********************/

#ifdef DRV_FM_AR1010
#define	CHIP_ADDRESS 		00100000B
	#define	CHIP_WR0_RD1_MASK 		(1<<0)
#define FM_READ_BYTES_NUM	 2
#define FM_WRITE_BYTES_NUM	 2

#define AR1010_I2C_WRITE_ADDR  0x20		
#define AR1010_I2C_READ_ADDR   0x21	

#define FM_REG_MUTE   		(1<<1)

#endif
/******************* CL6017S Register Macor Define**********************/

#ifdef DRV_FM_CL6017S

#define	CHIP_ADDRESS 		00100000B
	#define	CHIP_WR0_RD1_MASK 		(1<<0)
#define FM_READ_BYTES_NUM	 24
#define FM_WRITE_BYTES_NUM	 24

//Byte 0
#define	FM_REG_MUTE			(1<<5)

//Byte 1
#define	FM_REG_S			(1<<5)
#define	FM_REG_M			(1<<6)

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
#endif

/*******************SI4702 Register Macor Define**********************/

#ifdef DRV_FM_SI4702
	#define	CHIP_ADDRESS 			00100000B
	#define	CHIP_WR0_RD1_MASK 		(1<<0)
	#define FM_READ_BYTES_NUM	 	24
	#define FM_WRITE_BYTES_NUM	 	24

	#define FM_RESET_TYPE			MR_PBTYPE
	#define FM_RESET_PORT			MR_PBDATA
	#define FM_RESET_MASK			(1<<5)

	//R02h Byte 0
	#define	FM_REG_MUTE				(1<<6)
	#define FM_REG_MS				(1<<5)
	#define FM_REG_SUD				(1<<1)
#endif

/******************* RDA5807SP Register Macor Define**********************/

#ifdef DRV_FM_RDA5807SP
#define	CHIP_ADDRESS 		00100000B
#define	CHIP_WR0_RD1_MASK 	(1<<0)

#define FM_READ_BYTES_NUM	 8
#define FM_WRITE_BYTES_NUM	 8
//Byte 1
#define	FM_REG_DHIZ			(1<<7)
#define	FM_REG_MUTE			(1<<6)
#define	FM_REG_MS			(1<<5)
#define	FM_REG_CLK32_ENB	(1<<2)
#define	FM_REG_SUD			(1<<1)
#define	FM_REG_SEEKEN		(1<<0)
//Byte 2
#define	FM_REG_SKMODE		(1<<7)
#define	FM_REG_SWRST		(1<<1)
#define	FM_REG_ENABLE		(1<<0)
//Byte 4
#define	FM_REG_TUNE			(1<<4)
#define	FM_REG_BAND			(1<<2)

#endif

/******************* AR1010 Register Macor Define**********************/

#ifdef DRV_FM_BK1080

#define	CHIP_ADDRESS 		10000000B
#define FM_READ_BYTES_NUM	 2
#define FM_WRITE_BYTES_NUM	 2

#define	FM_REG_TUNE			(1<<7)
#define	FM_REG_BAND			(1<<2)
#endif



#endif