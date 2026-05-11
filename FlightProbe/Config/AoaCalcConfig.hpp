#pragma once

// Temporary Gen2 RV-4 flap-0 curve
inline constexpr float GEN2_AOA_CURVE_A = 0.0000f;
inline constexpr float GEN2_AOA_CURVE_B = 8.4845f;
inline constexpr float GEN2_AOA_CURVE_C = 24.0804f;
inline constexpr float GEN2_AOA_CURVE_D = 4.6157f;

// Gen2 defaults
inline constexpr int PRESSURE_FILTER_WINDOW = 15;
inline constexpr float AOA_EMA_ALPHA = 0.1f;     // aoaSmoothing = 10 -> alpha = 1/10
inline constexpr float PFWD_MIN_FOR_AOA = 50.0f; // raw corrected-count threshold, tune later
