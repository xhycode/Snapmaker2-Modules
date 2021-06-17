#include "./hal_uart.h"
#include "./hal_gpio.h"
#include "std_library/inc/stm32f10x.h"
#include "../utils/RingBuffer.h"

const USART_TypeDef *usarts[] = {
    USART1,
    USART2,
};

const uint8_t usarts_irqn[] = {
    USART1_IRQn,
    USART2_IRQn,
};

const uint32_t rcc_usarts[] = {
    RCC_APB2Periph_USART1,
    RCC_APB1Periph_USART2,
};

typedef struct {
  uint8_t tx;
  uint8_t rx;
} USART_PIN_T;

const USART_PIN_T usarts_pin[] = {
    {PA_9, PA_10}, // USART1
    {PA_2, PA_3},  // USART2
};

static RingBuffer<uint8_t> recv_buffer[USART_INVALID];

void HalUart::Init(uint8_t usart, uint32_t baudrate, uint32_t buf_size) {
  USART_InitTypeDef USART_InitStruct;
  if (usart < USART_INVALID) {
    port_ = usart;
    SetBufSize(buf_size);
  } else {
    port_ = USART_INVALID;
  }
  USART_DeInit((USART_TypeDef *)usarts[port_]);

  GpioInit(usarts_pin[port_].tx, GPIO_Mode_AF_PP);
  GpioInit(usarts_pin[port_].rx, GPIO_Mode_IN_FLOATING);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  if (port_ == USART_1)
    RCC_APB2PeriphClockCmd(rcc_usarts[port_], ENABLE);
  else
    RCC_APB1PeriphClockCmd(rcc_usarts[port_], ENABLE);

  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = 8;
  USART_InitStruct.USART_BaudRate = baudrate;

  USART_Init((USART_TypeDef *)usarts[port_], &USART_InitStruct);
  USART_Cmd((USART_TypeDef *)usarts[port_], ENABLE);
  USART_ClearFlag((USART_TypeDef *)usarts[port_], USART_FLAG_TC);
  USART_ClearITPendingBit((USART_TypeDef *)usarts[port_], USART_IT_RXNE);
  USART_ITConfig((USART_TypeDef *)usarts[port_], USART_IT_RXNE, ENABLE);

  SetNVIC(1, 1);
}

void HalUart::SetBufSize(uint32_t size) {
  if (port_ < USART_INVALID) {
    recv_buffer[port_].deinit();
    recv_buffer[port_].init(size);
  }
}

void HalUart::SetNVIC(uint8_t PreemptionPriority, uint8_t SubPriority) {
  NVIC_InitTypeDef NVicInit;
  if (port_ >= USART_INVALID) {
  }
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVicInit.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
  NVicInit.NVIC_IRQChannel = usarts_irqn[port_];
  NVicInit.NVIC_IRQChannelSubPriority = SubPriority;
  NVicInit.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVicInit);
}

void HalUart::PutC(unsigned char c) {
  if (port_ >= USART_INVALID) {
  }
  USART_SendData((USART_TypeDef *)usarts[port_], c);
  while (USART_GetFlagStatus((USART_TypeDef *)usarts[port_], USART_FLAG_TXE) == RESET);
}

void HalUart::Puts(uint8_t *str, size_t lenght) {
  uint32_t i;
  if (port_ >= USART_INVALID) {
    return;
  }
  for (i = 0; i < lenght; i++) {
    USART_SendData((USART_TypeDef *)usarts[port_], *str++);
    while (USART_GetFlagStatus((USART_TypeDef *)usarts[port_], USART_FLAG_TXE) == RESET);
  }
}

bool HalUart::GetC(uint8_t &out) {
  if (!recv_buffer[port_].isEmpty()) {
    out = recv_buffer[port_].remove();
    return true;
  }
  return false;
}

extern "C"
{

void __irq_usart1(void) {
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
    uint8_t ch = USART_ReceiveData(USART1);
    if (!recv_buffer[USART_1].isFull()) {
      recv_buffer[USART_1].insert(ch);
    }
    USART_ClearFlag(USART1, USART_IT_RXNE);
  }
}

void __irq_usart2(void)
{
  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
    uint8_t ch = USART_ReceiveData(USART2);
    if (!recv_buffer[USART_2].isFull()) {
      recv_buffer[USART_2].insert(ch);
    }
    USART_ClearFlag(USART1, USART_IT_RXNE);
  }
}

}
