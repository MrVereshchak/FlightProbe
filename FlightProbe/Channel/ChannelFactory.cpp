#include "ChannelFactory.hpp"

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "AaConfig.hpp"
#include "AardvarkI2C.hpp"
#include "Channel.hpp"
#include "ImuConfig.hpp"
#include "PressureConfig.hpp"

ChannelFactory::ChannelFactory()
{
}

std::vector<Channel> ChannelFactory::createAardvarkChannels(bool printDetails)
{
    std::vector<Channel> channels;

    auto aaDevices = AardvarkI2C::enumerateDevices(printDetails);

    for (std::unique_ptr<AardvarkI2C> &aaDevice : aaDevices)
    {
        std::string tmpName = std::format("Channel: {}", aaDevice->getBusName());

        channels.emplace_back(tmpName, std::move(aaDevice));

        Channel &channel = channels.back();

        std::uint32_t deviceId = channel.getAaId();

        if (deviceId == AaConfig::ID_PFWD)
        {
            channel.addPressureSensor(PressureConfig::MS4525DO_FWD);
        }
        else if (deviceId == AaConfig::ID_P45)
        {
            channel.addPressureSensor(PressureConfig::MS4525DO_45);
        }
        else if (deviceId == AaConfig::ID_PST_IMU)
        {
            channel.addPressureSensor(PressureConfig::SSCSRNN1_STAT);
            channel.addIMUSensor(ImuConfig::LSM9DS1);
        }

    }

    return channels;
}

ChannelFactory::~ChannelFactory()
{
}