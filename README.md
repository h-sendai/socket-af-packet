# socket-af-packet

参考URL

- http://www.microhowto.info/howto/send_an_arbitrary_ethernet_frame_using_an_af_packet_socket_in_c.html
- http://www.microhowto.info/tags/socket.html

## プログラム

- [send-arp](send-arp/)
- [ethernet-short-frame-dgram](ethernet-short-frame-dgram/) 最小フレームサイズより小さいサイズのフレームを
sendto()で送るとどこかで自動でパディングしてくれる様子をみる。socket(AF_PACKET, SOCK_DGRAM, ...)を使っている。
- [ethernet-short-frame-raw](ethernet-short-frame-raw/) 最小フレームサイズより小さいサイズのフレームを
sendto()で送るとどこかで自動でパディングしてくれる様子をみる。socket(AF_PACKET, SOCK_RAW, ...)を使っている。
SOCK_DGRAMを使ったとき同様、自動でパディングされている。
