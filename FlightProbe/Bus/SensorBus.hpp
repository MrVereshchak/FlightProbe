#pragma once

#include <cstdint>
#include <string>
#include <vector>

enum class I2cFlags
{
	None,
	NoStop
};

class SensorBus
{
public:
	SensorBus();

	// TODO: implement writeRead
	virtual int read(std::uint16_t address, std::uint16_t size, std::vector<std::uint8_t>& buffer) = 0;
	virtual int write(std::uint16_t address, std::uint16_t size, const std::vector<std::uint8_t>& buffer, I2cFlags flag = I2cFlags::None) = 0;
	virtual std::string getBusName(void) const = 0;
	virtual std::uint32_t getAaId(void) const = 0;

	virtual ~SensorBus();
};