/**
 * @file main.cpp
 * @brief Example showcasing EEPROM bypass and debugging features of the UIRBcore library in a simulated environment.
 * 
 * This example demonstrates the usage of the UIRBcore library for managing hardware features, such as 
 * power monitoring, sleep management, and hardware parameter configuration, while operating in EEPROM bypass mode. 
 * It is designed for use in the PlatformIO IDE with the `simavr` simulator or similar environments.
 * 
 * **Features:**
 * - Utilizes the `UIRB_EEPROM_BYPASS_DEBUG` macro to redirect EEPROM operations to RAM, enabling testing 
 *   and debugging without requiring physical EEPROM hardware.
 * - Supports custom charger programming resistor values through the `UIRB_EEPROM_RPROG_DEBUG` macro.
 * - Outputs detailed hardware and runtime statistics, including boot count, bandgap reference voltage, 
 *   Rprog resistance, and serial numbers.
 * - Demonstrates key UIRBcore library functionalities, such as hardware initialization, power settings, 
 *   and status LED control.
 * 
 * **Workflow:**
 * 1. Initializes the UIRBcore library and validates hardware readiness.
 * 2. Retrieves and displays key hardware information:
 *    - Boot count
 *    - Bandgap reference voltage
 *    - Rprog resistance
 *    - Serial numbers (board and USB)
 * 3. Configures various hardware settings:
 *    - Status LED brightness
 *    - Sleep mode settings
 *    - Wakeup source permissions
 * 4. Outputs runtime data to the `Serial` monitor for debugging.
 * 
 * @note 
 * - The `platformio.ini` file must define `UIRB_EEPROM_BYPASS_DEBUG` to enable EEPROM bypass mode 
 *   and `UIRB_EEPROM_RPROG_DEBUG` to set a custom Rprog resistance value.
 * - Data stored in RAM during EEPROM bypass mode is volatile and will not persist across reboots or resets.
 * - This example assumes a serial monitor baud rate of 1,000,000 for debugging purposes.
 * 
 * @warning 
 * - The EEPROM bypass mode is intended for debugging and simulation only. Do not use it in production.
 * - Ensure proper protection for GPIO pins and connected hardware during simulations.
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

using namespace uirbcore;

UIRB& uirb = UIRB::getInstance();

void setup()
{
    Serial.begin(1000000);

    if (!uirb.begin())
    {
        Serial.println(F("UIRBcore Initialization Failed!"));
        // Ensure uirb.begin() succeeds before proceeding to avoid hardware damage.
        while (1);
    }

    Serial.println("UIRBcore initialized successfully.");

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

    bool isSleepingAllowed = uirb.isSleepingAllowed();
    Serial.print(F("Sleeping Allowed: "));
    Serial.println(isSleepingAllowed ? F("Yes") : F("No"));

    bool sleepingAllowed_new = true;
    uirb.setSleepingAllowed(sleepingAllowed_new);
    bool isSerialDebuggerEnabled = uirb.isSerialDebuggerEnabled();
    Serial.print(F("Serial Debugger Present (AVR_DEBUG defined): "));
    Serial.println(isSerialDebuggerEnabled ? F("Yes") : F("No"));

    bool isWakeupIO3Allowed = uirb.isWakeupFromIO3Allowed();
    Serial.print(F("Wakeup from IO3 allowed: "));
    Serial.println(isWakeupIO3Allowed ? F("Yes") : F("No"));
}

void loop()
{
    // Main application logic
}