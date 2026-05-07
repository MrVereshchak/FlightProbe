#include "ImuSensor.hpp"

#include <cstdint>
#include <format>
#include <iostream>
#include <vector>

#include "ImuConfig.hpp"
#include "Sensor.hpp"
#include "SensorBus.hpp"
#include "SensorResult.hpp"


IMUSensor::IMUSensor(SensorBus* pBus, ImuConfig::Device& configParam) : Sensor(pBus), config(configParam)
{
	// Are accl/gyro and magn sensors alive?
	verifyWhoAmI(config.agAddress, config.agExpID);
	verifyWhoAmI(config.magnAddress, config.magExpID);

	// Wake-up accl/gyro sensors, setting up data rate and scale
	registerWrite(config.agAddress, config.acclReg, config.acclInitVal);
	registerWrite(config.agAddress, config.gyroReg, config.gyroInitVal);

	// Wake-up magn sensor, setting up data rate, scale, accuracy mode
	registerWrite(config.magnAddress, config.magnRegDr, config.magnInitValDr);
	registerWrite(config.magnAddress, config.magnRegSc, config.magnInitValSc);
	registerWrite(config.magnAddress, config.magnRegMd, config.magnInitValMd);
	registerWrite(config.magnAddress, config.magnRegPerfZ, config.magnInitValPerfZ);
}

void IMUSensor::verifyWhoAmI(const std::uint16_t address, const std::uint8_t expectedID)
{
	int writeStatusAG = Sensor::write(address, 1, { config.whoAmIReg }, I2cFlags::NoStop);
	std::vector<std::uint8_t> rawData{ 0 };
	int readStatusAG = Sensor::read(address, rawData.size(), rawData);

	std::cout << std::format(
		"Write whoAmI status: {:#06x} \nRead whoAmI status: {:#06x} \nReturned ID: {:#06x} \nExpected ID: {:#06x}\n",
		writeStatusAG, readStatusAG, rawData[0], expectedID
	);
}

int IMUSensor::registerWrite(const std::uint16_t address, const std::uint8_t reg, const std::uint8_t configVal)
{
	const std::vector<std::uint8_t> wakeUpAccl{ reg, configVal };
	int writeWakeUp = Sensor::write(address, wakeUpAccl.size(), wakeUpAccl);

	std::cout << std::format("Write wake up status: {:#06x} \n", writeWakeUp);

	return writeWakeUp;
}

SensorResult IMUSensor::getDecodedData()
{
	IMUResult results;

	auto read3Axis = [&](std::uint16_t address, std::uint8_t readRegAddress, float scale) -> XYZ
		{
			std::vector<std::uint8_t> rawBuffer(config.readSize, 0);

			int writeStatus = Sensor::write(address, 1, { readRegAddress }, I2cFlags::NoStop);
			if (writeStatus != 1)
			{
				std::cerr << "Invalid IMU write ";
			}

			int readStatus = Sensor::read(address, config.readSize, rawBuffer);
			if (readStatus < config.readSize)
			{
				std::cerr << "Invalid IMU read ";
			}

			int16_t rawDataCombinedX = ((int16_t)rawBuffer[1] << 8 | (int16_t)rawBuffer[0]);
			int16_t rawDataCombinedY = ((int16_t)rawBuffer[3] << 8 | (int16_t)rawBuffer[2]);
			int16_t rawDataCombinedZ = ((int16_t)rawBuffer[5] << 8 | (int16_t)rawBuffer[4]);

			return
			{
				(float)rawDataCombinedX * scale,
				(float)rawDataCombinedY * scale,
				(float)rawDataCombinedZ * scale
			};
		};

	results.accl = read3Axis(config.agAddress, config.acclReadReg, config.acclScale);
	results.gyro = read3Axis(config.agAddress, config.gyroReadReg, config.gyroScale);
	results.magn = read3Axis(config.magnAddress, config.magnReadReg, config.magnScale);

	return results;
}

IMUSensor::~IMUSensor()
{
}