#include "ir.h"

/* This is a UDP tunnel program. It receives packets from applications via
 * a tun device, forwards them to a remote endpoint via a UDP connection.
 * When packets are received from the UDP connection, they are written back
 * to the tun device, and will be received by apps.
 *
 * usage: tunudp tun_dev local_addr local_port remote_addr remote_port";
 */

 /* connect to the local tun device */

IR::IR()
{

}

IR::~IR()
{
}

int
IR::get_ip(char * ifname, struct in_addr *ip)
{
	//char *temp = NULL;
	int inet_sock;
	struct ifreq ifr;

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	memset(ifr.ifr_name, 0, sizeof(ifr.ifr_name));
	memcpy(ifr.ifr_name, ifname, strlen(ifname));

	if (0 != ioctl(inet_sock, SIOCGIFADDR, &ifr))
	{
		perror("ioctl error");
		return -1;
	}

	*ip = ((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr;

	close(inet_sock);
	return 0;
}
GPS
IR::get_gps()
{
	return GPS(123,456);
}
int
IR::tun_alloc(char *dev, int flags)
{
	struct ifreq ifr;
	int fd, err;
	char clonedev[] = "/dev/net/tun";

	/* Arguments taken by the function:
	 *
	 * char *dev: the name of an interface (or '\0'). MUST have enough
	 *   space to hold the interface name if '\0' is passed
	 * int flags: interface flags (eg, IFF_TUN etc.)
	 */

	 /* open the clone device */
	if ((fd = open(clonedev, O_RDWR)) < 0) {
		return fd;
	}

	/* preparation of the struct ifr, of type "struct ifreq" */
	memset(&ifr, 0, sizeof(ifr));

	ifr.ifr_flags = flags;

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	 *        IFF_TAP   - TAP device
	 *
	 *        IFF_NO_PI - Do not provide packet information
	 *
	 *	If flag IFF_NO_PI is not set each frame format is:
	 * 	     Flags [2 bytes]
	 *	     Proto [2 bytes]
	 *	     Raw protocol(IP, IPv6, etc) frame.
	 */


	if (*dev) {
		/* if a device name was specified, put it in the structure; otherwise,
		 * the kernel will try to allocate the "next" device of the
		 * specified type */
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}

	/* try to create the device */
	if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
		close(fd);
		return err;
	}

	/* if the operation was successful, write back the name of the
	 * interface to the variable "dev", so the caller can know
	 * it. Note that the caller MUST reserve space in *dev (see calling
	 * code below) */
	strcpy(dev, ifr.ifr_name);

	return fd;
}

void
IR::die(std::string msg)
{
	perror(msg.c_str());
	exit(1);
}

void
IR::init_timer()
{
	struct itimerval tv, otv;
	signal(SIGALRM, send_beacon);
	//how long to run the first time
	tv.it_value.tv_sec = 1;
	tv.it_value.tv_usec = 0;
	//after the first time, how long to run next time
	tv.it_interval.tv_sec = 1;
	tv.it_interval.tv_usec = 0;

	if (setitimer(ITIMER_REAL, &tv, &otv) != 0)
		printf("setitimer err %d\n", errno);
	return;
}

void
IR::build_beacon(struct BeaconMessage *message)
{
	struct  timeval    tv;
	struct  timezone   tz;
	message->m_vir_ip = m_vir_ip.s_addr;
	message->m_phy_ip = m_phy_ip.s_addr;
	message->src_gps_x = 111;
	message->src_gps_y = 222;
	message->src_speed_x = 333;
	message->src_speed_y = 444;;
	gettimeofday(&tv, &tz);
	//message->curr_time = tv.tv_sec;
}

void
IR::send_beacon(int sig)
{
	struct BeaconMessage beacon;
	build_beacon(&beacon);
	sendto(beacon_fd, (char *)&beacon, sizeof(beacon), 0, (struct sockaddr*) &broadcast_addr, sizeof(broadcast_addr));
}

int
IR::init(int argc, char * argv[])
{
	char tun_name[IFNAMSIZ];
//	char phy_name[IFNAMSIZ] = "wlan0";
//	char phy_name[IFNAMSIZ] = "eth0";  //hao @ 5-12
	char usage[] = "usage: tunudp dev";
//	int so_broadcast = 1;
//	int ret = -1;

//	struct ifreq ifr;
//	memset(&ifr, 0, sizeof(ifr));
	if (argc != 4) {
		printf("%s\n", usage);
		exit(1);
	}

//	beacon_buffer = (char*)malloc(sizeof(char) * FRAME_SIZE);
	tun_buffer = (char*)malloc(sizeof(char) * FRAME_SIZE);
	data_buffer = (char*)malloc(sizeof(char) * FRAME_SIZE);
	strncpy(tun_name, argv[1], IFNAMSIZ);

	memset(tun_buffer, '\0', sizeof(tun_buffer));

	/* get ip*/



	//hao: get remote IP
 // 	struct sockaddr_in remoteSA;
   	memset((char *) &remoteSA, 0, sizeof(remoteSA));
   	remoteSA.sin_family = AF_INET;
	if(inet_aton(argv[3], &(remoteSA.sin_addr)) == 0)
	   die("invalid remote address");
	remoteSA.sin_port = htons(DATA_PORT);


   	memset((char *) &localSA, 0, sizeof(localSA));
   	localSA.sin_family = AF_INET;
	if(inet_aton(argv[2], &(localSA.sin_addr)) == 0)
	   die("invalid local address");
	localSA.sin_port = htons(DATA_PORT);


	/*
	if (get_ip(phy_name, &m_phy_ip) < 0)
		die("get physical ip error");
	printf("PHYIP: %s\n", inet_ntoa(m_phy_ip));

	if (get_ip(tun_name, &m_vir_ip) < 0)
		die("get virtual ip error");
	printf("VIRIP: %s\n", inet_ntoa(m_vir_ip));
*/
	/* create tun_fd */
	/* connects to the tun device. IFF_TUN means the packet will include IP header, TCP / UDP header, and  the payload. */

	strncpy(tun_name, argv[1], IFNAMSIZ);
	tun_fd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI);
	if (tun_fd < 0)
		die("Allocating interface");
	if (ioctl(tun_fd, TUNSETNOCSUM, 1) < 0)
		die("ioctl TUNSETNOCSUM error");

	/* create a UDP socket for beacon, bind it to phy_ip*/
	/*
	beacon_fd = socket(AF_INET, SOCK_DGRAM, 0);//IPPROTO_UDP
	if (beacon_fd < 0)
		die("creating UDP socket for beacon");
	memset((char *)&broadcast_addr, 0, sizeof(broadcast_addr));
	strncpy(ifr.ifr_name, phy_name, strlen(phy_name));
	if (ioctl(beacon_fd, SIOCGIFBRDADDR, &ifr) == -1)
	{
		perror("beacono ioctl error");
		return -1;
	}
	memcpy(&broadcast_addr, (char *)&ifr.ifr_broadaddr, sizeof(struct sockaddr_in));
    	broadcast_addr.sin_family = AF_INET;
    	broadcast_addr.sin_port = htons(BEACON_PORT);
    	ret = setsockopt(beacon_fd, SOL_SOCKET, SO_BROADCAST, &so_broadcast,sizeof(so_broadcast));

    	m_beaconSA.sin_family=AF_INET;
    	m_beaconSA.sin_port=htons(BEACON_PORT);
    	m_beaconSA.sin_addr.s_addr=htonl(INADDR_ANY);
    	bzero(&(m_beaconSA.sin_zero),8);

	if (bind(beacon_fd, (struct sockaddr *) &m_beaconSA, sizeof(m_beaconSA)) != 0)
		die("beacon bind()");
*/
	/* create a UDP socket for data, bind it to phy_ip*/
	
	/*
	memset((char *)&m_dataSA, 0, sizeof(m_dataSA));
	m_dataSA.sin_family = AF_INET;
	m_dataSA.sin_addr = m_phy_ip;
	m_dataSA.sin_port = htons(DATA_PORT);
	*/
	data_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (data_fd < 0)
		die("creating UDP socket for data");
	if (bind(data_fd, (struct sockaddr *) &localSA, sizeof(localSA)) != 0)
		die("data bind()");

	if(connect(data_fd, (struct sockaddr *) &remoteSA, sizeof(remoteSA)) != 0)
	   die("connect()");

//	init_timer();   // hao @ 5-12
	return 0;
}

void
IR::schedule()
{
   char buffer[FRAME_SIZE];
   int maxfd, nread, retn, nwrite;
   fd_set rset;
   struct iphdr *p_ip;
   struct timeval tv;
   struct  timezone tz;
   gettimeofday(&tv,&tz);
   begin_time = tv.tv_sec;
   p_ip = (struct iphdr *) buffer;
//   maxfd = beacon_fd > tun_fd ? beacon_fd : tun_fd;
//   maxfd = data_fd > maxfd ? data_fd : maxfd;
   maxfd = data_fd > tun_fd ? data_fd : tun_fd;
   maxfd++;

	/* main loop */
	while (1)
	{
		FD_ZERO(&rset);
		FD_SET(tun_fd, &rset);
//		FD_SET(beacon_fd, &rset);
		FD_SET(data_fd, &rset);

		retn = select(maxfd, &rset, NULL, NULL, NULL);
		if (retn == -1) perror("select()");
		else if (retn == 0) perror("select timeout");
		else
		{ // retn > 0
			if (FD_ISSET(tun_fd, &rset))
			{
			//	recv_tun(tun_fd);
                printf("receive tun \n");
                nread = read(tun_fd, buf, sizeof(buf));
                printf("tun sent bytes = %d\n", nread);
                if(nread > 0)
                 {
                    if(write(data_fd, tun_buffer, nread) != nread)
                    perror("error in writing to UDP");
                 }
                 else
                    perror("error in reading from TUN");

			}
			/*
			if (FD_ISSET(beacon_fd, &rset))
			{
				recv_beacon(beacon_fd);
			}
			*/
			if (FD_ISSET(data_fd, &rset))
			{
			//	recv_data(data_fd);
		nread = read(data_fd, buf, sizeof(buf));
		printf("receive data \n");
                printf("sock gets bytes = %d\n", nread);
		if(nread > 0)
		{	
        		if(write(tun_fd, buf, nread) != nread)
            			perror("error in writing to TUN");
    		 }
    		else
       		 perror("error in reading from tun");


			}
		}
	}
}


				/*nread = read(tun_fd, buffer, sizeof(buffer));
				if (nread > 0) {
					// allow TCP packets in the tunnel.
					//if(p_ip->protocol == IPPROTO_TCP) {
					if (write(sock_fd, buffer, nread) != nread)
						perror("error in writing to UDP");
					//printf("from tun to udp\n");
				//}
				}
				else
					perror("error in reading from tun");
			}
			if (FD_ISSET(sock_fd, &rset)) {
				nread = read(sock_fd, buffer, sizeof(buffer));
				if (nread > 0) {
					//if(p_ip->protocol == IPPROTO_TCP) {
	//                  p_tcp = (struct tcphdr *)(buffer + p_ip->ihl * 4);
	//  not sure setting rawSA is required or not. setting it anyway.
	//                  rawSA.sin_addr.s_addr = p_ip->daddr;
	//                  rawSA.sin_port = p_tcp->dest;
	//                  if(sendto(raw_fd, buffer, nread, 0,
	//                    (struct sockaddr *)&rawSA, sizeof(rawSA)) != nread)
	//                      perror("sendto() error");
					if (write(tun_fd, buffer, nread) != nread)
						perror("error in writing to tun");
					//printf("from udp to tun\n");
					//}
				}
				else
					perror("error in reading from UDP");
			}
		}
	}
}*/
int
IR::gps_by_ip(uint32_t dst_vir_ip,uint32_t * dst_phy_ip,GPS * dst_gps)
{
	std::map<uint32_t, ForwardTableEntry*>::iterator it = m_forward_table.find(dst_vir_ip);
	if (it != m_forward_table.end())
	{
		*dst_gps = it->second->m_gps;
		*dst_phy_ip = it->second->GetNextHopIP();
		return 1;
	}
	else
	{
		return 0;
	}
}
void
IR::recv_tun(int fd)
{
   printf("receive tun \n");
  // uint16_t nread,nwrite;
   int nread, nwrite;
   uint32_t dst_phy_ip;
   int opt;

 //  if(connect(data_fd, (struct sockaddr *) &remoteSA, sizeof(remoteSA)) != 0)
   //   die("connect() to remote IP");

    nread = read(tun_fd, buf, sizeof(buf));
    printf("tun sent bytes = %d\n", nread);
    if(nread > 0)
    {
        if(write(data_fd, tun_buffer, nread) != nread)
            perror("error in writing to UDP");
    }
    else
        perror("error in reading from TUN");
    return;


/*
    struct iphdr *p_ip;
	GPS dst_gps;
	const char* curr_tun;
	struct DataMessage message;
	if((nread = read (fd, tun_buffer, FRAME_SIZE)) < 0)
	{
		die("reading tun");
	}
	p_ip = (struct iphdr *) tun_buffer;
	message.length = nread;
	m_gps = get_gps();
	opt = gps_by_ip(p_ip->daddr,&dst_phy_ip,&dst_gps);
	struct in_addr tun_addr_tmp;
	tun_addr_tmp.s_addr = p_ip->daddr;
	printf("sender dst ip: %s",inet_ntoa(tun_addr_tmp));
	message.src_gps_x = m_gps.x;
	message.src_gps_y = m_gps.y;
	if(opt)
	{
		struct sockaddr_in dst_addr;
		dst_addr.sin_family = AF_INET;
		dst_addr.sin_port = htons(DATA_PORT);
		dst_addr.sin_addr.s_addr = dst_phy_ip;
		message.dst_gps_x = dst_gps.x;
		message.dst_gps_y = dst_gps.y;
		printf("send data unicast: myGPS (%d,%d)\ndstGPS (%d,%d)",message.src_gps_x,message.src_gps_y = m_gps.y,message.dst_gps_x,message.dst_gps_y);
		sendto(data_fd, (char *)&message, sizeof(message), 0, (struct sockaddr*) &dst_addr, sizeof(dst_addr));
		sendto(data_fd, tun_buffer, sizeof(tun_buffer), 0, (struct sockaddr*) &dst_addr, sizeof(dst_addr));
	}
	else
	{
		broadcast_addr.sin_port = htons(DATA_PORT);
		message.dst_gps_x = 0;
		message.dst_gps_y = 0;
		printf("send data broadcast: myGPS (%d,%d)\ndstGPS (%d,%d)",message.src_gps_x,message.src_gps_y = m_gps.y,message.dst_gps_x,message.dst_gps_y);
		sendto(data_fd, (char *)&message, sizeof(message), 0, (struct sockaddr*) &broadcast_addr, sizeof(broadcast_addr));
		sendto(data_fd, tun_buffer, sizeof(tun_buffer), 0, (struct sockaddr*) &broadcast_addr, sizeof(broadcast_addr));
	}
*/
}

void
IR::recv_beacon(int fd)
{
	struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);
   	const char *curr;
	struct BeaconMessage beacon_message;
	int count;
	printf("receive beacon \n");
	count = recvfrom(fd, (char*) beacon_buffer, sizeof(BeaconMessage), 0, (struct sockaddr*) &from_addr, &from_len);
    curr = beacon_buffer ;
        //zz = (struct beacon_message*) buffer;


        pkt_get_u32(&curr, &(beacon_message.m_vir_ip));
        pkt_get_u32(&curr, &beacon_message.m_phy_ip);
        pkt_get_u32(&curr, &beacon_message.src_gps_x);
        pkt_get_u32(&curr, &beacon_message.src_gps_y);
        pkt_get_u32(&curr, &beacon_message.src_speed_x);
        pkt_get_u32(&curr, &beacon_message.src_speed_y);
        //pkt_get_u32(&curr, &beacon_message.curr_time);

        if(from_addr.sin_addr.s_addr != m_phy_ip.s_addr)
        {
           	printf("\tfound neighbor IP is %s, Port is %d\n",inet_ntoa(from_addr.sin_addr),htons(from_addr.sin_port));
           	build_neighbor(&beacon_message);
        }

}
bool
IR::IsLegal(uint32_t nghbrIP, GPS dstGPS)
{
    std::map<uint32_t, NeighborEntry*>::iterator it = m_neighbor_table.find(nghbrIP);
    if(it == m_neighbor_table.end())
    {
        return false;
    }
    struct timeval now;
    gettimeofday(&now,NULL);
    int until_time = (now.tv_sec - begin_time)*1000 + now.tv_usec/1000;
    if(until_time - (int)it->second->GetTimestamp() > TTLNEIGHBORTABLE)
    {
        delete it->second;
		m_neighbor_table.erase(it->first);
		return false;
    }
    GPS nghbrGPS = it->second->GetGPS();
    //GPS selfGPS = GPS::Uint64ToGPS(this->m_gps);
    GPS selfGPS = this->m_gps;
    GPSVec selfToDstVec(selfGPS, dstGPS);
    GPSVec selfToNghbrVec(selfGPS, nghbrGPS);
    double angle = GPSVec::CalTheata(selfToDstVec, selfToNghbrVec);
    if (angle < CALNEXTHOPANGLE)
    {
        return true;
    }
    return false;
}
uint32_t
IR::CalNexHop(GPS dstGPS)
{
    uint32_t nextHopID = 0;
    double disMax = MAXDISTANCE;
    //GPS selfGPS = GPS::Uint64ToGPS(this->m_gps);
    GPS selfGPS = this->m_gps;
    struct timeval now;
    gettimeofday(&now,NULL);
    int until_time = (now.tv_sec - begin_time)*1000 + now.tv_usec/1000;
    for(std::map<uint32_t, NeighborEntry*>::iterator iter = m_neighbor_table.begin();iter != m_neighbor_table.end();)
        {
            //cout<<iter->first<<":"<<iter->second<<endl;
            if(until_time - (int)iter->second->GetTimestamp() > TTLNEIGHBORTABLE)
            {
                  m_neighbor_table.erase(iter++);
            }
            else
            {
                GPS nghbrGPS = iter->second->GetGPS();
                GPSVec selfToDstVec(selfGPS, dstGPS);
				GPSVec selfToNghbrVec(selfGPS, nghbrGPS);
				double angle = GPSVec::CalTheata(selfToDstVec, selfToNghbrVec);
				if (angle < CALNEXTHOPANGLE)
				{
					double dis = GPSVec::CalModule(nghbrGPS, dstGPS);
					//std::cout << "dis : " << dis << std::endl;
					if (dis < disMax)
					{
						disMax = dis;
						nextHopID = iter->first;
					}
				}
                iter++;
            }
        }
        const double EPSINON = 0.00000001;
		double x = disMax - MAXDISTANCE;
		if ((x >= - EPSINON)&&(x <= EPSINON))
		{
		   for(std::map<uint32_t, NeighborEntry*>::iterator iter = m_neighbor_table.begin();iter != m_neighbor_table.end();iter++)
		   {
		       GPS nghbrGPS = iter->second->GetGPS();
               double dis = GPSVec::CalModule(nghbrGPS, dstGPS);
               if (dis < disMax)
				{
					disMax = dis;
					nextHopID = iter->first;
				}
		   }
		   const double EPSINON = 0.00000001;
           double x = disMax - MAXDISTANCE;
           if ((x >= - EPSINON)&&(x <= EPSINON))
           {
             return BROADCASTADDRESS;
           }
		}
		return nextHopID;
}
uint32_t
IR::RouteMakeup(GPS dstGPS)
{
    uint32_t nextHopID = 0;
    std::vector<std::pair<uint32_t, GPS> > localNeighborTable;
    struct timeval now;
    gettimeofday(&now,NULL);
    int until_time = (now.tv_sec - begin_time)*1000 + now.tv_usec/1000;
    double disMax = MAXDISTANCE;
    //GPS selfGPS = GPS::Uint64ToGPS(this->m_gps);
    GPS selfGPS = this->m_gps;
    for(std::map<uint32_t, NeighborEntry*>::iterator iter = m_neighbor_table.begin();iter != m_neighbor_table.end();)
        {
            //cout<<iter->first<<":"<<iter->second<<endl;
            if(until_time - (int)iter->second->GetTimestamp() > TTLNEIGHBORTABLE)
            {
                  m_neighbor_table.erase(iter++);
            }
            else
            {
                GPS nghbrGPS = iter->second->GetGPS();
                Speed nghbrSpeed = iter->second->GetSpeed();
                GPS gps;
                ///Linear prediction
                /*gps.x = nghbrGPS.x;
				gps.y = nghbrGPS.y;
				gps.x += nghbrSpeed.x * (double)(until_time - (int)iter->second->GetTimestamp())/1000.0;  //time unit :ms,  speed unit : m/s;
                gps.y += nghbrSpeed.y * (double)(until_time - (int)iter->second->GetTimestamp())/1000.0;*/
                ///wangzhe prediction
                gps = this->AdvancedPredict(iter->second->gpsq, until_time - (int)iter->second->GetTimestamp(), nghbrSpeed);
                std::pair<uint32_t, GPS> entry(iter->first,gps);
				localNeighborTable.push_back(entry);
                iter++;
            }
        }
    for(std::vector<std::pair<uint32_t, GPS> >::iterator iter = localNeighborTable.begin();iter != localNeighborTable.end();iter++)
    {
        GPS nghbrGPS = iter->second;
        GPSVec selfToDstVec(selfGPS, dstGPS);
        GPSVec selfToNghbrVec(selfGPS, nghbrGPS);
        double angle = GPSVec::CalTheata(selfToDstVec, selfToNghbrVec);
        if (angle < CALNEXTHOPANGLE)
			{
				double dis = GPSVec::CalModule(nghbrGPS, dstGPS);
				//std::cout << "dis : " << dis << std::endl;
				if (dis < disMax)
				{
					disMax = dis;
					nextHopID = iter->first;
				}
			}
    }
    const double EPSINON = 0.00000001;
    double x = disMax - MAXDISTANCE;
    if ((x >= - EPSINON)&&(x <= EPSINON))
    {
         for(std::vector<std::pair<uint32_t, GPS> >::iterator iter = localNeighborTable.begin();iter != localNeighborTable.end();iter++)
         {
             GPS nghbrGPS = iter->second;
             double dis = GPSVec::CalModule(nghbrGPS, dstGPS);
             if (dis < disMax)
				{
					disMax = dis;
					nextHopID = iter->first;
				}
         }
         const double EPSINON = 0.00000001;
         double x = disMax - MAXDISTANCE;
            if ((x >= - EPSINON)&&(x <= EPSINON))
				// still not find
			{
				return BROADCASTADDRESS;
			}
    }
    return nextHopID;
}
void
IR::Forwarding(uint32_t dstID, GPS dstGPS)
{
    uint32_t nextHopID = 0;
    std::map<uint32_t, ForwardTableEntry*>::iterator it = m_forward_table.find(dstID);
    struct timeval now;
    gettimeofday(&now,NULL);
    int until_time = (now.tv_sec - begin_time)*1000 + now.tv_usec/1000;
    if(it == m_forward_table.end())
    {
        nextHopID = RouteMakeup(dstGPS);
        //modify calnexhop()
        std::map<uint32_t, ForwardTableEntry*>::iterator it = m_forward_table.find(nextHopID);
        GPS nextHopGPS = it->second->m_gps;
        if (nextHopID != BROADCASTADDRESS)
        {
            ForwardTableEntry * entry = new ForwardTableEntry(nextHopID,until_time,nextHopGPS);
            m_forward_table[dstID] = entry;
        }

    }
    else if(until_time - (int)it->second->GetTimestamp() > TTLNEIGHBORTABLE)
    {
       	// find, but expired. calculate the next hop, and add this to m_forwardtable
        nextHopID = RouteMakeup(dstGPS);
        //modify calnexhop()
        //std::cout << "jixuefeng ::: " << (int)nextHopID << std::endl;
        if (nextHopID != BROADCASTADDRESS)
        {
            m_forward_table[dstID]->m_next_hop_ip = nextHopID;
            m_forward_table[dstID]->m_timestamp = until_time;
        }
    }
    else
    {
      //find, need to judge the ligality
        if (IsLegal(it->second->m_next_hop_ip, dstGPS))
			{
				//legal, send as the entry
				nextHopID = it->second->m_next_hop_ip;
			}
        else
        {
            //illegal, use RouteMakeup to get the nextHopID
            nextHopID = RouteMakeup(dstGPS);
            //std::cout << "jixuefeng ::: " << nextHopID << std::endl;
            m_forward_table[dstID]->m_next_hop_ip = nextHopID;
            m_forward_table[dstID]->m_timestamp = until_time;
        }
    }
    /////send////
    if(nextHopID != BROADCASTADDRESS)
    {
        ///send "nextHopID --- IP"
    }
}
///predict
GPS
IR::AdvancedPredict(std::queue<GPS> q, uint32_t time, Speed v)
{
    const double EPSINON = 0.00000001;
    double mu = -5.8937984e-06;
    const double sigma = 0.4476733;
    const double const_term = -0.0984;
    const double coeff[7] = {0.0454,-0.0020,0.0431,0.0581,0.1194,0.1186,0.3778};
    //this is the prediction based on the linear regression of the slope of the trace
    //and it is without the error correction

    //first, get the basic information
    float vel = sqrt(v.x*v.x + v.y*v.y);
    float time_s = time/1000.0;
    int case_num = int(time_s);
    double k[20];
    for(int i = 0;i<20;++i)
       k[i] = 0;
    bool x_inc = true;
    GPS last_point;
    if(q.size() < 8)
    {
        last_point = q.back();
        last_point.x += time*v.x/1000;
        last_point.y += time*v.y/1000;
        return last_point;
    }
    for(int i = 0; i<7; ++i)
    {
        GPS p1 = q.front();
        q.pop();
        GPS p2 = q.front();
        if(fabs(p1.x - p2.x) < EPSINON)
            k[i] = (p1.y - p2.y)/(p1.x - p2.x + EPSINON);
        else
            k[i] = (p1.y - p2.y)/(p1.x - p2.x);
        if(i == 6)
            {
                last_point = p2;
                if(p1.x > p2.x)
                    x_inc = false;
            }
    }
    //begin prediction
    //first get the integer part
    int ptr = 7;
    double predict_k = 0;
    for(int i = 0; i < case_num; ++i)
    {
        //calculate the slope
        predict_k = const_term;
        for(int j = ptr-7;j<ptr;++j)
            predict_k += coeff[j-ptr+7]*k[ptr];
        k[ptr] = predict_k;
        ptr++;
        //calculate the distance
        double dis = 0;
        for(int j = 0;j<10;++j)
        {
            double acc = gaussrand(mu, sigma);
            dis += vel*0.1+acc*0.1*0.1/2;
            vel += acc;
        }
        //calculate the predicted position
        if(x_inc)
        {
            last_point.x += dis/(sqrt(1+predict_k*predict_k));
            last_point.y += predict_k*dis/(sqrt(1+predict_k*predict_k));
        }
        else
        {
            last_point.x -= dis/(sqrt(1+predict_k*predict_k));
            last_point.y -= predict_k*dis/(sqrt(1+predict_k*predict_k));
        }
    }
    //now get the other part
    int dtime = time - case_num*1000;
    double dis = 0;
    for(int i = 0; i<dtime;i++)
    {
        double acc = gaussrand(mu, sigma);
        dis += vel*0.001+acc*0.001*0.001/2;
        vel += acc;
    }
    if(x_inc)
    {
        last_point.x += dis/(sqrt(1+predict_k*predict_k));
        last_point.y += predict_k*dis/(sqrt(1+predict_k*predict_k));
    }
    else
    {
        last_point.x -= dis/(sqrt(1+predict_k*predict_k));
        last_point.y -= predict_k*dis/(sqrt(1+predict_k*predict_k));
    }
    return last_point;
}
double
IR::gaussrand(double mu,double sigma)
{
    const double pi = 3.141592654;
    double U, V;
    double Z;
    U = rand() / (RAND_MAX + 1.0);
    V = rand() / (RAND_MAX + 1.0);
    Z = sqrt(-2.0 * log(U))* sin(2.0 * pi * V);
    return Z*sigma+mu;
}
///predict
void
IR::recv_data(int fd)
{
    struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);
   	const char *curr;
	uint32_t dst_ip;
	struct DataMessage data_message;
	int count;
	int nread;

	nread = read(fd, data_buffer, sizeof(data_buffer));
	printf("receive data \n");
	
	cout << nread << endl;
	if(nread > 0)
	{
        if(write(tun_fd, data_buffer, nread) != nread)
            perror("error in writing to TUN");
    }
    else
        perror("error in reading from tun");
    return;       
	
	/*
	count = recvfrom(fd, (char*) &data_message, sizeof(DataMessage), 0, (struct sockaddr*) &from_addr, &from_len);
	std::cout << data_message.length << "," << data_message.src_gps_x << "," << data_message.src_gps_y << "," << data_message.dst_gps_x << "," << data_message.dst_gps_y << std::endl;
	nread = recvfrom(fd, (char*) data_buffer, data_message.length, 0, (struct sockaddr*) &from_addr, &from_len);
	
	struct iphdr *p_ip;
	p_ip = (struct iphdr *) data_buffer;
	dst_ip = p_ip->daddr;
	struct in_addr dst_addr_t;
	dst_addr_t.s_addr = dst_ip;
	std::cout << inet_ntoa(dst_addr_t) << std::endl;
	if(dst_ip == m_phy_ip.s_addr)
	{
 		///Reach the destination
		std::cout << "reach destination" << std::endl;
		if(write(tun_fd, data_buffer, nread) < 0)
		{
			perror ("Writing tun");
			exit(1);
		}
	}
	else
	{
		//forwarding
		std::cout << "forwarding" << std::endl;
		GPS dst_gps = GPS(data_message.dst_gps_x,data_message.dst_gps_y);
		Forwarding(dst_ip, dst_gps);
	}
	*/
    //curr = data_buffer;
    /*pkt_get_u32(&curr, &data_message.length);
    pkt_get_u32(&curr, &data_message.src_gps_x);
    pkt_get_u32(&curr, &data_message.src_gps_y);
    pkt_get_u32(&curr, &data_message.src_gps_y);
    pkt_get_u32(&curr, &data_message.dst_gps_x);
    pkt_get_u32(&curr, &data_message.dst_gps_y);
    GPS self_gps = this->m_gps;
    GPS src_gps = GPS(data_message.src_gps_x,data_message.src_gps_y);
    
    if(self_gps.x == dst_gps.x&&self_gps.y == dst_gps.y)
    {
        ///Reach the destination
    }
    else
    {
        ///Forwarding(uint32_t dstID,uint64_t dstGPS)
    }*/
}


void
IR::build_neighbor(BeaconMessage * message)
{
        ///**update************
        struct timeval now;
        gettimeofday(&now,NULL);
        int until_time = (now.tv_sec - begin_time)*1000 + now.tv_usec/1000;
        for(std::map<uint32_t, NeighborEntry*>::iterator iter = m_neighbor_table.begin();iter != m_neighbor_table.end();)
        {
            //cout<<iter->first<<":"<<iter->second<<endl;
            if(until_time - (int)iter->second->GetTimestamp() > 5)
            {
                  m_neighbor_table.erase(iter++);
            }
            else
            {
                iter++;
            }
        }
        for(std::map<uint32_t, ForwardTableEntry*>::iterator iter = m_forward_table.begin();iter != m_forward_table.end();)
        {
            //cout<<iter->first<<":"<<iter->second<<endl;
            if(until_time - (int)iter->second->GetTimestamp() > 5)
            {
                  m_forward_table.erase(iter++);
            }
            else
            {
                iter++;
            }
        }
        ///********************
        Speed speed((float)message->src_speed_x, (float)message->src_speed_y);
        GPS gps((float)message->src_gps_x, (float)message->src_gps_y);

	ForwardTableEntry* fentry = new ForwardTableEntry(message->m_phy_ip, until_time, gps);
	m_forward_table[message->m_vir_ip] = fentry;

    std::map<uint32_t, NeighborEntry*>::iterator it = m_neighbor_table.find(message->m_vir_ip);
	if (it != m_neighbor_table.end())
	{
		//find, update directly
		it->second->SetTimestamp(until_time);
		it->second->SetSpeed(speed);
		it->second->SetGPS(gps);
		if(it->second->gpsq.size() == 10)
		{
		    it->second->gpsq.pop();
		}
		it->second->gpsq.push(gps);
	}
	else
	{
		//not find, insert a new entry;
		NeighborEntry * entry = new NeighborEntry(gps, until_time, speed, message->m_phy_ip);
		entry->gpsq.push(gps);
		m_neighbor_table[message->m_vir_ip] = entry;
	}
    	show_neighbor();
    	show_forward();
}

///////////////////////neighbor
void
IR::show_neighbor()
{
        struct in_addr tmp;
    	std::cout << "\n";
	std::cout << "********************neighborTable*********************" << "\n";
	std::cout << "IP :: " << " gps :: " << " timestamp :: " << " speed :: " << " phy_ip " << "\n";
	//for (auto &p : m_neighborTable)
	for(std::map<uint32_t, NeighborEntry*>::iterator p = m_neighbor_table.begin();p != m_neighbor_table.end();p++)
		{
                        tmp.s_addr = p-> first;
			//printf("%s\n",p->first);
			std::cout<< inet_ntoa(tmp) << " :: " << "(" << p->second->GetGPS().x << "," << p->second->GetGPS().y << ")"
				<< " :: " << p->second->GetTimestamp() << " :: "
				<< "(" << p->second->GetSpeed().x << "," << p->second->GetSpeed().y << ")" << " :: ";
			tmp.s_addr = p->second->GetPhyIP();
			std::cout << inet_ntoa(tmp) << std::endl;
				//printf("%s\n",p->second->GetVip());
		}
	std::cout << "********************neighborTable*********************" << "\n";
	std::cout << "\n";
}

void
IR::show_forward()
{
	struct in_addr tmp;
        std::cout << "\n";
	std::cout << "********************forwardTable*********************" << "\n";
	std::cout << "IP :: " << " next_hop_ip :: "<< " timestamp :: " << " gps "   << "\n";
	//for (auto &p : m_neighborTable)
	for(std::map<uint32_t, ForwardTableEntry*>::iterator p = m_forward_table.begin();p != m_forward_table.end();p++)
		{
			tmp.s_addr = p-> first;
			//printf("%s\n",p->first);
			std::cout<< inet_ntoa(tmp) << " :: ";
			tmp.s_addr = p->second->GetNextHopIP();
			std::cout << inet_ntoa(tmp) << " :: " << p->second->GetTimestamp() << " :: " << "(" << p->second->m_gps.x << "," << p->second->m_gps.y << ")"
				 << std::endl;
				//printf("%s\n",p->second->GetVip());
		}
	std::cout << "********************forwardTable*********************" << "\n";
	std::cout << "\n";
}


void
IR::pkt_get_u8(const char ** p, uint8_t * var)
{
  *var = *(const uint8_t *)(*p);
  *p += sizeof(uint8_t);
}
void
IR::pkt_get_u16(const char ** p, uint16_t * var)
{
  *var = **((const uint16_t **)p);
  *p += sizeof(uint16_t);
}
void
IR::pkt_get_u32(const char ** p, uint32_t * var)
{
  *var = **((const uint32_t **)p);
  *p += sizeof(uint32_t);
}

void
IR::pkt_get_u64(const char ** p, uint64_t * var)
{
  *var = **((const uint64_t **)p);
  *p += sizeof(uint64_t);
}



