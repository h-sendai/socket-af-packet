PROG = close-af-packet
CFLAGS += -g -O2 -Wall
CFLAGS += -std=gnu99
# CFLAGS += -pthread
# LDLIBS += -L/usr/local/lib -lmylib
# LDLIBS += -lrt
# LDFLAGS += -pthread

all: $(PROG)
# OBJS += $(PROG).o
# OBJS += file.o
# $(PROG): $(OBJS)
	sudo setcap 'CAP_NET_RAW+eip' $(PROG)

clean:
	rm -f *.o $(PROG)
