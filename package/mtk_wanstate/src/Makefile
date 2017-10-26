#CFLAGS += $(shell pkg-config --cflags json-c)
#LDFLAGS += $(shell pkg-config --libs json-c) -luci
# LDFLAGS +=-ljson-c -luci -lglutil
LDFLAGS +=

PROC=mtk_wanstate
SRC=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all : $(PROC)
	@echo "compile done"

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(PROC): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS) 

clean:
	rm -fr $(PROC) $(OBJS)
