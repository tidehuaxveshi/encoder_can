#ifndef TIME_H
#define TIME_H

#include "stdint.h"
#include "tim.h"
//Dwt时钟初始化
void DwtClock_Init(void);
//得到Dwt计数值
uint64_t DwtClock_GetDwtCnt(void);
//得到Dwt微秒数
uint64_t DwtClock_GetDwtUs(void);
//得到Dwt毫秒数
uint64_t DwtClock_GetDwtMs(void);
//得到Dwt秒数
double DwtClock_GetDwtTime(void);
//Dwt延时微秒
void DwtClock_DelayUs(uint64_t us);
//Dwt延时毫秒
void DwtClock_DelayMs(uint64_t ms);
void time_init(void);
double get_time(void);
extern float time_user;
#endif // TIME_H