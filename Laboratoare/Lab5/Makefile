CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -O0 -lm

ELFS = const_elim basic loop_reorder_better loop_reorder_worse block_lin block_vect

.PHONY: all clean

all: $(ELFS)

clean:
	rm -f $(ELFS)