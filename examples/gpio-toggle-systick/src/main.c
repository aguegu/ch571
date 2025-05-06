#include "CH57x_common.h"

#ifndef __HIGH_CODE
#define __HIGH_CODE __attribute__((section(".highcode")))
#endif

#ifndef __INTERRUPT
#define __INTERRUPT __attribute__((interrupt("WCH-Interrupt-fast")))
#endif

volatile uint32_t jiffies = 0;

void delayInJiffy(uint32_t t) {
  uint32_t start = jiffies;
  while (t) {
    if (jiffies != start) {
      t--;
      start++;
    } else {
      __WFI();
      __nop();
      __nop();
    }
  }
}

int main() {
  SetSysClock(CLK_SOURCE_PLL_60MHz);

  SysTick_Config(GetSysClock() / 60); // 60Hz

  GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeOut_PP_5mA); // Onboard LED

  while (1) {
    GPIOA_InverseBits(GPIO_Pin_8);
    delayInJiffy(60);
  }

  return 0;
}

__HIGH_CODE
__INTERRUPT
void SysTick_Handler(void) {
  jiffies++;
  SysTick->CNTFG = 0;
}
