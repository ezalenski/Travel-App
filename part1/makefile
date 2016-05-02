driver: driver.c pq.o
	gcc driver.c pq.o -o driver

test: test.c pq.o
	gcc test.c pq.o -o test

pq.o: pq.c pq.h
	gcc -c pq.c