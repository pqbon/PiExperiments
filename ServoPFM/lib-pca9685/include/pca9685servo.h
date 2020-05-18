/**
 * @file servo.h
 *
 */
/* Copyright (C) 2017-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <stdint.h>

#include "pca9685.h"

namespace PQPCA9685 {

#define ANGLE(x)	(static_cast<std::uint8_t>(x))

constexpr unsigned int  SERVO_LEFT_DEFAULT_US	 { 1000 };
constexpr unsigned int  SERVO_CENTER_DEFAULT_US	 { 1500 };
constexpr unsigned int  SERVO_RIGHT_DEFAULT_US 	 { 2000 };

class PCA9685Servo: public PCA9685 {
public:
	PCA9685Servo(std::uint8_t nAddress = I2C_ADDRESS_DEFAULT);
	~PCA9685Servo(void);

	void SetLeftUs(std::uint16_t);
	std::uint16_t GetLeftUs(void) const;

	void SetRightUs(std::uint16_t);
	std::uint16_t GetRightUs(void) const;

	void Set(std::uint8_t nChannel, std::uint16_t nData);
	void Set(std::uint8_t nChannel, std::uint8_t nData);

	void SetAngle(std::uint8_t nChannel, std::uint8_t nAngle);

private:
	void CalcLeftCount(void);
	void CalcRightCount(void);

private:
	std::uint16_t m_nLeftUs     {};
	std::uint16_t m_nRightUs    {};
	std::uint16_t m_nLeftCount  {};
	std::uint16_t m_nRightCount {};
};

};