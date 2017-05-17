#ifndef SPEED
#define SPEED
#include <iostream>
#include <stdint.h>

	class Speed
	{
	public:
		float x;
		float y;
		Speed();
		Speed(float a, float b);
		static Speed Uint64ToSpeed(uint64_t value);
		static uint64_t SpeedToUint64(Speed speed);
		static uint32_t DoubleToUint32(double speed);
		void Show();
	};

#endif // !SPEED


