#include "CH57x_common.h"

int main() {
  SetSysClock(CLK_SOURCE_PLL_60MHz);
  GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA); // onboard LED

  while (1) {
    GPIOA_InverseBits(GPIO_Pin_9);
    mDelaymS(1000);
  }

  return 0;
}
