DEBUG_FLAGS = -Wall -Wextra -fsanitize=address
ALL_SOURCE = $(wildcard ./src/*.c)
ALL_HEADERS = $(wildcard ./include/*.h)

tscan: $(ALL_HEADERS) $(ALL_SOURCE)
	$(CC) $(DEBUG_FLAGS) $(ALL_SOURCE) -o tscan

clean:
	rm -rf tscan
