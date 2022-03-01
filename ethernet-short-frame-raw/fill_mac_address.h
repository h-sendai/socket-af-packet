#ifndef _FILL_MAC_ADDRESS
#define _FILL_MAC_ADDRESS 1

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <net/ethernet.h> /* struct ether_addr */
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ether.h> /* ether_ntoa() */

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*
 * int fill_mac_address(char *if_name, struct ether_addr *my_mac_address);
 * if_name (eth0, etc)を指定してそのNICのMACアドレスを取得する。
 * 結果は呼び出し側が確保したstruct ether_addr構造体に入れる。
 * struct ether_addrは<net/ethernet.h>で定義されている:
 * struct ether_addr {
 *     uint8_t ether_addr_octet[6];
 * }
 * 途中失敗すると-1を返す。正常終了なら0を返す。
 *
 * 例題
 *
 * struct ether_addr my_mac_address;
 * fill_mac_address("eth0", &my_mac_address);
 * printf("%s\n", ether_ntoa(&my_mac_address));
 *
 */

extern int fill_mac_address(char *if_name, struct ether_addr *my_mac_address);
#endif
