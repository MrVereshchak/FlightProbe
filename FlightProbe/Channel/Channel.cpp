#include "Channel.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include "IMUSensor.hpp"
#include "ImuConfig.hpp"
#include "PressureConfig.hpp"
#include "PressureSensor.hpp"
#include "Sensor.hpp"
#include "SensorBus.hpp"

Channel::Channel(std::string name, std::unique_ptr<SensorBus> busParam) : chName(name), pBus(std::move(busParam))
{
}

std::uint32_t Channel::getAaId() const
{
    return pBus->getAaId();
}

void Channel::addPressureSensor(PressureConfig::Device configParam)
{
    std::unique_ptr<Sensor> sensor = std::make_unique<PressureSensor>(this->pBus.get(), configParam);
    pSensors.push_back(std::move(sensor));
}

void Channel::addIMUSensor(ImuConfig::Device configParam)
{
    std::unique_ptr<Sensor> sensor = std::make_unique<IMUSensor>(this->pBus.get(), configParam);
    pSensors.push_back(std::move(sensor));
}

Channel::~Channel()
{
}