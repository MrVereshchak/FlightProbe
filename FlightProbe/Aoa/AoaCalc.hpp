#pragma once

#include <vector>

struct AoaResult
{
	bool valid;
	int pfwdRawCount;
	int p45RawCount;
	float pfwdFiltered;
	float p45Filtered;
	float coeff0;
	float coeff1;
	float coeff2;
	float coeff3;
	float aoaRaw;
	float aoaFiltered;

	AoaResult();
};

class AoaCalc
{
private:
	std::vector<float> rawAoaMedianWindow;
	std::vector<float> rawAoaAvgWindow;
	std::vector<float> pfwdMedianWindow;
	std::vector<float> p45MedianWindow;
	std::vector<float> pfwdAvgWindow;
	std::vector<float> p45AvgWindow;
	float aoaFiltered;

private:
	float updateMedian(std::vector<float>& window, float value);
	float updateAverage(std::vector<float>& window, float value);
	float evalAoaCurve(float coeff);

public:
	AoaCalc();

	AoaResult update(int pfwdRawCount, int pfwdCorrectedCount, int p45RawCount, int p45CorrectedCount);
};