/*
 * Snapmaker2-Modules Firmware
 * Copyright (C) 2019-2020 Snapmaker [https://github.com/Snapmaker]
 *
 * This file is part of Snapmaker2-Modules
 * (see https://github.com/Snapmaker/Snapmaker2-Modules)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SNAPMAKERMODULES_MARLIN_SRC_MODULE_POWER_MODULE_H_
#define SNAPMAKERMODULES_MARLIN_SRC_MODULE_POWER_MODULE_H_

#include "src/device/switch.h"
#include "src/configuration.h"
#include "module_base.h"
#include "../device/switch.h"
#include "../device/speed.h"


class PowerModule : public ModuleBase {

 public:
  void Init();
  void HandModule(uint16_t func_id, uint8_t * data, uint8_t data_len);
  void Loop();
  void EmergencyStop();

 private:
  void PowerRelayOn();
  void PowerRelayOff();
  void ReportAllInfo();

 private:
    uint8_t vm_adc_;
    uint8_t master_adc_;
    uint8_t temp_adc_;
    SwitchOutput relay_en_;
    SwitchOutput r_, g_, b_;
    SwitchInput ac_fault_in_;
    Speed fan_;
};

#endif
