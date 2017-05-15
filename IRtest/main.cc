#include "ir.h"
struct in_addr IR::m_vir_ip;
struct in_addr IR::m_phy_ip;
int IR::beacon_fd;
//int IR::begin_time;
struct sockaddr_in IR::m_beaconSA;
struct sockaddr_in IR::broadcast_addr;
struct Speed IR::m_speed;

int main(int argc, char * argv[])
{
	IR ir;
	ir.init(argc, argv);
	ir.schedule();
	return 0;
}
