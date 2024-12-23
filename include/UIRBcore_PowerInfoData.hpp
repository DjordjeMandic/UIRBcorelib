/**
 * @file UIRBcore_PowerInfoData.hpp
 * @brief Power monitoring and battery/charger state management for the %UIRB system.
 *
 * This header file defines the @ref uirbcore::PowerInfoData class and supporting enumerations to 
 * manage power-related metrics, including:
 * - **Supply voltage monitoring**: Measure and validate the system's supply voltage.
 * - **Charging current estimation**: Analyze the charging current using hardware feedback.
 * - **Battery and charger state evaluation**: Identify key states such as charging, 
 *   fully charged, floating, and low battery conditions.
 *
 * @note Integration requires the @ref uirbcore::UIRB class defined in @ref UIRBcore.hpp 
 * for hardware-level measurements and updates.
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
#ifndef UIRBcore_PowerInfoData_hpp
#define UIRBcore_PowerInfoData_hpp

#include <Arduino.h>

namespace uirbcore
{
    /**
     * @brief Enum class representing the states of a battery charger as `uint8_t`.
     * 
     * This enumeration defines the possible states of a battery charger, including 
     * charging modes, idle states, and error conditions. It provides a standardized 
     * way to interpret and handle charger behavior in the application.
     */
    enum class ChargerState : uint8_t
    {
        ERROR = 0, /**< Indicates an error state, typically returned when the @ref PowerInfoData class cannot determine a valid state. */
        UNKNOWN, /**< The charger state is unknown, usually when it cannot be determined reliably. */
        CHARGING_CC, /**< Charging in constant current (CC) mode. This is the initial phase of Li-ion battery charging. */
        CHARGING_CV, /**< Charging in constant voltage (CV) mode. This is the final phase where the voltage is held constant. */
        FLOATING, /**< The charger is not actively charging but is floating and monitoring the battery for a recharge threshold. */
        TURNED_OFF /**< The charger is turned off, typically by pulling the @ref PIN_PROG pin above the constant current (@ref UIRB::PROG_CC_CHARGE_VOLTAGE_MAX_MV) threshold of TP4057. */
    };

    /**
     * @brief Logical NOT operator to determine if the charger is not actively charging.
     * 
     * This operator checks the @ref ChargerState to determine whether the charger is in a 
     * non-charging state. It returns `true` if the charger is not in `ChargerState::CHARGING_CC` or 
     * `ChargerState::CHARGING_CV` mode, indicating the charger is either idle, floating, turned off, 
     * or in an error/unknown state.
     * 
     * @param[in] result The #ChargerState value to evaluate.
     * @return bool
     * @retval true The charger is not actively charging (not in `ChargerState::CHARGING_CC` or `ChargerState::CHARGING_CV` mode).
     * @retval false The charger is actively charging (`ChargerState::CHARGING_CC` or `ChargerState::CHARGING_CV`).
     * 
     * @details
     * This operator provides a convenient way to check for non-charging states 
     * without explicitly comparing against multiple @ref ChargerState values. It is 
     * particularly useful in conditional statements for handling charger states.
     * 
     * Example usage:
     * @code
     * ChargerState state = UIRB::getInstance().getPowerInfo().getChargerState();
     * if (!state) {
     *     // Perform actions when the charger is not charging
     * }
     * @endcode
     */
    inline bool operator!(ChargerState result)
    {
        return result != ChargerState::CHARGING_CC &&
            result != ChargerState::CHARGING_CV;
    }

    /**
     * @brief Enum class representing the states of a battery as `uint8_t`.
     * 
     * This enumeration defines the possible states of a battery, including error states, 
     * charge levels, and charging behavior. It provides a standardized way to represent 
     * and evaluate battery conditions.
     */
    enum class BatteryState : uint8_t
    {
        ERROR = 0, /**< Indicates an error state, typically returned when the @ref PowerInfoData class cannot determine a valid state. */
        UNKNOWN, /**< The battery state is unknown, usually when it cannot be reliably determined. */
        EMPTY, /**< Indicates the battery is completely empty. */
        NOT_CHARGING, /**< The battery is neither empty nor full and is not currently charging. */
        CHARGING, /**< The battery is being charged. */
        FULLY_CHARGED /**< The battery is fully charged and ready for use. */
    };

    /**
     * @brief Logical NOT operator to check if the battery is considered empty.
     * 
     * This operator evaluates the given @ref BatteryState and determines whether the battery 
     * should be considered empty. It returns `true` for states `BatteryState::ERROR`, `BatteryState::UNKNOWN`, or `BatteryState::EMPTY`, 
     * indicating that the battery is not in a usable or charging state.
     * 
     * @param[in] result The #BatteryState to evaluate.
     * @return bool
     * @retval true The battery is in one of the following states: `BatteryState::ERROR`, `BatteryState::UNKNOWN`, or `BatteryState::EMPTY`.
     * @retval false The battery is not empty.
     * 
     * @details
     * This operator is particularly useful for simplifying battery state checks in conditional 
     * statements, allowing for concise evaluation of whether the battery is empty or unusable.
     * 
     * Example usage:
     * @code
     * BatteryState state = UIRB::getInstance().getPowerInfo().getBatteryState();
     * if (!state) {
     *     // Handle empty or error states
     * }
     * @endcode
     */
    inline bool operator!(BatteryState result)
    {
        return result == BatteryState::ERROR   ||
            result == BatteryState::UNKNOWN ||
            result == BatteryState::EMPTY;
    }

    /**
     * @brief Forward declaration of the @ref UIRB class.
     * 
     */
    class UIRB;

    /**
     * @brief A class for retrieving and managing power-related data provided by the @ref UIRB class.
     * 
     * The @ref PowerInfoData class serves as an interface to gather, process, and store information about 
     * the power system, including supply voltage, @ref PIN_PROG pin voltage, charging current, and the estimated 
     * states of the battery and charger. It provides utility functions for converting and interpreting 
     * raw voltage and current data, ensuring that the power system operates within safe and expected parameters.
     * 
     * @details
     * - The class relies on the @ref UIRB class for hardware-level measurements and initialization.
     * - It processes raw data to calculate meaningful metrics like charging current in milliamps or supply 
     *   voltage in volts.
     * - The class also evaluates the charger and battery states using predefined thresholds and system conditions.
     * 
     * **Key Features:**
     * - Sampling and validation of supply voltage, @ref PIN_PROG voltage, and charging current.
     * - Conversion utilities for voltage and current values.
     * - Functions to check battery and charger status, such as whether the battery is full, charging, or low.
     * - Support for detecting invalid or uninitialized states.
     * 
     * @note
     * - The @ref PowerInfoData class assumes that the system is periodically updated with accurate hardware data.
     * - Some functions depend on specific resistor values or configurations in the circuit (e.g., `PROG` resistor).
     * 
     * @see @ref UIRB for hardware-level details and dependencies.
     * @see @ref PowerInfoData::update(uint8_t) for the primary method to refresh sampled data.
     */
    class PowerInfoData
    {
        public:
            /**
             * @brief Updates the sampled data using the @ref UIRB class.
             * 
             * This function retrieves the latest supply voltage, @ref PIN_PROG pin voltage, pin mode, 
             * pin state, and estimated charging current. It validates the sampled data to ensure 
             * it is within acceptable ranges.
             * 
             * @param[in] samples The number of samples to take during measurement. Defaults to `5`.
             * @return bool Indicates if the sampled data is valid.
             * @retval true Sampled data is valid.
             * @retval false Sampled data is invalid, @ref UIRB class initialization failed, or @p samples is `0`.
             * 
             * @note The sampled data includes supply voltage, @ref PIN_PROG pin voltage, and estimated charging current.
             *       If data is valid, the estimated charger and battery states are also updated.
             * 
             * @see @ref PowerInfoData::isValid() For checking if the sampled data is valid after calling @ref PowerInfoData::update().
             * @see @ref UIRB::getSupplyVoltageMilivolts() For retrieving supply voltage values used in sampling.
             * @see @ref UIRB::getProgVoltageMilivolts() For retrieving @ref PIN_PROG pin voltage values used in sampling.
             * @see @ref PowerInfoData::prog_milivolts_to_charging_current_miliamps(const uint16_t, const uint16_t, const uint8_t, const bool)
             *      For calculating charging current based on the sampled @ref PIN_PROG pin voltage and other parameters.
             */
            bool update(uint8_t samples = 5);

            /**
             * @brief Checks if the sampled data is valid.
             * 
             * @return bool Indicates if the sampled data is valid.
             * @retval true Sampled data is valid.
             * @retval false Sampled data is invalid.
             * 
             * @details The sampled data is considered valid if:
             * - Supply voltage is not @ref UIRB::INVALID_VOLTAGE_MILIVOLTS.
             * - @ref PIN_PROG pin voltage is not @ref UIRB::INVALID_VOLTAGE_MILIVOLTS.
             * - Charging current is not @ref UIRB::INVALID_CURRENT_MILIAMPS.
             * - @ref PIN_PROG pin mode is not @ref INVALID_PIN_MODE.
             * 
             * @see @ref PowerInfoData::update(uint8_t) For refreshing the sampled data before validation.
             * @see @ref UIRB::INVALID_VOLTAGE_MILIVOLTS For identifying invalid voltage values during validation.
             * @see @ref UIRB::INVALID_CURRENT_MILIAMPS For identifying invalid current values during validation.
             * @see @ref INVALID_PIN_MODE For identifying invalid pin modes during validation.
             */
            bool isValid() const;

            /**
             * @brief Retrieves the voltage on the @ref PIN_PROG pin in volts.
             * 
             * @return float Voltage on the @ref PIN_PROG pin in volts.
             * @retval NAN Voltage data is invalid.
             * 
             * @see @ref PowerInfoData::prog_milivolts_to_volts(const uint16_t) For converting @ref PIN_PROG pin voltage to volts.
             * @see @ref UIRB::getProgVoltageMilivolts(const uint8_t) For retrieving the raw @ref PIN_PROG pin voltage in millivolts.
             */
            float getProgVoltage() const;

            /**
             * @brief Retrieves the charging current in amps.
             * 
             * @return float Charging current in amps.
             * @retval NAN Charging current data is invalid.
             * 
             * @see @ref PowerInfoData::charging_current_miliamps_to_amps(const uint16_t) For converting charging current to amps.
             * @see @ref PowerInfoData::prog_milivolts_to_charging_current_miliamps(const uint16_t, const uint16_t, const uint8_t, const bool)
             *      For calculating charging current based on sampled data.
             * @see @ref UIRB::getProgVoltageMilivolts(const uint8_t) For retrieving the @ref PIN_PROG pin voltage used in current calculations.
             */
            float getChargingCurrent() const;

            /**
             * @brief Retrieves the supply voltage in volts.
             * 
             * @return float Supply voltage in volts.
             * @retval NAN Supply voltage data is invalid.
             * @retval INFINITY Supply voltage exceeds the maximum allowable value.
             * @retval -INFINITY Supply voltage is below the minimum required for 8MHz operation.
             * 
             * @see @ref PowerInfoData::avcc_milivolts_to_volts(const uint16_t) For converting supply voltage to volts.
             * @see @ref UIRB::getSupplyVoltageMilivolts(const uint8_t) For retrieving the raw supply voltage in millivolts.
             */
            float getSupplyVoltage() const;

            /**
             * @brief Checks if the battery is low based on the supply voltage.
             * 
             * @param[in] flashSTATOnLowBattery If `true`, flashes the status indicator on low battery. Defaults to `false`.
             * @return bool Indicates if the battery is low.
             * @retval true Battery is empty, supply voltage is below the low-battery threshold, or supply voltage data is invalid.
             * @retval false Battery is not empty, supply voltage is at or above the low-battery threshold.
             * 
             * @see @ref UIRB::BATTERY_EMPTY_SUPPLY_VOLTAGE_MIN_MV For the low-battery threshold used in checks.
             * @see @ref UIRB::getSupplyVoltageMilivolts(const uint8_t) For retrieving supply voltage for comparison.
             * @see @ref UIRB::notifyStatusLowBattery() For flashing the status indicator on low battery.
             * 
             * @note If @p flashSTATOnLowBattery is `true` and the battery is low, the @ref UIRB::notifyStatusLowBattery() 
             *       function is called to indicate the condition.
             */
            bool isBatteryLow(const bool flashSTATOnLowBattery = false) const;

            /**
             * @brief Checks if the battery is fully charged based on the supply voltage.
             * 
             * @return bool Indicates if the battery is fully charged.
             * @retval true Battery is fully charged; supply voltage is at or above the full-battery threshold.
             * @retval false Battery is not fully charged; supply voltage is below the threshold or invalid.
             * 
             * @see @ref UIRB::FULLY_CHARGED_SUPPLY_VOLTAGE_MIN_MV For the fully charged threshold used in checks.
             * @see @ref UIRB::getSupplyVoltageMilivolts(const uint8_t) For retrieving supply voltage for comparison.
             */
            bool isBatteryFull() const;

            /**
             * @brief Checks if the charger is actively charging the battery.
             * 
             * @return bool Indicates if the charger is actively charging the battery.
             * @retval true #ChargerState is in constant current (`ChargerState::CHARGING_CC`) or constant voltage (`ChargerState::CHARGING_CV`) mode.
             * @retval false #ChargerState is not actively charging.
             * 
             * @see @ref PowerInfoData::getChargerState() For retrieving the current charger state used in checks.
             * @see #ChargerState For all possible charger states and their meanings.
             */
            bool isBatteryCharging() const;

            /**
             * @brief Retrieves the estimated state of the battery.
             * 
             * This function estimates the current state of the battery based on the supply voltage 
             * and the estimated charger state. The function evaluates whether the 
             * battery is actively charging, fully charged, low, not charging, or in an error state.
             * 
             * @return #BatteryState The estimated battery state. Possible values include:
             * @retval #BatteryState::CHARGING The battery is actively charging.
             * @retval #BatteryState::FULLY_CHARGED The battery is fully charged.
             * @retval #BatteryState::EMPTY The battery is low and needs recharging.
             * @retval #BatteryState::NOT_CHARGING The battery is not being charged but is not full.
             * @retval #BatteryState::ERROR An error occurred due to invalid inputs or system conditions.
             * @retval #BatteryState::UNKNOWN The battery state cannot be determined.
             * 
             * @see @ref operator!(BatteryState) For checking if the battery state represents an empty, error, or unknown condition.
             * @see @ref PowerInfoData::get_estimated_battery_state() For details on how the battery state is calculated.
             * @see @ref PowerInfoData::isBatteryCharging() For identifying if the battery is actively charging.
             * @see @ref PowerInfoData::isBatteryFull() For checking if the battery is fully charged.
             * @see @ref PowerInfoData::isBatteryLow() For checking if the battery is low.
             * @see #BatteryState For all possible battery states and their meanings.
             * 
             * @note This function assumes that the system periodically updates the input variables 
             *       to reflect the current conditions accurately.
             */
            BatteryState getBatteryState() const;

            /**
             * @brief Retrieves the estimated state of the charger.
             * 
             * This function estimates the current state of the charger based on the supply voltage,
             * @ref PIN_PROG pin voltage, and charging current. It evaluates whether the charger 
             * is actively charging, floating, turned off, or in an error/unknown state.
             * 
             * @return #ChargerState The estimated charger state. Possible values include:
             * @retval #ChargerState::CHARGING_CC The charger is in constant current mode.
             * @retval #ChargerState::CHARGING_CV The charger is in constant voltage mode.
             * @retval #ChargerState::FLOATING The charger is idle and maintaining the battery at a float voltage.
             * @retval #ChargerState::TURNED_OFF The charger is turned off.
             * @retval #ChargerState::UNKNOWN The charger state cannot be determined.
             * @retval #ChargerState::ERROR An error occurred due to invalid inputs or system conditions.
             * 
             * @see @ref operator!(ChargerState) For checking if the charger is not actively charging (i.e., not in constant current or constant voltage mode).
             * @see @ref PowerInfoData::get_estimated_charger_state() For details on how the charger state is calculated.
             * @see @ref PowerInfoData::getSupplyVoltage() For retrieving supply voltage used in charger state determination.
             * @see @ref PowerInfoData::getProgVoltage() For retrieving @ref PIN_PROG pin voltage used in charger state determination.
             * @see @ref PowerInfoData::getChargingCurrent() For retrieving charging current used in charger state determination.
             * @see #ChargerState For all possible charger states and their meanings.
             * 
             * @note This function assumes that the system periodically updates the input variables 
             *       to reflect the current conditions accurately.
             */
            ChargerState getChargerState() const;

        private:
            /**
             * @brief Friend class providing access to internal data and methods of this class.
             */
            friend class UIRB;

            /**
             * @brief Supply voltage in millivolts measured on the `AVcc` MCU pin.
             * 
             * Represents the battery voltage minus the voltage drop across the power switch MOSFET.
             * 
             * @details
             * - This value is typically used to estimate the battery voltage and ensure the system operates 
             *   within the recommended voltage range.
             * - The voltage is measured using @ref UIRB::getSupplyVoltageMilivolts(const uint8_t) and stored here.
             * 
             * @note The initial value is set to `UINT16_MAX` to indicate an uninitialized state.
             */
            uint16_t supply_voltage_milivolts_ = UINT16_MAX;

            /**
             * @brief Voltage on the @ref PIN_PROG pin in millivolts, measured after the RC low-pass filter.
             * 
             * Represents the charger’s @ref PIN_PROG pin voltage if the pin A1 is in a high-impedance state.
             * 
             * @details
             * - The value is obtained using @ref UIRB::getProgVoltageMilivolts(const uint8_t) and reflects 
             *   the charging control pin's voltage after filtering.
             * 
             * @note The initial value is set to `UINT16_MAX` to indicate an uninitialized state.
             */
            uint16_t prog_voltage_milivolts_ = UINT16_MAX;

            /**
             * @brief Estimated charging current in milliamps.
             * 
             * Represents the calculated charging current based on the voltage at the @ref PIN_PROG pin 
             * and the resistor value of the @ref PIN_PROG pin circuit.
             * 
             * @details
             * - This value is computed using @ref PowerInfoData::prog_milivolts_to_charging_current_miliamps(const uint16_t, const uint16_t, const uint8_t, const bool).
             * - The calculation accounts for the pin's mode and state as well as the RC circuit.
             * 
             * @note The initial value is set to `UINT16_MAX` to indicate an uninitialized state.
             */
            uint16_t charging_current_miliamps_ = UINT16_MAX;

            /**
             * @brief Pin mode of the MCU pin used for `Vprog` measurement.
             * 
             * Represents the mode of the pin used to measure the @ref PIN_PROG pin voltage. 
             * Typical values are `INPUT`, `OUTPUT`, or `INPUT_PULLUP`.
             * 
             * @note The initial value is set to `0xFF` @ref INVALID_PIN_MODE to indicate an uninitialized state.
             */
            uint8_t prog_pin_mode_ = 0xFFU;

            /**
             * @brief Digital state of the MCU pin used for `Vprog` measurement.
             * 
             * Represents the current digital state of the pin used to measure the @ref PIN_PROG pin voltage 
             * (e.g., HIGH or LOW).
             * 
             * @note The initial value is `false`, indicating a LOW state.
             */
            bool prog_pin_state_ = false;

            /**
             * @brief Represents the estimated state of the charger.
             * 
             * This variable holds the estimated current state of the charger, such as whether it is charging, 
             * floating, turned off, or in an error state. The initial value is set to `ChargerState::ERROR` 
             * to indicate that the charger state has not yet been determined or initialized.
             * 
             * @details
             * - The charger state is determined based on inputs such as the supply voltage, 
             *   charging current, and system conditions.
             * - Possible values are defined in the @ref ChargerState enum, including:
             *   - `ChargerState::CHARGING_CC`: Charging in constant current mode.
             *   - `ChargerState::CHARGING_CV`: Charging in constant voltage mode.
             *   - `ChargerState::FLOATING`: Charger is idle, waiting for a recharge threshold.
             *   - `ChargerState::TURNED_OFF`: Charger is turned off (e.g., manually or due to conditions).
             *   - `ChargerState::ERROR`: An error occurred or the state is indeterminate.
             * 
             * @note The charger state should be periodically updated by the system to reflect the 
             *       current charging status accurately.
             */
            ChargerState estimated_charger_state_ = ChargerState::ERROR;

            /**
             * @brief Retrieves the estimated charger state based on supply voltage, @ref PIN_PROG pin voltage, and charging current.
             * 
             * This function estimates the current state of the charger by analyzing the supply voltage (@ref PowerInfoData::supply_voltage_milivolts_), 
             * @ref PIN_PROG pin voltage (@ref PowerInfoData::prog_voltage_milivolts_), and charging current (@ref PowerInfoData::charging_current_miliamps_). The estimated state 
             * includes various charging modes (e.g., constant current, constant voltage), floating state, and error conditions.
             * 
             * @return #ChargerState The estimated charger state. Possible values include:
             * @retval #ChargerState::CHARGING_CC The charger is in constant current mode.
             * @retval #ChargerState::CHARGING_CV The charger is in constant voltage mode.
             * @retval #ChargerState::FLOATING The charger is idle and waiting for a recharge threshold.
             * @retval #ChargerState::TURNED_OFF The charger is turned off.
             * @retval #ChargerState::UNKNOWN The charger state cannot be determined.
             * @retval #ChargerState::ERROR An error occurred due to invalid inputs.
             * 
             * @details
             * - **Input Validation:**
             *   - The function returns `ChargerState::ERROR` if any of the following conditions are met:
             *     - @ref PowerInfoData::charging_current_miliamps_ == @ref UIRB::INVALID_CURRENT_MILIAMPS
             *     - @ref PowerInfoData::prog_voltage_milivolts_ == @ref UIRB::INVALID_VOLTAGE_MILIVOLTS
             *     - @ref PowerInfoData::supply_voltage_milivolts_ == @ref UIRB::INVALID_VOLTAGE_MILIVOLTS
             *   - If the charging current is @ref UIRB::UNKNOWN_CURRENT_MILIAMPS, the state is `ChargerState::UNKNOWN`.
             * 
             * - **Charger State Determination:**
             *   1. **Turned Off**: If the charging current is `0`, the state is `ChargerState::TURNED_OFF`.
             *   2. **Constant Current Mode**: 
             *      - The @ref PIN_PROG pin voltage is between @ref UIRB::PROG_CC_CHARGE_VOLTAGE_MIN_MV and @ref UIRB::PROG_CC_CHARGE_VOLTAGE_MAX_MV.
             *      - The supply voltage must be less than @ref UIRB::FULLY_CHARGED_SUPPLY_VOLTAGE_MIN_MV.
             *   3. **Constant Voltage Mode**: 
             *      - The @ref PIN_PROG pin voltage is greater than or equal to @ref UIRB::PROG_CV_CHARGE_VOLTAGE_MIN_MV.
             *      - The supply voltage must exceed @ref UIRB::FLOAT_VOLTAGE_RECHARGE_MIN_MV.
             *   4. **Floating Mode**: 
             *      - The supply voltage is greater than or equal to @ref UIRB::FLOAT_VOLTAGE_RECHARGE_MIN_MV.
             *   5. **Unknown State**: If none of the above conditions are met, the state is `ChargerState::UNKNOWN`.
             * 
             * @note 
             * - This function assumes that the system periodically updates @ref PowerInfoData::supply_voltage_milivolts_, 
             *   @ref PowerInfoData::prog_voltage_milivolts_, and @ref PowerInfoData::charging_current_miliamps_ to reflect the current conditions.
             * - The determination of floating mode may require calibration due to potential inaccuracies in supply voltage measurement.
             */
            ChargerState get_estimated_charger_state() const;

            /**
             * @brief Represents the estimated state of the battery.
             * 
             * This variable holds the estimated current state of the battery, such as whether it is charging,
             * fully charged, or in an error state. The initial value is set to `BatteryState::ERROR` to indicate
             * that the battery state has not been determined yet.
             * 
             * @details
             * - The battery state is determined based on various inputs, such as voltage, charging current, 
             *   and system conditions.
             * - Possible values are defined in the @ref BatteryState enum, including:
             *   - `BatteryState::CHARGING`: The battery is actively charging.
             *   - `BatteryState::FULLY_CHARGED`: The battery is fully charged.
             *   - `BatteryState::EMPTY`: The battery is empty.
             *   - `BatteryState::ERROR`: An error occurred while determining the battery state.
             * 
             * @note The battery state should be periodically updated by the system to reflect current conditions.
             */
            BatteryState estimated_battery_state_ = BatteryState::ERROR;

            /**
             * @brief Retrieves the estimated battery state based on supply voltage and @ref PowerInfoData::estimated_charger_state_.
             * 
             * This function determines the current battery state using the supply voltage, charger state, 
             * and other system conditions. It evaluates whether the battery is charging, fully charged, low, 
             * not charging, or in an error/unknown state.
             * 
             * @return #BatteryState The estimated battery state. Possible values include:
             * @retval #BatteryState::CHARGING The battery is actively charging.
             * @retval #BatteryState::FULLY_CHARGED The battery is fully charged.
             * @retval #BatteryState::EMPTY The battery is low and needs recharging.
             * @retval #BatteryState::NOT_CHARGING The battery is not being charged but is not full.
             * @retval #BatteryState::ERROR An error occurred (e.g., invalid supply voltage or charger state).
             * @retval #BatteryState::UNKNOWN The battery state cannot be determined.
             * 
             * @details
             * - **Input Conditions:**
             *   - The function relies on the @ref PowerInfoData::supply_voltage_milivolts_ and @ref PowerInfoData::estimated_charger_state_ variables.
             *   - If either value is invalid (e.g., @ref PowerInfoData::supply_voltage_milivolts_ == @ref UIRB::INVALID_VOLTAGE_MILIVOLTS or 
             *     @ref PowerInfoData::estimated_charger_state_ == `ChargerState::ERROR`), the function returns `BatteryState::ERROR`.
             * 
             * - **Battery State Determination:**
             *   1. If the battery is actively charging (@ref PowerInfoData::isBatteryCharging()), the state is `BatteryState::CHARGING`.
             *   2. If the charger is floating or the battery is full (@ref PowerInfoData::isBatteryFull()), the state is `BatteryState::FULLY_CHARGED`.
             *   3. If the battery is low (@ref PowerInfoData::isBatteryLow()), the state is `BatteryState::EMPTY`.
             *   4. If the charger is turned off and the battery is not full, the state is `BatteryState::NOT_CHARGING`.
             *   5. If none of the above conditions are met, the state defaults to `BatteryState::UNKNOWN`.
             * 
             * @note This function assumes that the system periodically updates @ref PowerInfoData::supply_voltage_milivolts_ 
             *       and @ref PowerInfoData::estimated_charger_state_ to reflect current conditions accurately.
             */
            BatteryState get_estimated_battery_state() const;

            /**
             * @brief Converts supply voltage on the `AVcc` pin from millivolts to volts.
             * 
             * This function converts the supply voltage value provided in millivolts (measured on the `AVcc` pin)
             * to volts as a floating-point number. It handles edge cases where the voltage is invalid, 
             * exceeds the maximum allowable range, or falls below the minimum voltage required for an 8 MHz clock.
             * 
             * @param[in] avcc_milivolts Voltage in millivolts on the `AVcc` pin.
             * @return float The equivalent voltage in volts.
             * @retval NAN The provided voltage is invalid (e.g., #UIRB::INVALID_VOLTAGE_MILIVOLTS).
             * @retval INFINITY The voltage exceeds the maximum allowable range (#UIRB::AVCC_MILIVOLTS_MAX).
             * @retval -INFINITY The voltage is below the minimum allowable range for 8 MHz operation (#UIRB::AVCC_MILIVOLTS_8MHZ_MIN).
             * 
             * @details
             * - **Special Cases:**
             *   - **Invalid Voltage (`NAN`)**: Indicates that the input voltage is invalid or unreadable.
             *   - **High Voltage (`INFINITY`)**: Indicates a potentially damaging voltage level, making the reading unreliable.
             *   - **Low Voltage (`-INFINITY`)**: Indicates the voltage is insufficient for 8 MHz operation, resulting in unreliable behavior.
             * 
             * @warning 
             * - Ensure that the supply voltage is within the recommended operating range for the hardware to avoid damage or unstable behavior.
             * - This function assumes that @p avcc_milivolts represents an accurate reading from the `AVcc` pin.
             */
            static float avcc_milivolts_to_volts(const uint16_t avcc_milivolts);

            /**
             * @brief Converts the charging current from milliamps to amps.
             * 
             * This function takes a current value in milliamps and converts it to amps 
             * as a floating-point number. If the input current is invalid (e.g., 
             * @ref UIRB::INVALID_CURRENT_MILIAMPS or @ref UIRB::UNKNOWN_CURRENT_MILIAMPS), 
             * the function returns `NAN` to indicate an invalid result.
             * 
             * @param[in] charging_current_miliamps Charging current in milliamps.
             * @return float The equivalent current in amps.
             * @retval NAN The provided current is invalid (e.g., @ref UIRB::INVALID_CURRENT_MILIAMPS 
             *             or @ref UIRB::UNKNOWN_CURRENT_MILIAMPS).
             * 
             * @note 
             * - The function assumes that @p charging_current_miliamps is a valid measurement.
             * - If the value equals @ref UIRB::INVALID_CURRENT_MILIAMPS or @ref UIRB::UNKNOWN_CURRENT_MILIAMPS, 
             *   the result is `NAN`, which signifies that the current could not be determined.
             */
            static float charging_current_miliamps_to_amps(const uint16_t charging_current_miliamps);

            /**
             * @brief Converts the @ref PIN_PROG pin voltage from millivolts to volts.
             * 
             * This function takes a voltage value in millivolts and converts it to volts 
             * as a floating-point number. If the input voltage is invalid (e.g., @ref UIRB::INVALID_VOLTAGE_MILIVOLTS), 
             * the function returns `NAN` to indicate an invalid result.
             * 
             * @param[in] prog_milivolts Voltage at the @ref PIN_PROG pin, in millivolts.
             * @return float The equivalent voltage in volts.
             * @retval NAN The provided voltage is invalid (e.g., #UIRB::INVALID_VOLTAGE_MILIVOLTS).
             * 
             * @note 
             * - The function assumes that @p prog_milivolts is a valid measurement from the @ref PIN_PROG pin.
             * - If the value equals @ref UIRB::INVALID_VOLTAGE_MILIVOLTS, the result is `NAN`, which 
             *   signifies that the voltage could not be determined.
             */
            static float prog_milivolts_to_volts(const uint16_t prog_milivolts);

            /**
             * @brief Converts the @ref PIN_PROG pin voltage to the corresponding charging current in milliamps.
             * 
             * This function calculates the charging current based on the voltage at the @ref PIN_PROG pin and the 
             * resistance of the corresponding `PROG` pin's resistor. It takes into account the mode and state of the MCU pin 
             * used for `Vprog` measurement and handles various edge cases, such as invalid inputs and special 
             * conditions where the current cannot be measured accurately.
             * 
             * @param[in] prog_milivolts Voltage at the @ref PIN_PROG pin, in millivolts.
             * @param[in] prog_resistor_ohms Resistance of the `PROG` resistor, in ohms.
             * @param[in] prog_pin_mode Mode of the MCU pin used for `Vprog` measurement (`INPUT`, `OUTPUT`, or `INPUT_PULLUP`).
             * @param[in] prog_pin_state Digital state (high or low) of the MCU pin used for `Vprog` measurement.
             * @return uint16_t Calculated charging current in milliamps.
             * @retval 0 Charging is turned off (indicated by #PIN_PROG pin voltage above #UIRB::PROG_CC_CHARGE_VOLTAGE_MAX_MV or below #UIRB::PROG_FLOAT_VOLTAGE_MAX_MV threshold).
             * @retval #UIRB::UNKNOWN_CURRENT_MILIAMPS Charging current cannot be determined due to configuration or measurement limitations.
             * @retval #UIRB::INVALID_CURRENT_MILIAMPS One or more input parameters are invalid.
             * 
             * @details
             * - **Valid Inputs:**
             *   - The @p prog_milivolts parameter must be a valid voltage.
             *   - The @p prog_resistor_ohms must be a positive non-zero value and match the expected resistance of the circuit.
             *   - The @p prog_pin_mode must be a valid MCU pin mode.
             * - **Special Cases:**
             *   - If the pin mode is `OUTPUT` and the pin is set to HIGH, the charging current is forced to `0` due to RC circuit behavior.
             *   - When the pin mode is `INPUT_PULLUP`, the charging current cannot be accurately measured, and the function returns @ref UIRB::UNKNOWN_CURRENT_MILIAMPS.
             * - **Charging Current Calculation:**
             *   - For valid input conditions, the function calculates the charging current using the formula:
             *     \f[
             *     I_{\text{chg}} \, \text{(mA)} = \frac{V_{\text{prog}} \, \text{(mV)} \cdot 1000}{R_{\text{prog}} \, \text{(Ω)}}
             *     \f]
             *   - This implementation uses:
             *     @code
             *     uint32_t charging_current_miliamps = (static_cast<uint32_t>(prog_milivolts) * 1000U) / prog_resistor_ohms;
             *     @endcode
             *   - The calculated current is clamped to a minimum of 1 mA, with `0` reserved for the off state.
             *   - See the implementation in @ref PowerInfoData.cpp for details.
             * 
             * @warning 
             * - Ensure that the @p prog_resistor_ohms value matches the actual resistor in the circuit to avoid inaccurate results.
             * - The function assumes proper hardware configuration and valid inputs. Invalid or inconsistent inputs may lead to undefined behavior.
             */
            static uint16_t prog_milivolts_to_charging_current_miliamps(const uint16_t prog_milivolts, const uint16_t prog_resistor_ohms, const uint8_t prog_pin_mode, const bool prog_pin_state);
    };
}  // namespace uirbcore

#endif  // UIRBcore_PowerInfoData_hpp