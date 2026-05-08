#include <chrono>   // for time measurement
#include <conio.h>  // for _kbhit and _getch
#include <cstdint>  // for std::uint*_t types
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>   // for sleep_for
#include <utility>  // for std::move
#include <variant>
#include <vector>

#include "AaConfig.hpp"
#include "AardvarkI2C.hpp"
#include "AoaCalc.hpp"
#include "AoaPresenter.hpp"
#include "AppConfig.hpp"
#include "Channel.hpp"
#include "PressureConfig.hpp"
#include "Sensor.hpp"
#include "SensorResult.hpp"

// main should read like a story
int main()
{
    std::vector<Channel> channels;
    AoaCalc aoaCalculator;
    // TODO: move declarations
    AoaPresenter::AoaIndicator aoaIndicator;
    std::ofstream calibLogs;

    // TODO: research #if vs if constexpr
    if constexpr (Config::DISPLAY_MODE)
    {
        if (!AoaPresenter::enableVirtualTerminal())
        {
            std::cerr << "Failed to enable virtual terminal mode.\n";
            return 1;
        }

        AoaPresenter::beginFullScreenUi();
    }

    // TODO: move to the AardvarkI2C class
    for (std::unique_ptr<AardvarkI2C>& aaDevice : AardvarkI2C::enumerateDevices(true))
    {
        std::string tmpName = std::format("Channel: {}", aaDevice->getBusName());

        channels.emplace_back(tmpName, std::move(aaDevice));

        auto &channel = channels.back();

        std::uint32_t deviceId = channel.getAaId();

        if (deviceId == AaConfig::ID_PFWD)
        {
            channel.addPressureSensor(PressureConfig::MS4525DO_FWD);
        }
        if (deviceId == AaConfig::ID_P45)
        {
            channel.addPressureSensor(PressureConfig::MS4525DO_45);
        }
#if 0
        else if (deviceId == AaConfig::ID_PST_IMU)
        {
            channel.addPressureSensor(SSCSRNN1_STAT);
            channel.addIMUSensor(LSM9DS1);
        }
#endif
    }

    if constexpr (Config::CALIBRATION_MODE)
    {
        std::string aoaCalibStr, fanLevelCalibStr;

        std::cout << "\nEnter AoA and fan level that you are using for calibration:\n";
        std::cin >> aoaCalibStr >> fanLevelCalibStr;

        // Get current system date and time
        auto now = std::chrono::system_clock::now();
        auto nowSeconds = std::chrono::floor<std::chrono::seconds>(now);
        auto localSeconds = std::chrono::zoned_time{ std::chrono::current_zone(), nowSeconds };
        std::string dateTimeStr = std::format("{:%Y-%m-%d_%H-%M-%S}", localSeconds);

        calibLogs.open(std::format("{}_{}_{}.csv", dateTimeStr, aoaCalibStr, fanLevelCalibStr));

        calibLogs << "count,timestamp_ms,aoa_valid,pfwd_raw,p45_raw,pfwd_filt,p45_filt,coeff0,coeff1,coeff2,coeff3,aoa_raw,aoa_filt\n";
    }

    // Count & Time
    std::uint32_t counter = 0;
    // auto = std::chrono::steady_clock::time_point
    auto startTime = std::chrono::steady_clock::now();

    while (true)
    {
        // auto = std::chrono::steady_clock::time_point
        auto currentTime = std::chrono::steady_clock::now();
        // auto = std::chrono::milliseconds
        auto loopTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);

        // Keyboard access without stopping the loop
        if (_kbhit())
        {
            char ch = _getch();

            if (ch == 'q' || ch == 'Q')
            {
                std::cout << std::endl << "Stopping loop..." << std::endl;
                break;
            }
        }

        bool havePfwd = false;
        bool haveP45 = false;
        int pfwdRawCount = 0;
        int p45RawCount = 0;
        int pfwdCorrectedCount = 0;
        int p45CorrectedCount = 0;

        if constexpr (!Config::DISPLAY_MODE)
        {
            // Print the timestamp
            std::cout << std::format("[{:05}] [{:08}ms] ", counter, loopTime.count());
        }

        for (Channel &channel : channels)
        {
            for (std::unique_ptr<Sensor> &sensor : channel.pSensors)
            {
                SensorResult rawResult = sensor->getDecodedData();

                if (PressureResult *pPressureResult = std::get_if<PressureResult>(&rawResult))
                {
                    if constexpr (!Config::DISPLAY_MODE)
                    {
                        std::cout << "(" << sensor->getBusName()
                            << std::format(" raw:{:>5} corr:{:>6} ",
                                pPressureResult->rawCount,
                                pPressureResult->correctedCount);
                    }

                    if (pPressureResult->valid)
                    {
                        if (pPressureResult->sensorType == PressureConfig::SensorType::PFWD)
                        {
                            pfwdRawCount = pPressureResult->rawCount;
                            pfwdCorrectedCount = pPressureResult->correctedCount;
                            havePfwd = true;
                        }
                        else if (pPressureResult->sensorType == PressureConfig::SensorType::P45)
                        {
                            p45RawCount = pPressureResult->rawCount;
                            p45CorrectedCount = pPressureResult->correctedCount;
                            haveP45 = true;
                        }
                    }
                }
#if 0
                else if (IMUResult* pIMUResult = std::get_if<IMUResult>(&rawResult))
                {
                    std::cout << sensor->getBusName()
                        << std::format(" Accl: ({:>6.1f} {:>6.1f} {:>6.1f})", pIMUResult->accl.x, pIMUResult->accl.y, pIMUResult->accl.z)
                        << std::format(" Gyro: ({:>6.1f} {:>6.1f} {:>6.1f})", pIMUResult->gyro.x, pIMUResult->gyro.y, pIMUResult->gyro.z)
                        << std::format(" Magn: ({:>6.1f} {:>6.1f} {:>6.1f})", pIMUResult->magn.x, pIMUResult->magn.y, pIMUResult->magn.z);
                }
#endif
            }
        }

        if (havePfwd && haveP45)
        {
            AoaResult aoaResult = aoaCalculator.update(pfwdRawCount, pfwdCorrectedCount, p45RawCount, p45CorrectedCount);

            if constexpr (Config::DISPLAY_MODE)
            {
                AoaPresenter::AoaIndicatorFrame frame = aoaIndicator.update(aoaResult.valid, aoaResult.coeff0);
                AoaPresenter::drawIndicatorScreen(frame, counter, loopTime.count());
            }

            if constexpr (Config::CALIBRATION_MODE)
            {
                calibLogs << std::format(
                    "{},{},{},{},{},{},{},{},{},{},{},{},{}\n",
                    counter,
                    loopTime.count(),
                    aoaResult.valid,
                    aoaResult.pfwdRawCount,
                    aoaResult.p45RawCount,
                    aoaResult.pfwdFiltered,
                    aoaResult.p45Filtered,
                    aoaResult.coeff0,
                    aoaResult.coeff1,
                    aoaResult.coeff2,
                    aoaResult.coeff3,
                    aoaResult.aoaRaw,
                    aoaResult.aoaFiltered
                );

                if (counter >= Config::CALIBRATION_COUNT)
                {
                    calibLogs.close();
                    std::cout << std::endl << "Stopping loop..." << std::endl;
                    break;
                }
            }

            if constexpr (!Config::DISPLAY_MODE)
            {
                if (aoaResult.valid)
                {
                    std::cout << std::format(
                        "|pfwd_f:{:>7.3f} p45_f:{:>7.3f} coeff:{:>7.4f} raw:{:>7.3f} filt:{:>7.3f}",
                        aoaResult.pfwdFiltered,
                        aoaResult.p45Filtered,
                        aoaResult.coeff0,
                        aoaResult.aoaRaw,
                        aoaResult.aoaFiltered
                    );
                }
                else
                {
                    std::cout << std::format(
                        " | AOA waiting for airflow (pfwdF:{:>7.2f})",
                        aoaResult.pfwdFiltered
                    );
                }
            }
        }
        else
        {
            if constexpr (!Config::DISPLAY_MODE)
            {
                std::cout << " | AOA waiting for airflow";
            }
        }

        if constexpr (!Config::DISPLAY_MODE)
        {
            std::cout << std::endl;
        }

        ++counter;

        std::this_thread::sleep_for(std::chrono::milliseconds(12)); // 50 Hz, but with 50 bytes we use only 0.5 milliseconds if the bitrate is 100 kHz (100,000 bits per second)
    }

    if constexpr (Config::DISPLAY_MODE)
    {
        AoaPresenter::endFullScreenUi();
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu