#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <string.h>

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
  void Init(uint8_t usart, uint32_t baudrate=115200, uint32_t buf_size=DEFAULT_UART_BUF_SIZE);
  void SetBufSize(uint32_t size);
  void SetNVIC(uint8_t PreemptionPriority, uint8_t SubPriority);
  void PutC(unsigned char c);
  void Puts(uint8_t *str, size_t num);
  bool GetC(uint8_t &out);
  void Puts(uint8_t *data, uint16_t len) {
    Puts(data, len);
  }

  void Puts(uint8_t *data) {
    Puts(data, strlen((char *)data));
  }

  void Puts(const uint8_t *data) {
    Puts((uint8_t *)data, strlen((char *)data));
  }

  void Puts(const char *data) {
    Puts((uint8_t *)data, strlen((char *)data));
  }

  void Putc(uint8_t ch) {
    PutC(ch);
  }

  void PutInt(int32_t val) {
    uint8_t buf[10];
    uint8_t index=0; 
    if (val == 0) {
      PutC('0');
    } else{
      if (val < 0) {
        PutC('-');
        val = -val;
      }
      while (val > 0 && (index < 10)) {
        buf[index++] = (val % 10) + '0';
        val /= 10;
      }
    }
    while (index) {
      PutC(buf[--index]);
    }
  }
  void PutFloat(float val) {
    PutInt((int32_t)val);
    Putc('.');
    if (val < 0) val = -val;
    PutInt((int32_t)(val * 1000) % 1000);
  }
 private:
  uint8_t port_ = USART_INVALID;
};

#endif
