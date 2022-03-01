#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>          /* SIOCGIFINDEX, ioctl() */
#include <sys/socket.h>         /* struct sockaddr, struct iovec, */
                                /* struct msghdr, AF_PACKET, SOCK_DGRAM, */
                                /* socket, sendto, sendmsg */

#include <arpa/inet.h>          /* in_addr_t, hton */
#include <net/ethernet.h>       /* ETHER_ADDR_LEN, ETH_P_* */
#include <net/if.h>             /* struct ifreq */
#include <netinet/if_ether.h>   /* struct ether_arp */
#include <netpacket/packet.h>   /* struct sockaddr_ll */

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "fill_mac_address.h"

int usage(void)
{
    char msg[] = "Usage: send-short-frame-raw if_name";
    fprintf(stderr, "%s\n", msg);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage();
        exit(1);
    }

    char *if_name  = argv[1];

    /* create AF_PACKET socket */
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        err(1, "socket");
    }

    /* Determine the index number of the Ethernet interface to be used. */
    unsigned int if_index;
    if ( (if_index = if_nametoindex(if_name)) == 0) {
        err(1, "if_nametoindex");
    }
    
    /* Construct the destination address */
    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(addr));

    addr.sll_family   = AF_PACKET;
    addr.sll_ifindex  = if_index;
    addr.sll_halen    = ETHER_ADDR_LEN;
    //addr.sll_protocol = htons(ETH_P_ALL);
    addr.sll_protocol = htons(0xfeed);
    addr.sll_addr[0]  = 0xff;
    addr.sll_addr[1]  = 0xff;
    addr.sll_addr[2]  = 0xff;
    addr.sll_addr[3]  = 0xff;
    addr.sll_addr[4]  = 0xff;
    addr.sll_addr[5]  = 0xff;

    /* Short ethernet payload (minimum ethernet payload is 46) */
    unsigned char en_payload[6+6+2+16];

    /* Ethernet dest address */
    for (int i = 0; i < 6; ++i) {
        en_payload[i] = 0xff;
    }

    /* Ethernet src address */
    //for (int i = 6; i < 12; ++i) {
    //    en_payload[i] = i;
    //}
    fill_mac_address(if_name, (struct ether_addr *)&en_payload[6]);

    /* type */
    en_payload[12] = 0xfe;
    en_payload[13] = 0xed;
    for (int i = 14; i < sizeof(en_payload); ++i) {
        en_payload[i] = 'X';
    }

    /* Send the Ethernet frame. */
    int n = sendto(sockfd, en_payload, sizeof(en_payload), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (n < 0) {
        err(1, "sendto");
        return -1;
    }
    printf("sendto: %d bytes\n", n);
    close(sockfd);

    return 0;
}
