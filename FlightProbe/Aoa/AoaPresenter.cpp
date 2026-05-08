#include "AoaPresenter.hpp"
#include <cstdint>
#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

namespace AoaPresenter
{
    AoaIndicatorFrame::AoaIndicatorFrame() : valid(false),
                                             state(AoaIndicatorState::NO_READING),
                                             title("NO DATA"),
                                             color("\x1b[37m"),
                                             bigSymbol("?"),
                                             helperText("Waiting for valid airflow"),
                                             coeff(0.0f),
                                             energy01(0.0f)
    {
    }

    float AoaIndicator::clamp01(float x)
    {
        if (x < 0.0f)
            return 0.0f;
        if (x > 1.6f)
            return 1.6f;
        return x;
    }

    AoaIndicatorFrame AoaIndicator::update(bool valid, float coeff)
    {
        AoaIndicatorFrame frame;
        frame.valid = valid;
        frame.coeff = coeff;

        if (!valid)
        {
            return frame;
        }

        const float coeffFast = 1.6f;
        const float coeffSlow = 0.8f;
        const float span = coeffFast - coeffSlow;

        // TODO: double-check the formula
        frame.energy01 = clamp01((coeffFast - coeff) / span);

        if (coeff > 1.4f)
        {
            frame.state = AoaIndicatorState::FAST;
            frame.title = "FAST";
            frame.color = "\x1b[36m"; // cyan
            frame.bigSymbol = "VVV";
            frame.helperText = "Low AOA / high margin";
        }
        else if (coeff > 1.2f)
        {
            frame.state = AoaIndicatorState::SLIGHTLY_FAST;
            frame.title = "SLIGHTLY FAST";
            frame.color = "\x1b[32m"; // green
            frame.bigSymbol = " V ";
            frame.helperText = "Approaching target band";
        }
        else if (coeff > 1.05f)
        {
            frame.state = AoaIndicatorState::ON_SPEED;
            frame.title = "ON-SPEED";
            frame.color = "\x1b[33m"; // yellow
            frame.bigSymbol = " O ";
            frame.helperText = "Target AOA cue band";
        }
        else if (coeff > 0.94f)
        {
            frame.state = AoaIndicatorState::SLOW;
            frame.title = "SLOW";
            frame.color = "\x1b[35m"; // magenta
            frame.bigSymbol = " ^ ";
            frame.helperText = "Higher AOA / lower margin";
        }
        else
        {
            frame.state = AoaIndicatorState::LOW_MARGIN;
            frame.title = "STALL MARGIN LOW";
            frame.color = "\x1b[31m"; // red
            frame.bigSymbol = "!!!";
            frame.helperText = "Approaching stall region";
        }

        return frame;
    }

    bool enableVirtualTerminal()
    {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode))
        {
            return false;
        }

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, dwMode))
        {
            return false;
        }

        return true;
    }

    std::string makeBar(float value01, int width)
    {
        if (value01 < 0.0f)
            value01 = 0.0f;
        if (value01 > 1.0f)
            value01 = 1.0f;

        int filled = (int)(value01 * width + 0.5f);
        if (filled < 0)
            filled = 0;
        if (filled > width)
            filled = width;

        std::string out = "[";
        for (int i = 0; i < width; ++i)
        {
            out += (i < filled) ? "#" : "-";
        }
        out += "]";
        return out;
    }

    std::string makeBigArt(const AoaIndicatorFrame &frame)
    {
        if (!frame.valid)
        {
            return "              ?????              \n"
                   "            ??     ??            \n"
                   "                  ??             \n"
                   "                ??               \n"
                   "                ??               \n";
        }

        switch (frame.state)
        {
            case AoaIndicatorState::FAST:
                return "                V                \n"
                       "               VVV               \n"
                       "              VVVVV              \n"
                       "                V                \n"
                       "                V                \n";

            case AoaIndicatorState::SLIGHTLY_FAST:
                return "                                 \n"
                       "                V                \n"
                       "               VVV               \n"
                       "                V                \n"
                       "                                 \n";

            case AoaIndicatorState::ON_SPEED:
                return "              OOOOO              \n"
                       "             OO   OO             \n"
                       "             OO   OO             \n"
                       "             OO   OO             \n"
                       "              OOOOO              \n";

            case AoaIndicatorState::SLOW:
                return "                                 \n"
                       "                ^                \n"
                       "               ^^^               \n"
                       "                ^                \n"
                       "                                 \n";

            case AoaIndicatorState::LOW_MARGIN:
                return "               ! ! !             \n"
                       "               ! ! !             \n"
                       "               ! ! !             \n"
                       "                                 \n"
                       "               ! ! !             \n";

            default:
                return "";
        }
    }

    void beginFullScreenUi()
    {
        // Alternate buffer + hide cursor + clear screen
        std::cout << "\x1b[?1049h\x1b[?25l\x1b[2J";
    }

    void endFullScreenUi()
    {
        // Reset color + show cursor + leave alternate buffer
        std::cout << "\x1b[0m\x1b[?25h\x1b[?1049l";
    }

    void drawIndicatorScreen(const AoaIndicatorFrame &frame, std::uint32_t counter, std::int64_t ms)
    {
        std::ostringstream ss;

        // Home cursor, clear screen, build full frame in one write
        ss << "\x1b[H\x1b[2J";

        ss << "\n";
        ss << "=============================================================\n";
        ss << "                 ANGLE OF ATTACK DEMONSTRATOR                \n";
        ss << "=============================================================\n\n";

        ss << std::format("Frame: {:05}    Time: {:08} ms\n\n", counter, ms);

        ss << frame.color;
        ss << std::format("STATE: {}\n\n", frame.title);
        ss << makeBigArt(frame);
        ss << "\x1b[0m";
        ss << "\n";

        ss << std::format("Cue      : {}\n", frame.helperText);
        ss << std::format("Energy   : {}\n", makeBar(frame.energy01, 40));

        if (frame.valid)
        {
            ss << std::format("Coeff    : {:>7.3f}\n", frame.coeff);
        }
        else
        {
            ss << "Coeff    : ---\n";
        }

        ss << "\n";
        ss << "Legend:\n";
        ss << "  VVV = fast / low AOA\n";
        ss << "   O  = on-speed cue band\n";
        ss << "   ^  = slow / higher AOA\n";
        ss << " !!!  = stall margin getting low\n";
        ss << "\n";
        ss << "Press Q to quit.\n";

        std::cout << ss.str() << std::flush;
    }
}
