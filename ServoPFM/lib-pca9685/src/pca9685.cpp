/**
 * @file pca9685.cpp
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

#include <cstdint>
#include <stdio.h>
#include <cassert>

#include <wiringPiI2C.h>

#include "pca9685.h"

namespace PQPCA9685 {

unsigned int DIV_ROUND_UP(unsigned int n, unsigned int d)	{ return (((n) + (d) - 1) / (d)); }

constexpr unsigned long OSC_FREQ { 25000000L };

enum TPCA9685Reg {
	MODE1 = 0x00,
	MODE2 = 0x01,
	ALLCALLADR = 0x05,
	LED0_ON_L = 0x06,
	LED0_ON_H = 0x07,
	LED0_OFF_L = 0x08,
	LED0_OFF_H = 0x09,
	ALL_LED_ON_L = 0xFA,
	ALL_LED_ON_H = 0xFB,
	ALL_LED_OFF_L = 0xFC,
	ALL_LED_OFF_H = 0xFD,
	PRE_SCALE = 0xFE
};

constexpr unsigned int PRE_SCALE_MIN	{ 0x03 };
constexpr unsigned int PRE_SCALE_MAX	{ 0xFF };

/*
 * 7.3.1 Mode register 1, MODE1
 */
enum TPCA9685Mode1 {
	ALLCALL = 1 << 0,
	SUB3 = 1 << 1,
	SUB2 = 1 << 2,
	SUB1 = 1 << 3,
	SLEEP = 1 << 4,
	AI = 1 << 5,
	EXTCLK = 1 << 6,
	RESTART = 1 << 7
};

/*
 * 7.3.2 Mode register 2, MODE2
 */
enum TPCA9685Mode2 {
	OUTDRV = 1 << 2,
	OCH = 1 << 3,
	INVRT = 1 << 4
};

PCA9685::PCA9685(std::uint8_t nAddress) : m_nAddress(nAddress) {
	i2c_begin();

	AutoIncrement(true);

	for (std::uint8_t i = 0; i < 16; i ++) {
		Write(i, static_cast<std::uint16_t>(0), static_cast<std::uint16_t>(0x1000));
	}

	Sleep(false);
}

PCA9685::~PCA9685(void) {
}

void PCA9685::Sleep(bool bMode) {
	std::uint8_t Data = I2cReadReg(TPCA9685Reg::MODE1);

	Data &= ~TPCA9685Mode1::SLEEP;

	if (bMode) {
		Data |= TPCA9685Mode1::SLEEP;
	}

	I2cWriteReg(TPCA9685Reg::MODE1, Data);

	if (Data & ~TPCA9685Mode1::RESTART) {
		udelay(500);
		Data |= TPCA9685Mode1::RESTART;
	}
}

void PCA9685::SetPreScaller(std::uint8_t nPrescale) {
	nPrescale = nPrescale < PRE_SCALE_MIN ? PRE_SCALE_MIN : nPrescale;

	Sleep(true);
	I2cWriteReg(TPCA9685Reg::PRE_SCALE, nPrescale);
	Sleep(false);
}

std::uint8_t PCA9685::GetPreScaller(void) {
	return 	I2cReadReg(TPCA9685Reg::PRE_SCALE);
}

void PCA9685::SetFrequency(std::uint16_t nFreq) {
	SetPreScaller(CalcPresScale(nFreq));
}

std::uint16_t PCA9685::GetFrequency(void) {
	return CalcFrequency(GetPreScaller());
}

void PCA9685::SetOCH(TPCA9685Och enumTPCA9685Och) {
	std::uint8_t Data = I2cReadReg(TPCA9685Reg::MODE2);

	Data &= ~TPCA9685Mode2::OCH;

	if (enumTPCA9685Och == TPCA9685Och::OCH_ACK) {
		Data |= TPCA9685Och::OCH_ACK;
	} // else, default Outputs change on STOP command

	I2cWriteReg(TPCA9685Reg::MODE2, Data);
}

TPCA9685Och PCA9685::GetOCH(void) {
	const std::uint8_t Data = I2cReadReg(TPCA9685Reg::MODE2) & TPCA9685Mode2::OCH;

	return static_cast<TPCA9685Och>(Data);
}

void PCA9685::SetInvert(bool bInvert) {
	std::uint8_t Data = I2cReadReg(TPCA9685Reg::MODE2);

	Data &= ~TPCA9685Mode2::INVRT;

	if (bInvert) {
		Data |= TPCA9685Mode2::INVRT;
	}

	I2cWriteReg(TPCA9685Reg::MODE2, Data);
}

bool PCA9685::GetInvert(void) {
	const std::uint8_t Data = I2cReadReg(TPCA9685Reg::MODE2) & TPCA9685Mode2::INVRT;

	return (Data == TPCA9685Mode2::INVRT);
}

void PCA9685::SetOutDriver(bool bOutDriver) {
	std::uint8_t Data = I2cReadReg(TPCA9685Reg::MODE2);

	Data &= ~TPCA9685Mode2::OUTDRV;

	if (bOutDriver) {
		Data |= TPCA9685Mode2::OUTDRV;
	}

	I2cWriteReg(TPCA9685Reg::MODE2, Data);
}

bool PCA9685::GetOutDriver(void) {
	const std::uint8_t Data = I2cReadReg(TPCA9685Reg::MODE2) & TPCA9685Mode2::OUTDRV;

	return (Data == TPCA9685Mode2::OUTDRV);
}

void PCA9685::Write(std::uint8_t nChannel, uint16_t nOn, uint16_t nOff) {
	std::uint8_t reg;

	if (nChannel <= 15) {
		reg = TPCA9685Reg::LED0_ON_L + (nChannel << 2);
	} else {
		reg = TPCA9685Reg::ALL_LED_ON_L;
	}

	I2cWriteReg(reg, nOn, nOff);
}

void PCA9685::Write(std::uint8_t nChannel, uint16_t nValue) {
	Write(nChannel, static_cast<std::uint16_t>(0), nValue);
}

void PCA9685::Write(std::uint16_t nOn, uint16_t nOff) {
	Write(static_cast<std::uint8_t>(16), nOn, nOff);
}

void PCA9685::Write(std::uint16_t nValue) {
	Write(static_cast<std::uint8_t>(16), nValue);
}

void PCA9685::Read(std::uint8_t nChannel, uint16_t *pOn, uint16_t *pOff) {
	assert(pOn != 0);
	assert(pOff != 0);

	std::uint8_t reg;

	if (nChannel <= 15) {
		reg = TPCA9685Reg::LED0_ON_L + (nChannel << 2);
	} else {
		reg = TPCA9685Reg::ALL_LED_ON_L;
	}

	if (pOn != 0) {
		*pOn = I2cReadReg16(reg);
	}

	if (pOff) {
		*pOff = I2cReadReg16(reg + 2);
	}
}

void PCA9685::Read(std::uint16_t *pOn, uint16_t *pOff) {
	Read(static_cast<std::uint8_t>(16), pOn, pOff);
}

void PCA9685::SetFullOn(std::uint8_t nChannel, bool bMode) {
	std::uint8_t reg;

	if (nChannel <= 15) {
		reg = TPCA9685Reg::LED0_ON_H + (nChannel << 2);
	} else {
		reg = TPCA9685Reg::ALL_LED_ON_H;
	}

	std::uint8_t Data = I2cReadReg(reg);

	Data = bMode ? (Data | 0x10) : (Data & 0xEF);

	I2cWriteReg(reg, Data);

	if (bMode) {
		SetFullOff(nChannel, false);
	}

}

void PCA9685::SetFullOff(std::uint8_t nChannel, bool bMode) {
	std::uint8_t reg;

	if (nChannel <= 15) {
		reg = TPCA9685Reg::LED0_OFF_H + (nChannel << 2);
	} else {
		reg = TPCA9685Reg::ALL_LED_OFF_H;
	}

	std::uint8_t Data = I2cReadReg(reg);

	Data = bMode ? (Data | 0x10) : (Data & 0xEF);

	I2cWriteReg(reg, Data);
}

std::uint8_t PCA9685::CalcPresScale(uint16_t nFreq) {
	nFreq = (nFreq > FREQUENCY_MAX ? FREQUENCY_MAX : (nFreq < FREQUENCY_MIN ? FREQUENCY_MIN : nFreq));

	const float f = static_cast<float>(OSC_FREQ) / 4096;

	const std::uint8_t Data = DIV_ROUND_UP(f, nFreq) - 1;

	return Data;
}

std::uint16_t PCA9685::CalcFrequency(uint8_t nPreScale) {
	std::uint16_t f_min;
	std::uint16_t f_max;
	const float f = static_cast<float>(OSC_FREQ) / 4096;
	const std::uint16_t Data = DIV_ROUND_UP(f, (static_cast<std::uint16_t>(nPreScale) + 1));

	for (f_min = Data; f_min > FREQUENCY_MIN; f_min--) {
		if (CalcPresScale(f_min) != nPreScale) {
			break;
		}
	}

	for (f_max = Data; f_max < FREQUENCY_MAX; f_max++) {
		if (CalcPresScale(f_max) != nPreScale) {
			break;
		}
	}

	return (f_max + f_min) / 2;
}

void PCA9685::Dump(void) {
	std::uint8_t reg = I2cReadReg(TPCA9685Reg::MODE1);

	printf("MODE1 - Mode register 1 (address 00h) : %02Xh\n", reg);
	printf("\tbit 7 - RESTART : Restart %s\n", reg & TPCA9685Mode1::RESTART ? "enabled" : "disabled");
	printf("\tbit 6 - EXTCLK  : %s\n", reg & TPCA9685Mode1::EXTCLK ? "Use EXTCLK pin clock" : "Use internal clock");
	printf("\tbit 5 - AI      : Register Auto-Increment %s\n", reg & TPCA9685Mode1::AI ? "enabled" : "disabled");
	printf("\tbit 4 - SLEEP   : %s\n", reg & TPCA9685Mode1::SLEEP ? "Low power mode. Oscillator off" : "Normal mode");
	printf("\tbit 3 - SUB1    : PCA9685 %s to I2C-bus subaddress 1\n", reg & TPCA9685Mode1::SUB1 ? "responds" : "does not respond");
	printf("\tbit 2 - SUB1    : PCA9685 %s to I2C-bus subaddress 2\n", reg & TPCA9685Mode1::SUB2 ? "responds" : "does not respond");
	printf("\tbit 1 - SUB1    : PCA9685 %s to I2C-bus subaddress 3\n", reg & TPCA9685Mode1::SUB3 ? "responds" : "does not respond");
	printf("\tbit 0 - ALLCALL : PCA9685 %s to LED All Call I2C-bus address\n", reg & TPCA9685Mode1::ALLCALL ? "responds" : "does not respond");

	reg = I2cReadReg(TPCA9685Reg::MODE2);

	printf("\nMODE2 - Mode register 2 (address 01h) : %02Xh\n", reg);
	printf("\tbit 7 to 5      : Reserved\n");
	printf("\tbit 4 - INVRT   : Output logic state %sinverted\n", reg & TPCA9685Mode2::INVRT ? "" : "not ");
	printf("\tbit 3 - OCH     : Outputs change on %s\n", reg & TPCA9685Mode2::OCH ? "ACK" : "STOP command");
	printf("\tbit 2 - OUTDRV  : The 16 LEDn outputs are configured with %s structure\n", reg & TPCA9685Mode2::OUTDRV ? "a totem pole" : "an open-drain");
	printf("\tbit 10- OUTNE   : %01x\n", reg & 0x3);

	reg = I2cReadReg(TPCA9685Reg::PRE_SCALE);

	printf("\nPRE_SCALE register (address FEh) : %02Xh\n", reg);
	printf("\t Frequency : %d Hz\n", CalcFrequency(reg));

	printf("\n");

	std::uint16_t on, off;

	for (std::uint8_t nLed = 0; nLed <= 15; nLed ++) {
		Read(nLed, &on, &off);
		printf("LED%d_ON  : %04x\n", nLed, on);
		printf("LED%d_OFF : %04x\n", nLed, off);
	}

	printf("\n");

	Read(16, &on, &off);
	printf("ALL_LED_ON  : %04x\n", on);
	printf("ALL_LED_OFF : %04x\n", off);
}

void PCA9685::AutoIncrement(bool bMode) {
	std::uint8_t Data = I2cReadReg(TPCA9685Reg::MODE1);

	Data &= ~TPCA9685Mode1::AI;	// 0 Register Auto-Increment disabled. {default}

	if (bMode) {
		Data |= TPCA9685Mode1::AI;	// 1 Register Auto-Increment enabled.
	}

	I2cWriteReg(TPCA9685Reg::MODE1, Data);
}

void PCA9685::I2cSetup(void) {
	i2c_set_address(m_nAddress);
	i2c_set_baudrate(I2C_FULL_SPEED);
}

void PCA9685::I2cWriteReg(std::uint8_t reg, uint8_t data) {
	char buffer[2];

	buffer[0] = reg;
	buffer[1] = data;

	I2cSetup();

	i2c_write(buffer, 2);
}

std::uint8_t PCA9685::I2cReadReg(uint8_t reg) {
	char data = reg;

	I2cSetup();

	i2c_write(&data, 1);
	i2c_read(&data, 1);

	return data;
}

void PCA9685::I2cWriteReg(std::uint8_t reg, uint16_t data) {
	char buffer[3];

	buffer[0] = reg;
	buffer[1] = (data & 0xFF);
	buffer[2] = (data >> 8);

	I2cSetup();

	i2c_write(buffer, 3);
}

std::uint16_t PCA9685::I2cReadReg16(uint8_t reg) {
	char data = reg;
	char buffer[2] = { 0, 0 };

	I2cSetup();

	i2c_write(&data, 1);
	i2c_read(reinterpret_cast<char *>(&buffer), 2);

	return (buffer[1] << 8) | buffer[0];
}

void PCA9685::I2cWriteReg(std::uint8_t reg, uint16_t data, uint16_t data2) {
	char buffer[5];

	buffer[0] = reg;
	buffer[1] = (data & 0xFF);
	buffer[2] = (data >> 8);
	buffer[3] = (data2 & 0xFF);
	buffer[4] = (data2 >> 8);

	I2cSetup();

	i2c_write(buffer, 5);
}

};