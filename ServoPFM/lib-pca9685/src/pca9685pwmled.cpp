/**
 * @file pwmled.cpp
 *
 */
/* Copyright (C) 2017-2018 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#include "pca9685pwmled.h"

namespace PQPCA9685 {

PCA9685PWMLed::PCA9685PWMLed(std::uint8_t nAddress): PCA9685(nAddress) {
	SetFrequency(PWMLED_DEFAULT_FREQUENCY);
}

PCA9685PWMLed::~PCA9685PWMLed(void) {
}

void PCA9685PWMLed::Set(std::uint8_t nChannel, std::uint16_t nData) {

	if (nData >= MAX_12BIT) {
		SetFullOn(nChannel, true);
	} else if (nData == 0) {
		SetFullOff(nChannel, true);
	} else {
		Write(nChannel, nData);
	}
}

void PCA9685PWMLed::Set(std::uint8_t nChannel, std::uint8_t nData) {

	if (nData == MAX_8BIT) {
		SetFullOn(nChannel, true);
	} else if (nData == 0) {
		SetFullOff(nChannel, true);
	} else {
		const std::uint16_t nValue { (nData << 4) | (nData >> 4) };
		Write(nChannel, nValue);
	}
}

};