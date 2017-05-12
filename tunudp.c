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
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>

/* This is a UDP tunnel program. It receives packets from applications via
 * a tun device, forwards them to a remote endpoint via a UDP connection.
 * When packets are received from the UDP connection, they are written back
 * to the tun device, and will be received by apps.
 *
 * usage: tunudp tun_dev local_addr local_port remote_addr remote_port";
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

/* Buffer should be at least the MTU size of the interface */
  const int FRAMESIZE = 65536;

  char tun_name[IFNAMSIZ];
  char buffer[FRAMESIZE];
  int tun_fd, sock_fd, maxfd, nread, retn;
  // int raw_fd;
  struct sockaddr_in localSA, remoteSA;
  //struct sockaddr_in  rawSA; 
  fd_set rset;
  //int one = 1;

  struct iphdr *p_ip;
  struct tcphdr *p_tcp;

  char usage[] = "usage: tunudp dev local_addr local_port remote_addr remote_port";

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
  tun_fd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI); 
  if(tun_fd < 0) die("Allocating interface");

  if(ioctl(tun_fd, TUNSETNOCSUM, 1) < 0)
      die("ioctl TUNSETNOCSUM error");


/* create a UDP socket and connect to a remote IP:port */
  sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sock_fd < 0) die("creating UDP socket");

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

/* the buffer always starts with an IP header */
  p_ip = (struct iphdr *) buffer;

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
            nread = read(tun_fd, buffer, sizeof(buffer));
            if(nread > 0) {
                // allow TCP packets in the tunnel.
                //if(p_ip->protocol == IPPROTO_TCP) {
                    if(write(sock_fd, buffer, nread) != nread)
                        perror("error in writing to UDP");
                    //printf("from tun to udp\n");
                //}
            } else
                perror("error in reading from tun");
        }                                              
        if(FD_ISSET(sock_fd, &rset)) {               
            nread = read(sock_fd,buffer,sizeof(buffer));
            if(nread > 0) {
                //if(p_ip->protocol == IPPROTO_TCP) {
//                  p_tcp = (struct tcphdr *)(buffer + p_ip->ihl * 4); 
//                  /* not sure setting rawSA is required or not. setting it anyway. */
//                  rawSA.sin_addr.s_addr = p_ip->daddr; 
//                  rawSA.sin_port = p_tcp->dest;
//                  if(sendto(raw_fd, buffer, nread, 0, 
//                    (struct sockaddr *)&rawSA, sizeof(rawSA)) != nread)
//                      perror("sendto() error");
                    if(write(tun_fd, buffer, nread) != nread)
                        perror("error in writing to tun");
                //printf("from udp to tun\n");
                //}
            } else 
                perror("error in reading from UDP");
        }                                              
    }
  }
}

