#ifndef  GPS_h
#define GPS_h

#include <iostream>
#include <stdint.h>

	class GPS
	{
	public:
		float x;
		float y;
		GPS();
		GPS(float a, float b);
		~GPS();
		static GPS Uint64ToGPS(uint64_t value);
		static uint64_t GPSToUint64(GPS gps);
		static GPS Uint32ToGPS(uint32_t valuex,uint32_t valuey);
		static uint32_t GPSXToUint32(GPS gps);
		static uint32_t GPSYToUint32(GPS gps);
		void Show();
	};


#endif //  GPS_h

