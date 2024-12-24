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
 * @def STR(x)
 * @brief Converts a macro argument into a string literal.
 * 
 * This macro takes the input `x` and converts it directly into a string literal without expanding 
 * the value of `x` if it is itself a macro.
 * 
 * @param x The input value or macro to be converted into a string literal.
 * 
 * @note Typically used as a helper for @ref XSTR to achieve macro expansion before stringification.
 * 
 * **Example:**
 * @code
 * #define MY_MACRO 123
 * STR(MY_MACRO) // Results in "MY_MACRO"
 * @endcode
 * 
 * @see XSTR
 */
#define STR(x) #x

/**
 * @def XSTR(x)
 * @brief Expands a macro and converts the result into a string literal.
 * 
 * This macro first expands the input `x` if it is a macro and then converts the expanded result 
 * into a string literal. It is a two-step process: expansion followed by stringification.
 * 
 * @param x The input macro to be expanded and converted into a string literal.
 * 
 * @note Uses @ref STR internally to perform stringification after expansion.
 * 
 * **Example:**
 * @code
 * #define MY_MACRO 123
 * XSTR(MY_MACRO) // Results in "123"
 * @endcode
 * 
 * @see STR
 */
#define XSTR(x) STR(x)

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
    
    /**
     * @brief Used to suppress the warning message for @ref UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS.
     * 
     */
    #define NO_WARN_UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS
#endif  // !defined(UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS)

// Check if UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS is a number
#if (UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS + 0) != UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS
    #error "UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS must be a numeric constant."
#endif  // (UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS + 0) != UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS

#if UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS < 3000 || UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS > 3600
    #error "Invalid value for `UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS`. Valid values are between 3000 and 3600."
#endif  // UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS < 3000 || UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS > 3600

#if !defined(NO_WARN_UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS)
    #warning "UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS is defined with value: " XSTR(UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS)
#else
    #undef NO_WARN_UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS
#endif  // !defined(NO_WARN_UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS)

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

    /**
     * @brief Used to suppress the warning message for @ref UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS.
     * 
     */
    #define NO_WARN_UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS
#endif  // !defined(UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS)

// Check if UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS is a number
#if (UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS + 0) != UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS
    #error "UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS must be a numeric constant."
#endif  // (UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS + 0) != UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS

#if UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS < 4100 || UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS > 4250
    #error "Invalid value for `UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS`. Valid values are between 4100 and 4250."
#endif  // UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS < 4100 || UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS > 4250

#if !defined(NO_WARN_UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS)
    #warning "UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS is defined with value: " XSTR(UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS)
#else
    #undef NO_WARN_UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS
#endif  // !defined(NO_WARN_UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS)
/** @} */ // End of Core configuration

/**
 * @name EEPROM
 * @{
 */
#if defined(__DOXYGEN__)
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
    #define UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON
    #undef UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON

    /**
     * @def UIRB_EEPROM_BYPASS_DEBUG
     * @brief Macro to bypass EEPROM storage by redirecting all data operations to RAM.
     * 
     * When this macro is defined, the `EEPROM.h` library is excluded, and data operations are redirected to RAM.
     * This allows full functionality of EEPROM-related operations while ensuring compatibility in scenarios where 
     * physical EEPROM is unavailable. However, all data will be lost after a system reboot or power cycle.
     * 
     * @details
     * - When enabled, data is replaced with default values defined in @ref uirbcore::eeprom::DEBUG_EEPROM_DATA.
     * - This macro is particularly useful for debugging scenarios, such as when the EEPROM hardware version does not match 
     *   or when using simulators like `simavr`, where physical EEPROM is not present.
     * - While this bypass facilitates debugging without EEPROM hardware, care must be taken to ensure proper 
     *   GPIO protection and hardware safety, as the library may interact with GPIO pins differently on all hardware.
     * 
     * @note 
     * - Data in RAM is volatile and will not persist between system reboots or power cycles.
     * - EEPROM data will remain unmodified unless explicitly updated using @ref uirbcore::eeprom::EEPROMDataManager.
     * 
     * @warning 
     * - This macro is intended for debugging purposes only. Improper use on actual hardware can lead to data loss or 
     *   potential damage to connected components.
     * - Review the library documentation thoroughly and ensure GPIO pins are adequately protected when enabling this define.
     * 
     * @see @ref uirbcore::eeprom::DEBUG_EEPROM_DATA for default EEPROM data values.
     */
    #define UIRB_EEPROM_BYPASS_DEBUG
    #undef UIRB_EEPROM_BYPASS_DEBUG

    /**
     * @def UIRB_EEPROM_RPROG_DEBUG
     * @brief Macro to override the default charger programming resistor (Rprog) value in debugging mode.
     * 
     * When this macro is defined, the`charger_prog_resistor_ohms` field in the @ref uirbcore::eeprom::DEBUG_EEPROM_DATA structure 
     * is set to the value of @ref UIRB_EEPROM_RPROG_DEBUG. This allows debugging and testing with custom Rprog values 
     * without requiring changes to the EEPROM.
     * 
     * @details
     * - This macro is only effective when @ref UIRB_EEPROM_BYPASS_DEBUG is also defined. If @ref UIRB_EEPROM_BYPASS_DEBUG 
     *   is not enabled, defining this macro will have no effect.
     * - The value of @ref UIRB_EEPROM_RPROG_DEBUG must be a numeric constant within the valid range of `0` to `65535`.
     * - When @ref UIRB_EEPROM_RPROG_DEBUG is not defined, the `charger_prog_resistor_ohms` field is set to 
     *   @ref uirbcore::eeprom::EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE.
     * 
     * @note
     * - This macro is intended for debugging purposes only and should not be used in production environments.
     * - Ensure that the provided value for @ref UIRB_EEPROM_RPROG_DEBUG accurately represents the desired Rprog value in ohms.
     * 
     * @warning
     * - Defining invalid values for @ref UIRB_EEPROM_RPROG_DEBUG (e.g., non-numeric or out-of-range values) will result in 
     *   a compilation error.
     * - Ensure @ref UIRB_EEPROM_BYPASS_DEBUG is enabled; otherwise, this macro will not apply and may cause confusion during debugging.
     * 
     * @see UIRB_EEPROM_BYPASS_DEBUG
     * @see uirbcore::eeprom::EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE
     */
    #define UIRB_EEPROM_RPROG_DEBUG
    #undef UIRB_EEPROM_RPROG_DEBUG
#endif  // defined(__DOXYGEN__)

#if defined(UIRB_EEPROM_BYPASS_DEBUG)
    #warning "UIRB_EEPROM_BYPASS_DEBUG is defined. All data operations will be redirected to RAM."
#endif  // defined(UIRB_EEPROM_BYPASS_DEBUG)

#if defined(UIRB_EEPROM_RPROG_DEBUG)
    #if !defined(UIRB_EEPROM_BYPASS_DEBUG)
        #warning "UIRB_EEPROM_RPROG_DEBUG is defined but will have no effect because UIRB_EEPROM_BYPASS_DEBUG is not enabled."
    #endif  // !defined(UIRB_EEPROM_BYPASS_DEBUG) && defined(UIRB_EEPROM_RPROG_DEBUG)
    // Check if UIRB_EEPROM_RPROG_DEBUG is a number
    #if (UIRB_EEPROM_RPROG_DEBUG + 0) != UIRB_EEPROM_RPROG_DEBUG
        #error "UIRB_EEPROM_RPROG_DEBUG must be a numeric constant."
    #endif  // (UIRB_EEPROM_RPROG_DEBUG + 0) != UIRB_EEPROM_RPROG_DEBUG

    #if UIRB_EEPROM_RPROG_DEBUG < 0 || UIRB_EEPROM_RPROG_DEBUG > 65535
        #error "Invalid value for `UIRB_EEPROM_RPROG_DEBUG`. Valid values are between 0 and 65535."
    #endif  // UIRB_EEPROM_RPROG_DEBUG < 0 || UIRB_EEPROM_RPROG_DEBUG > 65535

    #warning "UIRB_EEPROM_RPROG_DEBUG is defined with value: " XSTR(UIRB_EEPROM_RPROG_DEBUG)
#endif  // defined(UIRB_EEPROM_RPROG_DEBUG)
/** @} */ // End of EEPROM

#endif  // UIRBcore_Defs_h