#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define THRESCORR 1e-30

void crosscorrmax(const double* x, int xSize, const double * y, int ySize,
		double * xDev, double * yDelayedDev, double ySumCuadraticDiff,
		double &max) {
	//! Calculate the denominator (product of standard deviations)
	double xSumCuadraticDiff = 0.0;
	for (int i = 0; i < xSize; i++) {
		xSumCuadraticDiff += pow(xDev[i], 2);
	}

	double denom = sqrt(xSumCuadraticDiff * ySumCuadraticDiff);
	if (denom < THRESCORR) {
		return;
	}

	int maxDelay = ySize - xSize + 1;
	for (int delay = 0; delay < maxDelay; delay++) {
		double xySum = 0.0;
		for (int i = 0; i < xSize; i++) {
			xySum += xDev[i] * yDelayedDev[i + delay];
		}

		xySum *= xSize / denom;
		if (xySum > max) {
			max = xySum;
		}
	}
}

double findScore(const double* wave, int waveSize, const double* pattern,
		int patternSize) {
	double score = 0.0;
	int minSubvectorLength = 2;

	double yMean = 0.0;
	{
		for (int i = 0; i < patternSize; i++) {
			if (pattern[i] >= 0.0 && pattern[i] <= 15.0)
				yMean += pattern[i];
		}
		yMean /= patternSize;
	}

	double ySumCuadraticDiff = 0.0;
	for (int i = 0; i < patternSize; i++) {
		ySumCuadraticDiff += pow(pattern[i] - yMean, 2);
	}

	std::vector<double> yDelayedDev(patternSize, 0);
	for (int i = 0; i < patternSize; i++) {
		yDelayedDev[i] = pattern[i] - yMean;
	}

	std::vector<double> waveCumSum(waveSize + 1, 0.0);
	{
		double cumsum = 0.0;
		for (int i = 0; i < waveSize; ++i) {
			waveCumSum[i] = cumsum;
			cumsum += wave[i];
		}
		waveCumSum[waveSize] = cumsum;
	}
	double * waveCumSumPtr = waveCumSum.data();

	std::vector<double> xDev(waveSize, 0);
	double * xDevPtr = xDev.data();

	for (int subvectorStart = 0;
			subvectorStart <= waveSize - minSubvectorLength; subvectorStart++) {
		for (int subvectorLength = minSubvectorLength;
				subvectorLength <= MIN(waveSize - subvectorStart, patternSize);
				subvectorLength++) {
			double xMean = (waveCumSumPtr[subvectorStart + subvectorLength]
					- waveCumSumPtr[subvectorStart]) / subvectorLength;
			const double * xPtr = wave + subvectorStart;
			{
				for (int i = 0; i < subvectorLength; ++i) {
					xDevPtr[i] = xPtr[i] - xMean;
				}
			}
			crosscorrmax(xPtr, subvectorLength, pattern, patternSize, xDevPtr,
					yDelayedDev.data(), ySumCuadraticDiff, score);

		}
	}

	return score;
}

int main() {
	size_t P, W;
	std::cin >> P >> W;

	std::vector<double> pattern, wave;
	pattern.reserve(P);
	wave.reserve(W);

	while (P--) {
		double p;
		std::cin >> p;
		pattern.push_back(p);
	}

	std::cin.ignore();

	while (W--) {
		double w;
		std::cin >> w;
		wave.push_back(w);
	}

	double score;
	for (size_t i = 0; i < 90000; ++i) {
		score = findScore(wave.data(), wave.size(), pattern.data(),
				pattern.size());
	}
	std::cout << std::setprecision(5) << score << std::endl;
}
