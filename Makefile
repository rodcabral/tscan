DEBUG_FLAGS = -Wall -Wextra -fsanitize=address
RELEASE_FLAGS = -O2

ALL_SOURCE = $(wildcard ./src/*.c)
ALL_HEADERS = $(wildcard ./include/*.h)

tscan: $(ALL_HEADERS) $(ALL_SOURCE)
	$(CC) $(DEBUG_FLAGS) $(ALL_SOURCE) -o tscan

install: $(ALL_HEADERS) $(ALL_SOURCE)
	$(CC) $(RELEASE_FLAGS) $(ALL_SOURCE) -o tscan
	cp tscan /usr/bin
	mkdir /usr/share/tscan
	cp -r ./share/* /usr/share/tscan

uninstall:
	rm -rf /usr/bin/tscan /usr/share/tscan/

clean:
	rm -rf tscan
