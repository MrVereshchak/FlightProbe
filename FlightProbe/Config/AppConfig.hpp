#pragma once

#include <cstdint>

namespace Config
{
	constexpr bool		    DEBUG_MODE			    = true;
	constexpr bool		    DISPLAY_MODE			= true;
	constexpr bool		    CALIBRATION_MODE		= false;
	constexpr std::uint16_t CALIBRATION_COUNT	    = 500;
	constexpr std::uint32_t EXPECTED_NUM_OF_DEVICES = 3;
}