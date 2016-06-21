#ifndef A_FUZZY_24BIN_H
#define A_FUZZY_24BIN_H
namespace ACEDD_DESC {
	class Fuzzy24BIN
	{
	public:
		static void ApplyFitler(const double hsv[], const double fuzzy10BinHist[], double fuzzy24BinHist[]);
	private:
		static void computeHistogram(const double hist10bin[], const double sA[], const double vA[], double hist24bin[]);
		static void findMembershipValue(double in, int tSize, const double thresholds[], double result[]);
	};
}

#endif // A_FUZZY_24BIN_H
