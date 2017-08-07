PLATFORM=$(shell uname)
CC = gcc
AR = ar

STATIC_LIB =  wdf.a
SHARED_LIB = wdf.so
OBJS = wdf.o

LLIBS = 
CFLAGS = -c -O3 -Wall -fPIC -Werror=declaration-after-statement -std=c89 -pedantic
LDFLAGS = -O3 -Wall --shared

ifeq ($(PLATFORM),Linux)
else
	ifeq ($(PLATFORM), Darwin)
		LLIBS += -dynamiclib -Wl,-undefined,dynamic_lookup
	endif
endif

all : $(SHARED_LIB) $(STATIC_LIB)

$(SHARED_LIB): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LLIBS)

$(STATIC_LIB): $(OBJS)
	$(AR) crs $@ $^

test : test.c wdf.c
	$(CC) -g -o $@ $^

$(OBJS) : %.o : %.c
	$(CC) -o $@ $(CFLAGS) $<

clean : 
	rm -rf $(OBJS) $(SHARED_LIB) $(STATIC_LIB) test test.dSYM

.PHONY : clean

