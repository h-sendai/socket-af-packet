/* https://www.geekpage.jp/programming/linux-network/get-macaddr.php */

#include "fill_address.h"
/*
 * The structure ether_addr is defined in <net/ethernet.h> as:
 * struct ether_addr {
 *     uint8_t ether_addr_octet[6];
 * }
 */

int fill_sockaddr_in(char *if_name, struct sockaddr_in *sa)
{
    int fd;
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        warn("socket");
        return -1;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
        warn("ioctl(fd, SIOCGIFADDR, &ifr)");
        return -1;
    }

    close(fd);

    struct sockaddr_in *rv = (struct sockaddr_in *)&ifr.ifr_addr;
    memcpy(sa, rv, sizeof(struct sockaddr_in));

    return 0;
}

int fill_mac_address(char *if_name, struct ether_addr *ether_addr)
{
    int fd;
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        warn("socket");
        return -1;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
        warn("ioctl(fd, SIOCGIFHWADDR, &ifr)");
        return -1;
    }

    close(fd);

    for (int i = 0; i < 6; ++i) {
        ether_addr->ether_addr_octet[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
    }

    return 0;
}

#if USE_MAIN

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: get-mac-address if_name\n");
        fprintf(stderr, "if_name: eth0, enp0s3 etc.\n");
        exit(1);
    }

    char *if_name =argv[1];

    struct ether_addr my_mac_address;
    if (fill_mac_address(if_name, &my_mac_address) < 0) {
        exit(1);
    }
    printf("%s\n", ether_ntoa(&my_mac_address));

    struct sockaddr_in sa;
    fill_sockaddr_in(if_name, &sa);
    printf("%s\n", inet_ntoa(sa.sin_addr));
    return 0;
}

#endif
