#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "PressureConfig.hpp"

struct PressureResult
{
	std::string statusStr;
	std::uint16_t rawCount;
	int correctedCount;
	PressureConfig::SensorType sensorType;
	bool valid;
	float pressure,
		temperatureC,
		temperatureF;

	PressureResult()
		: statusStr(""), rawCount(0), correctedCount(0), sensorType(), valid(false),
		pressure(0.0f), temperatureC(0.0f), temperatureF(0.0f)
	{
	}
};

struct XYZ
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

struct IMUResult
{
	XYZ accl, gyro, magn;
};

using SensorResult = std::variant<PressureResult, IMUResult>;