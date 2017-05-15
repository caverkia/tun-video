#ifndef GPS_VECTOR
#define GPS_VECTOR

#include "GPS.h"
#include "Speed.h"
#include <queue>
#include <cmath>
#include <stdint.h>

#define EARTH_RADIUS 6371.004

	class GPSVec
	{
	public:
		GPSVec(GPS src, GPS dst);
		~GPSVec();
		static double CalDotMul(GPSVec vec1, GPSVec vec2);
		static double CalModule(GPSVec vec);
		static double CalModule(GPS gps1, GPS gps2);
		static double rad(double d);
		//static double CalModule(GPS gps1,GPS gps2);
		static double CalTheate(GPS selfGPS,GPS nghbrGPS,GPS dstGPS);
		static double CalTheata(GPSVec vec1, GPSVec vec2);
        static GPS MillierConvertion(double lat, double lon);
        static Speed SpeedChange(std::queue<GPS> q,double v);
	public:
		double x;
		double y;
	};

#endif
