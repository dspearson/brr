CC ?= gcc
CFLAGS = -g -O2 -Wall -Wextra -fPIE -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fsanitize=address
CFLAGS += -Wformat -Wformat-security -Werror=format-security -Wl,-z,relro -Wl,-z,now -Werror
CFLAGS += -Wshadow -Wpointer-arith -Wcast-align -Wformat=2 -Wstrict-overflow=5 -Wstrict-aliasing=2
TARGET = brr

all: $(TARGET)

$(TARGET): brr.c
	$(CC) $(CFLAGS) brr.c -o $(TARGET)

clean:
	rm -f $(TARGET)
