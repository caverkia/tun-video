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

	/*double
		GPSVec::CalModule(GPS gps1, GPS gps2)
	{
		GPSVec vec = GPSVec(gps1, gps2);
		return GPSVec::CalModule(vec);
	}*/

	double
		GPSVec::CalModule(GPSVec vec)
	{
		return sqrt(vec.x * vec.x + vec.y * vec.y);
	}
    ///JinWeidu
    double
        GPSVec::rad(double d)
    {
        return d * M_PI / 180.0;
    }
    double
        GPSVec::CalModule(GPS gps1,GPS gps2)
    {
        double lat1 = gps1.x;
        double lng1 = gps1.y;
        double lat2 = gps2.x;
        double lng2 = gps2.y;
        double radLat1 = GPSVec::rad(lat1);
        double radLat2 = GPSVec::rad(lat2);
        double a = radLat1 - radLat2;
        double b = GPSVec::rad(lng1) - GPSVec::rad(lng2);

        double s = 2 * asin(sqrt(pow(sin(a/2),2) +
        cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
        s = s * EARTH_RADIUS;
        //s = (s * 10000) / 10000;
        return s;
    }
    double
        GPSVec::CalTheate(GPS selfGPS,GPS nghbrGPS,GPS dstGPS)
    {
        double a = GPSVec::CalModule(selfGPS,nghbrGPS);
        double b = GPSVec::CalModule(selfGPS,dstGPS);
        double c = GPSVec::CalModule(nghbrGPS,dstGPS);
        double tmp = (a*a + b*b - c*c)/2*a*b;
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
    GPS
        GPSVec::MillierConvertion(double lat, double lon)
    {
         double L = 6381372 * M_PI * 2;
         //地球周长
         double W=L;
         // 平面展开后，x轴等于周长
         double H=L/2;
         // y轴约等于周长一半
         double mill=2.3;
         // 米勒投影中的一个常数，范围大约在正负2.3之间
         double x = lon * M_PI / 180;
         // 将经度从度数转换为弧度
         double y = lat * M_PI / 180;
         // 将纬度从度数转换为弧度
         y=1.25 * log( tan( 0.25 * M_PI + 0.4 * y ) );// 米勒投影的转换
         // 弧度转为实际距离
         x = ( W / 2 ) + ( W / (2 * M_PI) ) * x;
         y = ( H / 2 ) - ( H / ( 2 * mill ) ) * y;
         GPS gps(x,y);
         return gps;
    }
    Speed
        GPSVec::SpeedChange(std::queue<GPS> q,double v)
    {
        Speed s;
        std::queue<GPS> p = q;
        if(p.size() >= 2)
        {
            for(int i = 0;i < p.size() - 2;i++)
            {
                p.pop();
            }
            GPS g1 = p.front();
            GPS g2 = p.front();
            double a = fabs(g2.x-g1.x);
            double b = fabs(g2.y-g1.y);
            double c = sqrt((g2.x-g1.x)*(g2.x-g1.x) + (g2.y-g1.y)*(g2.y-g1.y));
            s.x = (float)(v*a/c);
            s.y = (float)(v*b/c);
        }
        else
        {
            s.x = (float)(v*sqrt(2)/2);
            s.y = (float)(v*sqrt(2)/2);
        }
        return s;
    }
    ///JinWeidu
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
