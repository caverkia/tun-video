#ifndef GPS_VECTOR
#define GPS_VECTOR

#include "GPS.h"
#include <cmath>
#include <stdint.h>

	class GPSVec
	{
	public:
		GPSVec(GPS src, GPS dst);
		~GPSVec();
		static double CalDotMul(GPSVec vec1, GPSVec vec2);
		static double CalModule(GPSVec vec);
		static double CalModule(GPS gps1, GPS gps2);
		static double CalTheata(GPSVec vec1, GPSVec vec2);

	public:
		double x;
		double y;
	};

#endif
