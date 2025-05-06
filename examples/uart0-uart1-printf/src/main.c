#include "CH57x_common.h"
#include "ringbuffer.h"
#include <stdio.h>

#ifndef __INTERRUPT
#define __INTERRUPT __attribute__((interrupt("WCH-Interrupt-fast")))
#endif

volatile uint32_t jiffies = 0;
RingBuffer txBuffer0;
RingBuffer txBuffer1;

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
  if (fd == 1) { // stdout
    for (int i = 0; i < size; i++) {
      ringbufferPut(&txBuffer1, *buf++, TRUE);
      if (R8_UART1_LSR & RB_LSR_TX_FIFO_EMP) {
        while (ringbufferAvailable(&txBuffer1) &&
               R8_UART1_TFC < UART_FIFO_SIZE) {
          R8_UART1_THR = ringbufferGet(&txBuffer1);
        }
      }
    }
  }
  if (fd == 2) { // stderr
    for (int i = 0; i < size; i++) {
      ringbufferPut(&txBuffer0, *buf++, TRUE);
      if (R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) {
        while (ringbufferAvailable(&txBuffer0) &&
               R8_UART0_TFC < UART_FIFO_SIZE) {
          R8_UART0_THR = ringbufferGet(&txBuffer0);
        }
      }
    }
  }

  return size;
}

int main() {
  SetSysClock(CLK_SOURCE_PLL_60MHz);

  SysTick_Config(GetSysClock() / 60); // 60Hz

  ringbufferInit(&txBuffer0, 64);
  ringbufferInit(&txBuffer1, 64);

  GPIOB_SetBits(GPIO_Pin_7);
  GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA); // UART0 TX
  UART0_DefInit();
  UART0_ByteTrigCfg(UART_7BYTE_TRIG);
  UART0_INTCfg(ENABLE, RB_IER_THR_EMPTY);
  PFIC_EnableIRQ(UART0_IRQn);

  GPIOA_SetBits(GPIO_Pin_9);
  GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA); // UART1 TX
  UART1_DefInit();
  UART1_ByteTrigCfg(UART_7BYTE_TRIG);
  UART1_INTCfg(ENABLE, RB_IER_THR_EMPTY);
  PFIC_EnableIRQ(UART1_IRQn);

  while (1) {
    fprintf(stdout, "ChipID: %02x\t SysClock: %ldHz\r\n", R8_CHIP_ID,
            GetSysClock());
    fprintf(stderr, "%ld\r\n", jiffies);  // UART0
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
void UART0_IRQHandler(void) {
  switch (UART0_GetITFlag()) {
  case UART_II_THR_EMPTY: // trigger when THR and FIFOtx all empty
    while (ringbufferAvailable(&txBuffer0) && R8_UART0_TFC != UART_FIFO_SIZE) {
      R8_UART0_THR = ringbufferGet(&txBuffer0);
    }
    break;
  }
}

__HIGH_CODE
__INTERRUPT
void UART1_IRQHandler(void) {
  switch (UART1_GetITFlag()) {
  case UART_II_THR_EMPTY: // trigger when THR and FIFOtx all empty
    while (ringbufferAvailable(&txBuffer1) && R8_UART1_TFC != UART_FIFO_SIZE) {
      R8_UART1_THR = ringbufferGet(&txBuffer1);
    }
    break;
  }
}
