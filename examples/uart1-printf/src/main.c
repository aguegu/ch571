#include "CH57x_common.h"
#include "ringbuffer.h"
#include <stdio.h>

#ifndef __INTERRUPT
#define __INTERRUPT __attribute__((interrupt("WCH-Interrupt-fast")))
#endif

volatile uint32_t jiffies = 0;
RingBuffer txBuffer;

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

int _write(int fd, char *buf, int size) {
  for (int i = 0; i < size; i++) {
    ringbufferPut(&txBuffer, *buf++, TRUE);
    if (R8_UART1_LSR & RB_LSR_TX_FIFO_EMP) {
      while (ringbufferAvailable(&txBuffer) && R8_UART1_TFC < UART_FIFO_SIZE) {
        R8_UART1_THR = ringbufferGet(&txBuffer);
      }
    }
  }
  return size;
}

int main() {
  SetSysClock(CLK_SOURCE_PLL_60MHz);

  SysTick_Config(GetSysClock() / 60); // 60Hz

  ringbufferInit(&txBuffer, 64);

  GPIOA_SetBits(GPIO_Pin_9);
  GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA); // UART1 TX
  UART1_DefInit();
  UART1_ByteTrigCfg(UART_7BYTE_TRIG);
  UART1_INTCfg(ENABLE, RB_IER_THR_EMPTY);
  PFIC_EnableIRQ(UART1_IRQn);

  while (1) {
    printf("ChipID: %02x\t SysClock: %ldHz\t", R8_CHIP_ID, GetSysClock());
    printf("%ld\r\n", jiffies);
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

__HIGH_CODE
__INTERRUPT
void UART1_IRQHandler(void) {
  switch (UART1_GetITFlag()) {
  case UART_II_THR_EMPTY: // trigger when THR and FIFOtx all empty
    while (ringbufferAvailable(&txBuffer) && R8_UART1_TFC != UART_FIFO_SIZE) {
      R8_UART1_THR = ringbufferGet(&txBuffer);
    }
    break;
  }
}
