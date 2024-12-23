/**
 * @file UIRBcore_Defs.h
 * @brief Core configuration definitions and EEPROM management macros for the %UIRB system.
 * 
 * The file ensures safe, validated, and consistent definitions for hardware configuration, 
 * aligning with standard battery operation parameters and system performance requirements.
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
#ifndef UIRBcore_Defs_h
#define UIRBcore_Defs_h

#include <Arduino.h>

/**
 * @name Core configuration
 * @{
 */
/**
 * @def UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS
 * @brief Macro defining the minimum battery voltage considered "empty," in millivolts.
 * 
 * This macro represents the threshold voltage for a Li-ion battery to be considered nearly empty.
 * By default, it is set to 3400 mV, which corresponds to approximately 15%-20% remaining capacity
 * for most Li-ion batteries. This value ensures safe operation and prevents over-discharge, which
 * can reduce battery lifespan or cause damage. 
 * 
 * @note For high-discharge-rate batteries, further discharge below this value is not recommended.
 * Modify this value only if the application requirements demand a different threshold.
 */
#if !defined(UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS)
    #define UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS 3400
#endif  // !defined(UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS)

// Check if UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS is a number
#if (UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS + 0) != UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS
    #error "UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS must be a numeric constant."
#endif  // (UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS + 0) != UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS

#if UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS < 3000 || UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS > 3600
    #error "Invalid value for `UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS`. Valid values are between 3000 and 3600."
#endif  // UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS < 3000 || UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS > 3600

/**
 * @def UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS
 * @brief Macro defining the floating voltage for a fully charged Li-ion battery, in millivolts.
 * 
 * This macro represents the constant voltage (CV) stage of a Li-ion battery charger, which is 
 * the voltage held when the battery is fully charged. By default, it is set to 4150 mV, 
 * corresponding to the typical float voltage range specified in the TP4057 datasheet:
 * 
 * - `Vfloat` range: 4158 mV to 4242 mV
 * - Typical value: 4200 mV
 * 
 * @note This value ensures proper termination of the charging cycle to prevent overcharging 
 * and ensure battery longevity. Adjust this value only if the application or charger requires 
 * a different float voltage.
 */
#if !defined(UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS)
    #define UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS 4150
#endif  // !defined(UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS)

// Check if UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS is a number
#if (UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS + 0) != UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS
    #error "UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS must be a numeric constant."
#endif  // (UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS + 0) != UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS

#if UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS < 4100 || UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS > 4250
    #error "Invalid value for `UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS`. Valid values are between 4100 and 4250."
#endif  // UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS < 4100 || UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS > 4250
/** @} */ // End of Core configuration

/**
 * @name EEPROM
 * @{
 */
/**
 * @def UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON
 * @brief Macro enabling the use of `memcmp` for comparing @ref uirbcore::eeprom::EEPROMData structures.
 * 
 * When this macro is defined, the comparison of two @ref uirbcore::eeprom::EEPROMData structures is performed 
 * using the `memcmp` function. This approach offers faster performance by directly comparing 
 * the memory regions of the structures, bypassing individual field comparisons.
 * 
 * @details
 * - The `memcmp` function compares the entire memory layout of the structures, including all fields 
 *   and potential padding bytes.
 * - This method is suitable for use when the @ref uirbcore::eeprom::EEPROMData structure is packed and aligned, 
 *   ensuring no uninitialized or undefined padding bytes are included in the memory comparison.
 * - If the structure includes dynamically allocated or non-trivial types (e.g., pointers or custom objects), 
 *   using `memcmp` may yield incorrect results. In such cases, field-by-field comparison should be used instead.
 * 
 * @note If this macro is undefined, a field-by-field comparison is performed, which may be slower 
 *       but avoids issues related to padding or alignment differences.
 * 
 * @warning 
 * - Use this macro only if the @ref uirbcore::eeprom::EEPROMData structure is tightly packed, aligned, and contains 
 *   no dynamically allocated or complex fields.
 * - Ensure consistent alignment between the compared structures to prevent unexpected results.
 * 
 * @see @ref uirbcore::eeprom::operator==(const uirbcore::eeprom::EEPROMData&, const uirbcore::eeprom::EEPROMData&) for the implementation using this macro.
 */
#if defined(__DOXYGEN__)
    #define UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON
    #undef UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON
#endif
/** @} */ // End of EEPROM

#endif  // UIRBcore_Defs_h