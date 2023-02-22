#include "delay/delay.h"
#include "sys.h"



static u32 fac_us = 0;							//us延时倍乘数




/**
 * @brief	初始化延迟函数,SYSTICK的时钟固定为AHB时钟
 *
 * @param   SYSCLK	系统时钟频率
 *
 * @return  void
 */
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
    u32 reload;
#endif
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK
    fac_us = SYSCLK;						//不论是否使用OS,fac_us都需要使用

#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
    reload = SYSCLK;					  //每秒钟的计数次数 单位为K
    reload *= 1000000 / delay_ostickspersec;	//根据delay_ostickspersec设定溢出时间
    //reload为24位寄存器,最大值:16777216,在80M下，约209.7ms左右
    fac_ms = 1000 / delay_ostickspersec;		//代表OS可以延时的最少单位
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //开启SYSTICK中断
    SysTick->LOAD = reload; 					//每1/OS_TICKS_PER_SEC秒中断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //开启SYSTICK
#else
#endif
}


/**
 * @brief	延时微秒(us)函数
 *
 * @remark	nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)
 *
 * @param   nus		需要延时多少微秒
 *
 * @return  void
 */
void delay_us(u32 nus)
{
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;				//LOAD的值
    ticks = nus * fac_us; 						//需要的节拍数
    told = SysTick->VAL;        				//刚进入时的计数器值

    while(1)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {
            if(tnow < told)tcnt += told - tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
            else tcnt += reload - tnow + told;
			
            told = tnow;
            if(tcnt >= ticks)break;			//时间超过/等于要延迟的时间,则退出.
        }
    }
}


/**
 * @brief	延时毫秒(ms)函数
 *
 * @param   nms		需要延时多少毫秒
 *
 * @return  void
 */
void delay_ms(u16 nms)
{
    u32 i;

    for(i = 0; i < nms; i++) delay_us(1000);
}







