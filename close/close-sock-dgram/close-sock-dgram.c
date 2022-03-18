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
#include <unistd.h>

#include "timespecop.h"

int do_socket()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        err(1, "socket");
    }

    struct timespec ts0, ts1, diff;
    clock_gettime(CLOCK_MONOTONIC, &ts0);
    int n = close(sockfd);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (n < 0) {
        err(1, "close");
    }
    timespecsub(&ts1, &ts0, &diff);
    printf("close: %ld nsec\n", 1000000000*diff.tv_sec + diff.tv_nsec);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: ./close-sock-dgram n_data\n");
        exit(1);
    }

    int n_data = strtol(argv[1], NULL, 0);
    for (int i = 0; i < n_data; ++i) {
        do_socket();
    }

    return 0;
}
