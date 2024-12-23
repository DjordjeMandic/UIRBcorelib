/**
 * @file PowerInfoData.cpp
 * @brief Implementation of power monitoring and state estimation for the %UIRB system.
 *
 * This file implements the @ref uirbcore::PowerInfoData class, providing functionality to:
 * - Monitor supply voltage and charging current.
 * - Estimate charger and battery states (e.g., charging, full, low battery).
 * - Validate and convert power-related measurements.
 *
 * @details
 * The class relies on hardware-level functions from the @ref uirbcore::UIRB class for accurate ADC measurements 
 * and pin state retrieval. It ensures safe operation by detecting invalid or out-of-range values.
 *
 * @author 
 * Djordje Mandic (https://linktr.ee/djordjemandic)
 *
 * @version 0.2.0.0
 * @date 2024-12-12
 *
 * @copyright Copyright (c) 2024 Djordje Mandic (https://linktr.ee/djordjemandic)
 * 
 * MIT License
 * 
 * Copyright (c) 2024 Djordje Mandic (https://linktr.ee/djordjemandic)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <Arduino.h>
#include <UIRBcore.hpp>
#include <Utility.hpp>

namespace uirbcore
{
    bool PowerInfoData::update(uint8_t samples)
    {
        UIRB& uirbInstance = UIRB::getInstance();

        // if uirb init failed, do not update to prevent hardware damage
        if (!uirbInstance.begin() || samples == 0)
        {
            return false;
        }

        // false if any of the sampled data is invalid
        bool sampled_data_valid = true;

        // get new data
        this->supply_voltage_milivolts_ = uirbInstance.getSupplyVoltageMilivolts(samples);
        sampled_data_valid &= (this->supply_voltage_milivolts_ != UIRB::INVALID_VOLTAGE_MILIVOLTS);

        this->prog_voltage_milivolts_ = uirbInstance.getProgVoltageMilivolts(samples);
        sampled_data_valid &= (this->prog_voltage_milivolts_ != UIRB::INVALID_VOLTAGE_MILIVOLTS);

        this->prog_pin_mode_ = getPinMode(PIN_PROG);
        sampled_data_valid &= (this->prog_pin_mode_ != INVALID_PIN_MODE);

        if (sampled_data_valid)
        {
            this->prog_pin_state_ = digitalRead(PIN_PROG);
            this->charging_current_miliamps_ = PowerInfoData::prog_milivolts_to_charging_current_miliamps(
                this->prog_voltage_milivolts_,
                uirbInstance.getChargerProgResistorResistance(),
                this->prog_pin_mode_,
                this->prog_pin_state_
            );

            sampled_data_valid &= (this->charging_current_miliamps_ != UIRB::INVALID_CURRENT_MILIAMPS);

            if(sampled_data_valid)
            {
                this->estimated_charger_state_ = get_estimated_charger_state();
                this->estimated_battery_state_ = get_estimated_battery_state();
            }
        }

        return sampled_data_valid;
    }
    
    bool PowerInfoData::isValid() const
    {
        return this->supply_voltage_milivolts_  != UIRB::INVALID_VOLTAGE_MILIVOLTS &&
               this->prog_voltage_milivolts_    != UIRB::INVALID_VOLTAGE_MILIVOLTS &&
               this->charging_current_miliamps_ != UIRB::INVALID_CURRENT_MILIAMPS  &&
               this->prog_pin_mode_             != INVALID_PIN_MODE;
    }

    BatteryState PowerInfoData::getBatteryState() const
    {
        return this->estimated_battery_state_;
    }

    BatteryState PowerInfoData::get_estimated_battery_state() const
    {
        // if any parameters are invalid, return error
        if (this->supply_voltage_milivolts_ == UIRB::INVALID_VOLTAGE_MILIVOLTS ||
            this->estimated_charger_state_  == ChargerState::ERROR)
        {
            return BatteryState::ERROR;
        }

        // if charging, return charging
        if (this->isBatteryCharging())
        {
            return BatteryState::CHARGING;
        }

        // if charger is floating (waiting for next recharge) or fully charged, return full
        if (this->estimated_charger_state_ == ChargerState::FLOATING ||
            this->isBatteryFull())
        {
            return BatteryState::FULLY_CHARGED;
        }
        // past this state, chargerstate is unknown or turned off

        // if battery is low, return low
        if (this->isBatteryLow())
        {
            return BatteryState::EMPTY;
        }

        // if charger is turned off and battery is not full, return not charging
        if (this->estimated_charger_state_ == ChargerState::TURNED_OFF &&
            !this->isBatteryFull())
        {
            return BatteryState::NOT_CHARGING;
        }

        // unhandled case
        return BatteryState::UNKNOWN;
    }

    ChargerState PowerInfoData::get_estimated_charger_state() const
    {
        // if uirb returns error, state is error
        if (this->charging_current_miliamps_ == UIRB::INVALID_CURRENT_MILIAMPS  ||
            this->prog_voltage_milivolts_    == UIRB::INVALID_VOLTAGE_MILIVOLTS ||
            this->supply_voltage_milivolts_  == UIRB::INVALID_VOLTAGE_MILIVOLTS)
        {
            return ChargerState::ERROR;
        }
        
        if (this->charging_current_miliamps_ == UIRB::UNKNOWN_CURRENT_MILIAMPS)
        {
            return ChargerState::UNKNOWN;
        }

        // past this state, the prog voltage is less than PROG_CC_CHARGE_VOLTAGE_MAX_MV

        // if charging current is not 0, prog voltage is less than PROG_CC_CHARGE_VOLTAGE_MAX_MV
        // anything in between PROG_CC_CHARGE_VOLTAGE_MIN_MV and PROG_CC_CHARGE_VOLTAGE_MAX_MV 
        // is considered constant current mode
        if (this->prog_voltage_milivolts_ >= UIRB::PROG_CC_CHARGE_VOLTAGE_MIN_MV)
        {
            // Charger cannot be in CC mode if the supply voltage is above or equal to fully charged voltage
            if (this->supply_voltage_milivolts_ >= UIRB::FULLY_CHARGED_SUPPLY_VOLTAGE_MIN_MV)
            {
                return ChargerState::UNKNOWN;
            }

            return ChargerState::CHARGING_CC;
        }
        // past this state, the prog voltage is less than PROG_CC_CHARGE_VOLTAGE_MIN_MV

        // if prog voltage is between PROG_CC_CHARGE_VOLTAGE_MIN_MV and PROG_CV_CHARGE_VOLTAGE_MIN_MV
        // charger is in constant voltage mode
        if (this->prog_voltage_milivolts_ >= UIRB::PROG_CV_CHARGE_VOLTAGE_MIN_MV)
        {
            // Charger cannot be in CV mode if the supply voltage is below recharge minimum
            if (this->supply_voltage_milivolts_ <= UIRB::FLOAT_VOLTAGE_RECHARGE_MIN_MV)
            {
                return ChargerState::UNKNOWN;
            }

            return ChargerState::CHARGING_CV;
        }
        // past this state, the prog voltage is less than PROG_CV_CHARGE_VOLTAGE_MIN_MV

        // if not in constant voltage mode, but supply voltage indicates floating then charger is in float voltage mode
        // todo: test and maybe use UIRB::FULLY_CHARGED_SUPPLY_VOLTAGE_MIN_MV
        // todo answr: AVCC measuring is not reliable to that point, in my case 4.2V is measured as 4107mV
        if (this->supply_voltage_milivolts_ >= UIRB::FLOAT_VOLTAGE_RECHARGE_MIN_MV)
        {
            return ChargerState::FLOATING;
        }

        // if uirb charging current returns 0 that means its off (externally)
        if (this->charging_current_miliamps_ == 0)
        {
            return ChargerState::TURNED_OFF;
        }

        // if supply voltage is below floating minimum and current is low then state is unknown
        return ChargerState::UNKNOWN;
    }

    bool PowerInfoData::isBatteryLow(const bool flashSTATOnLowBattery) const
    {
        bool batteryLow = true;
        
        // if supply voltage is invalid, return true (battery is low)
        if (this->supply_voltage_milivolts_ == UIRB::INVALID_VOLTAGE_MILIVOLTS)
        {
            return batteryLow;
        }

        batteryLow = this->supply_voltage_milivolts_ < UIRB::BATTERY_EMPTY_SUPPLY_VOLTAGE_MIN_MV;
        
        if (batteryLow && flashSTATOnLowBattery && !this->isBatteryCharging())
        {
            UIRB::notifyStatusLowBattery();
        }

        return batteryLow;
    }

    bool PowerInfoData::isBatteryFull() const
    {
        // if supply voltage is invalid, return false (battery is not full)
        if (this->supply_voltage_milivolts_ == UIRB::INVALID_VOLTAGE_MILIVOLTS)
        {
            return false;
        }

        return this->supply_voltage_milivolts_ >= UIRB::FLOAT_VOLTAGE_RECHARGE_MIN_MV;
    }

    bool PowerInfoData::isBatteryCharging() const
    {
        return this->estimated_charger_state_ == ChargerState::CHARGING_CV ||
               this->estimated_charger_state_ == ChargerState::CHARGING_CC;
    }

    ChargerState PowerInfoData::getChargerState() const
    {
        return this->estimated_charger_state_;
    }

    float PowerInfoData::getSupplyVoltage() const
    {
        return PowerInfoData::avcc_milivolts_to_volts(this->supply_voltage_milivolts_);
    }

    float PowerInfoData::getProgVoltage() const
    {
        return PowerInfoData::prog_milivolts_to_volts(this->prog_voltage_milivolts_);
    }

    float PowerInfoData::getChargingCurrent() const
    {
        return PowerInfoData::charging_current_miliamps_to_amps(this->charging_current_miliamps_);
    }

    float PowerInfoData::prog_milivolts_to_volts(const uint16_t prog_milivolts)
    {
        if (prog_milivolts == UIRB::INVALID_VOLTAGE_MILIVOLTS)
        {
            return NAN;
        }

        return static_cast<float>(prog_milivolts) / 1000.0f;
    }

    float PowerInfoData::avcc_milivolts_to_volts(const uint16_t avcc_milivolts)
    {
        // Check if in valid range
        if (avcc_milivolts == UIRB::INVALID_VOLTAGE_MILIVOLTS)
        {
            return NAN;
        }

        // Return infinity if read avcc is too high (possible damage to hardware, value not trustworthy)
        if (avcc_milivolts > UIRB::AVCC_MILIVOLTS_MAX)
        {
            return INFINITY;
        }

        // If read avcc is too low value is not trustworthy, return negative infinity because were operating outside of recommended Vcc range
        if (avcc_milivolts < UIRB::AVCC_MILIVOLTS_8MHZ_MIN)
        {
            return -INFINITY;
        }

        return static_cast<float>(avcc_milivolts) / 1000.0f;
    }

    float PowerInfoData::charging_current_miliamps_to_amps(const uint16_t charging_current_miliamps)
    {
        if (charging_current_miliamps == UIRB::INVALID_CURRENT_MILIAMPS ||
            charging_current_miliamps == UIRB::UNKNOWN_CURRENT_MILIAMPS)
        {
            return NAN;
        }

        return static_cast<float>(charging_current_miliamps) / 1000.0f;
    }

    uint16_t PowerInfoData::prog_milivolts_to_charging_current_miliamps(const uint16_t prog_milivolts, const uint16_t prog_resistor_ohms, const uint8_t prog_pin_mode, const bool prog_pin_state)
    {
        // Rprog resistance in ohms from eeprom. 1ohm if invalid, should not return 0

        if (prog_milivolts == UIRB::INVALID_VOLTAGE_MILIVOLTS ||
            prog_resistor_ohms == UIRB::INVALID_PROG_RESISTOR_RESISTANCE ||
            prog_resistor_ohms == 0 || prog_pin_mode == INVALID_PIN_MODE)
        {
            return UIRB::INVALID_CURRENT_MILIAMPS;
        }

        switch (prog_pin_mode)
        {
            case INPUT:
                // Input pin mode does not affect prog
                break;
            case OUTPUT:
                // if high, prog is above 1V due to RC circuit on PIN_PROG
                if (prog_pin_state)
                {
                    // above 1V prog, current should be 0
                    return 0;
                }
                
                // if low, RC circuit acts as aditional 10K parallel resistor
                // prog_resistor_ohms = (prog_resistor_ohms * 10000U) / (prog_resistor_ohms + 10000U);
                // it results in prog_resistor_ohms = 3333U; -> 300.3mA

                // anyways, current cannot be measured, only estimated in that case
                return UIRB::UNKNOWN_CURRENT_MILIAMPS;
            case INPUT_PULLUP:
                // Internal pullup might affect prog but voltage still can be read
                return UIRB::UNKNOWN_CURRENT_MILIAMPS;
                break;
            default:
                return UIRB::INVALID_CURRENT_MILIAMPS;
        }

        // TP4057 CA comparator/opamp lowers output current when prog is above Vprog
        // when output is below 10mV charging current is 0mA (When charger stops charging, output drops to around 10mV automatically)
        if (prog_milivolts > UIRB::PROG_CC_CHARGE_VOLTAGE_MAX_MV || prog_milivolts < UIRB::PROG_FLOAT_VOLTAGE_MAX_MV)
        {
            return 0;
        }

        // 5K Rprog gives 200mA Ichg limit at 1V Vprog

        // Calculate charging current in milliamps (mA) using Ohm's Law (I = V / R)
        // if (Ibat <= 0.3A) Rprog = (1000 / Ibat) // Rprog minimum 3333ohm (5k||10k)
        // if (Ibat > 0.3A) Rprog = (1000 / Ibat) * (1.3 - Ibat)
        uint32_t charging_current_miliamps = (static_cast<uint32_t>(prog_milivolts) * 1000U) / prog_resistor_ohms;

        // limit to minimum of 1 mA, 0 is for off state
        return static_cast<uint16_t>((charging_current_miliamps == 0U) ? 1U : charging_current_miliamps);
    }
}