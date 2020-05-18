/**
 * @file pca9685.h
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

#include <cstdint>

namespace PQPCA9685 {
constexpr unsigned int I2C_ADDRESS_DEFAULT  { 0x40 };
constexpr unsigned int I2C_ADDRESS_FIXED	{ 0x70 };
constexpr unsigned int I2C_ADDRESSES_MAX	{ 62 };

constexpr unsigned int MAX_12BIT	{ 0xFFF };
constexpr unsigned int MAX_8BIT		{ 0xFF } ;

constexpr std::uint8_t  CHANNEL(unsigned int const x)	{ return static_cast<std::uint8_t>(x); };
constexpr std::uint16_t VALUE(unsigned int const x)	    { return static_cast<std::uint16_t>(x); };

constexpr unsigned int VALUE_MIN	{ VALUE(0) };
constexpr unsigned int VALUE_MAX	{ VALUE(4096) };

constexpr unsigned int PWM_CHANNELS { 16 };

enum TPCA9685FrequencyRange {
	FREQUENCY_MIN = 24,
	FREQUENCY_MAX = 1526
};

enum TPCA9685Och {
	OCH_STOP = 0,
	OCH_ACK = 1 << 3
};

class PCA9685 {
public:
	PCA9685() = delete;
	PCA9685(std::uint8_t nAddress = I2C_ADDRESS_DEFAULT);
	PCA9685(PCA9685 &) = delete;
	PCA9685(PCA9685 &&) = delete;
	PCA9685& operator=(PCA9685 &) = delete;
	PCA9685& operator=(PCA9685 &&) = delete;
	~PCA9685(void);

	void SetPreScaller(std::uint8_t);
	std::uint8_t GetPreScaller(void);

	void SetFrequency(std::uint16_t);
	std::uint16_t GetFrequency(void);

	void SetOCH(TPCA9685Och);
	TPCA9685Och GetOCH(void);

	void SetInvert(bool);
	bool GetInvert(void);

	void SetOutDriver(bool);
	bool GetOutDriver(void);

	void Write(std::uint8_t, std::uint16_t, std::uint16_t);
	void Read(std::uint8_t, std::uint16_t *, std::uint16_t *);

	void Write(std::uint16_t, std::uint16_t);
	void Read(std::uint16_t *, std::uint16_t *);

	void Write(std::uint8_t, std::uint16_t);
	void Write(std::uint16_t);

	void SetFullOn(std::uint8_t, bool);
	void SetFullOff(std::uint8_t, bool);

	void Dump(void);

private:
	std::uint8_t CalcPresScale(std::uint16_t);
	std::uint16_t CalcFrequency(std::uint8_t);

private:
	void Sleep(bool);
	void AutoIncrement(bool);

private:
	void I2cSetup(void);

	void I2cWriteReg(std::uint8_t, std::uint8_t);
	std::uint8_t I2cReadReg(std::uint8_t);

	void I2cWriteReg(std::uint8_t, std::uint16_t);
	std::uint16_t I2cReadReg16(std::uint8_t);

	void I2cWriteReg(std::uint8_t, std::uint16_t, std::uint16_t);

private:
	std::uint8_t m_nAddress { I2C_ADDRESS_DEFAULT };
};

};