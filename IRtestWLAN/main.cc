#include "ir.h"
struct in_addr IR::m_vir_ip;
struct in_addr IR::m_phy_ip;
int IR::beacon_fd;
struct sockaddr_in IR::m_beaconSA;
struct sockaddr_in IR::broadcast_addr;
double IR::m_speed;
GPS IR::m_gps;
std::queue<GPS> IR::q_gps;

GPS g_gps(0,0);
double g_speed = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * gps_thread(void * arg)
{
	int rc;
	struct gps_data_t gps_data;
	if ((rc = gps_open("localhost", "2947", &gps_data)) == -1) 
	{
    		printf("code: %d, reason: %s\n", rc, gps_errstr(rc));
    		return NULL;
	}
	gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
	
    	// wait for 2 seconds to receive data
	//int i = 20;
	while(1)
	{ 
    		if (gps_waiting (&gps_data, 20000000)) 
		{
			// read data //
			if ((rc = gps_read(&gps_data)) == -1) 
			{
				printf("error occured reading gps data. code: %d, reason: %s\n", rc, gps_errstr(rc));
        		} 
			else
			{
            		// Display data from the GPS receiver. //
				if ((gps_data.status == STATUS_FIX) && 
		        	(gps_data.fix.mode == MODE_2D || gps_data.fix.mode == MODE_3D) &&
		        	!isnan(gps_data.fix.latitude) && 
		        	!isnan(gps_data.fix.longitude))
				{
		                	//gettimeofday(&tv, NULL); EDIT: tv.tv_sec isn't actually the timestamp!
					printf("latitude: %f, longitude: %f, speed: %f\n", gps_data.fix.latitude, gps_data.fix.longitude, gps_data.fix.speed); 
					//updata gps and speed
					pthread_mutex_lock(&mutex);
					g_speed = (double)gps_data.fix.speed;
					g_gps = GPS(gps_data.fix.latitude, gps_data.fix.longitude);
		            		pthread_mutex_unlock(&mutex);
				} 
				else 
				{
		        		printf("no GPS data available\n");
					//return GPS(0,90);
				}
        		}
		}
		else
		{
		//perror( "Error" );
        		printf( "Error: %d\n", errno);
			perror("Fail waiting:");
			//break;
		}
	}

	// When you are done... //
	gps_stream(&gps_data, WATCH_DISABLE, NULL);
	gps_close (&gps_data);
	return NULL;
}

int main(int argc, char * argv[])
{
	IR ir;
	pthread_t pth;
	pthread_create(&pth, NULL, gps_thread, NULL);
	ir.init(argc, argv);
	ir.schedule();
	return 0;
}
