
#include "Speed.h"


	Speed::Speed() {}

	Speed::Speed(float a, float b)
	{
		this->x = a;
		this->y = b;
	}

	Speed
		Speed::Uint64ToSpeed(uint64_t value)
	{
		Speed speed;
		float * f = (float *)&value;
		speed.x = *(f + 1);
		speed.y = *f;
		return speed;
	}

	uint64_t
		Speed::SpeedToUint64(Speed speed)
	{
		uint64_t ui;
		float *f = (float *)&ui;
		*f = speed.y;
		*(f + 1) = speed.x;
		return ui;
	}
	uint32_t 
		Speed::DoubleToUint32(double speed)
	{
		uint32_t ui;
		float *f = (float *)&ui;
		*f = speed;
		return ui;
	}
	void
		Speed::Show()
	{
		std::cout << "X = " << x << "Y =  " << y << " " << std::endl;
	}

