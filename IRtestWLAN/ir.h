#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <linux/if.h>
#include <linux/if_tun.h>
//#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>


#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <netdb.h>
#include <cmath>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include <gps.h>
#include <math.h>
#include <pthread.h>

#include "Entry.h"
#include "GPS.h"
#include "Speed.h"
#include "GPSVec.h"

#define BEACON_PORT 5555
#define DATA_PORT 4000
#define FRAME_SIZE 65536


#define TTLNEIGHBORTABLE 5
#define PI 3.14159
#define CALNEXTHOPANGLE PI/3
#define MAXDISTANCE 200
#define BROADCASTADDRESS 0xFFFFFFFF

struct BeaconMessage {
	//uint16_t type;        //2
	uint32_t m_vir_ip;      //4
	uint32_t m_phy_ip;     //4
	uint32_t src_gps_x;   //4
	uint32_t src_gps_y;   //4
	uint32_t src_speed_x; //4
	uint32_t src_speed_y; //4
	//uint32_t curr_time;   //4
}__attribute__((packed));

struct DataMessage {
	//uint16_t type;       //2
	uint32_t length;     //4
	uint32_t src_gps_x;  //4
	uint32_t src_gps_y;  //4
	uint32_t dst_gps_x;  //4
	uint32_t dst_gps_y;  //4
	uint32_t ttl;
}__attribute__((packed));


class IR
{
public:
	IR();
	~IR();
	int init(int argc, char * argv[]);
	void schedule();

	static int beacon_fd;
	static struct in_addr m_phy_ip;
	static struct in_addr m_vir_ip;
	static struct sockaddr_in m_beaconSA;
	static struct sockaddr_in broadcast_addr;
	int begin_time;
private:
	int get_ip(char * dev, struct in_addr* ip);
	int tun_alloc(char *dev, int flags);
	void die(std::string msg);
	void init_timer();
	static void get_gps();
	static void send_beacon(int sig);
	static void build_beacon(struct BeaconMessage* message);
	void recv_beacon(int fd);
	void recv_data(int fd);
	void recv_tun(int fd);
	void build_neighbor(BeaconMessage * message);
	void pkt_get_u8(const char ** p, uint8_t * var);
	void pkt_get_u16(const char ** p, uint16_t * var);
	void pkt_get_u32(const char ** p, uint32_t * var);
	void pkt_get_u64(const char ** p, uint64_t * var);
	void show_forward();
	void show_neighbor();
	///jixuefeng
	void Forwarding(uint32_t dstIP, GPS dstGPS, int nread);
	bool IsLegal(uint32_t nghbrIP, GPS dstGPS);
	uint32_t CalNexHop(GPS dstGPS);
	uint32_t RouteMakeup(GPS dstGPS);
    	//predict
    	GPS AdvancedPredict(std::queue<GPS> q, uint32_t time, Speed v);
    	double gaussrand(double mu,double sigma);
	///jixuefeng

	void data_broadcast(int nread);
	void my_send(uint32_t dst_phy_ip, int nread);
	int gps_by_ip(uint32_t dst_vir_ip,in_addr * dst_phy_ip, GPS * dst_gps);
	int data_fd;
	int tun_fd;
	int beacon_interval;
	static std::queue<GPS> q_gps;
	struct sockaddr_in m_dataSA;
   	char data_buffer[FRAME_SIZE];
    	char beacon_buffer[FRAME_SIZE];
	char tun_buffer[FRAME_SIZE];
	char data_recv_buffer[FRAME_SIZE];
	static double m_speed;
	static GPS m_gps;
	std::map<uint32_t, ForwardTableEntry*> m_forward_table;
	std::map<uint32_t, NeighborEntry*> m_neighbor_table;


};
