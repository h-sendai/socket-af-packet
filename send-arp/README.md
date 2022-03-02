## ARP フォーマット

TCP/Illusrated Vol. 1 (1st edition)
4.4 ARP Packet Format (p. 56)

```
Ethernet destination addr (6)
Ethernet source addr      (6)
Ethernet frame type       (2) ARP request, ARP reply: 0x0806
Hardware type             (2) Ethernet: 0x0001
Protocol type             (2) IP Address: 0x0800
Hardware size             (1) Ethernet:   0x06
Protocol size             (1) IP Address: 0x04
op                        (2) ARP request: 1, ARP reply: 2. RARP request: 3, RARP reply: 4
Sender Hardware address   (6)
Sender IP address         (4)
Target Hardware address   (6)
Target IP address         (4)
```

- ARP requestではSender Hardware addressはEthernetヘッダと
ARP requestの両方に含まれることになる。
- ARP requestはtarget hardware address以外の全ての
フィールドを埋めて送ることになる。

## 構造体

<net/if_arp.h>
```
struct arphdr {
    unsigned short int ar_hrd;          /* Format of hardware address. */
    unsigned short int ar_pro;          /* Format of protocol address. */
    unsigned char ar_hln;               /* Length of hardware address. */
    unsigned char ar_pln;               /* Length of protocol address. */
    unsigned short int ar_op;           /* ARP opcode (command).       */
};
```

<netinet/if_ether.h>
```
struct  ether_arp {
    struct  arphdr ea_hdr;      /* fixed-size header       */
    uint8_t arp_sha[ETH_ALEN];  /* sender hardware address */
    uint8_t arp_spa[4];         /* sender protocol address */
    uint8_t arp_tha[ETH_ALEN];  /* target hardware address */
    uint8_t arp_tpa[4];         /* target protocol address */
};
#define arp_hrd ea_hdr.ar_hrd
#define arp_pro ea_hdr.ar_pro
#define arp_hln ea_hdr.ar_hln
#define arp_pln ea_hdr.ar_pln
#define arp_op  ea_hdr.ar_op
```

## ARP request送信、ARP reply受信側でのtcpdump

Ethernetペイロードの最小値は46バイト。

ARP request送り元でtcpdumpを使ってキャプチャするとARP requestの
Ethernetペイロード長は28バイトのキャプチャがとれる。
``length 42``の42はEther dst (6) + Ether src (6) + Type (2)
をたしたもの（6 + 6 + 2 + 28 = 42)。

ARP replyはpaddingされている。

ARP Request送信のsendto()は28バイトを返す。

```
% sudo tcpdump -nn -i exp0 -e -XX
dropped privs to tcpdump
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on exp0, link-type EN10MB (Ethernet), capture size 262144 bytes
09:19:16.300146 00:15:17:1c:ef:9d > ff:ff:ff:ff:ff:ff, ethertype ARP (0x0806), length 42: Request who-has 192.168.1.205 tell 192.168.1.201, length 28
        0x0000:  ffff ffff ffff 0015 171c ef9d 0806 0001  ................
        0x0010:  0800 0604 0001 0400 0000 0000 c0a8 01c9  ................
        0x0020:  0000 0000 0000 c0a8 01cd                 ..........
09:19:16.300288 00:10:18:35:9a:ef > 04:00:00:00:00:00, ethertype ARP (0x0806), length 60: Reply 192.168.1.205 is-at 00:10:18:35:9a:ef, length 46
        0x0000:  0400 0000 0000 0010 1835 9aef 0806 0001  .........5......
        0x0010:  0800 0604 0002 0010 1835 9aef c0a8 01cd  .........5......
        0x0020:  0400 0000 0000 c0a8 01c9 0000 0000 0000  ................
        0x0030:  0000 0000 0000 0000 0000 0000            ............
```

## ARP request 受信、ARP reply送信側でのtcpdump

ARP Replyを送っている側でのtcpdump
ARP request受信側ではrequestパケットはpaddingされているキャプチャがとれる。
Replyはpaddingなしのキャプチャがとれる。

```
% sudo tcpdump -nn -i exp0 -e -XX
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on exp0, link-type EN10MB (Ethernet), capture size 262144 bytes
09:19:16.304628 00:15:17:1c:ef:9d > ff:ff:ff:ff:ff:ff, ethertype ARP (0x0806), length 60: Request who-has 192.168.1.205 tell 192.168.1.201, length 46
        0x0000:  ffff ffff ffff 0015 171c ef9d 0806 0001  ................
        0x0010:  0800 0604 0001 0400 0000 0000 c0a8 01c9  ................
        0x0020:  0000 0000 0000 c0a8 01cd 0000 0000 0000  ................
        0x0030:  0000 0000 0000 0000 0000 0000            ............
09:19:16.304649 00:10:18:35:9a:ef > 04:00:00:00:00:00, ethertype ARP (0x0806), length 42: Reply 192.168.1.205 is-at 00:10:18:35:9a:ef, length 28
        0x0000:  0400 0000 0000 0010 1835 9aef 0806 0001  .........5......
        0x0010:  0800 0604 0002 0010 1835 9aef c0a8 01cd  .........5......
        0x0020:  0400 0000 0000 c0a8 01c9                 ..........
```
