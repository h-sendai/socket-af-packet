# socket(AF_PACKET, ...)をclose()するのにかかる時間

``socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_PAUSE))``
で作ったソケットをclose()するのにはだいぶ時間がかかる。

たとえば
``socket(AF_INET, SOCK_DGRAM, 0)``で作ったソケットを
``close()``するのは数マイクロ秒だが、AF_PACKETのソケットを
``close()``するのは8000マイクロ秒くらいかかる。
もちろんCPUスピードでもかわるだろうが、1000倍以上かかるようだ。
