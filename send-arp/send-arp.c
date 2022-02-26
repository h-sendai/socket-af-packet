#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* http://www.microhowto.info/howto/send_an_arbitrary_ethernet_frame_using_an_af_packet_socket_in_c.html */
#include <errno.h>               /* errno */
#include <string.h>              /* memcpy, strerror, strlen */
#include <arpa/inet.h>           /* in_addr_t, htons */
#include <net/ethernet.h>        /* ETHER_ADDR_LEN, ETH_P_* */
#include <net/if.h>              /* struct ifreq */
#include <netinet/if_ether.h>    /* struct ether_arp */
#include <netpacket/packet.h>    /* struct sockaddr_ll */
#include <sys/ioctl.h>           /* SIOCGIFINDEX, ioctl */
#include <sys/socket.h>          /* struct sockaddr, struct iovec, struct msghdr, AF_PACKET, SOCK_DGRAM, socket, sendto, sendmsg */

int main(int argc, char *argv[])
{

    /* Create the AF_PACKET socket */
    int fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
    if (fd == -1) {
        err(1, "socket");
    }

    /* Determine the index number of the Ethernet interface to be used */
    struct ifreq ifr;
    char *if_name = "enp0s3";
    size_t if_name_len=strlen(if_name);
    if (if_name_len<sizeof(ifr.ifr_name)) {
        memcpy(ifr.ifr_name,if_name,if_name_len);
        ifr.ifr_name[if_name_len]=0;
    } else {
        errx(1, "interface name is too long");
    }
    if (ioctl(fd,SIOCGIFINDEX,&ifr)==-1) {
        err(1, "ioctl(fd,SIOCGIFINDEX,&ifr)");
    }
    int ifindex=ifr.ifr_ifindex;

    /* Construct the destination address */
    const unsigned char ether_broadcast_addr[]= {0xff,0xff,0xff,0xff,0xff,0xff};
    struct sockaddr_ll addr={0};
    addr.sll_family=AF_PACKET;
    addr.sll_ifindex=ifindex;
    addr.sll_halen=ETHER_ADDR_LEN;
    addr.sll_protocol=htons(ETH_P_ARP);
    memcpy(addr.sll_addr,ether_broadcast_addr,ETHER_ADDR_LEN);

    /* Send the Ethernet frame */

    struct ether_arp req;
    req.arp_hrd=htons(ARPHRD_ETHER);
    req.arp_pro=htons(ETH_P_IP);
    req.arp_hln=ETHER_ADDR_LEN;
    req.arp_pln=sizeof(in_addr_t);
    req.arp_op=htons(ARPOP_REQUEST);
    memset(&req.arp_tha,0,sizeof(req.arp_tha));

    // const char* target_ip_string="192.168.56.1";
    const char* target_ip_string="10.0.2.2";
    const char* sender_ip_string="10.0.2.15";
    struct in_addr target_ip_addr={0};
    struct in_addr sender_ip_addr={0};
    if (!inet_aton(target_ip_string,&target_ip_addr)) {
        errx(1, "%s is not a valid IP address",target_ip_string);
    }
    if (!inet_aton(sender_ip_string,&sender_ip_addr)) {
        errx(1, "%s is not a valid IP address",sender_ip_string);
    }
    memcpy(&req.arp_tpa,&target_ip_addr.s_addr,sizeof(req.arp_tpa));
    memcpy(&req.arp_spa,&sender_ip_addr.s_addr,sizeof(req.arp_spa));

    /* Send the frame (using sendto) */

    if (sendto(fd,&req,sizeof(req),0,(struct sockaddr*)&addr,sizeof(addr))==-1) {
        err(1, "sendto");
    }

    return 0;
}
