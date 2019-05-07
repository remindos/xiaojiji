#include "sh86278_compiler.h"
#include "sh86278_sfr_reg.h"

extern void jump_to_pm1_start(void);
extern void Test_EV_Mode(void);
extern void Test_EV_Enter(void);
extern void Ice_Main_Ibuf(void);

void main(void)
{
	IPH0 = 0x00;				//set interrupt porioty
	IPL0 = 0x00;				
	IPH1 = 0x38;
	IPL1 = 0x02;
#ifdef SH86278_TEST_EV
#ifdef ICE_Ibuf_Cmd
	Ice_Main_Ibuf();		//jump to run load sfc command
#else
#ifdef ICE_Mode
	Test_EV_Mode();			//jump to run init function
#else
 	Test_EV_Enter();		//jump to run mode function
#endif
#endif

#else
	jump_to_pm1_start();	//jump to pm1
#endif
}