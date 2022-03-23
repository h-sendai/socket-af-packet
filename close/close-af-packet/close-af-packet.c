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

int do_socket()
{
    int sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_PAUSE));
    if (sockfd < 0) {
        err(1, "socket");
    }

    struct timeval tv0, tv1, diff;
    gettimeofday(&tv0, NULL);
    int n = close(sockfd);
    gettimeofday(&tv1, NULL);
    if (n < 0) {
        err(1, "close");
    }
    timersub(&tv1, &tv0, &diff);
    printf("close: %ld usec\n", 1000000*diff.tv_sec + diff.tv_usec);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: close-af-packet n_data\n");
        exit(1);
    }

    int n_data = strtol(argv[1], NULL, 0);
    for (int i = 0; i < n_data; ++i) {
        fprintf(stderr, "%d\n", i);
        do_socket();
    }

    return 0;
}
