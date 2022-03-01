# Ethernet padding

socket(AF_PACKET, SOCK_RAW, ...)を使って、16バイトのデータの
イーサネットフレームを送信してみる。

## 送信元tcpdump

```
% sudo tcpdump -nn -i exp0 -e -XX
dropped privs to tcpdump
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on exp0, link-type EN10MB (Ethernet), capture size 262144 bytes
12:57:02.885374 00:15:17:1c:ef:9d > ff:ff:ff:ff:ff:ff, ethertype Unknown (0xfeed), length 30:
        0x0000:  ffff ffff ffff 0015 171c ef9d feed 5858  ..............XX
        0x0010:  5858 5858 5858 5858 5858 5858 5858       XXXXXXXXXXXXXX
```

## 受信先tcpdump

```
12:57:02.884033 00:15:17:1c:ef:9d > ff:ff:ff:ff:ff:ff, ethertype Unknown (0xfeed), length 60:
        0x0000:  ffff ffff ffff 0015 171c ef9d feed 5858  ..............XX
        0x0010:  5858 5858 5858 5858 5858 5858 5858 0000  XXXXXXXXXXXXXX..
        0x0020:  0000 0000 0000 0000 0000 0000 0000 0000  ................
        0x0030:  0000 0000 0000 0000 0000 0000            ...........
```

受信先ではethernet最小長になるようにパディングされているフレームが
観測される。
