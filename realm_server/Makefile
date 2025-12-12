CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -fPIC -I../include
TARGET = realm_server
OBJS = realm_server.o 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

osv: $(OBJS)
	$(CC) -shared -o $(TARGET).so $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).so
