#include "GPSVec.h"

	GPSVec::GPSVec(GPS src, GPS dst)
	{
		x = double(dst.x - src.x);
		y = double(dst.y - src.y);
	}

	GPSVec::~GPSVec()
	{
	}

	double
		GPSVec::CalDotMul(GPSVec vec1, GPSVec vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y;
	}

	double
		GPSVec::CalModule(GPS gps1, GPS gps2)
	{
		GPSVec vec = GPSVec(gps1, gps2);
		return GPSVec::CalModule(vec);
	}

	double
		GPSVec::CalModule(GPSVec vec)
	{
		return sqrt(vec.x * vec.x + vec.y * vec.y);
	}

	double
		GPSVec::CalTheata(GPSVec vec1, GPSVec vec2)
	{
		double tmp = GPSVec::CalDotMul(vec1, vec2) / (GPSVec::CalModule(vec1) * GPSVec::CalModule(vec2));
		//std::cout << "tmp : " << tmp << std::endl;
		const double EPSINON = 0.00000001;
		double  x = tmp - 1;
		double  y = tmp + 1;
		if ((x >= -EPSINON) && (x <= EPSINON))
		{
			return 0.0;
		}
		else if ((y >= -EPSINON) && (y <= EPSINON))
		{
			return 3.14159;
		}
		else
		{
			return acos(tmp);
		}
	}
