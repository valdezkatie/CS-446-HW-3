sched: sched.c
	gcc -Wall -Wextra -std=c99 sched.c -o sched -pthread

clean:
	rm -f sched-o sched