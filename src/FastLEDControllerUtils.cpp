/*
   Copyright 2019 Leon Kiefer

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "FastLEDControllerUtils.h"
#include <math.h>
#include <FastLED.h>

void CLP::transformLLFanToStrip(FastLEDController* controller, uint8_t channelIndex)
{
	auto& channel = controller->getChannel(channelIndex);
	if (channel.mode == ChannelMode::SoftwarePlayback) {
		auto leds = controller->getLEDs(channelIndex);
		for (uint8_t fanIndex = 0; fanIndex < controller->getLEDCount(channelIndex) / 16; fanIndex++) {
			for (uint8_t ledIndex = 0; ledIndex < 8; ledIndex++) {
				CRGB temp = leds[fanIndex * 16 + ledIndex];
				leds[fanIndex * 16 + ledIndex] = leds[fanIndex * 16 + 15 - ledIndex];
				leds[fanIndex * 16 + 15 - ledIndex] = temp;
			}
		}
	}
}

void CLP::scale(FastLEDController* controller, uint8_t channelIndex, int scaleToSize)
{
	auto leds = controller->getLEDs(channelIndex);
	const float scaleFactor = (float)controller->getLEDCount(channelIndex) / scaleToSize;
	for (int ledIndex = scaleToSize - 1; ledIndex >= 0; ledIndex--) {
		leds[ledIndex] = leds[lround(ledIndex * scaleFactor)];
	}
}

void CLP::repeat(FastLEDController* controller, uint8_t channelIndex, uint8_t times)
{
	auto leds = controller->getLEDs(channelIndex);
	auto count = controller->getLEDCount(channelIndex);
	//skip first iteration, because LEDs already contains the data at the first position
	for (int i = 1; i < times; i++) {
		memcpy(leds + (count * i), leds, sizeof(CRGB) * count);
	}
}

void CLP::scaleSegments(FastLEDController* controller, uint8_t channelIndex, const SegmentScaling* const segments, int segmentsCount)
{
	auto leds = controller->getLEDs(channelIndex);
	int ledStripIndexAfterScaling = 0;
	int ledStripIndexBeforeScaling = 0;
	for (int i = 0; i < segmentsCount; i++) {
		ledStripIndexAfterScaling += segments[i].scaleToSize;
		ledStripIndexBeforeScaling += segments[i].segmentLength;
	}

	for (int i = segmentsCount - 1; i >= 0; i--) {
		const float scaleFactor = (float)segments[i].segmentLength / segments[i].scaleToSize;
		ledStripIndexAfterScaling -= segments[i].scaleToSize;
		ledStripIndexBeforeScaling -= segments[i].segmentLength;
		for (int ledIndex = segments[i].scaleToSize - 1; ledIndex >= 0; ledIndex--) {
			leds[ledStripIndexAfterScaling + ledIndex] = leds[ledStripIndexBeforeScaling + lround(ledIndex * scaleFactor)];
		}
	}
}

void CLP::reverse(FastLEDController* controller, uint8_t channelIndex)
{
	auto leds = controller->getLEDs(channelIndex);
	auto maxIndex = controller->getLEDCount(channelIndex) - 1;
	for (int ledIndex = 0; ledIndex < maxIndex - ledIndex; ledIndex++) {
		CRGB temp = leds[ledIndex];
		leds[ledIndex] = leds[maxIndex - ledIndex];
		leds[maxIndex - ledIndex] = temp;
	}
}

void CLP::gammaCorrection(FastLEDController* controller, uint8_t channelIndex) {
	auto leds = controller->getLEDs(channelIndex);
	auto count = controller->getLEDCount(channelIndex);
	for (int ledIndex = 0; ledIndex < count; ledIndex++) {
		leds[ledIndex].r = dim8_video(leds[ledIndex].r);
		leds[ledIndex].g = dim8_video(leds[ledIndex].g);
		leds[ledIndex].b = dim8_video(leds[ledIndex].b);
	}
}