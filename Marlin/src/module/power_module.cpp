
#include "power_module.h"
#include "src/HAL/hal_adc.h"
#include "src/HAL/hal_uart.h"
#include "src/core/thermistor_table.h"
#include <wirish_time.h>

HalUart debg_uart;

#define POWER_RELAY_EN_PIN PB5
#define AC_FAULT_IN_PIN PB2

#define ADC_PERIOD_US 500
#define ADC_USED_TIM ADC_TIM_1
#define ADC_VM_PIN PA3
#define ADC_24V_MASTER_PIN PA4
#define ADC_TEMP_PIN PA6

#define FAN_PWM_TIM  PWM_TIM2
#define FAN_PWM_CH   PWM_CH2
#define FAN_PWM_PIN  PA1

#define FAN_FLAG_PIN PA2
#define FAN_FLAG_TIM 4

#define R_PIN PA7
#define G_PIN PB0
#define B_PIN PB1

void DebugInfo(const char *info, int32_t val) {
  debg_uart.Puts(info);
  debg_uart.PutC(':');
  debg_uart.PutInt(val);
  debg_uart.PutC('\n');
}

void ReportFaulitIn(uint8_t status, uint16_t v) {
  DebugInfo("in", (int)status);
  DebugInfo("vm", (int)v);

}

static uint16_t AdcToMv(uint16_t adc_val) {
  return 3.3 * adc_val * 10 * 1000 / 4096;
}

void PowerModule::Init() {
  debg_uart.Init(USART_1, 115200);
  vm_adc_ = HAL_adc_init(ADC_VM_PIN, ADC_USED_TIM, ADC_PERIOD_US);
  master_adc_ = HAL_adc_init(ADC_24V_MASTER_PIN, ADC_USED_TIM, ADC_PERIOD_US);
  temp_adc_ = HAL_adc_init(ADC_TEMP_PIN, ADC_USED_TIM, ADC_PERIOD_US);

  relay_en_.Init(POWER_RELAY_EN_PIN, 1, OUTPUT);
  ac_fault_in_.Init(AC_FAULT_IN_PIN);
  debg_uart.Puts("power module start");

  r_.Init(R_PIN, 0, OUTPUT);
  g_.Init(G_PIN, 0, OUTPUT);
  b_.Init(B_PIN, 0, OUTPUT);

  fan_.InitCapture(FAN_FLAG_PIN, FAN_FLAG_TIM);
  fan_.InitOut(FAN_PWM_PIN, FAN_PWM_TIM, FAN_PWM_CH, 24*1000);
  fan_.PwmInverter(true);
  fan_.SetLapPulse(2);

  fan_.SetSpeed(0);
}

void PowerModule::ReportAllInfo() {
  int32_t val;
  ReportFaulitIn(ac_fault_in_.Read(), ADC_Get(vm_adc_));
  val = fan_.ReadCurSpeed();
  DebugInfo("speed", val);

  val = ADC_Get(vm_adc_);
  DebugInfo("VM", AdcToMv(val));

  val = ADC_Get(master_adc_);
  DebugInfo("master", AdcToMv(val));

  val = TempTableCalcCurTemp(ADC_GetCusum(temp_adc_)) * 10;
  
}

void PowerModule::HandModule(uint16_t func_id, uint8_t * data, uint8_t data_len) {

}

uint32_t test_time = 0;
bool power_loss = false;
void PowerModule::Loop() {
  if (ac_fault_in_.CheckStatusLoop()) {
    ReportFaulitIn(ac_fault_in_.Read(), ADC_Get(vm_adc_));
    if (ac_fault_in_.Read() == 0) {
      power_loss = true;
      test_time = millis();
    } else {
      power_loss = false;
    }
  }
  if (power_loss) {
    uint32_t val = AdcToMv(ADC_Get(vm_adc_));
    if (val < 10000) {
      power_loss = false;
      DebugInfo("loss", millis() - test_time);
    }
  }

  fan_.SpeedOutCtrl();
  uint8_t ch;
  if (debg_uart.GetC(ch)) {
    if (ch == 'i') {
      ReportAllInfo();
    } else if (ch == 'a') {
      fan_.SetSpeed(100);
    } else if ('0' <= ch && ch <= '9') {
      fan_.SetSpeed((ch - '0') * 10);
    } else if (ch == 'r') {
      r_.Out(1);
    } else if (ch == 'g') {
      g_.Out(1);
    } else if (ch == 'b') {
      b_.Out(1);
    } else if (ch == 'c') {
      r_.Out(0);
      g_.Out(0);
      b_.Out(0);
    }
    
  }
}

void PowerModule::EmergencyStop() {

}
