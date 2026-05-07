#pragma once

#include <cstdint>

#include "ImuConfig.hpp"
#include "Sensor.hpp"
#include "SensorBus.hpp"
#include "SensorResult.hpp"

class IMUSensor : public Sensor
{
private:
	ImuConfig::Device config;

public:
	IMUSensor(SensorBus* pBus, ImuConfig::Device& configParam);

	// Block copying to prevent dangling pInterface pointers
	IMUSensor(const IMUSensor&) = delete;
	IMUSensor& operator=(const IMUSensor&) = delete;

	IMUSensor(IMUSensor&&) = default;
	IMUSensor& operator=(IMUSensor&&) = default;

	void verifyWhoAmI(const std::uint16_t address, const std::uint8_t expectedID);
	int registerWrite(const std::uint16_t address, const std::uint8_t reg, const std::uint8_t configVal);
	SensorResult getDecodedData() override;

	~IMUSensor();
};