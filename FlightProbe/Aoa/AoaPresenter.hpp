#pragma once

#include <cstdint>
#include <string>

namespace AoaPresenter
{
    enum class AoaIndicatorState
    {
        NO_READING,
        FAST,
        SLIGHTLY_FAST,
        ON_SPEED,
        SLOW,
        LOW_MARGIN
    };

    struct AoaIndicatorFrame
    {
        bool valid;
        AoaIndicatorState state;
        std::string title;
        std::string color;
        std::string bigSymbol;
        std::string helperText;
        float coeff;
        float energy01; // 0 fast side, 1 slow side

        AoaIndicatorFrame();
    };

    class AoaIndicator
    {
    private:
        float clamp01(float x);

    public:
        AoaIndicatorFrame update(bool valid, float coeff);
    };

    bool enableVirtualTerminal();

    std::string makeBar(float value01, int width);
    std::string makeBigArt(const AoaIndicatorFrame &frame);

    void beginFullScreenUi();
    void endFullScreenUi();

    void drawIndicatorScreen(const AoaIndicatorFrame &frame, std::uint32_t counter, std::int64_t ms);
}