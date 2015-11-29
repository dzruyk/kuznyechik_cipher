CC=gcc
CFLAGS=-Wall -g
OBJS=kuzn.o
TEST_OBJS=kuzn_test.o
SPEED_OBJS=main.o

KUZN_TEST=./kuzn_test
SPEED_TEST=./kuzn_speed

all: $(KUZN_TEST) $(SPEED_TEST)

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@

$(KUZN_TEST): $(OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SPEED_TEST): $(OBJS) $(SPEED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(KUZN_TEST) $(SPEED_OBJS) $(SPEED_OBJS) $(TEST_OBJS) $(SPEED_TEST)
