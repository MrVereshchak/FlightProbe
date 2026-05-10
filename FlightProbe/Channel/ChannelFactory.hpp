#pragma once

#include <vector>

#include "Channel.hpp"

class ChannelFactory
{
public:
    ChannelFactory();

    // TODO: add a method that creates only one channel
    static std::vector<Channel> createAardvarkChannels(bool printDetails = false);

    ~ChannelFactory();

private:
};