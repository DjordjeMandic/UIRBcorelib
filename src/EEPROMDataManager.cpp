/**
 * @file EEPROMDataManager.cpp
 * @brief Implementation of EEPROM data management class and associated functions for managing configuration data in EEPROM.
 * 
 * This file contains the implementation of the @ref uirbcore::eeprom::EEPROMDataManager class, which provides high-level
 * access and management of the EEPROM memory for storing and retrieving hardware configuration,
 * metadata, and runtime statistics for the %UIRB system.
 * 
 * @details
 * - The @ref UIRB_EEPROM_BYPASS_DEBUG macro allows all EEPROM operations to be redirected to RAM for debugging purposes, 
 *   ensuring data persistence is emulated without requiring physical EEPROM.
 * - The @ref UIRB_EEPROM_RPROG_DEBUG macro enables debugging of the charger programming resistor value (`Rprog`) by 
 *   setting it to a specific value during runtime. This is especially useful in simulations or for testing different configurations.
 * 
 * @note Both macros are intended for debugging and testing scenarios. Refer to their documentation for detailed usage guidelines.
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
#include <UIRBcore_Defs.h>
#include <UIRBcore_EEPROM.hpp>

#if !defined(UIRB_EEPROM_BYPASS_DEBUG)
    #include <EEPROM.h>
#endif  // !defined(UIRB_EEPROM_BYPASS_DEBUG)

namespace uirbcore::eeprom
{
#if defined(UIRB_EEPROM_BYPASS_DEBUG) || defined(__DOXYGEN__)
    /**
     * @brief Active emulated EEPROM data used when @ref UIRB_EEPROM_BYPASS_DEBUG is enabled.
     * 
     * This variable serves as the in-memory representation of the EEPROM data when the 
     * @ref UIRB_EEPROM_BYPASS_DEBUG macro is defined. It is initialized to the default 
     * values specified in @ref DEBUG_EEPROM_DATA and acts as a substitute for EEPROM storage 
     * during debugging or simulations.
     * 
     * @details
     * - When @ref UIRB_EEPROM_BYPASS_DEBUG is defined, all EEPROM-related operations 
     *   are redirected to this in-memory structure instead of actual EEPROM.
     * - The variable is initialized with @ref DEBUG_EEPROM_DATA, ensuring that all fields 
     *   are set to safe defaults for debugging purposes.
     * - This variable provides full functionality for EEPROM operations in RAM while avoiding 
     *   modifications to actual EEPROM.
     * 
     * @note
     * - Data stored in @ref EEPROM_DATA is volatile and will not persist between reboots 
     *   or power cycles.
     * - This variable is defined only when @ref UIRB_EEPROM_BYPASS_DEBUG is enabled.
     * 
     * @warning
     * - Do not use this variable in production environments; it is intended for debugging 
     *   and simulation scenarios only.
     * - Ensure that the values in @ref DEBUG_EEPROM_DATA are appropriate for your testing 
     *   and debugging use cases.
     * 
     * @see @ref DEBUG_EEPROM_DATA for the default debug data values.
     * @see @ref UIRB_EEPROM_BYPASS_DEBUG for enabling RAM-based EEPROM emulation.
     */
    static EEPROMData EEPROM_DATA = DEBUG_EEPROM_DATA;
#endif // defined(UIRB_EEPROM_BYPASS_DEBUG) || defined(__DOXYGEN__)

    // Compare two EEPROMData structures for equality
    bool operator==(const EEPROMData& lhs, const EEPROMData& rhs)
    {
    #if defined(UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON)
        return memcmp(&lhs, &rhs, sizeof(EEPROMData)) == 0; 
    #else  // defined(UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON)
        // Compare each member field by field
        if (lhs.hardware_version.version_byte != rhs.hardware_version.version_byte ||
            lhs.bandgap_1v1_reference_offset != rhs.bandgap_1v1_reference_offset ||
            lhs.stat_led_brightness != rhs.stat_led_brightness ||
            lhs.charger_prog_resistor_ohms != rhs.charger_prog_resistor_ohms ||
            lhs.software_config.config_byte != rhs.software_config.config_byte ||
            lhs.hardware_manufacture_date.month_year_byte != rhs.hardware_manufacture_date.month_year_byte ||
            lhs.boot_count != rhs.boot_count ||
            lhs.uirb_serial_number.serial_number_u16 != rhs.uirb_serial_number.serial_number_u16) {
            return false;
        }
        // Compare serial numbers manually (since string is not null terminated)
        for (uint8_t i = 0; i < DATA_FACTORY_CP2104_SERIAL_NUM_LEN; ++i) {
            if (lhs.factory_cp2104_usb_serial_number[i] != rhs.factory_cp2104_usb_serial_number[i]) {
                return false;
            }
        }
        return true;
    #endif  // defined(UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON)
    }

    // Compare two EEPROMData structures for inequality
    bool operator!=(const EEPROMData& lhs, const EEPROMData& rhs)
    {
        return !(lhs == rhs);
    }

    EEPROMDataManager::EEPROMDataManager()
    {
    #if defined(UIRB_EEPROM_BYPASS_DEBUG)
        this->eeprom_core_data_.hardware_version = DEBUG_EEPROM_DATA.hardware_version;
    #else
        EEPROM.get(EEPROMDataManager::CORE_DATA_ADDR_START, this->eeprom_core_data_.hardware_version);
    #endif

        if(this->hardware_version_matches())
        {
            this->load_from_eeprom();
        }
    }

    EEPROMDataManager::EEPROMDataManager(const EEPROMData& data)
    {
        this->set(data);
    }

    void EEPROMDataManager::get(EEPROMData& data) const
    {
        data = this->get();
    }

    EEPROMData EEPROMDataManager::get() const
    {
        return this->eeprom_core_data_;
    }

    void EEPROMDataManager::set(const EEPROMData& data)
    {
        this->eeprom_core_data_ = data;
    }

    void EEPROMDataManager::set(const EEPROMDataManager& dataManager)
    {
        this->set(dataManager.get());
    }

    void EEPROMDataManager::load_from_eeprom()
    {
        EEPROMDataManager::read_from_eeprom(this->eeprom_core_data_);
    }

    bool EEPROMDataManager::save_to_eeprom() const
    {
        return EEPROMDataManager::store_to_eeprom(this->eeprom_core_data_);
    }

    HardwareVersion EEPROMDataManager::get_hardware_version() const
    {
        return this->eeprom_core_data_.hardware_version;
    }

    bool EEPROMDataManager::hardware_version_matches() const
    {
        return this->eeprom_core_data_.hardware_version.version_byte == UIRB_HW_VER.version_byte; 
    }

    uint16_t EEPROMDataManager::get_bandgap_reference_milivolts() const
    {
        return static_cast<uint16_t>(1100 + this->eeprom_core_data_.bandgap_1v1_reference_offset);
    }

    bool EEPROMDataManager::set_bandgap_reference_milivolts(const uint16_t milivolts)
    {
        if (milivolts < 1100U + INT8_MIN || milivolts > 1100U + INT8_MAX) {
            return false;
        }

        this->eeprom_core_data_.bandgap_1v1_reference_offset = static_cast<int8_t>(milivolts - 1100);

        return true;
    }

    uint8_t EEPROMDataManager::get_stat_led_brightness() const
    {
        return this->eeprom_core_data_.stat_led_brightness;
    }

    void EEPROMDataManager::set_stat_led_brightness(const uint8_t brightness)
    {
        this->eeprom_core_data_.stat_led_brightness = brightness;
    }

    uint16_t EEPROMDataManager::get_charger_prog_resistor_ohms() const
    {
        return this->eeprom_core_data_.charger_prog_resistor_ohms <= EEPROMDataManager::CHARGER_PROG_RESISTANCE_MIN
            ? EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE
            : this->eeprom_core_data_.charger_prog_resistor_ohms;
    }

    bool EEPROMDataManager::set_charger_prog_resistor_ohms(const uint16_t ohms)
    {
        if (ohms < EEPROMDataManager::CHARGER_PROG_RESISTANCE_MIN)
        {
            return false;
        }

        this->eeprom_core_data_.charger_prog_resistor_ohms = ohms;

        return true;
    }

    bool EEPROMDataManager::is_avr_serial_debugger_enabled() const
    {
        return this->eeprom_core_data_.software_config.avr_serial_debugger_enabled;
    }

    void EEPROMDataManager::set_avr_serial_debugger(const bool enabled)
    {
        this->eeprom_core_data_.software_config.avr_serial_debugger_enabled = enabled;
    }

    bool EEPROMDataManager::is_sleep_mode_allowed() const
    {
        return this->eeprom_core_data_.software_config.sleep_mode_allowed;
    }

    void EEPROMDataManager::allow_sleep_mode(const bool allowed)
    {
        this->eeprom_core_data_.software_config.sleep_mode_allowed = allowed;
    }

    bool EEPROMDataManager::is_sleep_mode_io3_wakeup_allowed() const
    {
        return this->eeprom_core_data_.software_config.sleep_mode_io3_wakeup_enabled;
    }

    void EEPROMDataManager::allow_sleep_mode_io3_wakeup(const bool allowed)
    {
        this->eeprom_core_data_.software_config.sleep_mode_io3_wakeup_enabled = allowed;
    }

    bool EEPROMDataManager::is_boot_count_increment_allowed() const
    {
        return this->eeprom_core_data_.software_config.boot_count_increment_enabled;
    }

    void EEPROMDataManager::allow_boot_count_increment(const bool allowed)
    {
        this->eeprom_core_data_.software_config.boot_count_increment_enabled = allowed;
    }

    uint16_t EEPROMDataManager::get_board_manufacture_year() const
    {
        return 2020U + this->eeprom_core_data_.hardware_manufacture_date.year_offset_from_2020;
    }

    bool EEPROMDataManager::set_board_manufacture_year(const uint16_t year)
    {
        if (year < 2020U || year > 2035U)
        {
            return false;
        }

        this->eeprom_core_data_.hardware_manufacture_date.year_offset_from_2020 = static_cast<uint8_t>(year - 2020U);

        return true;
    }

    uint8_t EEPROMDataManager::get_board_manufacture_month() const
    {
        return this->eeprom_core_data_.hardware_manufacture_date.month <= 12U
            ? this->eeprom_core_data_.hardware_manufacture_date.month
            : EEPROMDataManager::INVALID_MANUFACTURE_MONTH;
    }

    bool EEPROMDataManager::set_board_manufacture_month(const uint8_t month)
    {
        if (month < 1U || month > 12U)
        {
            return false;
        }

        this->eeprom_core_data_.hardware_manufacture_date.month = month;

        return true;
    }

    uint32_t EEPROMDataManager::get_boot_count() const
    {
        return this->eeprom_core_data_.boot_count;
    }

    void EEPROMDataManager::set_boot_count(const uint32_t boot_count)
    {
        this->eeprom_core_data_.boot_count = boot_count;
    }

    bool EEPROMDataManager::increment_boot_count()
    {
        if (!this->is_boot_count_increment_allowed() || this->eeprom_core_data_.boot_count == UINT32_MAX)
        {
            return false;
        }

        this->eeprom_core_data_.boot_count++;

        return true;
    }

    uint16_t EEPROMDataManager::get_uirb_board_serial_number() const
    {
        return this->eeprom_core_data_.uirb_serial_number.reserved_bit_1 == 1 || 
               this->eeprom_core_data_.uirb_serial_number.number > EEPROMDataManager::UIRB_SERIAL_NUMBER_MAX
            ? EEPROMDataManager::INVALID_UIRB_SERIAL_NUMBER
            : this->eeprom_core_data_.uirb_serial_number.number;
    }

    bool EEPROMDataManager::set_uirb_board_serial_number(const uint16_t serial_number)
    {
        if (serial_number == 0 || serial_number > EEPROMDataManager::UIRB_SERIAL_NUMBER_MAX)
        {
            return false;
        }
        this->eeprom_core_data_.uirb_serial_number.number = serial_number;
        return true;
    }

    bool EEPROMDataManager::set_factory_cp2104_usb_serial_number_cstr(const char* cstr)
    {
        if (strnlen(cstr, DATA_FACTORY_CP2104_SERIAL_NUM_LEN + 1) != DATA_FACTORY_CP2104_SERIAL_NUM_LEN) {
            return false;
        }
        memcpy(this->eeprom_core_data_.factory_cp2104_usb_serial_number, cstr, DATA_FACTORY_CP2104_SERIAL_NUM_LEN);
        return true;
    }

    char* EEPROMDataManager::get_factory_cp2104_usb_serial_number_cstr(char* buffer, const size_t buffer_len) const
    {
        if (buffer_len < DATA_FACTORY_CP2104_SERIAL_NUM_LEN + 1) {
            return nullptr;
        }
        memcpy(buffer, this->eeprom_core_data_.factory_cp2104_usb_serial_number, DATA_FACTORY_CP2104_SERIAL_NUM_LEN);
        buffer[DATA_FACTORY_CP2104_SERIAL_NUM_LEN] = '\0';
        return buffer;
    }

    void EEPROMDataManager::read_from_eeprom(EEPROMData& data)
    {
    #if defined(UIRB_EEPROM_BYPASS_DEBUG)
        data = EEPROM_DATA;
    #else
        EEPROM.get(EEPROMDataManager::CORE_DATA_ADDR_START, data);
    #endif
    }

    EEPROMData EEPROMDataManager::read_from_eeprom()
    {
        EEPROMData data_read;
        EEPROMDataManager::read_from_eeprom(data_read);
        return data_read;
    }

    bool EEPROMDataManager::store_to_eeprom(const EEPROMData& data)
    {
    #if defined(UIRB_EEPROM_BYPASS_DEBUG)
        EEPROM_DATA = data;
    #else
        EEPROM.put(EEPROMDataManager::CORE_DATA_ADDR_START, data);
    #endif
        return EEPROMDataManager::read_from_eeprom() == data;
    }
}
