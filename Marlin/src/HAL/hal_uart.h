#ifndef USART_H
#define USART_H

#include "../common/common.h"

#define DEFAULT_UART_BUF_SIZE (512)

enum {
    USART_1,
    USART_2,
    USART_INVALID
};

class HalUart {
 public:
  HalUart() {
    port_ = USART_INVALID;
  }
  ErrCode Init(uint8_t usart, uint32_t baudrate=115200, uint32_t buf_size=DEFAULT_UART_BUF_SIZE);
  ErrCode SetBufSize(uint32_t size);
  ErrCode SetNVIC(uint8_t PreemptionPriority, uint8_t SubPriority);
  ErrCode PutC(unsigned char c);
  int32_t Puts(unsigned char *str,int num);
  ErrCode GetC(uint8_t &out);
 private:
  uint8_t port_ = USART_INVALID;
};

#endif
