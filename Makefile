TARGET = can
CC = gcc
RM = rm -rf
RMDIR = rm -rf
MKDIR = mkdir -p
DEPS=$(OBJS:.o=.d)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

OBJS = can.o dstring.o

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

