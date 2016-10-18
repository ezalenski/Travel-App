#include <stdio.h>
#include "pq.h"

int main() 
{
  int i;
  double val;
  PQ *q = pq_create(10, 1);
  for(i = 0, val = 1.0; (i < 10 && val > 0); i++, val -= .1) {
    pq_insert(q, i, val);
  }
  while(pq_size(q) > 0){ 
    pq_delete_top(q, &i, &val);
    printf("Min value %.2lf with id %d\n", val, i);
  }
  pq_free(q);
  return 0;
}
