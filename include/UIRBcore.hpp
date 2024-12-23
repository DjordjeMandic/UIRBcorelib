/**
 * @file UIRBcore.hpp
 * @brief Core header for the Universal Infrared Remote Board (%UIRB) system.
 * 
 * This header defines the main %UIRB library, including hardware management, power monitoring, 
 * wakeup handling, and EEPROM-based configuration storage. It provides a robust and modular 
 * interface for integrating %UIRB functionalities into applications, ensuring efficient hardware 
 * control and low-power operation.
 *
 * **Features:**
 * - Singleton-based system management
 * - Supply voltage and charging current monitoring
 * - Low-power sleep modes with wakeup interrupt support
 * - EEPROM management for persistent configurations
 * - Boot count tracking, hardware versioning, and diagnostic tools
 * 
 * **Usage:**
 * - Include this header in your sketch.
 * - Call @ref uirbcore::UIRB::getInstance() to access the singleton instance.
 * - Call @ref uirbcore::UIRB::begin() and check its result before using other functions.
 *
 * @note This file is part of the %UIRB firmware library and is designed specifically for the 
 *       ATMega328P microcontroller-based %UIRB system.
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

/**
 * @example examples/Basic/Basic.ino
 * @brief Basic example demonstrating the use of the UIRBcore library.
 */

/**
 * @example examples/Calibration/Calibration.ino
 * @brief Example demonstrating the calibration and usage of the 1.1V internal reference using the UIRBcore library.
 */

#ifndef UIRBcore_hpp
#define UIRBcore_hpp

#include <Arduino.h>

#if !defined(UIRB_BOARD_V02) || !defined(ARDUINO_AVR_ATmega328P) || !defined(ARDUINO_ARCH_AVR) || !defined(__AVR_ATmega328P__) 
    #error "Unsupported platform."
#endif  // !defined(UIRB_BOARD_V02) || !defined(ARDUINO_AVR_ATmega328P) || !defined(ARDUINO_ARCH_AVR) || !defined(__AVR_ATmega328P__) 

/**
 * @def AVR_DEBUG
 * @brief Macro to indicate that AVR debugging is enabled.
 * 
 * This macro is defined only if debugging support is available in the current build environment.
 * 
 * @details 
 * - If `avr8-stub.h` is included, or if either `AVR_STUB` or `AVR8_STUB` is defined,
 *   the @ref AVR_DEBUG macro is automatically defined.
 * - Ensures debugging functionality is enabled for AVR-based microcontrollers when appropriate.
 * - Prevents redundant redefinition of @ref AVR_DEBUG using a conditional check.
 */
#if defined(__DOXYGEN__)
    #define AVR_DEBUG ///< Macro for AVR debugging support.
#endif

/**
 * @brief Enables AVR debugging support if certain conditions are met.
 * 
 * This block checks for the presence of debugging-related macros or headers,
 * such as `avr8-stub.h`, `AVR_STUB`, or `AVR8_STUB`. If any of these are
 * available, it ensures that the @ref AVR_DEBUG macro is defined.
 * 
 * @note This configuration is used to enable AVR debugging functionality during compilation.
 */
#if __has_include("avr8-stub.h") || defined(AVR_STUB) || defined(AVR8_STUB)
    #if !defined(AVR_DEBUG)
        #define AVR_DEBUG
    #endif
#endif  // __has_include("avr8-stub.h") || defined(AVR_STUB) || defined(AVR8_STUB)

/**
 * @def UIRB_CORE_LIB
 * @brief Macro to indicate the inclusion of the %UIRB core library.
 * 
 * This macro is defined to indicate that the %UIRB core library is included in a
 * single compilation unit.
 */
#if !defined(UIRB_CORE_LIB)
    #define UIRB_CORE_LIB
#endif  // UIRB_CORE_LIB

#include <UIRBcore_Defs.h>
#include <UIRBcore_Pins.h>
#include <UIRBcore_Version.h>
#include <UIRBcore_PowerInfoData.hpp>
#include <UIRBcore_EEPROM.hpp>

/**
 * @brief Core namespace for %UIRB system functionalities.
 *
 * The @ref uirbcore namespace serves as the foundation for managing the %UIRB system. 
 * It includes classes, utilities, and abstractions for hardware control, power management, 
 * wakeup handling, and EEPROM-based configuration storage.
 *
 * **Key Components:**
 * - @ref uirbcore::UIRB : Main class responsible for system initialization, state management, 
 *   and hardware-level interactions.
 * - @ref uirbcore::PowerInfoData : Class for power monitoring, handling supply voltage, 
 *   charger states, and battery conditions.
 * - @ref uirbcore::eeprom : Sub-namespace providing tools for storing and retrieving configuration 
 *   and runtime data in EEPROM.
 *
 * @details
 * This namespace is designed to simplify hardware integration and provide modular, 
 * maintainable, and scalable code for %UIRB applications. It abstracts low-level 
 * complexities, offering clean interfaces for high-level system development.
 *
 * @note All components within this namespace assume compatibility with the %UIRB platform.
 */
namespace uirbcore
{
    /**
     * @brief Enum class representing the return values of core functions as `uint8_t`.
     * 
     * This enumeration defines standardized return values for core functions in the system. 
     * Each value provides specific feedback on the outcome of a function call, 
     * including success, errors, and hardware-related issues.
     */
    enum class CoreResult : uint8_t
    {
        SUCCESS = 0, /**< Operation completed successfully. */
        ERROR_NOT_INITIALIZED, /**< The component or system has not been initialized (or constructed) yet. */
        ERROR_INVALID_ARGUMENT, /**< An invalid argument was provided to the function. */
        ERROR_EEPROM_HW_VER_MISMATCH, /**< Hardware version stored in EEPROM does not match the version expected by this library. */
        ERROR_EEPROM_CHARGER_PROG_RESISTANCE_INVALID, /**< Charger Rprog resistor value stored in EEPROM is invalid. */
        ERROR_EEPROM_SAVE_FAILED /**< Failed to save data to EEPROM. */
    };

    /**
     * @brief Logical NOT operator to check if a @ref CoreResult value indicates failure.
     * 
     * This operator provides a convenient way to test whether a @ref CoreResult value 
     * is not equal to `CoreResult::SUCCESS`. It simplifies error handling by allowing 
     * direct use of the logical NOT (`!`) operator on @ref CoreResult values.
     * 
     * @param[in] result The #CoreResult value to check.
     * @return bool
     * @retval true The #CoreResult is not `CoreResult::SUCCESS` (indicating an error or failure).
     * @retval false The #CoreResult is `CoreResult::SUCCESS` (indicating success).
     * 
     * @note This operator is particularly useful for concise and readable error-checking 
     * in conditional statements.
     * 
     * Example usage:
     * @code
     * CoreResult result = UIRB::getInstance().begin();
     * if (!result) {
     *     // Handle error, as the result is not SUCCESS
     * }
     * @endcode
     */
    inline bool operator!(CoreResult result)
    {
        return result != CoreResult::SUCCESS;
    }

    /**
     * @brief Enum class representing the interrupt sources for waking up the system.
     * 
     * This enumeration defines the available wakeup interrupt sources that can bring 
     * the system out of a low-power state. It standardizes the configuration options 
     * for the `UIRB::powerDown()` function, enabling precise control over wakeup behavior.
     */
    enum class WakeupInterrupt : uint8_t
    {
        NONE, /**< No wakeup source is configured. The system can only wake up using the 
                 Watchdog Timer (WDT) if `UIRB::powerDown()` is called with a non-zero sleep time. */
        WAKE_BUTTON, /**< The system can wake up via the wakeup button (@ref PIN_BUTTON_WAKEUP), 
                          which generates an external interrupt on INT0. This is typically connected 
                          to a physical button for user interaction. */
        USB_IO3, /**< The system can wake up when the state of USB IO3 pin (@ref PIN_USB_IO3) is changed, 
                      triggering a pin change interrupt (PCINT20) in the `PCINT2` group on the ATMega328P. 
                      This source is useful for wake events tied to USB host. */
        WAKE_BUTTON_AND_USB_IO3 /**< The system can wake up from either the wakeup button 
                                     (@ref PIN_BUTTON_WAKEUP, INT0) or the USB IO3 pin (@ref PIN_USB_IO3, PCINT20). 
                                     This configuration provides flexibility for multiple wakeup triggers. */
    };

    /**
     * @brief Universal Infrared Remote Board (%UIRB) hardware interface and power management class.
     * 
     * The @ref UIRB class provides a comprehensive interface for managing the hardware and power-related functionalities 
     * of the Universal Infrared Remote Board (%UIRB). This class encapsulates the hardware configuration, power management, 
     * and interaction with EEPROM-stored settings. It is designed to ensure safe and reliable operation, enabling advanced 
     * features like sleep modes, wakeup interrupts, and power monitoring.
     * 
     * **Key Features:**
     * - Implements a singleton pattern to ensure a single instance of the class is used throughout the application.
     * - Provides power management utilities, including supply voltage and charging current measurement.
     * - Supports low-power operation with configurable sleep modes and wakeup sources.
     * - Integrates with EEPROM to persist settings like hardware version, calibration data, and user preferences.
     * - Supports ADC-based voltage and current sampling, with automatic reference adjustment for precision.
     * - Includes diagnostic utilities like boot counting, hardware version retrieval, and serial number generation.
     * 
     * **Usage:**
     * - Access the singleton instance using @ref UIRB::getInstance().
     * - Verify the initialization result using @ref UIRB::begin().
     * - Use power-related functions like @ref UIRB::getPowerInfo() to retrieve detailed power metrics.
     * - Configure sleep modes and wakeup sources via @ref UIRB::powerDown() and related wakeup flag methods.
     * - Access EEPROM-stored settings for customization and calibration.
     * 
     * **Design Considerations:**
     * - The class assumes a hardware environment based on the AVR architecture, specifically targeting the ATMega328P microcontroller, designed for use with this library.
     * - Critical thresholds and constants are derived from datasheets (e.g., TP4057 Li-ion charger IC, ATMega328P).
     * - Extensive validation ensures safe operation, with error codes and warnings for invalid configurations.
     * 
     * **Important Notes:**
     * - All public functions are designed to fail gracefully, returning error codes or default values for invalid operations.
     * - The EEPROM-based configuration must be initialized correctly for accurate results.
     * 
     * **Example:**
     * @code
     * UIRB& board = UIRB::getInstance();
     * if (!board.begin())
     * {
     *     // Handle initialization failure
     *     while(true); // Hang
     * }
     * 
     * PowerInfoData& powerInfo = board.getPowerInfo();
     * if (powerInfo.isValid())
     * {
     *     // Use sampled data
     * }
     * @endcode
     * 
     * **Dependencies:**
     * - Relies on the @ref PowerInfoData class for detailed power-related calculations and state management.
     * - Uses the @ref eeprom::EEPROMDataManager class for handling persistent settings in EEPROM.
     * 
     * @see @ref UIRB::getInstance() for accessing the singleton instance.
     * @see @ref UIRB::begin() for hardware initialization.
     * @see @ref UIRB::getPowerInfo() for retrieving power metrics.
     * @see @ref PowerInfoData for detailed power-related data.
     * @see @ref eeprom::EEPROMDataManager for EEPROM handling.
     */
    class UIRB
    {
        public:
            /**
             * @brief Retrieves the singleton instance of the @ref UIRB class.
             * 
             * This function provides access to the singleton instance of the @ref UIRB class, ensuring that only 
             * one instance of the class exists throughout the program's lifecycle. The instance is lazily initialized 
             * upon the first call to this function.
             * 
             * @return UIRB& Reference to the singleton instance of the @ref UIRB class.
             * 
             * @note The singleton instance is created using a `static` local variable.
             */
            static UIRB& getInstance()
            {
                static UIRB instance;
                return instance;
            }

            /**
             * @brief Deleted copy constructor to enforce singleton pattern.
             * 
             * Prevents copying of the @ref UIRB singleton instance to ensure that only one instance of the class 
             * exists at any given time.
             */
            UIRB(const UIRB&) = delete;

            /**
             * @brief Deleted assignment operator to enforce singleton pattern.
             * 
             * Prevents assignment to the @ref UIRB singleton instance, maintaining the integrity of the singleton pattern.
             */
            void operator=(const UIRB&) = delete;

            /**
             * @brief Result of @ref UIRB class initialization.
             * 
             * The initialization succeeds if the hardware version matches the value stored in @ref uirbcore::eeprom::UIRB_HW_VER.
             * However, some operations may still fail, so it is essential to verify the returned value for potential errors.
             * 
             * @note The @ref CoreResult type includes an overloaded `!` operator, which returns `true` if the result is not `CoreResult::SUCCESS`.
             * 
             * @warning If returned value is not `CoreResult::SUCCESS`, continued use of the @ref UIRB class may result in hardware damage.
             * In such cases, it is strongly recommended to halt execution and reset the board using the watchdog timer.
             * 
             * @return @ref CoreResult Result of initialization.
             * @retval #CoreResult::SUCCESS Initialization was successful.
             * @retval #CoreResult::ERROR_EEPROM_HW_VER_MISMATCH Hardware version from eeprom does not match the hardware version of the board defined in this library.
             * @retval #CoreResult::ERROR_EEPROM_SAVE_FAILED Failed to save data to eeprom.
             * @retval #CoreResult::ERROR_EEPROM_CHARGER_PROG_RESISTANCE_INVALID Charger Rprog resistor resistance from eeprom is invalid.
             */
            CoreResult begin() const;

            /**
             * @brief Reloads the @ref UIRB configuration from EEPROM.
             * 
             * This function retrieves the stored configuration data from the EEPROM. 
             * This allows the application to restore previously saved settings, 
             * such as hardware version, calibration data, and user-defined preferences.
             * 
             * **Operation Details:**
             * - Calls the internal @ref eeprom::EEPROMDataManager::load_from_eeprom() method to load data.
             * - Validates the loaded configuration by checking the PROG resistor resistance.
             * 
             * @note This function is useful for reinitializing the device state after a reset 
             *       or when reverting to stored configurations.
             * 
             * @return bool
             * @retval true Reload operation was successful, and the configuration is valid.
             * @retval false Reload operation failed, the configuration contains invalid PROG resistor resistance.
             * 
             * @see @ref UIRB::saveToEEPROM() for saving configurations back to EEPROM.
             */
            bool reloadFromEEPROM();

            /**
             * @brief Saves the @ref UIRB configuration to EEPROM.
             * 
             * This function stores the current configuration data managed by 
             * @ref eeprom::EEPROMDataManager into the EEPROM. It ensures that persistent settings 
             * are saved, allowing them to be restored using the @ref UIRB::reloadFromEEPROM() function.
             * 
             * @return bool
             * @retval true The configuration was successfully saved to EEPROM.
             * @retval false The save operation failed, typically because the hardware 
             *               was not properly initialized (#UIRB::begin() is not #CoreResult::SUCCESS).
             * 
             * @note This function should be called only after verifying that the hardware 
             * and initialization process completed successfully.
             * 
             * @see @ref UIRB::reloadFromEEPROM() for reloading configurations from EEPROM.
             */
            bool saveToEEPROM();

            /**
             * @brief Checks if there are unsaved changes in the current RAM configuration compared to EEPROM.
             * 
             * This function compares the current data stored in RAM with the data stored in EEPROM to determine 
             * if any modifications have been made that have not yet been saved. It is useful for identifying 
             * if the configuration needs to be persisted to EEPROM to ensure consistency.
             * 
             * @return bool
             * @retval true If the current RAM configuration differs from the EEPROM data (unsaved changes exist).
             * @retval false If the current RAM configuration matches the EEPROM data (no unsaved changes).
             * 
             * @note This function performs a deep comparison of the stored data in RAM and EEPROM.
             *       Use this to decide whether to call @ref UIRB::saveToEEPROM() to persist changes.
             * 
             * @see @ref UIRB::saveToEEPROM() for saving changes to EEPROM.
             * @see @ref UIRB::reloadFromEEPROM() for reloading data from EEPROM.
             */
            bool hasUnsavedDataChanges() const;

            /**
             * @brief Retrieves a copy of the data currently stored in RAM from the @ref eeprom::EEPROMDataManager.
             * 
             * This function returns a copy of the @ref eeprom::EEPROMData stored in RAM, as maintained by the
             * internal @ref eeprom::EEPROMDataManager object. The data is returned as a value.
             * 
             * @return eeprom::EEPROMData A copy of the data currently stored in RAM.
             * 
             * @note This function provides a copy of the stored data in RAM, not directly from EEPROM.
             * 
             * @see @ref eeprom::EEPROMDataManager::get() for the underlying implementation.
             */
            eeprom::EEPROMData getDataStoredInRAM() const;

            /**
             * @brief Retrieves the data currently stored in RAM from the @ref eeprom::EEPROMDataManager.
             * 
             * This function forwards the call to the internal @ref eeprom::EEPROMDataManager object to fetch 
             * the current state of EEPROM data stored in RAM. The retrieved data is populated 
             * into the provided @ref eeprom::EEPROMData reference.
             * 
             * @param[out] data Reference to an @ref eeprom::EEPROMData object that will be populated with 
             * the stored data.
             * 
             * @note This function provides a copy of the stored data in RAM, not directly from EEPROM.
             * 
             * @see eeprom::EEPROMDataManager::get(eeprom::EEPROMData&) for the underlying implementation.
             */
            void getDataStoredInRAM(eeprom::EEPROMData& data) const;

            /**
             * @brief Updates power-related information and retrieves it as a reference.
             * 
             * This function updates the power metrics by sampling the supply voltage, 
             * @ref PIN_PROG pin voltage, and estimating the charging current. It also 
             * evaluates the battery and charger states based on the updated data. 
             * Additionally, it provides the option to flash the STAT LED if the battery 
             * is low.
             * 
             * @param[in] samples Number of samples to take during power information update. Defaults to `5`.
             * @param[in] flashSTATOnLowBattery If `true`, flashes the STAT LED to indicate low battery. Defaults to `true`.
             * @return PowerInfoData& Reference to the updated @ref PowerInfoData object containing power metrics and states.
             * 
             * @note Ensure the system periodically calls this function to keep power information accurate.
             * 
             * @see @ref PowerInfoData for the structure of the returned data.
             * @see @ref PowerInfoData::update(uint8_t) for details on how the power metrics are updated.
             * @see @ref UIRB::notifyStatusLowBattery() for the implementation of the low battery notification.
             */
            PowerInfoData& getPowerInfo(const uint8_t samples = 5, const bool flashSTATOnLowBattery = true);

            /**
             * @brief Flashes the status LED (on @ref PIN_STAT_LED pin) to indicate a low battery condition using Morse code.
             * 
             * This function uses the status LED on @ref PIN_STAT_LED pin to visually signal that the battery voltage 
             * is below the configured low-battery threshold. The LED flashes the Morse 
             * code for the letter "L" (dot-dash-dot-dot: `.-..`) to communicate the condition.
             * 
             * @details
             * - The function ensures the original pin mode and state of @ref PIN_STAT_LED 
             *   are restored after signaling.
             * - The flashing pattern consists of:
             *   - A dot (`.`): LED on for 50 ms, off for 200 ms.
             *   - A dash (`-`): LED on for 200 ms, off for 200 ms.
             * - The sequence is followed by a 500 ms pause to signify the end of the pattern.
             * 
             * **Function Workflow:**
             * 1. Saves the original mode and state of @ref PIN_STAT_LED.
             * 2. Sets @ref PIN_STAT_LED as an output to control the LED.
             * 3. Executes the Morse code flashing pattern.
             * 4. Restores the original mode and state of @ref PIN_STAT_LED.
             * 
             * @note This function is typically called by @ref UIRB::getPowerInfo() when 
             *       a low-battery condition is detected.
             * 
             * @see @ref UIRB::getPowerInfo() for triggering this function during a power 
             *      information update.
             * @see @ref UIRB::BATTERY_EMPTY_SUPPLY_VOLTAGE_MIN_MV for the low-battery threshold.
             */
            static void notifyStatusLowBattery();

            /**
             * @brief Measures the voltage at the @ref PIN_PROG pin in millivolts.
             * 
             * This function performs an analog-to-digital conversion on the @ref PIN_PROG pin, using the internal bandgap 
             * reference voltage or AVcc as the reference, depending on the voltage range. The result is converted 
             * into millivolts and returned. Multiple samples can be taken and averaged for better accuracy.
             * 
             * The internal 1.1V reference voltage (INTERNAL1V1) is used initially. If the voltage on the @ref PIN_PROG 
             * pin exceeds the range of the 1.1V reference, the function automatically switches to the default AVcc reference. 
             * 
             * @note
             * The 1.1V internal reference must be calibrated by physically measuring it at the ARef pin capacitor and 
             * setting the value using @ref UIRB::setInternalBandgapReferenceVoltageMilivolts(). Accurate calibration is essential 
             * for precise voltage readings.
             * 
             * @param[in] samples Number of ADC samples to take for averaging. Defaults to 5.
             * @return uint16_t The measured voltage at the @ref PIN_PROG pin in millivolts.
             * @retval #UIRB::INVALID_VOLTAGE_MILIVOLTS If an error occurs during the measurement or the voltage is out of range.
             * 
             * @see @ref UIRB::getInternalBandgapReferenceVoltageMilivolts() for retrieving the calibrated bandgap reference voltage.
             *      @ref UIRB::setInternalBandgapReferenceVoltageMilivolts() for calibrating the 1.1V reference.
             */
            uint16_t getProgVoltageMilivolts(const uint8_t samples = 5);

            /**
             * @brief Measures the supply voltage (AVcc) in millivolts.
             * 
             * This function determines the supply voltage (AVcc) by sampling the internal bandgap reference voltage 
             * using AVcc as the reference. The result is then calculated based on the calibrated bandgap value and 
             * returned in millivolts. Multiple samples can be taken and averaged for better accuracy.
             * 
             * @note
             * The internal 1.1V reference voltage (INTERNAL1V1) must be calibrated by physically measuring it at the 
             * ARef pin capacitor and setting the value using @ref UIRB::setInternalBandgapReferenceVoltageMilivolts(). 
             * Accurate calibration is critical for precise AVcc calculations.
             * 
             * @param[in] samples Number of ADC samples to take for averaging. Defaults to 5.
             * @return uint16_t The measured supply voltage (AVcc) in millivolts.
             * @retval #UIRB::INVALID_VOLTAGE_MILIVOLTS If an error occurs during the measurement or the value is out of range.
             * 
             * @see @ref UIRB::getInternalBandgapReferenceVoltageMilivolts() for retrieving the calibrated bandgap reference voltage.
             *      @ref UIRB::setInternalBandgapReferenceVoltageMilivolts() for calibrating the 1.1V reference.
             */
            uint16_t getSupplyVoltageMilivolts(const uint8_t samples = 5);

            /**
             * @brief Puts the MCU into power-down sleep mode with optional wakeup sources and sleep duration.
             * 
             * This function minimizes the MCU's power consumption by entering power-down sleep mode. It supports optional wakeup 
             * sources, such as a button connected to @ref PIN_BUTTON_WAKEUP or the USB host connected via CP2104 on @ref PIN_USB_IO3. 
             * The MCU can wake up after a specified timeout or via interrupt triggers. To handle wakeup events, ensure user-defined 
             * callback functions are set beforehand using @ref UIRB::setButtonWakeupCallback() and @ref UIRB::setIO3WakeupCallback().
             * 
             * **Functionality:**
             * - Configures the MCU for power-down mode and disables peripherals like the ADC and analog comparator.
             * - Supports wakeup through:
             *   - Button pin (@ref PIN_BUTTON_WAKEUP).
             *   - IO3 pin (@ref PIN_USB_IO3).
             *   - Both pins.
             *   - Watchdog timer (for timed wakeup).
             * - Automatically restores peripheral and pin states after waking up.
             * - Utilizes the watchdog timer for managing sleep intervals, with support for multiple consecutive intervals.
             * 
             * @param[in] sleeptime_milliseconds Sleep duration in milliseconds. Defaults to @ref UIRB::SLEEP_FOREVER for indefinite sleep.
             *                                   The watchdog timer handles timed wakeups.
             * @param[in] wakeupSource Specifies the wakeup source. Options include:
             *                         - `WakeupInterrupt::WAKE_BUTTON` (default): Wakeup via the button pin.
             *                         - `WakeupInterrupt::USB_IO3`: Wakeup via the IO3 pin.
             *                         - `WakeupInterrupt::WAKE_BUTTON_AND_USB_IO3`: Wakeup via both button and IO3.
             *                         - `WakeupInterrupt::NONE`: Only a timeout can wake the MCU.
             * 
             * @note This function has no effect if @ref AVR_DEBUG is defined or if sleeping is not allowed, as determined by
             *       @ref UIRB::isSleepingAllowed().
             * 
             * **Wakeup Behavior:**
             * - Executes user-defined callbacks for the triggered wakeup source:
             *   - Use @ref UIRB::setButtonWakeupCallback() for button interrupts.
             *   - Use @ref UIRB::setIO3WakeupCallback() for IO3 interrupts.
             * - Clears wakeup flags to prepare for subsequent sleep cycles.
             * - If no interrupt source is triggered, the watchdog timer is assumed to have caused the wakeup.
             * 
             * @note This function is optimized for low power, disabling certain peripherals during sleep and restoring them after 
             *       wakeup. Any changes to pins or peripherals during sleep are reverted.
             * 
             * **Limitations:**
             * - Watchdog timer intervals range from 16 ms to 8 seconds per interval.
             * - Sleep durations exceeding the maximum watchdog timer interval are split into multiple intervals.
             * 
             * @warning Configure pins, wakeup sources, and callbacks properly before calling this function to prevent unintended 
             *          behavior. Debugging can be aided using interrupt flags such as @ref UIRB::getButtonWakeupISRFlag() and 
             *          @ref UIRB::getIO3WakeupISRFlag().
             * 
             * @see @ref UIRB::setButtonWakeupCallback(), @ref UIRB::setIO3WakeupCallback(), 
             *      @ref UIRB::getButtonWakeupISRFlag(), @ref UIRB::getIO3WakeupISRFlag(), @ref UIRB::getWakeupISRFlag(), 
             *      @ref UIRB::clearButtonWakeupISRFlag(), @ref UIRB::clearIO3WakeupISRFlag(), @ref UIRB::clearWakeupISRFlags(), 
             *      @ref UIRB::getAndClearButtonWakeupISRFlag(), @ref UIRB::getAndClearIO3WakeupISRFlag(), 
             *      @ref UIRB::getAndClearWakeupISRFlag().
             */
            void powerDown(const uint32_t sleeptime_milliseconds = UIRB::SLEEP_FOREVER, const WakeupInterrupt wakeupSource = WakeupInterrupt::WAKE_BUTTON) __attribute__((optimize("-O1")));

            /**
             * @brief Sets the callback function for the button wakeup interrupt.
             * 
             * Assigns a user-defined callback function to execute when the button wakeup interrupt is triggered. This allows custom
             * behavior to be implemented during the interrupt service routine.
             * 
             * @note This function has no effect if @ref AVR_DEBUG is defined.
             * 
             * @param[in] callback Pointer to the callback function to be executed on the button wakeup interrupt.
             * 
             * @see @ref PIN_BUTTON_WAKEUP for the button pin used to trigger the wakeup interrupt.
             */
            void setButtonWakeupCallback(void (*callback)());

            /**
             * @brief Sets the callback function for the IO3 wakeup interrupt.
             * 
             * Assigns a user-defined callback function to execute when the IO3 wakeup interrupt is triggered. This allows custom
             * behavior to be implemented during the interrupt service routine.
             * 
             * @note This function has no effect if @ref AVR_DEBUG is defined.
             * 
             * @param[in] callback Pointer to the callback function to be executed on the IO3 wakeup interrupt.
             * 
             * @see @ref PIN_USB_IO3 for the pin used to trigger the IO3 wakeup interrupt.
             */
            void setIO3WakeupCallback(void (*callback)());

            /**
             * @brief Retrieves the button wakeup interrupt flag.
             * 
             * Indicates whether the button wakeup interrupt flag is currently set. This flag is set 
             * when the corresponding ISR is triggered by a button wakeup event.
             * 
             * @note Returns `false` if @ref AVR_DEBUG is defined.
             * 
             * @return bool True if the button wakeup interrupt flag is set, false otherwise.
             * @retval true The button wakeup interrupt flag is set.
             * @retval false The flag is not set, or @ref AVR_DEBUG is defined.
             * 
             * @see @ref UIRB::clearButtonWakeupISRFlag() to clear this flag.
             */
            bool getButtonWakeupISRFlag() const;

            /**
             * @brief Retrieves and clears the button wakeup interrupt flag.
             * 
             * This function checks the current state of the button wakeup interrupt flag using 
             * @ref UIRB::getButtonWakeupISRFlag(), and then clears the flag.
             * 
             * @note Returns `false` and does nothing if @ref AVR_DEBUG is defined.
             * 
             * @return bool True if the flag was set before clearing, false otherwise.
             * @retval true The button wakeup interrupt flag was set before clearing.
             * @retval false The flag was not set, or @ref AVR_DEBUG is defined.
             * 
             * @see @ref UIRB::clearButtonWakeupISRFlag() for direct flag clearing.
             */
            bool getAndClearButtonWakeupISRFlag();

            /**
             * @brief Retrieves the IO3 wakeup interrupt flag.
             * 
             * Indicates whether the IO3 wakeup interrupt flag is currently set. This flag is set when 
             * the corresponding ISR is triggered by an event on @ref PIN_USB_IO3.
             * 
             * @note Returns `false` if @ref AVR_DEBUG is defined.
             * 
             * @return bool True if the IO3 wakeup interrupt flag is set, false otherwise.
             * @retval true The IO3 wakeup interrupt flag is set.
             * @retval false The flag is not set, or @ref AVR_DEBUG is defined.
             * 
             * @see @ref UIRB::clearIO3WakeupISRFlag() to clear this flag.
             */
            bool getIO3WakeupISRFlag() const;

            /**
             * @brief Retrieves and clears the IO3 wakeup interrupt flag.
             * 
             * This function checks the current state of the IO3 wakeup interrupt flag using 
             * @ref UIRB::getIO3WakeupISRFlag(), and then clears the flag.
             * 
             * @note Returns `false` and does nothing if @ref AVR_DEBUG is defined.
             * 
             * @return bool True if the flag was set before clearing, false otherwise.
             * @retval true The IO3 wakeup interrupt flag was set before clearing.
             * @retval false The flag was not set, or @ref AVR_DEBUG is defined.
             * 
             * @see @ref UIRB::clearIO3WakeupISRFlag() for direct flag clearing.
             */
            bool getAndClearIO3WakeupISRFlag();

            /**
             * @brief Checks if any wakeup interrupt flags are set.
             * 
             * This function determines if any of the wakeup interrupt flags (button or IO3) are currently set. If no flags are set, 
             * but the MCU has still woken up, the wakeup event was likely triggered by a watchdog timer (WDT) timeout.
             * 
             * @note Returns `false` if @ref AVR_DEBUG is defined.
             * 
             * @return bool True if at least one wakeup interrupt flag is set, false otherwise.
             * @retval true At least one wakeup interrupt flag is set.
             * @retval false None of the wakeup interrupt flags are set, or @ref AVR_DEBUG is defined.
             * 
             * @see @ref UIRB::getButtonWakeupISRFlag() and @ref UIRB::getIO3WakeupISRFlag() for individual flags.
             */
            bool getWakeupISRFlag() const;

            /**
             * @brief Retrieves and clears all wakeup interrupt flags.
             * 
             * This function checks if any wakeup interrupt flags (button or IO3) are set using @ref UIRB::getWakeupISRFlag(),
             * and then clears all the flags. If no flags are set but the MCU has woken up, the wakeup event was likely triggered 
             * by a watchdog timer (WDT) timeout.
             * 
             * @note Returns `false` and does nothing if @ref AVR_DEBUG is defined.
             * 
             * @return bool True if any flags were set before clearing, false otherwise.
             * @retval true At least one wakeup interrupt flag was set before clearing.
             * @retval false None of the flags were set, or @ref AVR_DEBUG is defined.
             * 
             * @see @ref UIRB::clearWakeupISRFlags() for direct clearing of all flags.
             */
            bool getAndClearWakeupISRFlag();

            /**
             * @brief Clears the button wakeup interrupt flag.
             * 
             * This function resets the internal flag for the button wakeup interrupt (@ref PIN_BUTTON_WAKEUP), ensuring that it does not 
             * affect subsequent operations. Typically used after handling a wakeup event triggered by the button.
             * 
             * @note This function has no effect if @ref AVR_DEBUG is defined.
             * 
             * @see @ref UIRB::clearWakeupISRFlags() to clear all wakeup interrupt flags at once.
             */
            void clearButtonWakeupISRFlag();

            /**
             * @brief Clears the IO3 wakeup interrupt flag.
             * 
             * Resets the internal flag for the IO3 wakeup interrupt, ensuring it does not interfere with future wakeup events. 
             * IO3 corresponds to @ref PIN_USB_IO3.
             * 
             * @note This function has no effect if @ref AVR_DEBUG is defined.
             * 
             * @see @ref UIRB::clearWakeupISRFlags() to clear all wakeup interrupt flags at once.
             */
            void clearIO3WakeupISRFlag();

            /**
             * @brief Clears all wakeup interrupt flags.
             * 
             * This function resets both the button and IO3 wakeup interrupt flags, ensuring a clean state for future wakeup events.
             * 
             * @note This function has no effect if @ref AVR_DEBUG is defined.
             * 
             * @see @ref UIRB::clearButtonWakeupISRFlag() and @ref UIRB::clearIO3WakeupISRFlag() for clearing individual flags.
             */
            void clearWakeupISRFlags();

            /**
             * @brief Retrieves the internal bandgap reference voltage in millivolts.
             * 
             * The internal bandgap reference voltage is stored in EEPROM as an offset from the nominal 1100mV. 
             * This value is used as the reference voltage for ADC conversions with `analogReference(INTERNAL1V1)`.
             * 
             * @return uint16_t The internal bandgap reference voltage in millivolts.
             * 
             * @see @ref UIRB::setInternalBandgapReferenceVoltageMilivolts() to update the value.
             * @see @ref UIRB::getInternalBandgapReferenceVoltage() for the value in volts.
             */
            uint16_t getInternalBandgapReferenceVoltageMilivolts() const;

            /**
             * @brief Sets the internal bandgap reference voltage in millivolts and saves it to EEPROM.
             * 
             * This function allows the measured internal bandgap reference voltage to be stored as an offset from 1100mV 
             * in the EEPROM. The offset ensures accurate ADC conversions. The input voltage must be within the valid 
             * range of 972mV to 1227mV.
             * 
             * @param[in] milivolts The measured bandgap reference voltage in millivolts. Valid range: [972-1227].
             * @param[in] saveToEEPROM If `true`, the calculated offset is saved to EEPROM.
             * @return bool Indicates whether the new value was successfully set and saved to EEPROM.
             * @retval true The new bandgap reference voltage offset was successfully stored.
             * @retval false The input voltage is out of range, saving to EEPROM failed, or @p saveToEEPROM is `false`.
             * 
             * @see @ref UIRB::getInternalBandgapReferenceVoltageMilivolts() to read the stored value.
             */
            bool setInternalBandgapReferenceVoltageMilivolts(const uint16_t milivolts, const bool saveToEEPROM);

            /**
             * @brief Sets the internal bandgap reference voltage in millivolts.
             * 
             * This function allows the measured internal bandgap reference voltage to be stored as an offset from 1100mV 
             * in the RAM. The offset ensures accurate ADC conversions. The input voltage must be within the valid 
             * range of 972mV to 1227mV.
             * 
             * @param[in] milivolts The measured bandgap reference voltage in millivolts. Valid range: [972-1227].
             * @return bool Indicates whether the new value was successfully set and saved to RAM.
             * @retval true The new bandgap reference voltage offset was successfully stored to RAM.
             * @retval false The input voltage is out of range.
             * 
             * @see @ref UIRB::getInternalBandgapReferenceVoltageMilivolts() to read the stored value.
             */
            bool setInternalBandgapReferenceVoltageMilivolts(const uint16_t milivolts);

            /**
             * @brief Retrieves the internal bandgap reference voltage in volts.
             * 
             * Converts the stored internal bandgap reference voltage from millivolts to volts for convenience. 
             * Useful for applications requiring floating-point precision.
             * 
             * @return float The internal bandgap reference voltage in volts.
             * 
             * @see @ref UIRB::getInternalBandgapReferenceVoltageMilivolts() for the value in millivolts.
             */
            float getInternalBandgapReferenceVoltage() const;

            /**
             * @brief Retrieves the full hardware version number as a floating-point value.
             * 
             * Combines the major and minor hardware version numbers into a single floating-point value, 
             * where the integer part represents the major version and the decimal part represents the minor version.
             * For example, version 0.2 represents major version 0 and minor version 2.
             * 
             * @return float The full hardware version number.
             * 
             * @see @ref UIRB::getVersionMajor() for the major version.
             * @see @ref UIRB::getVersionMinor() for the minor version.
             */
            float getVersion() const;

            /**
             * @brief Retrieves the major hardware version number.
             * 
             * Returns the major version of the hardware as an integer in the range [0-15]. 
             * 
             * @return uint8_t The major hardware version number (0-15).
             * 
             * @see @ref UIRB::getVersionMinor() for the minor version.
             * @see @ref UIRB::getVersion() for the combined version.
             */
            uint8_t getVersionMajor() const;

            /**
             * @brief Retrieves the minor hardware version number.
             * 
             * Returns the minor version of the hardware as an integer in the range [0-15]. 
             * 
             * @return uint8_t The minor hardware version number (0-15).
             * 
             * @see @ref UIRB::getVersionMajor() for the major version.
             * @see @ref UIRB::getVersion() for the combined version.
             */
            uint8_t getVersionMinor() const;

            /**
             * @brief Retrieves the month of the board's manufacture.
             * 
             * This function returns the month when the board was manufactured, represented as a value 
             * between 1 (January) and 12 (December). The value is stored in the device's EEPROM and read 
             * during initialization.
             * 
             * @return uint8_t The month of board manufacture (1-12).
             * @retval #INVALID_MANUFACTURE_MONTH Indicates that the month of manufacture is invalid.
             * 
             * @see @ref UIRB::getYearOfManufacture() for the corresponding year of manufacture.
             */
            uint8_t getMonthOfManufacture() const;

            /**
             * @brief Retrieves the year of the board's manufacture.
             * 
             * This function returns the year when the board was manufactured. The value is stored in the 
             * device's EEPROM and read during initialization.
             * 
             * @return uint16_t The year of board manufacture.
             * 
             * @see @ref UIRB::getMonthOfManufacture() for the corresponding month of manufacture.
             */
            uint16_t getYearOfManufacture() const;

            /**
             * @brief Retrieves the brightness setting of the status LED.
             * 
             * This function returns the current brightness level of the status LED, which is represented 
             * as a value between 0 and 255. A value of 0 indicates that the LED is off, while 255 corresponds 
             * to maximum brightness.
             * 
             * @return uint8_t The current brightness level of the status LED (0 to 255).
             * 
             * @see @ref UIRB::setStatusLEDBrightness() for setting the current brightness level.
             */
            uint8_t getStatusLEDBrightness() const;

            /**
             * @brief Sets a new brightness level for the status LED and optionally saves it to EEPROM.
             * 
             * This function adjusts the brightness of the status LED to the specified level and optionally 
             * saves the new setting to EEPROM for persistence. The brightness level can range from 0 (off) to 
             * 255 (maximum brightness). If the @p saveToEEPROM parameter is set to `true`, the new brightness 
             * level is stored in EEPROM.
             * 
             * @param[in] brightness The desired brightness level (0 to 255).
             * @param[in] saveToEEPROM Whether to save the new brightness setting to EEPROM.
             * @return bool Indicates the success of the operation.
             * @retval true The brightness level was successfully updated and, if requested, saved to EEPROM.
             * @retval false Failed to save the brightness level to EEPROM or @p saveToEEPROM was set to `false`.
             * 
             * @see @ref UIRB::getStatusLEDBrightness() for retrieving the current brightness level.
             */
            bool setStatusLEDBrightness(const uint8_t brightness, const bool saveToEEPROM);

            /**
             * @brief Sets a new brightness level for the status LED.
             * 
             * This function adjusts the brightness of the status LED to the specified level and saves the 
             * new setting to RAM. The brightness level can range from 0 (off) to 255 (maximum brightness).
             * 
             * @param[in] brightness The desired brightness level (0 to 255).
             * 
             * @see @ref UIRB::getStatusLEDBrightness() for retrieving the current brightness level.
             */
            void setStatusLEDBrightness(const uint8_t brightness);

            /**
             * @brief Retrieves the resistance of the charger @ref PIN_PROG resistor in ohms.
             * 
             * This function returns the resistance value of the @ref PIN_PROG resistor, which determines the charging current 
             * for the connected Li-ion battery. The value is stored in the device's EEPROM and is read during initialization.
             * 
             * @return uint16_t The resistance of the @ref PIN_PROG resistor in ohms.
             * @retval #INVALID_PROG_RESISTOR_RESISTANCE Indicates that the resistance value is invalid or not configured.
             * 
             * @see @ref PIN_PROG for the associated hardware pin used to set the charging current via the PROG resistor.
             */
            uint16_t getChargerProgResistorResistance() const;

            /**
             * @brief Checks if the serial debugger was enabled at compile time.
             * 
             * This function determines whether the firmware was compiled with the @ref AVR_DEBUG macro defined. 
             * The setting is initialized during the construction of the @ref UIRB class and reflects the compile-time 
             * configuration. If @ref AVR_DEBUG is defined, this function returns `true`; otherwise, it returns `false`.
             * 
             * @return bool Indicates whether the firmware was compiled with the serial debugger enabled.
             * @retval true The firmware was compiled with @ref AVR_DEBUG defined.
             * @retval false The firmware was compiled without @ref AVR_DEBUG defined.
             */
            bool isSerialDebuggerEnabled() const;
            
            /**
             * @brief Reads the EEPROM configuration to determine if sleep mode is allowed.
             * 
             * This function checks the stored setting in EEPROM to decide whether sleep mode is enabled. 
             * It does not directly control or configure the hardware for entering sleep mode.
             * 
             * @note Always returns `false` if @ref AVR_DEBUG is defined, overriding the EEPROM configuration 
             *       to disable sleep mode during debugging.
             * 
             * @return bool True if the EEPROM setting allows sleep mode, false otherwise.
             * @retval true Sleep mode is allowed based on the stored configuration.
             * @retval false Sleep mode is disallowed by the configuration or @ref AVR_DEBUG is defined.
             * @see @ref UIRB::setSleepingAllowed() to modify the stored configuration.
             */
            bool isSleepingAllowed() const;

            /**
             * @brief Updates the EEPROM configuration to allow or disallow sleep mode.
             * 
             * Modifies the stored setting in EEPROM to enable or disable sleep mode. This function 
             * only updates the configuration and does not directly interact with the hardware to 
             * initiate or block sleep mode.
             * 
             * @param[in] allowed Set to `true` to enable sleep mode, or `false` to disable it.
             * @param[in] saveToEEPROM Set to `true` to persist the new configuration in EEPROM.
             * 
             * @return bool True if the configuration was successfully updated and, if specified, saved to EEPROM.
             * @retval true The EEPROM configuration was successfully updated.
             * @retval false The configuration was not saved due to an error or @p saveToEEPROM being set to `false`.
             * @see @ref UIRB::isSleepingAllowed() to read the current stored configuration.
             */
            bool setSleepingAllowed(const bool allowed, const bool saveToEEPROM);

            /**
             * @brief Updates the configuration in RAM to allow or disallow sleep mode.
             * 
             * Modifies the stored setting in RAM to enable or disable sleep mode. This function 
             * only updates the configuration and does not directly interact with the hardware to 
             * initiate or block sleep mode.
             * 
             * @param[in] allowed Set to `true` to enable sleep mode, or `false` to disable it.
             * 
             * @see @ref UIRB::isSleepingAllowed() to read the current stored configuration.
             */
            void setSleepingAllowed(const bool allowed);

            /**
             * @brief Reads the EEPROM configuration to determine if wakeup from the @ref PIN_USB_IO3 pin is allowed.
             * 
             * This function checks the stored setting in EEPROM to decide whether the interrupt on the @ref PIN_USB_IO3 
             * pin should be attached for wakeup functionality. It does not interact with the actual hardware or configure the pin.
             * 
             * @note Returns `false` if @ref AVR_DEBUG is defined, overriding the EEPROM setting to disable sleep-related functionality during debugging.
             * 
             * @return bool True if the EEPROM setting allows wakeup from the @ref PIN_USB_IO3 pin, false otherwise.
             * @retval true The stored configuration enables wakeup from the @ref PIN_USB_IO3 pin.
             * @retval false The stored configuration disables wakeup, or @ref AVR_DEBUG is defined.
             * @see @ref UIRB::setWakeupFromIO3Allowed() to modify the stored configuration.
             */
            bool isWakeupFromIO3Allowed() const;

            /**
             * @brief Updates the EEPROM configuration to allow or disallow wakeup from the @ref PIN_USB_IO3 pin.
             * 
             * Modifies the stored setting in EEPROM to enable or disable the use of the @ref PIN_USB_IO3 pin 
             * as a wakeup source. This function only updates the configuration and does not interact with the 
             * hardware or configure the interrupt.
             * 
             * @param[in] allowed Set to `true` to enable wakeup from the @ref PIN_USB_IO3 pin, or `false` to disable it.
             * @param[in] saveToEEPROM Set to `true` to persist the new configuration in EEPROM.
             * 
             * @return bool True if the configuration was successfully updated and, if specified, saved to EEPROM.
             * @retval true The EEPROM configuration was successfully updated.
             * @retval false The configuration was not saved due to an error or @p saveToEEPROM being set to `false`.
             * @see @ref UIRB::isWakeupFromIO3Allowed() to read the current stored configuration.
             */
            bool setWakeupFromIO3Allowed(const bool allowed, const bool saveToEEPROM);

            /**
             * @brief Updates the configuration in RAM to allow or disallow wakeup from the @ref PIN_USB_IO3 pin.
             * 
             * Modifies the stored setting in RAM to enable or disable the use of the @ref PIN_USB_IO3 pin 
             * as a wakeup source. This function only updates the configuration and does not interact with the 
             * hardware or configure the interrupt.
             * 
             * @param[in] allowed Set to `true` to enable wakeup from the @ref PIN_USB_IO3 pin, or `false` to disable it.
             *
             * @see @ref UIRB::isWakeupFromIO3Allowed() to read the current stored configuration.
             */
            void setWakeupFromIO3Allowed(const bool allowed);

            /**
             * @brief Checks if boot count incrementing is enabled.
             * 
             * Determines whether the boot count is incremented during the construction of this class.
             * If enabled, the count increases each time the device is powered on or reset.
             * 
             * @return bool True if boot count incrementing is enabled, false otherwise.
             * @see @ref UIRB::getBootCount() for retrieving the current boot count.
             */
            bool isBootCountingEnabled() const;

            /**
             * @brief Retrieves the current boot count.
             * 
             * Returns the total number of times the device has been powered on or reset, 
             * as incremented during class construction. If the count exceeds the maximum 
             * supported value, it is capped at UINT32_MAX.
             * 
             * @return uint32_t The current boot count.
             * @retval UINT32_MAX If the boot count has reached its maximum value.
             * @see @ref UIRB::isBootCountingEnabled() for checking if boot counting is active.
             */
            uint32_t getBootCount() const;

            /**
             * @brief Retrieves the %UIRB board serial number.
             * 
             * @return uint16_t The board serial number in the range `[0 - ` @ref eeprom::EEPROMDataManager::UIRB_SERIAL_NUMBER_MAX `]`.
             * @retval #INVALID_UIRB_SERIAL_NUMBER If the serial number is invalid.
             */
            uint16_t getBoardSerialNumber() const;
            
            /**
             * @brief Retrieves the formatted %UIRB board USB serial number.
             * 
             * The serial number is formatted as `"UIRB-Vab-yyyyuuuu-cccccccc"`, where:
             * - `a` = Major hardware version.
             * - `b` = Minor hardware version.
             * - `yyyy` = Year of manufacture.
             * - `uuuu` = Board serial number (zero-padded to 4 digits).
             * - `cccccccc` = Factory USB serial number (8 ASCII characters).
             * 
             * @return String The formatted USB serial number.
             * @retval String("") If one or more parameters are invalid in @ref eeprom::EEPROMDataManager.
             */
            String getUSBSerialNumber() const;

            /**
             * @brief Retrieves the factory CP2104 USB serial number.
             * 
             * @return String The 8-character factory USB serial number from CP2104.
             * @retval String("") Indicates failure to retrieve the serial number from @ref eeprom::EEPROMDataManager.
             */
            String getFactoryUSBSerialNumber() const;

            /**
             * @brief Constant representing indefinite sleep mode.
             * 
             * Specifies that the MCU should remain in sleep mode until explicitly woken by an external interrupt, without a predefined timeout.
             * 
             * @see @ref UIRB::powerDown() for details on entering power-down mode and configuring wakeup sources.
             */
            static constexpr uint8_t SLEEP_FOREVER = 0;

            /**
             * @brief Indicates an invalid current measurement in milliamps.
             * 
             * Defined as `UINT16_MAX`, representing an invalid state.
             */
            static constexpr uint16_t INVALID_CURRENT_MILIAMPS = UINT16_MAX;

            /**
             * @brief Indicates an invalid voltage measurement in millivolts.
             * 
             * Defined as `UINT16_MAX`, representing an invalid state.
             */
            static constexpr uint16_t INVALID_VOLTAGE_MILIVOLTS = UINT16_MAX;

            /**
             * @brief Indicates an unknown current measurement in milliamps.
             * 
             * Represents a condition where the current cannot be determined or estimated. 
             * Defined as @ref INVALID_CURRENT_MILIAMPS - 1 to distinguish it from invalid states.
             */
            static constexpr uint16_t UNKNOWN_CURRENT_MILIAMPS = INVALID_CURRENT_MILIAMPS - 1;

            /**
             * @brief Alias for @ref eeprom::EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE.
             */
            static constexpr uint16_t INVALID_PROG_RESISTOR_RESISTANCE = eeprom::EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE;

            /**
             * @brief Alias for @ref eeprom::EEPROMDataManager::INVALID_UIRB_SERIAL_NUMBER.
             */
            static constexpr uint16_t INVALID_UIRB_SERIAL_NUMBER = eeprom::EEPROMDataManager::INVALID_UIRB_SERIAL_NUMBER;

            /**
             * @brief Alias for @ref eeprom::EEPROMDataManager::INVALID_MANUFACTURE_MONTH.
             */
            static constexpr uint8_t INVALID_MANUFACTURE_MONTH = eeprom::EEPROMDataManager::INVALID_MANUFACTURE_MONTH;
        private:
            /**
             * @brief Private constructor to enforce the singleton pattern.
             * 
             * This constructor is private to ensure that the @ref UIRB class follows the singleton design pattern, 
             * allowing only one instance to exist throughout the program's lifecycle. It performs essential 
             * initialization tasks, including:
             * 
             * - Verifying the hardware version stored in EEPROM against the library's expected version.
             * - Configuring default pin modes and states for the device.
             * - Incrementing the boot count during initialization.
             * - Setting the result of the initialization process, accessible via the @ref UIRB::begin() method.
             * 
             * **Operation Details:**
             * - If the hardware version stored in EEPROM does not match the expected version, the system watchdog 
             *   timer is configured to reset the device after a 2-second delay to prevent further execution.
             * - Pins are reset to their default states to ensure consistent behavior across reboots.
             * - The boot count, which tracks the number of device restarts, is incremented and saved to EEPROM.
             * 
             * @note The constructor should never be called directly. Use @ref UIRB::getInstance() to access the singleton instance.
             */
            UIRB();

            /**
             * @brief Stores the result of the constructor's initialization process.
             * 
             * This variable holds the result of the @ref UIRB constructor, indicating the success or failure 
             * of the initialization process. It is set during the constructor's execution and can be 
             * accessed via the @ref UIRB::begin() method to determine whether the library and hardware are ready 
             * for use.
             * 
             * @note The initialization result should always be checked using the @ref UIRB::begin() method before 
             * interacting with other parts of the @ref UIRB class.
             */
            CoreResult initializationResult_ = CoreResult::ERROR_NOT_INITIALIZED;

            /**
             * @brief Performs an ADC conversion of the internal bandgap reference voltage using AVcc as the reference.
             * 
             * This function samples the internal 1.1V bandgap reference voltage against AVcc, providing a raw ADC result. 
             * Multiple samples can be averaged to improve precision and reduce noise. The function manually configures 
             * the ADC to use the internal bandgap reference and restores the original ADC settings after the operation.
             * 
             * **Operation Details:**
             * 1. Configures the ADC to use AVcc as the reference voltage and the internal bandgap reference as the input.
             * 2. Takes one or more ADC samples, with optional delays between successive samples.
             * 3. If multiple samples are taken, their average is calculated and returned.
             * 4. Restores the ADC settings and ensures no side effects from the operation.
             * 
             * @param[out] result Pointer to a `uint16_t` variable where the ADC result or averaged value will be stored.
             * @param[in] samples The number of ADC samples to take for averaging. Must be greater than 0. Default is 1.
             * 
             * @return @ref CoreResult Status of the operation.
             * @retval #CoreResult::SUCCESS The ADC conversion was successful, and the result is valid.
             * @retval #CoreResult::ERROR_INVALID_ARGUMENT One or more arguments are invalid:
             *                                   - @p result is `nullptr`.
             *                                   - @p samples is `0`.
             * 
             * @warning Ensure that AVcc is stable and within the expected range for accurate measurements.
             */
            CoreResult get_raw_bandgap_adc_sample(uint16_t* result, const uint8_t samples = 1);

            /**
             * @brief Retrieves an averaged ADC sample from the @ref PIN_PROG pin and adjusts the reference voltage if necessary.
             * 
             * This function performs an analog-to-digital conversion (ADC) on the voltage of the @ref PIN_PROG pin, allowing for 
             * multiple samples to be averaged for greater accuracy. If the ADC result is at the upper limit when using 
             * the `INTERNAL1V1` reference, the function automatically switches to the `DEFAULT` (AVcc) reference for 
             * re-sampling. The final averaged result is returned through the provided pointer.
             * 
             * **Operation Details:**
             * 1. Ensures the @ref PIN_PROG pin is set to `INPUT` mode before sampling to avoid interference from previous configurations.
             * 2. Performs the ADC conversion using the specified reference voltage (@p adcReference).
             * 3. If the `INTERNAL1V1` reference is used and the ADC result is at the maximum value, the reference is switched to 
             *    `DEFAULT` (AVcc), and sampling is repeated.
             * 4. Averages multiple samples if requested, incorporating delays between samples to ensure stability.
             * 5. Restores the pin mode, ADC reference, and prescaler settings to their original states after sampling.
             * 
             * @param[out] result Pointer to a `uint16_t` variable where the averaged ADC result will be stored.
             * @param[in, out] adcReference The initial ADC reference voltage to use (`DEFAULT` or `INTERNAL1V1`). If the conversion 
             *                               with `INTERNAL1V1` exceeds the valid range, the reference is switched to `DEFAULT`.
             *                               The final reference used is stored back in this parameter.
             * @param[in] samples The number of ADC samples to take for averaging. Must be greater than 0. Default is 1.
             * 
             * @return @ref CoreResult Status of the operation.
             * @retval #CoreResult::SUCCESS The ADC conversion was successful, and the result is valid.
             * @retval #CoreResult::ERROR_INVALID_ARGUMENT One or more arguments are invalid:
             *                                   - @p result is `nullptr`.
             *                                   - @p samples is `0`.
             *                                   - @p adcReference is not `DEFAULT` or `INTERNAL1V1`.
             * 
             * @warning The function assumes that the @ref PIN_PROG pin is properly connected to an appropriate voltage source for sampling.
             */
            CoreResult get_raw_prog_adc_sample(uint16_t* result, uint8_t& adcReference, const uint8_t samples = 1);

            /**
             * @brief Grants @ref PowerInfoData class access to private and protected members of this class.
             *
             * Declaring @ref PowerInfoData as a friend allows it to access private methods and variables of the @ref UIRB class. 
             * This is essential for enabling detailed power-related calculations and state management that require internal data.
             * 
             * @details 
             * The @ref PowerInfoData class depends on accurate and up-to-date measurements provided by the @ref UIRB class, such as:
             * - Supply voltage in millivolts.
             * - Charger state estimation.
             * - Battery charging current and state.
             */
            friend class PowerInfoData;

            /**
             * @brief Private instance of the @ref PowerInfoData class for managing power-related information.
             * 
             * This instance provides detailed power system metrics, including supply voltage, charging current, and charger/battery states.
             * It serves as an interface for external classes and functions to access and interpret power-related data in a structured manner.
             * 
             * @note 
             * - The @ref PowerInfoData instance is updated internally during power-related operations.
             * - Access this object via the @ref UIRB::getPowerInfo() method to ensure data consistency and accuracy.
             */
            PowerInfoData powerInfoData_ = PowerInfoData();

            /**
             * @brief Pointer to a user-defined callback function executed when the wakeup button triggers an MCU wakeup.
             * 
             * This callback function is invoked during the wakeup process initiated by the wakeup button ISR.
             * It allows the user to define custom behavior upon waking up the MCU.
             * 
             * @note If no callback is assigned (`nullptr`), no user-defined action will be executed.
             */
            void (*button_wakeup_user_callback_)() = nullptr;

            /**
             * @brief Pointer to a user-defined callback function executed when the @ref PIN_USB_IO3 pin triggers an MCU wakeup.
             * 
             * This callback function is invoked during the wakeup process initiated by the @ref PIN_USB_IO3 PCINT ISR.
             * It allows the user to define custom behavior upon waking up the MCU.
             * 
             * @note If no callback is assigned (`nullptr`), no user-defined action will be executed.
             */
            void (*io3_wakeup_user_callback_)() = nullptr;

            /**
             * @brief Interrupt Service Routine (ISR) executed when the wakeup button triggers an MCU wakeup.
             * 
             * This ISR is called when the wakeup button (on @ref PIN_BUTTON_WAKEUP pin) generates an external interrupt (INT0).
             * It detaches the interrupt to prevent further triggering during execution, and if a user-defined
             * callback is assigned via @ref button_wakeup_user_callback_, it will be executed.
             * 
             * @note This function is inactive and performs no operations if @ref AVR_DEBUG is defined.
             */
            static void button_wakeup_isr();

            /**
             * @brief Interrupt Service Routine (ISR) executed when the  @ref PIN_USB_IO3 pin triggers an MCU wakeup.
             * 
             * This ISR is called when a pin change interrupt (PCINT20) on the  @ref PIN_USB_IO3 pin wakes up the MCU.
             * It detaches the interrupt to prevent further triggering during execution, and if a user-defined
             * callback is assigned via @ref io3_wakeup_user_callback_, it will be executed.
             * 
             * @note This function is inactive and performs no operations if @ref AVR_DEBUG is defined.
             */
            static void usb_io3_wakeup_isr();

            /**
             * @brief Internal flag indicating if the wakeup button interrupt service routine (ISR) was triggered.
             * 
             * This flag is set to `true` when the wakeup button ISR is executed. It is cleared automatically 
             * after the wakeup process completes. Used internally for precise ISR state tracking.
             */
            volatile bool isr_wakeup_button_flag_internal_ = false;

            /**
             * @brief Internal flag indicating if the IO3 interrupt service routine (ISR) was triggered.
             * 
             * This flag is set to `true` when the IO3 ISR is executed. It is cleared automatically 
             * after the wakeup process completes. Used internally for precise ISR state tracking.
             */
            volatile bool isr_wakeup_io3_flag_internal_ = false;

            /**
             * @brief External flag indicating if the wakeup button ISR was triggered.
             * 
             * This flag is accessible externally to check if the wakeup button ISR has been executed. 
             * It must be cleared explicitly by the application after processing the wakeup event.
             */
            volatile bool isr_wakeup_button_flag_ = false;

            /**
             * @brief External flag indicating if the IO3 ISR was triggered.
             * 
             * This flag is accessible externally to check if the IO3 ISR has been executed. 
             * It must be cleared explicitly by the application after processing the wakeup event.
             */
            volatile bool isr_wakeup_io3_flag_ = false;

            /**
             * @brief Private instance of the @ref eeprom::EEPROMDataManager.
             * 
             * Manages the storage and retrieval of configuration data from EEPROM. 
             * This private instance is used internally by the @ref UIRB class to handle 
             * persistent settings such as hardware version, calibration data, and user-defined preferences.
             * 
             * @details The @ref eeprom::EEPROMDataManager class ensures reliable read/write operations 
             * to the EEPROM and validates data consistency. This instance is initialized 
             * during the construction of the @ref UIRB class.
             */
            eeprom::EEPROMDataManager eepromDataManager_ = eeprom::EEPROMDataManager();

            /**
             * @brief Minimum supply voltage in millivolts for 8 MHz operation.
             * 
             * Specifies the minimum supply voltage required for stable operation of the microcontroller at an 8 MHz clock frequency. 
             * According to the ATMega328P datasheet, this threshold is 2.7V.
             * 
             * @note Operating below this voltage may lead to unpredictable behavior or malfunction.
             */
            static constexpr uint16_t AVCC_MILIVOLTS_8MHZ_MIN = 2700U;

            /**
             * @brief Maximum recommended supply voltage in millivolts.
             * 
             * Represents the highest supply voltage recommended for reliable operation, as specified in the ATMega328P datasheet. 
             * The typical upper limit is 5.5V.
             * 
             * @note Operating beyond this value may cause damage to the microcontroller.
             */
            static constexpr uint16_t AVCC_MILIVOLTS_MAX = 5500U;

            /**
             * @brief Absolute maximum supply voltage in millivolts.
             * 
             * Indicates the absolute maximum voltage the microcontroller can tolerate without damage, as stated in the ATMega328P datasheet.
             * The typical absolute maximum value is 6.0V.
             * 
             * @warning Exceeding this voltage can cause permanent damage to the device.
             */
            static constexpr uint16_t AVCC_MILIVOLTS_ABSOLUTE_MAX = 6000U;
            
            /**
             * @brief ADC resolution in decimal representation.
             * 
             * Defines the resolution of the ADC as a 10-bit converter, which corresponds to \f$ 2^{10} = 1024 \f$ discrete steps. 
             * Each step represents \f$ 1/1024 \f$ of the reference voltage.
             */
            static constexpr uint16_t ADC_RESOLUTION_DEC = 1024U;

            /**
             * @brief Maximum valid ADC sample value.
             * 
             * Represents the highest possible value for a 10-bit ADC sample, which is @ref ADC_RESOLUTION_DEC - 1 (i.e., 1023).
             * 
             * @note This value is commonly used as the upper bound for ADC readings when interpreting analog input values.
             */
            static constexpr uint16_t ADC_SAMPLE_MAX = ADC_RESOLUTION_DEC - 1U;

            /**
             * @brief Minimum valid ADC sample value for the internal bandgap reference with `AVcc` as the reference voltage.
             * 
             * This constant defines the lower threshold for valid ADC readings when measuring the internal bandgap 
             * reference voltage using `AVcc` as the ADC reference. Values below this threshold are considered invalid and may 
             * indicate a hardware or measurement error.
             * 
             * @note This threshold is specific to configurations where `AVcc` is used as the reference voltage for ADC operations.
             */
            static constexpr uint8_t ADC_BANDGAP_AVCC_SAMPLE_MIN = 160U;

            /**
             * @brief Delay in milliseconds to allow the ADC reference voltage to stabilize after a change.
             * 
             * This constant defines the minimum recommended wait time to ensure 
             * the ADC reference voltage has fully settled after switching to a new reference. This delay 
             * is critical for achieving accurate ADC conversions and avoiding transient measurement errors.
             * 
             * @note This delay is applicable whenever the ADC reference voltage is changed, such as switching 
             * between internal and external references.
             */
            static constexpr uint8_t ADC_VREF_SETTLE_DELAY_MS = 5U;

            /**
             * @brief Delay in milliseconds between consecutive ADC samples.
             * 
             * This constant specifies the time interval to wait between ADC readings. 
             * Introducing this delay ensures that the ADC has sufficient time to stabilize between samples, 
             * minimizing measurement noise and improving accuracy.
             * 
             * @note This delay is particularly important in scenarios where rapid successive ADC reads could 
             * result in erroneous data due to insufficient stabilization time.
             */
            static constexpr uint8_t ADC_SAMPLE_DELAY_MS = 5U;

            /**
             * @brief Maximum valid voltage of the charger's @ref PIN_PROG pin in constant current (CC) mode, in millivolts.
             * 
             * This constant defines the upper threshold (\f$1100 \, \text{mV}\f$) for the @ref PIN_PROG pin voltage 
             * required to maintain the charger in constant current (CC) mode. Based on the NanJing Top Power ASIC Corp. TP4057 datasheet:
             * 
             * - The typical \f$ V_{\text{prog}} \f$ range for CC mode is between \f$ 0.9 \, \text{V} \f$ and \f$ 1.1 \, \text{V} \f$, 
             *   with \f$ 1.0 \, \text{V} \f$ as the nominal value.
             * - \f$ V_{\text{prog}} \f$ is internally limited to \f$ 1.1 \, \text{V} \f$ when a resistor is connected.
             * - Any voltage above \f$ 1.1 \, \text{V} \f$ should result in a charging current of \f$ 0 \, \text{mA} \f$, as determined by the internal comparator (CA opamp).
             * - If \f$ V_{\text{prog}} \f$ exceeds \f$ 1.22 \, \text{V} \f$, the charger enters a shutdown state, as inferred from the internal block diagram.
             * 
             * @note 
             * - The shutdown comparator (\f$ C1 \f$) appears to compare \f$ V_{\text{prog}} \f$ against a reference voltage 
             *   (\f$ 1.22 \, \text{V} \f$ or possibly \f$ 2.7 \, \text{V} \f$ based on datasheet ambiguity).
             * - This threshold ensures the charger transitions reliably between CC mode and shutdown, maintaining safe operation.
             * 
             * @see @ref UIRB::PROG_CC_CHARGE_VOLTAGE_MIN_MV for the lower limit of CC mode.
             */
            static constexpr uint16_t PROG_CC_CHARGE_VOLTAGE_MAX_MV = 1100U;

            /**
             * @brief Minimum valid voltage of the charger's @ref PIN_PROG pin in constant current (CC) mode, in millivolts.
             * 
             * This constant defines the lower threshold (\f$900 \, \text{mV}\f$) for the @ref PIN_PROG pin voltage required to maintain 
             * the charger in constant current (CC) mode. Based on the NanJing Top Power ASIC Corp. TP4057 datasheet:
             * 
             * - \f$ V_{\text{prog}} \f$ range for CC mode is typically between \f$ 0.9 \, \text{V} \f$ and \f$ 1.1 \, \text{V} \f$, with \f$ 1.0 \, \text{V} \f$ as the nominal value.
             * - When \f$ V_{\text{prog}} \f$ falls below \f$ 0.9 \, \text{V} \f$, the charger transitions to constant voltage (CV) mode.
             * 
             * This value is crucial for accurately detecting the operating mode of the charger and ensuring proper battery charging behavior.
             * 
             * @see @ref UIRB::PROG_CC_CHARGE_VOLTAGE_MAX_MV for the upper limit of CC mode.
             */
            static constexpr uint16_t PROG_CC_CHARGE_VOLTAGE_MIN_MV = 900U;

            /**
             * @brief Minimum voltage of the @ref PIN_PROG pin required for constant voltage (CV) mode, in millivolts.
             * 
             * This constant defines the threshold voltage (\f$100 \, \text{mV}\f$) below which the charger transitions to float mode
             * (no active charging). The value is based on the NanJing Top Power ASIC Corp. TP4057 datasheet, with the following details:
             * 
             * - Typical \f$ V_{\text{prog}} \f$ range for constant current (CC) mode: [\f$ 0.9 \, \text{V} \f$ - \f$ 1.1 \, \text{V} \f$].
             * - The internal comparator for float mode operates at approximately 1/10th of this range, setting a threshold of [\f$ 90 \, \text{mV} \f$ - \f$ 110 \, \text{mV} \f$].
             * - A typical reference value of \f$ 100 \, \text{mV} \f$ is chosen for accurate operation.
             * 
             * This value is critical for distinguishing float mode (no charging) from active charging in CV mode, ensuring the charger
             * operates reliably and maintains the battery in an optimal state.
             * 
             * @see @ref UIRB::PROG_CC_CHARGE_VOLTAGE_MIN_MV for the minimum voltage required for constant current mode.
             * @see @ref UIRB::PROG_CC_CHARGE_VOLTAGE_MAX_MV for the maximum voltage allowable for constant current mode.
             */
            static constexpr uint8_t PROG_CV_CHARGE_VOLTAGE_MIN_MV = 100U;

            /**
             * @brief Maximum voltage of @ref PIN_PROG for floating mode, in millivolts.
             * 
             * Specifies the threshold voltage (\f$15 \, \text{mV}\f$) below which the charger enters floating mode, 
             * indicating no charging current. This value is measured experimentally using a multimeter and represents 
             * the maximum voltage on the @ref PIN_PROG pin in this state.
             */
            static constexpr uint8_t PROG_FLOAT_VOLTAGE_MAX_MV = 15U;

            /**
             * @brief Floating supply voltage for a fully charged Li-ion battery, in millivolts.
             * 
             * This constant is defined based on @ref UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS and represents 
             * the typical constant voltage (CV) level during the final stage of charging. The value is 
             * derived from the NanJing Top Power ASIC Corp. TP4057 datasheet and ensures safe and reliable 
             * charging behavior.
             * 
             * - Float voltage (`Vfloat`) range: 4158 mV to 4242 mV
             * - Typical float voltage: 4200 mV
             * 
             * Using this value prevents overcharging, which could harm the battery or reduce its lifespan. 
             * It is critical for maintaining safe and optimal charging performance.
             * 
             * @note Adjustments to this value should only be made if the charging IC or application-specific 
             * requirements demand a different constant voltage.
             */
            static constexpr uint16_t FULLY_CHARGED_SUPPLY_VOLTAGE_MIN_MV = UIRB_CORE_FULLY_CHARGED_VOLTAGE_MILIVOLTS;

            /**
             * @brief Defines the voltage hysteresis between float voltage (\f$V_{\text{float}}\f$) and recharge voltage (\f$V_{\text{recharge}}\f$) in millivolts.
             * 
             * This constant specifies the voltage difference that determines when the battery transitions from float mode 
             * back to active charging. Derived from the TP4057 datasheet, it represents the range between the fully charged 
             * float voltage and the voltage threshold for initiating a recharge cycle.
             * 
             * **Formula:**
             * \f[
             * \Delta V_{\text{recharge}} = V_{\text{float}} - V_{\text{recharge}}
             * \f]
             * 
             * **Key Parameters:**
             * - \f$V_{\text{float}}\f$: Fully charged float voltage, defined as @ref UIRB::FULLY_CHARGED_SUPPLY_VOLTAGE_MIN_MV.
             * - \f$V_{\text{recharge}}\f$: Recharge threshold voltage, which is calculated using @ref UIRB::FLOAT_VOLTAGE_RECHARGE_MIN_MV.
             * - \f$\Delta V_{\text{recharge}}\f$: Hysteresis voltage difference, typically 150 mV, but can range from 100 mV to 200 mV.
             * 
             * **Default Value:**
             * - Typical hysteresis: `150` mV.
             */
            static constexpr uint8_t FLOAT_VOLTAGE_RECHARGE_DELTA_MV = 150U;

            /**
             * @brief Defines the minimum supply voltage for float mode before initiating a recharge cycle.
             * 
             * This constant represents the threshold voltage below which the battery is considered to require recharging, 
             * based on the characteristics of the TP4057 charging IC. The calculation is derived from the datasheet and 
             * ensures the battery is maintained within safe operating limits to maximize longevity and performance.
             * 
             * **Formula:**
             * \f[
             * V_{\text{recharge}} = V_{\text{float}} - \Delta V_{\text{recharge}}
             * \f]
             * 
             * Where:
             * - \f$V_{\text{float}}\f$ is the typical float voltage of a fully charged Li-ion battery, 
             *   defined as @ref UIRB::FULLY_CHARGED_SUPPLY_VOLTAGE_MIN_MV.
             * - \f$\Delta V_{\text{recharge}}\f$ is the hysteresis voltage difference, defined as 
             *   @ref UIRB::FLOAT_VOLTAGE_RECHARGE_DELTA_MV, which typically ranges from 100 mV to 200 mV (typical: 150 mV).
             */
            static constexpr uint16_t FLOAT_VOLTAGE_RECHARGE_MIN_MV = FULLY_CHARGED_SUPPLY_VOLTAGE_MIN_MV - FLOAT_VOLTAGE_RECHARGE_DELTA_MV;

            /**
             * @brief Minimum supply voltage indicating a nearly empty battery, in millivolts.
             * 
             * This constant is defined based on @ref UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS and represents
             * the threshold below which the battery is considered nearly empty. For most Li-ion batteries,
             * 3400 mV corresponds to roughly 15%-20% remaining capacity, ensuring safe operation without
             * over-discharging. Adjusting this value impacts the behavior of low-battery indicators or
             * power-saving measures.
             * 
             * @note Over-discharging Li-ion batteries can lead to capacity degradation or permanent damage.
             */
            static constexpr uint16_t BATTERY_EMPTY_SUPPLY_VOLTAGE_MIN_MV = UIRB_CORE_LOW_BATTERY_VOLTAGE_MILIVOLTS;
            
    };
}

#endif  // UIRBcore_hpp