/**
 * @file Utility.hpp
 * @brief Declarations of utility functions for pin mode detection and analog reference management.
 * 
 * This file declares helper functions for retrieving the mode of a digital pin, setting the analog 
 * reference for ADC conversions, and querying the current analog reference configuration. These utilities 
 * simplify low-level hardware interactions on AVR-based microcontrollers with Arduino framework..
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
#ifndef Utility_hpp
#define Utility_hpp

#include <Arduino.h>

/**
 * @brief Return value representing an invalid pin mode.
 * 
 * This constant is returned by the @ref getPinMode() function when the specified pin 
 * is not valid or does not correspond to a physical digital pin on the microcontroller.
 * 
 * @details 
 * - @ref INVALID_PIN_MODE is used as an error code to indicate that the input pin 
 *   is outside the range of usable digital pins.
 * - Typical scenarios include providing a pin number that is not mapped or attempting 
 *   to query a pin that does not support digital I/O.
 * 
 * @note This value (`0xFFU`) is chosen as it does not overlap with valid pin mode values 
 * (`INPUT`, `OUTPUT`, `INPUT_PULLUP`).
 */
static constexpr uint8_t INVALID_PIN_MODE = 0xFFU;

/**
 * @brief Return value representing an invalid analog reference.
 * 
 * This constant is returned by the @ref getAnalogReference() function when the 
 * currently configured analog reference is not recognized or falls outside 
 * the expected set of valid references.
 * 
 * @details 
 * - @ref INVALID_ANALOG_REF indicates that the ADC reference configuration is 
 *   either unset, misconfigured, or in an unexpected state.
 * - Valid references include `EXTERNAL`, `DEFAULT` (AVcc), and `INTERNAL1V1`.
 * 
 * @note This value (`0xFFU`) is chosen as it does not overlap with valid analog 
 * reference identifiers, ensuring clear differentiation between valid and invalid states.
 */
static constexpr uint8_t INVALID_ANALOG_REF = 0xFFU;

/**
 * @brief Retrieves the mode configuration of a digital pin.
 * 
 * This function determines the current configuration of a specified digital pin, 
 * identifying whether it is set as an input, output, or input with an internal 
 * pull-up resistor enabled. If the provided pin is invalid or not mapped to a 
 * digital I/O port, the function returns @ref INVALID_PIN_MODE.
 * 
 * @param[in] pin The digital pin number to query.
 * @return uint8_t The current mode of the specified pin.
 * @retval INPUT The pin is configured as a standard input.
 * @retval INPUT_PULLUP The pin is configured as an input with an internal pull-up resistor.
 * @retval OUTPUT The pin is configured as an output.
 * @retval #INVALID_PIN_MODE The provided pin is invalid or not a digital pin.
 * 
 * @details
 * This function uses internal registers to determine the mode of the pin:
 * - The Data Direction Register (DDR) is checked to identify if the pin is an output.
 * - The PORT register is checked to determine if the pin is an input with a pull-up resistor enabled.
 * - If neither condition is met, the pin is assumed to be a standard input.
 * 
 * @note This function does not configure the pin mode; it only queries the current state.
 * Ensure the pin has been initialized correctly using [pinMode()](https://docs.arduino.cc/language-reference/en/functions/digital-io/pinMode/) before calling this function.
 */
uint8_t getPinMode(const uint8_t pin);

/**
 * @brief Configures the analog reference for the ADC (Analog-to-Digital Converter).
 * 
 * This function sets the analog voltage reference for the ADC. A dummy conversion is 
 * performed immediately after setting the reference to allow it to stabilize. If the 
 * provided `aref` value is invalid, the function exits without modifying the ADC configuration.
 * 
 * @param[in] aref The analog reference to set. Valid values include:
 * - `EXTERNAL` : External voltage reference connected to the `AREF` pin.
 * - `DEFAULT`  : Default reference, typically AVcc.
 * - `INTERNAL1V1` : Internal 1.1V voltage reference.
 * 
 * @details
 * - The analog reference controls the upper range of the ADC conversion. For example, 
 *   when set to `DEFAULT` and AVcc is 5V, the ADC will map 0–5V to 0–1023.
 * - A dummy conversion is performed by reading from an analog pin `A7` to ensure the 
 *   new reference voltage stabilizes before further ADC operations.
 * 
 * @warning
 * - When using `DEFAULT` or `INTERNAL1V1`, the `AREF` pin must be left floating or 
 *   connected to a low-value capacitor. Connecting an external voltage to `AREF` in these 
 *   modes can cause damage to the microcontroller.
 * - Ensure the `aref` value provided matches the desired reference. Misconfiguration can 
 *   lead to inaccurate ADC readings.
 */
void setAnalogReference(const uint8_t aref);

/**
 * @brief Retrieves the current analog reference setting used by the ADC.
 * 
 * This function queries the ADC's configuration to determine the currently active analog
 * voltage reference. The analog reference controls the upper limit of the ADC conversion range.
 * If the current configuration does not match a known reference, the function returns @ref INVALID_ANALOG_REF.
 * 
 * @return uint8_t The current analog reference setting.
 * @retval EXTERNAL External voltage reference connected to the `AREF` pin.
 * @retval DEFAULT Default reference, typically AVcc.
 * @retval INTERNAL1V1 Internal 1.1V voltage reference.
 * @retval #INVALID_ANALOG_REF The analog reference setting is unrecognized or invalid.
 * 
 * @details
 * The function inspects the ADC Multiplexer Selection Register (`ADMUX`) to identify the reference 
 * voltage being used:
 * - Bits 7 and 6 of the `ADMUX` register determine the reference voltage configuration.
 * - The recognized values are mapped to `DEFAULT`, `EXTERNAL`, and `INTERNAL1V1`.
 * - If the bits indicate an unsupported or unknown configuration, @ref INVALID_ANALOG_REF is returned.
 * 
 * @note
 * - The default reference voltage (`DEFAULT`) is typically AVcc (supply voltage).
 * - Ensure the reference configuration matches the expected range for accurate ADC readings.
 */
uint8_t getAnalogReference();

#endif  // Utility_hpp