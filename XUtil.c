#include "XUtil.h"
#include <math.h>

int GetDecimalDigits(double v)
{
	int digits = 0;
	double decimal = fmod(v, 1.);
	if (!IsZeroFloat(decimal)) {
#if 1
		for (digits = 1;digits < 6;digits++)
		{
			double temp = decimal*pow(10., digits);
			temp = fmod(temp, 1.);
			if (IsZeroFloat(temp)) {
				break;
			}
		}
#else
		char buf[16] = { 0 };
		sprintf(buf, "%.6lf", decimal);
		for (digits = 6; digits > 0; digits--)
		{
			//x.xxxxxx
			if (buf[1 + digits] != '0') {
				break;
			}
		}
#endif//
	}
	return digits;
}

