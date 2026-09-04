#include "time_user.h"
volatile static uint64_t DwtClockFreq;
volatile static uint64_t DwtRoundCnt;
volatile static uint32_t lastDwtCnt;    // 保存下一次计数值
volatile static uint32_t currentDwtCnt; // 从寄存器读取当前计数值
float time_user;
void DwtClock_Init(void)
{
    DwtClockFreq = SystemCoreClock;                 // 系统主频
    DwtRoundCnt = 0u;                               // 初始化溢出计数为0
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 使能 DWT 和 ITM 模块
    DWT->CYCCNT = (uint32_t)0u;                     // DWT计数清0（32位计数器）
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // 使能 CYCCNT 寄存器
    currentDwtCnt = 0u;
    lastDwtCnt = 0u;
}
inline uint64_t DwtClock_GetDwtCnt(void)
{
    __disable_irq();
    currentDwtCnt = DWT->CYCCNT;
    if (currentDwtCnt < lastDwtCnt)
    {
        // 如果当前计数值小于上一次的计数值，说明发生了溢出
        DwtRoundCnt++; // 溢出计数
    }
    lastDwtCnt = currentDwtCnt;
    __enable_irq();                                       // 更新上一次计数值
    return ((uint64_t)DwtRoundCnt << 32) | currentDwtCnt; // 组合溢出值和计数值
}
// 得到Dwt微秒数
inline uint64_t DwtClock_GetDwtUs(void)
{
    return DwtClock_GetDwtCnt() / (DwtClockFreq / 1000000u);
}
// 得到Dwt毫秒数
inline uint64_t DwtClock_GetDwtMs(void)
{
    return DwtClock_GetDwtCnt() / (DwtClockFreq / 1000u);
}
// 得到Dwt秒数
inline double DwtClock_GetDwtTime(void)
{
    return (double)DwtClock_GetDwtCnt() / (double)DwtClockFreq;
}
inline double get_time(void)
{
    time_user = DwtClock_GetDwtTime();
    return time_user;
}
// Dwt延时微秒
inline void DwtClock_DelayUs(uint64_t us)
{
    uint64_t start = DwtClock_GetDwtUs();
    while (DwtClock_GetDwtUs() - start < us)
        ;
}
// Dwt延时毫秒
inline void DwtClock_DelayMs(uint64_t ms)
{
    uint64_t start = DwtClock_GetDwtMs();
    while (DwtClock_GetDwtMs() - start < ms)
        ;
}

void tim_init(void)
{
    // __HAL_TIM_SET_PRESCALER(&htim2, 72 - 1);
    // __HAL_TIM_SET_AUTORELOAD(&htim2, 1000 - 1);
    // __HAL_TIM_SET_COUNTER(&htim2, 0);
    // HAL_TIM_Base_Start_IT(&htim2);
    __HAL_TIM_SET_PRESCALER(&htim1, 72 - 1);
    __HAL_TIM_SET_AUTORELOAD(&htim1, 1000 - 1);
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    HAL_TIM_Base_Start_IT(&htim1);
}
void time_init(void)
{
    DwtClock_Init();
    tim_init();
}