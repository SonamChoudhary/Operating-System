CC      = gcc
CFLAGS  = -pthread -O2 -fmessage-length=0 -pedantic-errors -std=gnu99 -Werror -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wshadow -Wuninitialized -Wold-style-definition
RM      = rm -f


default: all

all: problem_1 problem_2 problem_3

problem_1: problem_1.c
	$(CC) $(CFLAGS) -o problem_1 problem_1.c

problem_2: problem_2.c
	$(CC) $(CFLAGS) -o problem_2 problem_2.c

problem_3: problem_3.c
	$(CC) $(CFLAGS) -o problem_3 problem_3.c


clean veryclean:
	$(RM) problem_1 problem_2 problem_3