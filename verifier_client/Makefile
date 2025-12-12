# verifier_client/Makefile

CC = gcc
# 중요: ../realm_server 에 있는 헤더파일을 참조하기 위해 -I 옵션 추가
CFLAGS = -Wall -Wextra -std=c99 -I../include
TARGET = verifier_client
OBJS = verifier_client.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) cca_example_token.cbor
