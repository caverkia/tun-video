#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
//#include <net/if.h>
#include <net/ethernet.h>

/* This is a UDP tunnel program. It receives packets from applications via
 * a tap device, forwards them to a remote endpoint via a UDP connection.
 * When packets are received from the UDP connection, they are written back
 * to the tap device, and will be received by apps.
 *
 *
 * usage: tapudp tun_dev local_addr local_port remote_addr remote_port";
 */

/* connect to the local tun device */

int tun_alloc(char *dev, int flags) 
{
  struct ifreq ifr;
  int fd, err;
  char *clonedev = "/dev/net/tun";

  /* Arguments taken by the function:
   *
   * char *dev: the name of an interface (or '\0'). MUST have enough
   *   space to hold the interface name if '\0' is passed
   * int flags: interface flags (eg, IFF_TUN etc.)
   */

   /* open the clone device */
   if( (fd = open(clonedev, O_RDWR)) < 0 ) {
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
   if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
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

void die(char *msg)
{
  perror(msg);
  exit(1);
}

int main (int argc, char * argv[])
{

/* Buffer should be at least the MTU size of the interface, eg 1500 bytes */
  const int FRAMESIZE = 2000;

  char tun_name[IFNAMSIZ];
  char buffer[FRAMESIZE];
  int tun_fd, sock_fd;
  //int raw_fd;
  int  maxfd, nread, retn, len;
  //u_int16_t localPort, remotePort;
  struct sockaddr_in localSA, remoteSA;
  // struct sockaddr_in  rawSA; 
  //struct sockaddr_ll rawSA;
  fd_set rset;
  //int one = 1;

  //struct ifreq ifr;

  struct ethhdr * p_ether;
  int ethertype;
  struct iphdr *p_ip;

  char usage[] = "usage: tapudp dev local_addr local_port remote_addr remote_port";

  if(argc != 6) {
    printf("%s\n", usage);
    exit(1);
  }

  strncpy(tun_name, argv[1], IFNAMSIZ);

  memset((char *) &localSA, 0, sizeof(localSA));
  localSA.sin_family = AF_INET;
  if(inet_aton(argv[2], &(localSA.sin_addr)) == 0)
    die("invalid local address");
  localSA.sin_port = htons(atoi(argv[3]));
  
  memset((char *) &remoteSA, 0, sizeof(remoteSA));
  remoteSA.sin_family = AF_INET;
  if(inet_aton(argv[4], &(remoteSA.sin_addr)) == 0)
    die("invalid remote address");
  remoteSA.sin_port = htons(atoi(argv[5]));

//  memset((char *) &rawSA, 0, sizeof(rawSA));
//  rawSA.sin_family = AF_INET;

/* connects to the tun device. IFF_TUN means the packet will include IP header, 
 * TCP/UDP header, and  the payload. 
 */
  tun_fd = tun_alloc(tun_name, IFF_TAP | IFF_NO_PI); 
  if(tun_fd < 0) die("Allocating interface");

  if(ioctl(tun_fd, TUNSETNOCSUM, 1) < 0)
      die("ioctl TUNSETNOCSUM error");

/* XXX: try to get the interface index, don't know why it doesn't work.
 */
/*
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, "lo", IFNAMSIZ);
  if(ioctl(tun_fd, TUNGETIFF, (void *) &ifr) < 0)
      die("ioctl TUNGETIFF error");
  printf("device name: %s, index %u\n", ifr.ifr_name, ifr.ifr_ifindex);

  memset((char *) &rawSA, 0, sizeof(rawSA));
  rawSA.sll_family = AF_PACKET;
  // XXX: hard coded ifindex. Need to fix.
  // send raw IP packets to loopback.
  rawSA.sll_ifindex = 1;
  rawSA.sll_halen = ETH_ALEN;
  //rawSA.sll_hatype = ARPHRD_ETHER;
*/

/* create a UDP socket and connect to a remote IP:port */
  sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sock_fd < 0) die("creating socket");

  if (bind(sock_fd, (struct sockaddr *) &localSA, sizeof(localSA)) != 0)
    die("bind()");

  if(connect(sock_fd, (struct sockaddr *) &remoteSA, sizeof(remoteSA)) != 0)
    die("connect()");

/* create the raw socket for sending data to apps */
//  raw_fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
//  if(raw_fd < 0) die("creating raw socket");
  // the raw packet will include IP/TCP header
//  if(setsockopt(raw_fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0)
//    die("setsockopt() IP_HDRINCL error");


/* create the raw socket for sending data to apps */
/* According to http://standards.ieee.org/develop/regauth/ethertype/eth.txt,
 * Ethertype 0x88b5 is for "public use for prototype".
 * Use this so that we don't receive any packet on the raw socket.
 * The raw socket is only used to send packets.
 */
 /*
  raw_fd = socket(AF_PACKET, SOCK_RAW, htons(0x88B5));
  if(raw_fd < 0) die("creating raw socket");
*/

/* the buffer always starts with an ethernet header */
  p_ether = (struct ethhdr *)buffer;
  p_ip = (struct iphdr *) (buffer + ETH_HLEN);

  maxfd = sock_fd > tun_fd ? sock_fd : tun_fd;
  maxfd++;
/* main loop */
  while(1) {

    FD_ZERO(&rset);                                    
    FD_SET(sock_fd, &rset);                           
    FD_SET(tun_fd, &rset);                          

    retn = select(maxfd, &rset, NULL, NULL, NULL);
    if(retn == -1) perror("select()");
    else if(retn == 0) perror("select timeout");           
    else { // retn > 0
        if(FD_ISSET(tun_fd, &rset)) {                
            nread = read(tun_fd,buffer,sizeof(buffer));
            if(nread > 0) {
                ethertype = ntohs(p_ether->h_proto);
                if(ethertype == ETHERTYPE_IP || ethertype == ETHERTYPE_ARP) {
                    if(write(sock_fd, buffer, nread) != nread)
                        perror("error in writing to UDP");
                    //printf("from tun to udp\n");
                }
            } else
                perror("error in reading from tap");
        }                                              
        if(FD_ISSET(sock_fd, &rset)) {               
            nread = read(sock_fd,buffer,sizeof(buffer));
            if(nread > 0) {
                ethertype = ntohs(p_ether->h_proto);
                // send IP packets to the kernel;
                if(ethertype == ETHERTYPE_IP || ethertype == ETHERTYPE_ARP) {
                  // don't know if it's needed, but set the sll_addr anyway.
                  //memcpy(rawSA.sll_addr, p_ether->h_dest, ETH_ALEN);
                  //rawSA.sin_addr.s_addr = p_ip->daddr;
                  //len = nread - ETH_HLEN; 
                  //if(sendto(raw_fd, (char *)p_ip, len, 0, 
                  //  (struct sockaddr *)&rawSA, sizeof(rawSA)) != len) ;
                  //  perror("sendto() error");
                    if(write(tun_fd, buffer, nread) != nread)
                        perror("error in writing to tun");
                } 
            } else 
                perror("error in reading from UDP");
        }                                              
    }
  }
}

