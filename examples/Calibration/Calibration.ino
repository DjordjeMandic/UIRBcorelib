/**
 * @file Calibration.ino
 * @brief Example demonstrating the calibration and usage of the 1.1V internal reference using the UIRBcore library.
 * 
 * This example showcases how to initialize and interact with the Universal Infrared Remote Board (UIRB) library 
 * for managing the internal bandgap reference voltage, power monitoring, and EEPROM operations. It provides a 
 * structured demonstration of how to measure and update critical parameters related to `AVcc` and #PIN_PROG voltages.
 * 
 * **Workflow:**
 * 1. The `uirbcore::UIRB` class instance is initialized, and stored EEPROM values are retrieved.
 * 2. The analog reference is set to `INTERNAL1V1`, and the user is prompted to measure the AREF voltage 
 *    (pin 20 of ATMega328P in TQFP package).
 * 3. User inputs a voltage value (in millivolts) to update the internal bandgap reference value in RAM.
 * 4. The user can choose to save the value permanently to EEPROM (`SAVE`) or reload the EEPROM data (`RELOAD`).
 * 5. Voltages (`AVcc` (pin 18 of ATMega328P in TQFP package), #PIN_PROG (pin 24 of ATMega328P in TQFP package), and 
 *    the bandgap reference value in RAM) are periodically displayed during idle periods.
 * 
 * @note The program uses `Serial` for debugging purposes and outputs relevant system information.
 *       Ensure a serial monitor is connected at 1000000 baud.
 * 
 * @note The valid range for the 1.1V reference voltage is between 972 mV and 1227 mV.
 *       Values outside this range are considered invalid and will be rejected.
 * 
 * @warning If the hardware initialization fails, the program will enter an infinite loop and provide 
 *          diagnostic messages over `Serial`.
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
#include <Utility.hpp>

using namespace uirbcore;

/**
 * @brief Instance of the `UIRB` class.
 * 
 */
UIRB& uirb = UIRB::getInstance();

/**
 * @brief Prints a line of dashes to the Serial output.
 * 
 * Outputs a specified number of dashes (default: 64) followed by a newline.
 * 
 * @param[in] count The number of dashes to print. Defaults to 64.
 */
void printLnDashes(uint8_t count = 64)
{
    for(uint8_t i = 0; i < count; i++)
    {
        Serial.print('-');
    }
    Serial.println();
}

/**
 * @brief Prints available commands to the Serial output.
 * 
 * Displays instructions for user input, including setting a value, 
 * reloading data from EEPROM, or saving data to EEPROM.
 */
void printLnCommands()
{
    Serial.println(F("Enter a new value in milivolts to set it in RAM."));
    Serial.println(F("Type 'RELOAD' to reload data from EEPROM into RAM."));
    Serial.println(F("Type 'SAVE' to store the data from RAM to EEPROM."));
    printLnDashes();
}

/**
 * @brief Prints voltage information to the Serial output.
 * 
 * Displays the current internal 1.1V reference, AVcc voltage, 
 * and PROG voltage in millivolts.
 */
void printLnVoltages()
{
    Serial.print(F("Current in RAM 1.1V Reference: "));
    Serial.print(uirb.getInternalBandgapReferenceVoltageMilivolts());
    Serial.println(F(" mV"));

    uint16_t avcc_mV = uirb.getSupplyVoltageMilivolts();
    uint16_t prog_mV = uirb.getProgVoltageMilivolts();

    Serial.print(F("AVcc Voltage: "));
    Serial.print(avcc_mV);
    Serial.println(F(" mV"));

    Serial.print(F("PROG Voltage: "));
    Serial.print(prog_mV);
    Serial.println(F(" mV"));
    printLnDashes();
}

/**
 * @brief Initializes the calibration process for the internal 1.1V reference.
 * 
 * Configures Serial communication and initializes the UIRB library. Prompts the user
 * to switch the analog reference to INTERNAL1V1 and measure the AREF voltage.
 */
void setup()
{
    Serial.begin(1000000);
    Serial.println(F("=== Internal 1.1V Reference Calibration ==="));

    if (!uirb.begin())
    {
        Serial.println(F("UIRB Initialization Failed!"));
        while (1);
    }

    Serial.print(F("Stored 1.1V Reference in EEPROM: "));
    Serial.print(uirb.getInternalBandgapReferenceVoltageMilivolts());
    Serial.println(F(" mV"));

    Serial.println(F("Press Enter to switch AREF to INTERNAL1V1..."));
    while (Serial.read() != '\n'); // Wait for Enter key

    setAnalogReference(INTERNAL1V1);
    delay(500); // Let AREF stabilize
    Serial.println(F("AREF switched to INTERNAL1V1. Measure the voltage on the AREF pin."));
    printLnDashes();
}

/**
 * @brief Handles user input for calibrating the internal 1.1V reference.
 * 
 * Continuously displays voltage information and listens for user commands to 
 * set, save, or reload the reference voltage. Updates and validates inputs 
 * interactively.
 */
void loop()
{   
    printLnCommands();
    String input = "";
    bool inputComplete = false;
    unsigned long startTime = millis();
    uint8_t cnt = 0;

    while (!inputComplete)
    {
        while (Serial.available() > 0)
        {
            char c = Serial.read();
            if (c == '\n') 
            {
                input.trim();
                inputComplete = true;

                // Flush rest of the buffer
                while (Serial.available() > 0) 
                {
                    Serial.read();
                }
                break;       
            }
            input += c;
        }

        if (millis() - startTime >= 5000)
        {
            printLnVoltages();
            cnt++;
            if (cnt == 3)
            {
                printLnCommands();
                cnt = 0;
            }
            startTime = millis();
        }
    }

    if (input.equalsIgnoreCase("SAVE"))
    {
        if (uirb.saveToEEPROM())
        {
            Serial.print(F("Saved new value to EEPROM: "));
            Serial.print(uirb.getInternalBandgapReferenceVoltageMilivolts());
            Serial.println(F(" mV"));
        }
        else
        {
            Serial.println(F("Failed saving data to EEPROM."));
        }
    }
    else if (input.equalsIgnoreCase("RELOAD"))
    {
        if (uirb.reloadFromEEPROM())
        {
            Serial.println(F("Data succesfully reloaded from EEPROM."));
        }
        else
        {
            Serial.println(F("Warning: Reloaded data but detected an invalid PROG resistance!"));
        }
    }
    else
    {
        long inputLong = input.toInt();
        Serial.print(F("User input: "));
        Serial.print(inputLong);
        Serial.println(F(" mV"));
        // If inputLong cannot fit in uint16_t, set it to 0
        uint16_t inputNumber = (inputLong < 0 || inputLong > UINT16_MAX) ? 0 : static_cast<uint16_t>(inputLong);
        // Set new reference temporarily in RAM
        if (uirb.setInternalBandgapReferenceVoltageMilivolts(inputNumber))
        {
            Serial.print(F("New 1.1V Reference temporarily set in RAM: "));
            Serial.print(uirb.getInternalBandgapReferenceVoltageMilivolts());
            Serial.println(F(" mV"));
        }
        else
        {
            Serial.println(F("Invalid input. Enter a voltage value between 972 mV and 1227 mV."));
        }
    }
    printLnVoltages();
}
