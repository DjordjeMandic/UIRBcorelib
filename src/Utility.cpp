/**
 * @file Utility.cpp
 * @brief Implementation of utility functions for pin mode detection and analog reference management.
 * 
 * This file provides implementations of helper functions to retrieve the mode of a digital pin, 
 * set the analog reference for ADC conversions, and retrieve the currently configured analog reference. 
 * These utilities simplify low-level operations related to pin configuration and analog-to-digital conversions 
 * on AVR-based microcontrollers with Arduino framework.
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
#include <Utility.hpp>

uint8_t getPinMode(const uint8_t pin)
{
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    
    if (port == NOT_A_PIN) {
        return INVALID_PIN_MODE;
    }

    volatile uint8_t *ddrReg = portModeRegister(port);  // DDR register
    volatile uint8_t *portReg = portOutputRegister(port); // PORT register

    if (ddrReg == nullptr || portReg == nullptr)
    {
        return INVALID_PIN_MODE;
    }

    if (*ddrReg & bit)
    {
        return OUTPUT;
    }
    
    if (*portReg & bit)
    {
        return INPUT_PULLUP;
    }

    return INPUT;
}

void setAnalogReference(const uint8_t aref)
{
    if (aref != EXTERNAL && aref != DEFAULT && aref != INTERNAL1V1)
    {
        return;
    }

    // Using the Arduino framework, setting the analog reference and allowing it 
    // to stabilize can be done as follows:
    // 
    // analogReference(aref); // Set the desired analog reference (e.g., DEFAULT, INTERNAL1V1, EXTERNAL)
    // analogRead(A7);        // Perform a dummy read to allow the new reference to be set and stabilized

    // Set the new analog reference and select GND as the input channel (MUX[3:0] = 0b1111)
    ADMUX = (aref << 6) | 0b1111;

    // Start a dummy ADC conversion to apply the new reference and ensure stabilization
    ADCSRA |= _BV(ADSC);

    // Wait for the conversion to complete (ADSC bit clears automatically when done)
    while (ADCSRA & _BV(ADSC));
}

uint8_t getAnalogReference()
{
    switch ((ADMUX & 0xC0) >> 6)
    {
        case 0:
            return DEFAULT; // Default reference (usually AVcc)
        case 1:
            return EXTERNAL; // External reference
        case 3:
            return INTERNAL1V1; // Internal 1.1V reference
        default:
            return INVALID_ANALOG_REF; // Invalid reference (should not happen)
    }
}