#include "GPS.h"

	GPS::GPS() {}
	GPS::GPS(float a, float b)
	{
		this->x = a;
		this->y = b;
	}
	GPS::~GPS() {}
	GPS
		GPS::Uint64ToGPS(uint64_t value)
	{
		GPS gps;
		float * f = (float *)&value;
		gps.x = *(f + 1);
		gps.y = *f;
		return gps;
	}
	uint64_t
		GPS::GPSToUint64(GPS gps)
	{
		uint64_t ui;
		float *f = (float *)&ui;
		*f = gps.y;
		*(f + 1) = gps.x;
		return ui;
	}
	GPS
        GPS::Uint32ToGPS(uint32_t valuex,uint32_t valuey)
    {
        GPS gps;
		float * fx = (float *)&valuex;
		float * fy = (float *)&valuey;
		gps.x = *fx;
		gps.y = *fy;
		return gps;
    }
	uint32_t
        GPS::GPSXToUint32(GPS gps)
    {
        uint32_t ui;
		float *f = (float *)&ui;
		*f = gps.x;
		return ui;
    }
    uint32_t
        GPS::GPSYToUint32(GPS gps)
    {
        uint32_t ui;
		float *f = (float *)&ui;
		*f = gps.y;
		return ui;
    }
	void
		GPS::Show()
	{
		std::cout << "X = " << x << "Y =  " << y << " " << std::endl;
	}

