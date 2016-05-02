
#include <stdio.h>
#include <stdlib.h>
#include "pq.h"

/**
* takes a full line of user input and 
*   parses it.
*
* Executes specified command and prints appropriate
*   message if syntactially correct.
*   otherwise prints an error message.
*
* Returns 1 only to indicate the the quite command 'q' was 
*   entered.
* Otherwise, 0 is returned.
*/
int execute_cmd(PQ *q, char *buf) {
int tok;
char cmd;
int id;
 double priority;
char junk[128];

 tok = sscanf(buf, " %c %i %lf %s", &cmd, &id, &priority, junk);

  if(tok==0) return 0;
  if(tok > 3){
	printf("    bad command. try again\n");
	return 0;
  }
  switch (cmd) {
	case 'q':
		if(tok != 1){
		  printf("    bad command. try again\n");
		  return 0;
		}
		else{
		  printf("  goodbye...\n");
		  return 1;
		}
	case 'p' : 
		if(tok != 1)
		  printf("    bad command. try again\n");
		else
		  pq_print(q);
		return 0;
	case 's':
		if(tok != 1)
		  printf("    bad command. try again\n");
		else
		  printf("  len: %i\n", pq_size(q));
		return 0;
	case 'i':
		if(tok != 3)
		  printf("    bad command. try again\n");
		else							
		  if(pq_insert(q, id, priority))
		    printf("  insert successful\n");
		  else
		    printf("  insert failed\n");
		return 0;
	case 'c':
		if(tok != 3)
		  printf("    bad command. try again\n");
		else							
		  if(pq_change_priority(q, id, priority))
		    printf("  change successful\n");
		  else
		    printf("  change failed\n");
		return 0;
	case 'd':
		if(tok != 1)
		  printf("    bad command. try again\n");
		else
		  if(pq_delete_top(q, &id, &priority))
		    printf("  delete_top: id = %i priority = %lf\n", id, priority);
		  else
		    printf("  delete_top failed\n");
		return 0;
	case 'r':
		if(tok != 2)
		  printf("    bad command. try again\n");
		else {
		  if(pq_remove_by_id(q, id)) 
			printf("  %i is outta here!\n", id);
		  else 
			printf("  could not remove %i!\n", id);
		}
		return 0;
	case 'g':
		if(tok != 2)
		  printf("    bad command. try again\n");
		else {
		  if(pq_get_priority(q, id, &priority)) 
		    printf("  priority of id %i is %lf\n", id, priority);
		  else 
		    printf("  bad id\n");

		}
		return 0;
	default:
	    printf("    bad command. try again\n");
	    return 0;
  }
}
	




int main() {
char buf[128];
int done = 0;

  printf("\nWelcome to the simple service-queue interactive program\n");
  printf("   An empty service queue has been created for you\n");
  printf("   Commands:\n");
  printf("    d                 : delete top\n");
  printf("    r <id>            : remove specified id\n");
  printf("    g <id>            : get priority of id\n");
  printf("    p                 : print priority queue\n");
  printf("    s                 : get current queue size\n");
  printf("    i <id> <priority> : insert id and priority into the queue\n");
  printf("    c <id> <priority> : change priority of id in the queue\n");
  printf("    q                 : quit\n");
  printf("-----------------------------------\n\n");
  
  PQ *q = pq_create(100, 0);
  while(!done) {
	printf("cmd > ");
	if(fgets(buf, 127, stdin) != NULL) {
	    if(execute_cmd(q, buf))
		done = 1;
	}
  }
  pq_free(q);
  return 0;
}




