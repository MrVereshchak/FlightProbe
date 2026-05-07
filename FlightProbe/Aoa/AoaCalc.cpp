#include "AoaCalc.hpp"

#include <algorithm>
#include <vector>

#include "AoaCalcConfig.hpp"

AoaResult::AoaResult() :
	valid(false),
	pfwdRawCount(0),
	p45RawCount(0),
	pfwdFiltered(0.0f),
	p45Filtered(0.0f),
	coeff0(0.0f),
	coeff1(0.0f),
	coeff2(0.0f),
	coeff3(0.0f),
	aoaRaw(0.0f),
	aoaFiltered(0.0f)
{
}

AoaCalc::AoaCalc() : aoaFiltered(0.0f)
{
}

float AoaCalc::updateMedian(std::vector<float>& window, float value)
{
	window.push_back(value);

	if (window.size() > 5)
	{
		window.erase(window.begin());
	}

	std::vector<float> sorted = window;
	std::sort(sorted.begin(), sorted.end());

	return sorted[sorted.size() / 2];
}

float AoaCalc::updateAverage(std::vector<float>& window, float value)
{
	window.push_back(value);

	if (window.size() > PRESSURE_FILTER_WINDOW)
	{
		window.erase(window.begin());
	}

	float sum = 0.0f;

	for (float item : window)
	{
		sum += item;
	}

	return sum / (float)window.size();
}

float AoaCalc::evalAoaCurve(float coeff)
{
	return -41.522f * coeff + 47.122f;

#if 0
	return GEN2_AOA_CURVE_A * coeff * coeff * coeff
		+ GEN2_AOA_CURVE_B * coeff * coeff
		+ GEN2_AOA_CURVE_C * coeff
		+ GEN2_AOA_CURVE_D;
#endif
}

AoaResult AoaCalc::update(int pfwdRawCount, int pfwdCorrectedCount, int p45RawCount, int p45CorrectedCount)
{
	AoaResult result;

	result.pfwdRawCount = pfwdRawCount;
	result.p45RawCount = p45RawCount;

	float pfwdMedian = updateMedian(pfwdMedianWindow, (float)pfwdCorrectedCount);
	float p45Median = updateMedian(p45MedianWindow, (float)p45CorrectedCount);

	result.pfwdFiltered = updateAverage(pfwdAvgWindow, pfwdMedian);
	result.p45Filtered = updateAverage(p45AvgWindow, p45Median);

	// floor for calculations
	//if (result.pfwdFiltered <= 0)
	//{
	//	return result;
	//}

	result.coeff0 = result.pfwdFiltered / result.p45Filtered;
	result.coeff1 = (result.p45Filtered - result.pfwdFiltered) / result.p45Filtered;
	result.coeff2 = result.p45Filtered / (result.p45Filtered + result.pfwdFiltered);
	result.coeff3 = (result.p45Filtered - result.pfwdFiltered) / (result.p45Filtered + result.pfwdFiltered);

	// TODO: add evalAoaCurve for the others coeff
	float aoaRawMedian = updateMedian(rawAoaMedianWindow, evalAoaCurve(result.coeff0));
	result.aoaRaw = updateAverage(rawAoaAvgWindow, aoaRawMedian) + 7.0f;

	//result.aoaRaw = evalAoaCurve(result.coeff0);
	aoaFiltered = AOA_EMA_ALPHA * result.aoaRaw + (1.0f - AOA_EMA_ALPHA) * aoaFiltered;
	result.aoaFiltered = aoaFiltered;
	result.valid = true;

	return result;
}