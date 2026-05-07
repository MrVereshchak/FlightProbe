#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "aardvark.h"
#include "SensorBus.hpp"

class AardvarkI2C : public SensorBus
{
public:
	struct AaPortInfo
	{
		std::uint16_t port;
		std::uint32_t uniqueId;
	};

private:
	struct AaHandle {
		Aardvark    sessionHandle; // active session ID from aa_open
		std::uint32_t    uniqueId;      // serial number
		std::uint16_t    port;          // current USB index
		std::string name;          // aardvark name

		// Status and Configuration
		AardvarkVersion aaVersion;
		int status,
			config,
			bitrate,
			pullup;

        AaHandle() : 
            sessionHandle(),
            uniqueId(0),
            port(0),          
            name(""),
            aaVersion(),
            status(-1),
            config(-1),
            bitrate(-1),
            pullup(-1)
        {
        }
	};

	AaHandle aaHandle;

private:
	AardvarkI2C(AaHandle&& aaHandlePassed);

	static AardvarkI2cFlags toAardvarkFlags(I2cFlags flag);

public:
	// Physical hardware is unique, so no copying, only moving
	AardvarkI2C(AardvarkI2C&) = delete;
	AardvarkI2C& operator=(AardvarkI2C&) = delete;

	AardvarkI2C(AardvarkI2C&&) = default;
	AardvarkI2C& operator=(AardvarkI2C&&) = default;

	/**
	 * Calls aa_find_devices_ext.
	 * Returns a list of available hardware ports without claiming them.
	 */
	static std::vector<AaPortInfo> discover(bool printDetails = false);

	/**
	* Claims the hardware handle (aa_open), sets bitrate, config, pullups, and fetches version info.
	* Returns a ready-to-use AardvarkI2C object or nullptr on failure.
	*/
	static std::unique_ptr<AardvarkI2C> open(const AaPortInfo& info, bool printDetails = false);

	/**
	* Runs Discovery and tries to Open every device found.
	* Use this in main() for a "one-line" setup.
	*/
	static std::vector<std::unique_ptr<AardvarkI2C>> enumerateDevices(bool printDetails = false);

	int read(std::uint16_t address, std::uint16_t readSize, std::vector<std::uint8_t>& buffer) override;
	int write(std::uint16_t address, std::uint16_t writeSize, const std::vector<std::uint8_t>& buffer, I2cFlags flag = I2cFlags::None) override;
	std::string getBusName() const override;
	std::uint32_t getAaId() const override;

	~AardvarkI2C();
};