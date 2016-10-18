travel: travel.c graph.o pq.o hmap.o
	gcc travel.c graph.o hmap.o pq.o -o travel

graph.o: graph.c graph.h
	gcc -c graph.c  

pq.o: pq.c pq.h
	gcc -c pq.c

hmap.o: hmap.c hmap.h
	gcc -c hmap.c