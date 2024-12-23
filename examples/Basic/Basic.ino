/**
 * @file Basic.ino
 * @brief Basic example demonstrating the use of the UIRBcore library.
 * 
 * This example showcases how to initialize and use the Universal Infrared Remote Board (UIRB) library 
 * for hardware control, power monitoring, sleep management, and wakeup interrupts. It provides a 
 * structured demonstration of key functionalities including EEPROM data management, ADC-based 
 * voltage measurements, battery state monitoring, and low-power sleep operations with wakeup sources.
 * 
 * **Features:**
 * - Initialization and hardware verification using the UIRBcore library.
 * - Reading and displaying boot count, bandgap reference, supply voltage, and charger state.
 * - Monitoring battery state: empty, charging, fully charged, and low-battery detection.
 * - Managing wakeup sources (#PIN_BUTTON_WAKEUP and #PIN_USB_IO3) with interrupt callbacks.
 * - Entering low-power sleep mode with configurable wakeup time and sources.
 * 
 * **Workflow:**
 * 1. The `uirbcore::UIRB` class instance is initialized.
 * 2. Various hardware parameters (voltage, brightness, serial number, etc.) are read and displayed.
 * 3. Power information is periodically sampled to monitor supply voltage, charger status, and battery state.
 * 4. The system enters low-power sleep mode with wakeup interrupts configured.
 * 5. Wakeup events (via #PIN_BUTTON_WAKEUP, #PIN_USB_IO3, or watchdog timer) are handled and displayed.
 * 
 * @note The program uses `Serial` for debugging purposes and outputs relevant system information.
 * 
 * @author 
 * Djordje Mandic (https://linktr.ee/djordjemandic)
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

/* Use namespace to prevent typing uirbcore:: before each member. */
using namespace uirbcore;

/**
 * @brief Instance of `UIRB` class.
 * 
 * During construction, the program will hang and do WDT reset after 2 seconds if hardware version in EEPROM does not match `uirbcore::eeprom::UIRB_HW_VER`.
 */
UIRB& uirb = UIRB::getInstance(); 

/**
 * @brief Variable to store number of times board has been woken up by wake button.
 * 
 */
volatile uint32_t buttonWakeupCount = 0;

/**
 * @brief Variable to store number of times board has been woken up by IO3 pin.
 * 
 */
volatile uint32_t io3WakeupCount = 0;

/**
 * @brief Function to be called when wake button is pressed.
 * 
 */
void buttonWakeupCallback()
{
    buttonWakeupCount++;
}

/**
 * @brief Function to be called when IO3 pin is pressed.
 * 
 */
void io3WakeupCallback()
{
    io3WakeupCount++;
}

/**
 * @brief Initializes the UIRB system and prints diagnostic information.
 * 
 * This function configures the Serial communication, initializes the UIRB class,
 * and performs diagnostic checks, including EEPROM validation, voltage readings,
 * and system configurations. Results are output to Serial for debugging.
 */
void setup()
{
    Serial.begin(1000000);
    Serial.println(F("UIRBcore basic example"));

    
    // Result of `UIRB` initialization.
    // If `beginResult` is not `CoreResult::SUCCESS`, continued use of the `UIRB` class may result in hardware damage.
    // In such cases, it is strongly recommended to halt execution and reset the board using the watchdog timer.
    CoreResult beginResult = uirb.begin();
    if (!beginResult)
    {
        while (true)
        {
            Serial.print(F("UIRB initialization failed: "));
            switch (beginResult)
            {
                case CoreResult::ERROR_EEPROM_HW_VER_MISMATCH:
                    Serial.println(F("The hardware version stored in EEPROM does not match the board's hardware version."));
                    break;
                case CoreResult::ERROR_EEPROM_SAVE_FAILED:
                    Serial.println(F("Failed to save data to EEPROM."));
                    break;
                case CoreResult::ERROR_EEPROM_CHARGER_PROG_RESISTANCE_INVALID:
                    Serial.println(F("The charger Rprog resistor value stored in EEPROM is invalid."));
                    break;
                default:
                    Serial.println(F("An unknown error occurred."));
            }
            delay(5000);
        }
    }

    // Use the `UIRB::reloadFromEEPROM()` function to reload the configuration from EEPROM into the RAM.
    // Library automatically reloads data from the EEPROM when `UIRB` class is initialized.
    // Use this function to reload data from the EEPROM if the data has been wrongly modified.
    if (!uirb.reloadFromEEPROM())
    {
        Serial.println(F("Failed to reload data from EEPROM into RAM. PROG resistor resistance is invalid, voltage functions will not work correctly."));
    }

    // Use the `UIRB::saveToEEPROM()` function to save the configuration from RAM into to the EEPROM.
    // Functions `bool UIRB::set...()` accept second parameter `bool saveToEEPROM` to indicate whether to imediately save the data to EEPROM after setting.
    if (!uirb.saveToEEPROM())
    {
        Serial.println(F("Failed to save data from RAM to EEPROM."));
    }

    uint32_t boot_count = uirb.getBootCount();
    Serial.print(F("Boot Count: "));
    Serial.println(boot_count);

    bool bootCountEn = uirb.isBootCountingEnabled();
    Serial.print(F("Boot count incrementing: "));
    Serial.println(bootCountEn ? F("Enabled") : F("Disabled"));

    uint16_t bandgapRef_mV = uirb.getInternalBandgapReferenceVoltageMilivolts();
    float bandgapRef_V = uirb.getInternalBandgapReferenceVoltage();
    Serial.print(F("Bandgap Reference: "));
    Serial.print(bandgapRef_mV);
    Serial.print(F(" mV ; "));
    Serial.print(bandgapRef_V, 3);  // Print with 3 decimal places
    Serial.println(F(" V"));

    uint16_t newBandgapRef_mV = bandgapRef_mV;
    if (!uirb.setInternalBandgapReferenceVoltageMilivolts(newBandgapRef_mV))
    {
        Serial.println(F("Failed to save bandgap reference value to EEPROM"));
    }

    uint16_t rprog_ohms = uirb.getChargerProgResistorResistance();
    Serial.print(F("Rprog: "));
    if (rprog_ohms == UIRB::INVALID_PROG_RESISTOR_RESISTANCE)
    {
        Serial.println(F("Invalid"));
    }
    else
    {
        Serial.print(rprog_ohms);
        Serial.println(F(" ohms"));
    }

    uint8_t statLedBrightness = uirb.getStatusLEDBrightness();
    Serial.print(F("Status LED Brightness: "));
    Serial.print(map(statLedBrightness, 0, 255, 0, 100));
    Serial.println(F("%"));

    uint8_t statLedBrightness_new = statLedBrightness;
    uirb.setStatusLEDBrightness(statLedBrightness_new);

    // Library version from <UIRBcore_Version.h>
    Serial.println(F("Library Version: " UIRB_CORE_LIB_VER_STR));// "{UIRB_CORE_LIB_MAJOR}.{UIRB_CORE_LIB_MINOR}.{UIRB_CORE_LIB_PATCH}"

    uint32_t serialNumber = uirb.getBoardSerialNumber();
    Serial.print(F("Board SN: "));
    if (serialNumber == UIRB::INVALID_UIRB_SERIAL_NUMBER)
    {
        Serial.println(F("Invalid"));
    }
    else
    {
        Serial.println(serialNumber);
    }

    String usbSerialNumber = uirb.getUSBSerialNumber();
    Serial.print(F("USB SN: "));
    Serial.println(usbSerialNumber);

    String factoryCP2104USBSerialNumber = uirb.getFactoryUSBSerialNumber();
    Serial.print(F("CP2104 Factory SN: "));
    Serial.println(factoryCP2104USBSerialNumber);

    uint8_t samples = 5;

    uint16_t supplyVoltage_mV = uirb.getSupplyVoltageMilivolts(samples);
    Serial.print(F("Supply Voltage: "));
    if (supplyVoltage_mV == UIRB::INVALID_VOLTAGE_MILIVOLTS)
    {
        Serial.println(F("Invalid"));
    }
    else
    {
        Serial.print(supplyVoltage_mV);
        Serial.println(F(" mV"));
    }

    uint16_t progVoltage_mV = uirb.getProgVoltageMilivolts(samples);
    Serial.print(F("Prog Voltage: "));
    if (progVoltage_mV == UIRB::INVALID_VOLTAGE_MILIVOLTS)
    {
        Serial.println(F("Invalid"));
    }
    else
    {
        Serial.print(progVoltage_mV);
        Serial.println(F(" mV"));
    }

    bool isSleepingAllowed = uirb.isSleepingAllowed();
    Serial.print(F("Sleeping Allowed: "));
    Serial.println(isSleepingAllowed ? F("Yes") : F("No"));

    bool sleepingAllowed_new = true;
    uirb.setSleepingAllowed(sleepingAllowed_new);

    // If AVR_DEBUG was defined during compilation, this would have been detected at compile time 
    // because the avr8-stub library is incompatible with Serial communication on the ATMega328P.
    bool isSerialDebuggerEnabled = uirb.isSerialDebuggerEnabled();
    Serial.print(F("Serial Debugger Present (AVR_DEBUG defined): "));
    Serial.println(isSerialDebuggerEnabled ? F("Yes") : F("No"));

    bool isWakeupIO3Allowed = uirb.isWakeupFromIO3Allowed();
    Serial.print(F("Wakeup from IO3 allowed: "));
    Serial.println(isWakeupIO3Allowed ? F("Yes") : F("No"));

    bool wakeupIO3Allowed_new = true;
    uirb.setWakeupFromIO3Allowed(wakeupIO3Allowed_new);

    // Set button and IO3 wakeup callbacks (can be changed at any time but must be set before `UIRB::powerDown`).
    uirb.setButtonWakeupCallback(buttonWakeupCallback);
    uirb.setIO3WakeupCallback(io3WakeupCallback);
}

/**
 * @brief Monitors power information and manages sleep-wakeup cycles.
 * 
 * Continuously retrieves and displays power information, including supply voltage, 
 * charging state, and battery status. Manages sleep mode with configurable wakeup 
 * sources and handles wakeup events.
 */
void loop()
{
    const uint8_t samples = 10;
    bool flashLEDLowBat = true;

    PowerInfoData powerInfo = uirb.getPowerInfo(samples, flashLEDLowBat);

    if (powerInfo.isValid())
    {
        Serial.print(F("Supply Voltage: "));
        Serial.print(powerInfo.getSupplyVoltage(), 3); // Print `float` with 3 decimal places
        Serial.print(F(" V, Prog Voltage: "));
        Serial.print(powerInfo.getProgVoltage(), 3); // Print `float` with 3 decimal places
        Serial.print(F(" V, Charging Current: "));
        Serial.print(powerInfo.getChargingCurrent(), 3); // Print `float` with 3 decimal places
        Serial.println(F(" mA"));

        flashLEDLowBat = false;
        Serial.print(F("Battery low: "));
        Serial.println(powerInfo.isBatteryLow(flashLEDLowBat) ? F("Yes") : F("No"));

        Serial.print(F("Battery full: "));
        Serial.println(powerInfo.isBatteryFull() ? F("Yes") : F("No"));

        Serial.print(F("Battery charging: "));
        Serial.println(powerInfo.isBatteryCharging() ? F("Yes") : F("No"));

        ChargerState chgState = powerInfo.getChargerState();

        Serial.print(F("Estimated charger state (!chgState): "));
        Serial.println(!chgState ? F("Not Charging") : F("Charging"));

        Serial.print(F("Estimated charger state: "));
        switch (chgState)
        {
            case ChargerState::ERROR:
                Serial.println(F("Internal error"));
                break;
            case ChargerState::UNKNOWN:
                Serial.println(F("Unknown"));
                break;
            case ChargerState::CHARGING_CC:
                Serial.println(F("Charging Constant Current"));
                break;
            case ChargerState::CHARGING_CV:
                Serial.println(F("Charging Constant Voltage"));
                break;
            case ChargerState::FLOATING:
                Serial.println(F("Floating, Waiting For Recharge"));
                break;
            case ChargerState::TURNED_OFF:
                Serial.println(F("Turned Off"));
                break;
            default:
                Serial.println(F("Unknown Error"));
        }
        
        BatteryState batState = powerInfo.getBatteryState();

        Serial.print(F("Estimated battery state (!batState): "));
        Serial.println(!batState ? F("Empty or Error or Unknown") : F("Not Empty"));

        Serial.print(F("Estimated battery state: "));
        switch (batState)
        {
            case BatteryState::ERROR:
                Serial.println(F("Internal error"));
                break;
            case BatteryState::UNKNOWN:
                Serial.println(F("Unknown"));
                break;
            case BatteryState::EMPTY:
                Serial.println(F("Empty"));
                break;
            case BatteryState::NOT_CHARGING:
                Serial.println(F("Not Charging"));
                break;
            case BatteryState::CHARGING:
                Serial.println(F("Charging"));
                break;
            case BatteryState::FULLY_CHARGED:
                Serial.println(F("Fully Charged"));
                break;
            default:
                Serial.println(F("Unknown Error"));
        }
    }
    else
    {
        Serial.println(F("Power information is not valid"));
    }

    uirb.clearWakeupISRFlags();

    bool attachIO3Interrupt = uirb.isWakeupFromIO3Allowed();

    WakeupInterrupt wakeupINT = attachIO3Interrupt ? WakeupInterrupt::WAKE_BUTTON_AND_USB_IO3 : WakeupInterrupt::WAKE_BUTTON;
    uint32_t sleeptime_ms = 3000;

    Serial.print(F("Powering down, "));  
    if (sleeptime_ms == UIRB::SLEEP_FOREVER)
    {
        Serial.println(F("no automatic wakeup"));
    }
    else
    {
        Serial.print(F("waking up in approximately "));
        Serial.print(sleeptime_ms);
        Serial.println(F(" ms"));
    }
    Serial.println();
    Serial.flush();

    uirb.powerDown(sleeptime_ms, wakeupINT);

    Serial.println(F("Woke up"));

    if (uirb.getWakeupISRFlag())
    {
        Serial.println(F("Woken up externally via one of wakeup interrupts."));
    }
    else
    {
        Serial.println(F("Woken up by WDT timeout."));
    }

    if (uirb.getAndClearButtonWakeupISRFlag())
    {
        Serial.print(F("Button wakeup count: "));
        Serial.println(buttonWakeupCount);
    }

    if (uirb.getAndClearIO3WakeupISRFlag())
    {
        Serial.println(F("IO3 wakeup count: "));
        Serial.println(io3WakeupCount);
    }
}
