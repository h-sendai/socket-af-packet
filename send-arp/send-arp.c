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
#include <errno.h>            /* errno */
#include <string.h>           /* memcpy, strerror, strlen */
#include <arpa/inet.h>        /* in_addr_t, htons */
#include <net/ethernet.h>     /* ETHER_ADDR_LEN, ETH_P_* */
#include <net/if.h>           /* struct ifreq */
#include <netinet/if_ether.h> /* struct ether_arp */
#include <netpacket/packet.h> /* struct sockaddr_ll */
#include <sys/ioctl.h>        /* SIOCGIFINDEX, ioctl */
#include <sys/socket.h>       /* struct sockaddr, struct iovec, struct msghdr, AF_PACKET, SOCK_DGRAM, socket, sendto, sendmsg */

#include "fill_address.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: send-arp if_name target_ip_address\n");
        fprintf(stderr, "example: ./send-arp eth0 192.168.10.10\n");
        exit(1);
    }

    char *if_name          = argv[1];
    char *target_ip_string = argv[2];

    /* Create the AF_PACKET socket */
    int sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
    if (sockfd == -1) {
        err(1, "socket");
    }

    /* Determine the index number of the Ethernet interface to be used */
    struct ifreq ifr;
    size_t if_name_len = strlen(if_name);
    if (if_name_len < sizeof(ifr.ifr_name)) {
        memcpy(ifr.ifr_name, if_name, if_name_len);
        ifr.ifr_name[if_name_len] = 0;
    } else {
        errx(1, "interface name is too long");
    }
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1) {
        err(1, "ioctl(sockfd, SIOCGIFINDEX, &ifr)");
    }
    int ifindex = ifr.ifr_ifindex;

    /* Construct the destination address */
    const unsigned char ether_broadcast_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    struct sockaddr_ll addr = {0};
    addr.sll_family  = AF_PACKET;
    addr.sll_ifindex = ifindex;
    addr.sll_halen   = ETHER_ADDR_LEN;
    addr.sll_protocol= htons(ETH_P_ARP);
    memcpy(addr.sll_addr,ether_broadcast_addr,ETHER_ADDR_LEN);

    /*
     * need bind() to recv() at the bottom of this program
     * not to recv() the ARPs from the other network interfaces.
     * If we do sendto() only, does not need this bind().
     */
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        err(1, "bind");
    }

    /* Prepare ARP request structure */
    struct ether_arp req;

    req.arp_hrd = htons(ARPHRD_ETHER);
    req.arp_pro = htons(ETH_P_IP);
    req.arp_hln = ETHER_ADDR_LEN;
    req.arp_pln = sizeof(in_addr_t);
    req.arp_op  = htons(ARPOP_REQUEST);

    /* sender hardware address (MAC address) */
    struct ether_addr my_mac_address;
    fill_mac_address(if_name, &my_mac_address);
    for (int i = 0; i < 6; ++i) {
        req.arp_sha[i] = my_mac_address.ether_addr_octet[i];
    }

    /* sender protocol address (IP address) */
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    fill_sockaddr_in(if_name, &sa);
    memcpy(&req.arp_spa, &sa.sin_addr.s_addr, sizeof(sa.sin_addr.s_addr));

    /* target hardware address (MAC address) */
    memset(&req.arp_tha, 0, sizeof(req.arp_tha));

    /* target protocol address (IP address) */
    struct in_addr target_ip_addr = {0};
    if (inet_aton(target_ip_string, &target_ip_addr) == 0) { /* inet_aton() returns 0 if error */
        errx(1, "%s is not a valid IP address", target_ip_string);
    }
    memcpy(&req.arp_tpa, &target_ip_addr.s_addr, sizeof(req.arp_tpa));
    /* Prepare ARP request structure DONE */

    /* Send the frame (using sendto) */

    int n;
    n = sendto(sockfd, &req, sizeof(req), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n < 0) {
        err(1, "sendto");
    }
    printf("sendto return value: %d\n", n); 

    unsigned char reply_buf[1500];
    /* if there is no ARP reply, program will block here */
    n = recv(sockfd, reply_buf, sizeof(reply_buf), 0);
    printf("recv return value: %d\n", n);

    for (int i = 0; i < n; ++i) {
        if (i % 8 != 0) {
            printf(" ");
        }
        printf("%02x", reply_buf[i]);
        if ((i+1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    return 0;
}
