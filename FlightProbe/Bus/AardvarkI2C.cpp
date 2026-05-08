#include "AardvarkI2C.hpp"

#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "AaConfig.hpp"
#include "aardvark.h"
#include "AppConfig.hpp"
#include "SensorBus.hpp"

AardvarkI2C::AardvarkI2C(AaHandle &&aaHandlePassed) : aaHandle(std::move(aaHandlePassed))
{
}

AardvarkI2cFlags AardvarkI2C::toAardvarkFlags(I2cFlags flag)
{
    switch (flag)
    {
        case I2cFlags::None:
            return AA_I2C_NO_FLAGS;
        case I2cFlags::NoStop:
            return AA_I2C_NO_STOP;
        default:
            return AA_I2C_NO_FLAGS;
    }
}

std::vector<AardvarkI2C::AaPortInfo> AardvarkI2C::discover(bool printDetails)
{
    std::vector<std::uint16_t> rawPorts(Config::EXPECTED_NUM_OF_DEVICES);
    std::vector<std::uint32_t> rawIds(Config::EXPECTED_NUM_OF_DEVICES);

    // Aardvark detection
    const int foundDevicesCount = aa_find_devices_ext(
        Config::EXPECTED_NUM_OF_DEVICES, rawPorts.data(), Config::EXPECTED_NUM_OF_DEVICES, rawIds.data());

    if (foundDevicesCount < 0)
    {
        if (printDetails)
        {
            const char *statusStr = aa_status_string(foundDevicesCount);

            std::cerr << "aa_find_devices_ext failed: "
                      << foundDevicesCount
                      << " = "
                      << (statusStr ? statusStr : "aa_status_string returned null")
                      << std::endl;
        }
        return {};
    }

    if (printDetails)
    {
        std::cout << "Number of found Aardvark devices: " << foundDevicesCount << std::endl;
    }

    std::vector<AaPortInfo> aaPorts;
    aaPorts.reserve(foundDevicesCount);

    for (int i = 0; i < foundDevicesCount; ++i)
    {
        aaPorts.emplace_back(rawPorts[i], rawIds[i]);

        if (printDetails)
        {
            std::cout << std::format(
                "Port: {:#06x} | Aardvark ID: {:#010x}\n",
                aaPorts[i].port, aaPorts[i].uniqueId);
        }
    }

    return aaPorts;
}

std::unique_ptr<AardvarkI2C> AardvarkI2C::open(const AaPortInfo &info, bool printDetails)
{
    AaHandle tempHandle;
    tempHandle.port = info.port;
    tempHandle.uniqueId = info.uniqueId;

    switch (tempHandle.uniqueId)
    {
        case AaConfig::ID_PFWD:
            tempHandle.name = "pFWD";
            break;
        case AaConfig::ID_P45:
            tempHandle.name = "p45 ";
            break;
        case AaConfig::ID_PST_IMU:
            tempHandle.name = "pStat";
            break;
        default:
            tempHandle.name = "Unknown AA";
    }

    auto errorHandler = [&](const std::string &msg, int code, bool ifClose = true) -> std::unique_ptr<AardvarkI2C> {
        if (printDetails)
        {
            std::cerr << msg << "\n Error: " << code << std::endl;
        }

        if (ifClose)
        {
            int closeStatus = aa_close(tempHandle.sessionHandle);
            if (closeStatus < 0)
            {
                std::cerr
                    << "Error "
                    << closeStatus
                    << " closing Aardvark: "
                    << std::format("{:#010x} ", tempHandle.uniqueId)
                    << std::endl;
            }
        }

        return nullptr;
    };

    tempHandle.sessionHandle = aa_open(tempHandle.port);
    if (tempHandle.sessionHandle <= 0)
    {
        return errorHandler("Failed to open Aardvark on port.", tempHandle.sessionHandle, false);
    }

    // Aardvark versions of software, hardware, and firmware
    tempHandle.status = aa_version(tempHandle.sessionHandle, &tempHandle.aaVersion);
    if (tempHandle.status < 0)
    {
        return errorHandler("Error fetching version info.", tempHandle.status);
    }

    // Config\Enable I2C
    tempHandle.config = aa_configure(tempHandle.sessionHandle, AaConfig::CONFIG);
    if (tempHandle.config < 0)
    {
        return errorHandler("Error during configuration.", tempHandle.config);
    }

    // Set bitrate
    tempHandle.bitrate = aa_i2c_bitrate(tempHandle.sessionHandle, AaConfig::BITRATE);
    if (tempHandle.bitrate < 0)
    {
        return errorHandler("Error setting bitrate.", tempHandle.bitrate);
    }

    // Disable Aardvark pullups
    tempHandle.pullup = aa_i2c_pullup(tempHandle.sessionHandle, AaConfig::PULLUP);
    if (tempHandle.pullup < 0)
    {
        return errorHandler("Error setting a pullup.", tempHandle.pullup);
    }

    if (printDetails)
    {
        // TODO: use one format
        std::cout << '\n'
                  << "Aardvark session handle: " << tempHandle.sessionHandle << '\n'
                  << "Aardvark ID: " << std::format("{:#010x} ", tempHandle.uniqueId) << '\n'
                  << "Port number: " << tempHandle.port << '\n'
                  << "Software version: " << std::format("{:#06x} ", tempHandle.aaVersion.software) << '\n'
                  << "Firmware version: " << std::format("{:#06x} ", tempHandle.aaVersion.firmware) << '\n'
                  << "Hardware version: " << std::format("{:#06x} ", tempHandle.aaVersion.hardware) << '\n'
                  << "API version: " << std::format("{:#06x} ", AaConfig::API_VERSION) << '\n'
                  << "Firmware requires software version must be at least: " << std::format("{:#06x} ", tempHandle.aaVersion.sw_req_by_fw) << '\n'
                  << "Software requires firmware version must be at least: " << std::format("{:#06x} ", tempHandle.aaVersion.fw_req_by_sw) << '\n'
                  << "Software requires the API interface must be at least: " << std::format("{:#06x} ", tempHandle.aaVersion.api_req_by_sw) << '\n'
                  << "Config status: " << tempHandle.config << '\n'
                  << "Bitrate in KHz: " << tempHandle.bitrate << '\n'
                  << "Pullups: " << tempHandle.pullup << '\n';
    }

    return std::unique_ptr<AardvarkI2C>(new AardvarkI2C(std::move(tempHandle)));
}

std::vector<std::unique_ptr<AardvarkI2C>> AardvarkI2C::enumerateDevices(bool printDetails)
{
    std::vector<AaPortInfo> portsInfo{ discover(printDetails) };

    // TODO: move aaPointers and other variables to static
    std::vector<std::unique_ptr<AardvarkI2C>> aaPointers;
    aaPointers.reserve(portsInfo.size());

    for (const AaPortInfo &portInfo : portsInfo)
    {
        std::unique_ptr<AardvarkI2C> aaPointer = open(portInfo, printDetails);

        if (aaPointer)
        {
            aaPointers.emplace_back(std::move(aaPointer));
        }
    }

    return aaPointers;
}

int AardvarkI2C::read(std::uint16_t address, std::uint16_t readSize, std::vector<std::uint8_t> &buffer)
{
    return aa_i2c_read(aaHandle.sessionHandle, address, AA_I2C_NO_FLAGS, readSize, buffer.data());
}

int AardvarkI2C::write(std::uint16_t address, std::uint16_t writeSize, const std::vector<std::uint8_t> &buffer, I2cFlags flag)
{
    return aa_i2c_write(aaHandle.sessionHandle, address, toAardvarkFlags(flag), writeSize, buffer.data());
}

std::string AardvarkI2C::getBusName() const
{
    return aaHandle.name;
}

std::uint32_t AardvarkI2C::getAaId() const
{
    return aaHandle.uniqueId;
}

AardvarkI2C::~AardvarkI2C()
{
    int closeStatus = aa_close(aaHandle.sessionHandle);
    if (closeStatus < 0)
    {
        std::cerr
            << "Error " << closeStatus
            << " closing Aardvark: "
            << std::format("{:#010x} ", aaHandle.uniqueId)
            << std::endl;
    }
}