#ifndef A_FUZZY_10BIN_H
#define A_FUZZY_10BIN_H

namespace ACEDD_DESC {
	class Fuzzy10BIN{
	public:
		static void ApplyFilter(const double hsv[], double hist10bin[] );
	private:
		static void MultiParticipateDefazzificator(double hA[], double sA[], double vA[], double resultTable[]);
		static void findMembershipValue(double in, int tSize, const double thresholds[], double actArr[]);
	};
}
#endif // A_FUZZY_10BIN_H
