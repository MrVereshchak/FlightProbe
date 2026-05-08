#pragma once

#include "PressureConfig.hpp"
#include "Sensor.hpp"
#include "SensorBus.hpp"
#include "SensorResult.hpp"

class PressureSensor : public Sensor
{
private:
    PressureConfig::Device config;

public:
    PressureSensor(SensorBus *pBus, PressureConfig::Device configParam);

    // Block copying to prevent dangling pInterface pointers
    PressureSensor(const PressureSensor &) = delete;
    PressureSensor &operator=(const PressureSensor &) = delete;

    PressureSensor(PressureSensor &&) = default;
    PressureSensor &operator=(PressureSensor &&) = default;

    SensorResult getDecodedData() override;

    ~PressureSensor();
};