/**
 * @file UIRBcore.cpp
 * @brief Implementation of core functionalities for the %UIRB system.
 *
 * This file contains the implementation of the @ref uirbcore::UIRB class, providing essential 
 * hardware initialization, power management, EEPROM configuration, and system monitoring.
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
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

using namespace uirbcore;

static volatile bool pcint2_interrupt_flag = false;

bool UIRB::getButtonWakeupISRFlag() const
{
#if defined(AVR_DEBUG)
    return false;
#else  // defined(AVR_DEBUG)
    return this->isr_wakeup_button_flag_;
#endif  // defined(AVR_DEBUG)
}

bool UIRB::getAndClearButtonWakeupISRFlag()
{
#if defined(AVR_DEBUG)
    return false;
#else  // defined(AVR_DEBUG)
    bool flag = this->getButtonWakeupISRFlag();
    this->clearButtonWakeupISRFlag();
    return flag;
#endif  // defined(AVR_DEBUG)
}

bool UIRB::getIO3WakeupISRFlag() const
{
#if defined(AVR_DEBUG)
    return false;
#else  // defined(AVR_DEBUG)
    return this->isr_wakeup_io3_flag_;
#endif  // defined(AVR_DEBUG)
}

bool UIRB::getAndClearIO3WakeupISRFlag()
{
#if defined(AVR_DEBUG)
    return false;
#else  // defined(AVR_DEBUG)
    bool flag = this->getIO3WakeupISRFlag();
    this->clearIO3WakeupISRFlag();
    return flag;
#endif  // defined(AVR_DEBUG)
}

bool UIRB::getWakeupISRFlag() const
{
#if defined(AVR_DEBUG)
    return false;
#else  // defined(AVR_DEBUG)
    return this->isr_wakeup_button_flag_ || this->isr_wakeup_io3_flag_;
#endif  // defined(AVR_DEBUG)
}

bool UIRB::getAndClearWakeupISRFlag()
{
#if defined(AVR_DEBUG)
    return false;
#else  // defined(AVR_DEBUG)
    bool flag = this->getWakeupISRFlag();
    this->clearButtonWakeupISRFlag();
    this->clearIO3WakeupISRFlag();
    return flag;
#endif  // defined(AVR_DEBUG)
}

void UIRB::clearButtonWakeupISRFlag()
{
#if !defined(AVR_DEBUG)
    this->isr_wakeup_button_flag_ = false;
#endif  // !defined(AVR_DEBUG)
}

void UIRB::clearIO3WakeupISRFlag()
{
#if !defined(AVR_DEBUG)
    this->isr_wakeup_io3_flag_ = false;
#endif  // !defined(AVR_DEBUG)
}

void UIRB::clearWakeupISRFlags()
{
#if !defined(AVR_DEBUG)
    this->clearButtonWakeupISRFlag();
    this->clearIO3WakeupISRFlag();
#endif  // !defined(AVR_DEBUG)
}

void UIRB::setButtonWakeupCallback(void (*callback)())
{
#if !defined(AVR_DEBUG)
    this->button_wakeup_user_callback_ = callback;
#endif  // !defined(AVR_DEBUG)
}


void UIRB::setIO3WakeupCallback(void (*callback)())
{
#if !defined(AVR_DEBUG)
    this->io3_wakeup_user_callback_ = callback;
#endif  // !defined(AVR_DEBUG)
}
void UIRB::button_wakeup_isr()
{
#if !defined(AVR_DEBUG)
    UIRB& instance = UIRB::getInstance();
    instance.isr_wakeup_button_flag_ = true;
    instance.isr_wakeup_button_flag_internal_ = true;
#endif  // !defined(AVR_DEBUG)
}

void UIRB::usb_io3_wakeup_isr()
{
#if !defined(AVR_DEBUG)
    UIRB& instance = UIRB::getInstance();
    instance.isr_wakeup_io3_flag_ = true;
    instance.isr_wakeup_button_flag_internal_ = true;
#endif  // !defined(AVR_DEBUG)
}

PowerInfoData& UIRB::getPowerInfo(const uint8_t samples, const bool flashSTATOnLowBattery)
{
    this->powerInfoData_.update(samples);
    this->powerInfoData_.isBatteryLow(flashSTATOnLowBattery);
    return this->powerInfoData_;
}

UIRB::UIRB()
{
    // Check this first to prevent damage to hardware
    if (!this->eepromDataManager_.hardware_version_matches())
    {
        this->initializationResult_ = CoreResult::ERROR_EEPROM_HW_VER_MISMATCH;
        wdt_enable(WDTO_2S);
        while (1);
    }

    pinMode(PIN_IR_LED, OUTPUT);    digitalWrite(PIN_IR_LED, LOW); 
    pinMode(PIN_STAT_LED, OUTPUT);  digitalWrite(PIN_STAT_LED, HIGH);
    pinMode(PIN_PROG, INPUT);
    pinMode(PIN_PULLDOWN_RESISTOR, INPUT);
    pinMode(PIN_USB_IO3, INPUT_PULLUP);
    pinMode(PIN_BUTTON_OPTION_1, INPUT_PULLUP);
    pinMode(PIN_BUTTON_OPTION_2, INPUT_PULLUP);
    pinMode(PIN_BUTTON_OPTION_3, INPUT_PULLUP);
    pinMode(PIN_BUTTON_WAKEUP, INPUT_PULLUP);

    this->eepromDataManager_.increment_boot_count();
    
#if defined(AVR_DEBUG)
    this->eepromDataManager_.set_avr_serial_debugger(true);
    #warning "AVR debugger detected. Serial debugging setting will be set to enabled."
#else  // defined(AVR_DEBUG)
    this->eepromDataManager_.set_avr_serial_debugger(false);
#endif  // defined(AVR_DEBUG)
    
    if (!this->eepromDataManager_.save_to_eeprom())
    {
        this->initializationResult_ = CoreResult::ERROR_EEPROM_SAVE_FAILED;
        return;
    }

    if (this->eepromDataManager_.get_charger_prog_resistor_ohms() == eeprom::EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE)
    {
        this->initializationResult_ = CoreResult::ERROR_EEPROM_CHARGER_PROG_RESISTANCE_INVALID;
        return;
    }
    digitalWrite(PIN_STAT_LED, LOW);
    this->initializationResult_ = CoreResult::SUCCESS;
}

CoreResult UIRB::begin() const
{
    return this->initializationResult_;
}

bool UIRB::reloadFromEEPROM()
{
    this->eepromDataManager_.load_from_eeprom();

    return this->getChargerProgResistorResistance() != eeprom::EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE;
}

bool UIRB::hasUnsavedDataChanges() const
{
    return this->eepromDataManager_.get() != eeprom::EEPROMDataManager::read_from_eeprom();
}

bool UIRB::saveToEEPROM()
{
    if (!this->initializationResult_)
    {
        return false;
    }
    return this->eepromDataManager_.save_to_eeprom();
}

eeprom::EEPROMData UIRB::getDataStoredInRAM() const
{
    return this->eepromDataManager_.get();
}

void UIRB::getDataStoredInRAM(eeprom::EEPROMData& data) const
{
    return this->eepromDataManager_.get(data);
}

void UIRB::powerDown(const uint32_t sleeptime_milliseconds, const WakeupInterrupt wakeupSource)
{
#if defined(AVR_DEBUG)
    #warning "AVR debugger detected. Sleeping is disabled. UIRB::powerDown() will do nothing."
#else  // defined(AVR_DEBUG)
    if (!this->isSleepingAllowed())
    {
        return;
    }
    bool attachWake = wakeupSource == WakeupInterrupt::WAKE_BUTTON || wakeupSource == WakeupInterrupt::WAKE_BUTTON_AND_USB_IO3;
    bool attachIO3 = this->isWakeupFromIO3Allowed() && (wakeupSource == WakeupInterrupt::USB_IO3 ||
                                                        wakeupSource == WakeupInterrupt::WAKE_BUTTON_AND_USB_IO3);

    digitalWrite(PIN_IR_LED, LOW); // turn off ir led
    uint8_t io3Mode_old = INVALID_PIN_MODE;
    bool io3State_old = false;
    uint8_t adcsra_old = ADCSRA; // save adc state
    uint8_t acsr_old = ACSR; // save comparator state
    uint8_t oldAnalogRef = getAnalogReference();

    setAnalogReference(EXTERNAL);
    bitClear(ADCSRA, ADEN); // turn off adc
    bitClear(ACSR, ACD); // Disable Analog Comparator
    power_adc_disable();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    noInterrupts();
    
    if (attachWake)
    {
        attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_WAKEUP), button_wakeup_isr, FALLING);
    }
    
    if (attachIO3)
    {
        volatile uint8_t* pcicr = digitalPinToPCICR(PIN_USB_IO3);
        volatile uint8_t* pcmsk = digitalPinToPCMSK(PIN_USB_IO3);
        if (pcicr != nullptr && pcmsk != nullptr)
        {
            io3Mode_old = getPinMode(PIN_USB_IO3);
            io3State_old = digitalRead(PIN_USB_IO3);
            pinMode(PIN_USB_IO3, INPUT_PULLUP);
            *pcicr |= (1 << digitalPinToPCICRbit(PIN_USB_IO3)); // Enable the PCINT group in PCICR
            *pcmsk |= (1 << digitalPinToPCMSKbit(PIN_USB_IO3)); // Enable specific pin interrupt in PCMSK
        }
        else
        {
            attachIO3 = false;
        }
    }

    uint32_t remaining_time = sleeptime_milliseconds;
    const uint16_t wdt_intervals[] = {16, 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000}; // ms for WDTO_15MS to WDTO_8S
    uint8_t wdt_period = 0;
    
    this->isr_wakeup_button_flag_internal_ = false;
    this->isr_wakeup_io3_flag_internal_ = false;
    pcint2_interrupt_flag = false;

    if (sleeptime_milliseconds > 0)
    {
        while (remaining_time > 0)
        {
            // Find the largest interval less than or equal to remaining_time
            for (int8_t i = sizeof(wdt_intervals) / sizeof(wdt_intervals[0]) - 1; i >= 0; --i)
            {
                if (remaining_time >= wdt_intervals[i])
                {
                    wdt_period = i;
                    break;
                }
            }

            // Configure and enable the watchdog timer
            wdt_enable(wdt_period);
            bitSet(WDTCSR, WDIE); // Set Watchdog interrupt enable

            cli();
            sleep_enable();
            sei();
            sleep_cpu(); // enter sleep mode
            sleep_disable();
            sei();

            // Disable watchdog after waking up
            wdt_disable();
            // Calculate remaining time, set to 0 if wakeup was triggered from IO
            if (this->isr_wakeup_button_flag_internal_ || pcint2_interrupt_flag)
            {
                remaining_time = 0;
            }
            else if (remaining_time > wdt_intervals[wdt_period])
            {
                remaining_time -= wdt_intervals[wdt_period];
            }
            else
            {
                remaining_time = 0;
            }
        }
    }
    else
    {
        cli();
        sleep_enable();
        sei();
        sleep_cpu(); // enters sleep mode
        sleep_disable();
        sei();
    }

    if (pcint2_interrupt_flag)
    {
        noInterrupts();
        UIRB::usb_io3_wakeup_isr();
        interrupts();
    }

    if (attachWake)
    {
        detachInterrupt(digitalPinToInterrupt(PIN_BUTTON_WAKEUP));
    }

    if (attachIO3)
    {
        volatile uint8_t* pcicr = digitalPinToPCICR(PIN_USB_IO3);
        volatile uint8_t* pcmsk = digitalPinToPCMSK(PIN_USB_IO3);
        if (pcicr != nullptr && pcmsk != nullptr)
        {
            *pcicr &= ~(1 << digitalPinToPCICRbit(PIN_USB_IO3)); // Disable the PCINT group in PCICR
            *pcmsk &= ~(1 << digitalPinToPCMSKbit(PIN_USB_IO3)); // Disable specific pin interrupt in PCMSK
        }

        if (io3Mode_old != INVALID_PIN_MODE)
        {
            pinMode(PIN_USB_IO3, io3Mode_old);
            digitalWrite(PIN_USB_IO3, io3State_old);
        }
    }

    power_adc_enable();
    ADCSRA = adcsra_old; // restore adc state
    ACSR = acsr_old; // restore comparator state

    if (oldAnalogRef != INVALID_ANALOG_REF && oldAnalogRef != EXTERNAL)
    {
        setAnalogReference(oldAnalogRef);
    }

    if (this->isr_wakeup_button_flag_internal_ && this->button_wakeup_user_callback_ != nullptr)
    {
        this->button_wakeup_user_callback_();
    }
    this->isr_wakeup_button_flag_internal_ = false;

    if (attachIO3 && this->isr_wakeup_io3_flag_internal_ && this->io3_wakeup_user_callback_ != nullptr)
    {
        this->io3_wakeup_user_callback_();
    }
    this->isr_wakeup_io3_flag_internal_ = false;
#endif  // defined(AVR_DEBUG)
}

#if !defined(AVR_DEBUG)
ISR (WDT_vect)
{
    wdt_disable();
}

ISR (PCINT2_vect)
{
    pcint2_interrupt_flag = true;
}
#endif

uint8_t UIRB::getVersionMajor() const
{
    return this->eepromDataManager_.get_hardware_version().major;
}


uint8_t UIRB::getVersionMinor() const
{
    return this->eepromDataManager_.get_hardware_version().minor;
}

float UIRB::getVersion() const
{
    return static_cast<float>(this->getVersionMajor()) + (static_cast<float>(this->getVersionMinor()) / 10.0f);
}

uint8_t UIRB::getMonthOfManufacture() const
{
    return this->eepromDataManager_.get_board_manufacture_month();
}

uint16_t UIRB::getYearOfManufacture() const
{
    return this->eepromDataManager_.get_board_manufacture_year();
}

uint16_t UIRB::getBoardSerialNumber() const
{
    return this->eepromDataManager_.get_uirb_board_serial_number();
}

uint32_t UIRB::getBootCount() const
{
    return this->eepromDataManager_.get_boot_count();
}

uint8_t UIRB::getStatusLEDBrightness() const
{
    return this->eepromDataManager_.get_stat_led_brightness();
}

bool UIRB::setStatusLEDBrightness(const uint8_t brightness, const bool saveToEEPROM)
{
    this->eepromDataManager_.set_stat_led_brightness(brightness);
    if (!saveToEEPROM)
    {
        return false;
    }
    return this->saveToEEPROM();
}

void UIRB::setStatusLEDBrightness(const uint8_t brightness)
{
    this->eepromDataManager_.set_stat_led_brightness(brightness);
}

uint16_t UIRB::getChargerProgResistorResistance() const
{
    return this->eepromDataManager_.get_charger_prog_resistor_ohms();
}

bool UIRB::isSerialDebuggerEnabled() const
{
    return this->eepromDataManager_.is_avr_serial_debugger_enabled();
}

bool UIRB::isSleepingAllowed() const
{
#if defined(AVR_DEBUG)
    return false;
#else  // defined(AVR_DEBUG)
    return this->eepromDataManager_.is_sleep_mode_allowed();
#endif  // defined(AVR_DEBUG)
}

bool UIRB::setSleepingAllowed(const bool allowed, const bool saveToEEPROM)
{
    this->eepromDataManager_.allow_sleep_mode(allowed);
    if (!saveToEEPROM)
    {
        return false;
    }
    return this->saveToEEPROM();
}

void UIRB::setSleepingAllowed(const bool allowed)
{
    this->eepromDataManager_.allow_sleep_mode(allowed);
}

bool UIRB::isWakeupFromIO3Allowed() const
{
#if defined(AVR_DEBUG)
    return false;
#else  // defined(AVR_DEBUG)
    return this->eepromDataManager_.is_sleep_mode_io3_wakeup_allowed();
#endif  // defined(AVR_DEBUG)
}

bool UIRB::setWakeupFromIO3Allowed(const bool allowed, const bool saveToEEPROM)
{
    this->eepromDataManager_.allow_sleep_mode_io3_wakeup(allowed);
    if (!saveToEEPROM)
    {
        return false;
    }
    return this->saveToEEPROM();
}

void UIRB::setWakeupFromIO3Allowed(const bool allowed)
{
    this->eepromDataManager_.allow_sleep_mode_io3_wakeup(allowed);
}

bool UIRB::isBootCountingEnabled() const
{
    return this->eepromDataManager_.is_boot_count_increment_allowed();
}

String UIRB::getUSBSerialNumber() const
{
    eeprom::HardwareVersion hardware_version = this->eepromDataManager_.get_hardware_version();
    uint16_t serial_number = this->getBoardSerialNumber();
    String factory_serial_number = this->getFactoryUSBSerialNumber();

    if (hardware_version.version_byte == 0 || serial_number == eeprom::EEPROMDataManager::INVALID_UIRB_SERIAL_NUMBER || factory_serial_number.length() != eeprom::DATA_FACTORY_CP2104_SERIAL_NUM_LEN)
    {
        return String(F(""));
    }

    uint16_t manufacture_year = this->getYearOfManufacture();
    
    String serial_number_str = String(serial_number);
    while (serial_number_str.length() < 4) {
        serial_number_str = "0" + serial_number_str;
    }

    return String(F("UIRB-V")) + String(hardware_version.major) + String(hardware_version.minor) + String(F("-")) + 
            String(manufacture_year) + serial_number_str + String(F("-")) + factory_serial_number;
}

String UIRB::getFactoryUSBSerialNumber() const
{
    char buffer[eeprom::DATA_FACTORY_CP2104_SERIAL_NUM_LEN + 1];

    char* result = this->eepromDataManager_.get_factory_cp2104_usb_serial_number_cstr(buffer, sizeof(buffer));

    if (result == nullptr)
    {
        return String(F(""));
    }
    else
    {
        return String(result);
    }
}

uint16_t UIRB::getInternalBandgapReferenceVoltageMilivolts() const
{
    return this->eepromDataManager_.get_bandgap_reference_milivolts();
}

bool UIRB::setInternalBandgapReferenceVoltageMilivolts(const uint16_t milivolts, const bool saveToEEPROM)
{
    bool res = this->setInternalBandgapReferenceVoltageMilivolts(milivolts);
    if (!saveToEEPROM)
    {
        return false;
    }
    return res && this->saveToEEPROM();
}

bool UIRB::setInternalBandgapReferenceVoltageMilivolts(const uint16_t milivolts)
{
    return this->eepromDataManager_.set_bandgap_reference_milivolts(milivolts);
}

float UIRB::getInternalBandgapReferenceVoltage() const
{
    return static_cast<float>(this->getInternalBandgapReferenceVoltageMilivolts()) / 1000.0f;
}

uint16_t UIRB::getProgVoltageMilivolts(const uint8_t samples)
{
    uint16_t result_prog_raw = 0;
    uint8_t adcRef = INTERNAL1V1; // start sampling first with 1v1 reference
    
    if (!this->get_raw_prog_adc_sample(&result_prog_raw, adcRef, samples))
    {
        return UIRB::INVALID_VOLTAGE_MILIVOLTS;
    }

    uint16_t reference_voltage_milivolts = this->getInternalBandgapReferenceVoltageMilivolts();

    if (adcRef == DEFAULT) // if reference was changed to default, avcc was used as reference
    {
        reference_voltage_milivolts = this->getSupplyVoltageMilivolts();
        if (reference_voltage_milivolts == UIRB::INVALID_VOLTAGE_MILIVOLTS || 
            reference_voltage_milivolts > UIRB::AVCC_MILIVOLTS_ABSOLUTE_MAX ||
            reference_voltage_milivolts < UIRB::AVCC_MILIVOLTS_8MHZ_MIN)
        {
            return UIRB::INVALID_VOLTAGE_MILIVOLTS;
        }
    }

    uint32_t prog_voltage_milivolts = (static_cast<uint32_t>(result_prog_raw) * reference_voltage_milivolts);
    prog_voltage_milivolts += (UIRB::ADC_RESOLUTION_DEC / 2U);
    prog_voltage_milivolts /= UIRB::ADC_RESOLUTION_DEC;

    return static_cast<uint16_t>(prog_voltage_milivolts);
}

uint16_t UIRB::getSupplyVoltageMilivolts(const uint8_t samples)
{
    // Expected higher than ADC_BANDGAP_AVCC_SAMPLE_MIN, lower than 1024, higher the value, lower the AVcc
    uint16_t result_avcc_raw = 0;
    
    // Check if adc samling was successful and if the result is in valid range (most important not 0)
    if (!this->get_raw_bandgap_adc_sample(&result_avcc_raw, samples) || result_avcc_raw <= UIRB::ADC_BANDGAP_AVCC_SAMPLE_MIN || result_avcc_raw > UIRB::ADC_RESOLUTION_DEC - 1)
    {
        return UIRB::INVALID_VOLTAGE_MILIVOLTS;
    }

    uint32_t supply_voltage_milivolts = (static_cast<uint32_t>(UIRB::ADC_RESOLUTION_DEC) * this->getInternalBandgapReferenceVoltageMilivolts());
    supply_voltage_milivolts += (result_avcc_raw / 2U);
    supply_voltage_milivolts /= result_avcc_raw;

    // Convert to mV, max adc value is 2^10 = 1024 (10 bits)
    return static_cast<uint16_t>(supply_voltage_milivolts);
}

void UIRB::notifyStatusLowBattery()
{
    uint8_t oldMode = getPinMode(PIN_STAT_LED);
    uint8_t oldState = digitalRead(PIN_STAT_LED);
    
    if (oldMode != OUTPUT)
    {
        pinMode(PIN_STAT_LED, OUTPUT);
    }

    digitalWrite(PIN_STAT_LED, LOW);
    delay(500); // Pause
    // Morse code for "L": dot-dash-dot-dot
    digitalWrite(PIN_STAT_LED, HIGH); // Dot
    delay(50);
    digitalWrite(PIN_STAT_LED, LOW);
    delay(200);

    digitalWrite(PIN_STAT_LED, HIGH); // Dash
    delay(200);
    digitalWrite(PIN_STAT_LED, LOW);
    delay(200);

    digitalWrite(PIN_STAT_LED, HIGH); // Dot
    delay(50);
    digitalWrite(PIN_STAT_LED, LOW);
    delay(200);

    digitalWrite(PIN_STAT_LED, HIGH); // Dot
    delay(50);
    digitalWrite(PIN_STAT_LED, LOW);
    delay(500); // Pause

    if (oldMode != INVALID_PIN_MODE)
    {
        pinMode(PIN_STAT_LED, oldMode);
        if (oldMode == OUTPUT)
        {
            digitalWrite(PIN_STAT_LED, oldState);
        }
    }
}

CoreResult UIRB::get_raw_bandgap_adc_sample(uint16_t* result, const uint8_t samples)
{
    if (result == nullptr || samples == 0)
    {
        return CoreResult::ERROR_INVALID_ARGUMENT;
    }

    // Make sure that the IR LED is off
    digitalWrite(PIN_IR_LED, LOW);

    // Can store up to 256 10bit samples, should not exceed 18 bits
    uint32_t sample_sum = 0;

    // wiring library applies 0x07 mask to MUX[3..0] turning it into MUX[2..0] (ADMUX register) 
    // analogReference() and analogRead() Can set REFS1/REFS0 but mask in analogRead 
    // does not allow setting of MUX3 bit required for any other Single Ended Input
    // that is not ADC channel or temperature sensor. Vbg => MUX[3..0] = 0b1110
    uint8_t oldADCRef = getAnalogReference();
    uint8_t oldADCSRA = ADCSRA;
    
    ADCSRA |= bit (ADPS0) |  bit (ADPS1) | bit (ADPS2);  // Prescaler of 128
    // Read 1.1V reference against AVcc manually. Do first conversion with 1.1V reference
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // sets `DEFAULT` analog reference and 1v1 as analog input
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA,ADSC)); // Wait for conversion to complete
    delay(UIRB::ADC_VREF_SETTLE_DELAY_MS); // Wait for Vref to settle

    for(uint8_t i = 0; i < samples; i++)
    {
        ADCSRA |= _BV(ADSC); // Convert
        while (bit_is_set(ADCSRA,ADSC)); // Wait for conversion to complete
        // ADC macro takes care of reading ADC register.
        // avr-gcc implements the proper reading order: ADCL is read first.

        // if samples is set to 1, store result and return
        if (samples == 1)
        {
            *result = ADC;
            ADCSRA = oldADCSRA;
            if (oldADCRef != INVALID_ANALOG_REF && oldADCRef != DEFAULT) // default was already used by this method
            {
                setAnalogReference(oldADCRef);
            }
            return CoreResult::SUCCESS;
        }

        sample_sum += ADC;
        // delay all but last sample
        if (i < (samples - 1U))
        {
            delay(UIRB::ADC_SAMPLE_DELAY_MS);
        }
    }
    sample_sum += (samples / static_cast<uint8_t>(2U)); // https://stackoverflow.com/a/2422723
    // Store average in result
    *result = static_cast<uint16_t>(sample_sum / samples);
    ADCSRA = oldADCSRA;
    if (oldADCRef != DEFAULT) // default was already used by this method
    {
        setAnalogReference(oldADCRef);
    }
    return CoreResult::SUCCESS;
}

CoreResult UIRB::get_raw_prog_adc_sample(uint16_t* result, uint8_t& adcReference, const uint8_t samples)
{
    if (result == nullptr || samples == 0 || ((adcReference != DEFAULT) && (adcReference != INTERNAL1V1)))
    {
        return CoreResult::ERROR_INVALID_ARGUMENT;
    }

    // Make sure that the IR LED is off
    digitalWrite(PIN_IR_LED, LOW);
    bool oldPinMode = digitalRead(PIN_PROG);
    pinMode(PIN_PROG, INPUT); // It might be set to input_pullup or output with specific state

    uint8_t oldADCRef = getAnalogReference();
    uint8_t oldADCSRA = ADCSRA;
    uint32_t sample_adc = 0;

    bool outOfRange = false;

    // Retry sampling with different reference voltage if the result is out of range.
    do
    {
        // Can store up to 256 10bit samples, should not exceed 18 bits
        analogReference(adcReference); // Set internal 1.1V reference
        ADCSRA |= bit (ADPS0) |  bit (ADPS1) | bit (ADPS2);  // Prescaler of 128
        analogRead(PIN_PROG); // Set ADMUX and do initial conversion
        delay(UIRB::ADC_VREF_SETTLE_DELAY_MS); // Wait for Vref to settle
        if (samples == 1)
        {
            sample_adc = analogRead(PIN_PROG);
        }
        else
        {
            sample_adc = 0;
            for(uint8_t i = 0; i < samples; i++)
            {
                sample_adc += analogRead(PIN_PROG);
                // delay all but last sample
                if (i < (samples - 1U))
                {
                    delay(UIRB::ADC_SAMPLE_DELAY_MS);
                }
            }
            sample_adc += (samples / static_cast<uint8_t>(2U)); // https://stackoverflow.com/a/2422723
            sample_adc /= samples;
        }

        if ((adcReference == INTERNAL1V1) && (sample_adc == UIRB::ADC_SAMPLE_MAX))
        {
            adcReference = DEFAULT;
            outOfRange = true;
        } 
        else
        {
            outOfRange = false;
        }
    } while (outOfRange);

    // Store average in result
    *result = static_cast<uint16_t>(sample_adc);
    ADCSRA = oldADCSRA;
    if (oldPinMode != INPUT) // restore prog pin mode
    {
        pinMode(PIN_PROG, oldPinMode);
    } 

    if (oldADCRef != INVALID_ANALOG_REF && oldADCRef != adcReference) // if adc ref was changed, set it back 
    {
        setAnalogReference(oldADCRef);
    }
    return CoreResult::SUCCESS;
}
