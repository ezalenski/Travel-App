/**
 * General description:  priority queue which stores pairs
 *   <id, priority>.  Top of queue is determined by priority
 *   (min or max depending on configuration).
 *
 *   There can be only one (or zero) entry for a particular id.
 *
 *   Capacity is fixed on creation.
 *
 *   IDs are integers in the range [0..N-1] where N is the capacity
 *   of the priority queue set on creation.  Any values outside this
 *   range are not valid IDs.
 **/
#include <stdlib.h>
#include <stdio.h>
#include "pq.h"

typedef struct pq_node {
  double priority; 
  int id;
  int heapindx;
} NODE;


struct pq_struct {
  NODE **arrHeap;
  int **arrID;
  int capacity;
  int size;
  int dir; /* 0 = max_heap; anything else = min_heap */
};
  
  
/**
 * Function: pq_create
 * Parameters: capacity - self-explanatory
 *             min_heap - if 1 (really non-zero), then it is a min-heap
 *                        if 0, then a max-heap
 *                        
 * Returns:  Pointer to empty priority queue with given capacity and
 *           min/max behavior as specified.
 *
 */
PQ * pq_create(int capacity, int min_heap) {
  if(capacity <= 0)
    capacity = 50;
  int i;
  PQ *ret = malloc(sizeof(PQ));
  ret->arrHeap = malloc(sizeof(NODE*) * (capacity + 1));
  ret->arrID = malloc(sizeof(int*)*capacity);
  for(i = 0; i < capacity; i++) {
    ret->arrHeap[i] = NULL;
    ret->arrID[i] = NULL;
  }
  ret->arrHeap[i] = NULL;
  ret->size = 0;
  ret->capacity = capacity;
  ret->dir = 1;
  if(min_heap != 0)
    ret->dir = -1;
  return ret;
}

/**
 * Function: pq_free
 * Parameters: PQ * pq
 * Returns: --
 * Desc: deallocates all memory associated with passed priority 
 *       queue.
 *
 */
void pq_free(PQ * pq) {
  int i;
  for(i = 0; i < pq_capacity(pq); i++)
    if(pq->arrID[i] != NULL)
      free(pq->arrHeap[*(pq->arrID[i])]);
  free(pq->arrHeap);
  free(pq->arrID);
  free(pq);
}

/**
 * Function: perc_up
 * Parameters: priority queue pq
 *             index of the NODE to perc_up
 *          
 * Desc: Percolates up the NODE at the index (if needed)
 *       updates all the array indices of any NODE that are moved
 *       while percolating.
 *
 * Runtime: O(h) where h is the distance between the NODE and the root of the heap.
 */
static void perc_up(PQ * pq, int i) {
  NODE *target = pq->arrHeap[i];
  int dir = pq->dir;
  int p = i/2;
  while(p >= 1 && (pq->arrHeap[p]->priority * dir) < (target->priority * dir)) {
    pq->arrHeap[i] = pq->arrHeap[p];
    pq->arrHeap[p]->heapindx = i;
    i = p;
    p = i/2;
  }
  target->heapindx = i;
  pq->arrHeap[i] = target;
}

/**
 * Function: perc_down
 * Parameters: priority queue pq
 *             index of the NODE to perc_down
 *          
 * Desc: Percolates down the NODE at the index (if needed)
 *       updates all the array indices of any NODE that are moved
 *       while percolating.
 *
 * Runtime: O(h) where h is the distance between the NODE and the bottom of the heap.
 */
static void perc_down(PQ * pq, int i) {
  NODE *target = pq->arrHeap[i];
  int l, r, done, n;
  int dir = pq->dir;
  done = 0;
  n = pq->size;
  l = 2*i;
  r = l+1;
  while(l <= n && !done) {
    int min_i = l;
    if(r <= n && (pq->arrHeap[r]->priority * dir) > (pq->arrHeap[l]->priority * dir)) 
      min_i = r;
    if((pq->arrHeap[min_i]->priority * dir) > (target->priority * dir)) {
      pq->arrHeap[i] = pq->arrHeap[min_i];
      pq->arrHeap[i]->heapindx = i;
      i = min_i;
      l = 2*i;
      r = l+1;
    }
    else
      done = 1;
  }
  target->heapindx = i;
  pq->arrHeap[i] = target;
}

/**
 * Function: pq_insert
 * Parameters: priority queue pq
 *             id of entry to insert
 *             priority of entry to insert
 * Returns: 1 on success; 0 on failure.
 *          fails if id is out of range or
 *            there is already an entry for id
 *          succeeds otherwise.
 *          
 * Desc: self-explanatory
 *
 * Runtime:  O(log n)
 *
 */
int pq_insert(PQ * pq, int id, double priority) {
  if(id < 0 || id >= pq_capacity(pq) || pq_contains(pq, id))
    return 0;
  (pq->size)++;
  int index = pq->size;
  pq->arrHeap[index] = malloc(sizeof(NODE));
  pq->arrHeap[index]->priority = priority;
  pq->arrHeap[index]->id = id;
  pq->arrID[id] = &(pq->arrHeap[index]->heapindx);
  perc_up(pq, index);
  return 1;
}

/**
 * Function: pq_change_priority
 * Parameters: priority queue ptr pq
 *             element id
 *             new_priority
 * Returns: 1 on success; 0 on failure
 * Desc: If there is an entry for the given id, its associated
 *       priority is changed to new_priority and the data 
 *       structure is modified accordingly.
 *       Otherwise, it is a failure (id not in pq or out of range)
 * Runtime:  O(log n)
 *       
 */
int pq_change_priority(PQ * pq, int id, double new_priority) {
  if(id < 0 || id > pq->capacity || !pq_contains(pq, id))
    return 0;
  double old_priority;
  int dir = pq->dir;
  int index = *(pq->arrID[id]);
  NODE *target = pq->arrHeap[index];
  old_priority = target->priority;
  target->priority = new_priority;
  
  if((old_priority * dir) > (new_priority * dir)) 
      perc_down(pq, index);
    else 
      perc_up(pq, index);
  return 1;
}

/**
 * Function: pq_remove_by_id
 * Parameters: priority queue pq, 
 *             element id
 * Returns: 1 on success; 0 on failure
 * Desc: if there is an entry associated with the given id, it is
 *       removed from the priority queue.
 *       Otherwise the data structure is unchanged and 0 is returned.
 * Runtime:  O(log n)
 *
 */
int pq_remove_by_id(PQ * pq, int id) {
  if(id < 0 || id > pq->capacity || !pq_contains(pq, id))
    return 0;
  int index = *(pq->arrID[id]);
  NODE *target = pq->arrHeap[index];
  pq->arrHeap[index] = pq->arrHeap[pq->size];
  //  pq->arrHeap[pq->size] = NULL;
  perc_down(pq, index);
  pq->arrID[id] = NULL;
  free(target);
  (pq->size)--;
  return 1;
}

/**
 * Function: pq_get_priority
 * Parameters: priority queue pq
 *             elment id
 *             double pointer priority ("out" param)
 * Returns: 1 on success; 0 on failure
 * Desc: if there is an entry for given id, *priority is assigned 
 *       the associated priority and 1 is returned.
 *       Otherwise 0 is returned and *priority has no meaning.
 * Runtime:  O(1)
 *
 */
int pq_get_priority(PQ * pq, int id, double *priority) {
  if(pq_contains(pq, id)) {
    *priority = pq->arrHeap[*(pq->arrID[id])]->priority;
    return 1;
  }
  return 0;
}

/**
 * Function: pq_delete_top
 * Parameters: priority queue pq
 *             int pointers id and priority ("out" parameters)
 * Returns: 1 on success; 0 on failure (empty priority queue)
 * Desc: if queue is non-empty the "top" element is deleted and
 *       its id and priority are stored in *id and *priority; 
 *       The "top" element will be either min or max (wrt priority)
 *       depending on how the priority queue was configured.
 *    
 *       If queue is empty, 0 is returned.
 *
 * Runtime:  O(log n)
 *
 *
 */
int pq_delete_top(PQ * pq, int *id, double *priority) {
  if(pq->size <= 0)
    return 0;
  *id = pq->arrHeap[1]->id;
  *priority = pq->arrHeap[1]->priority;
  int ret = pq_remove_by_id(pq, *id);
  if(ret == 0) 
    fprintf(stderr, "ERROR: pq_delete_top failed to delete top.\n");
  return ret;
}

/**
 * Function:  pq_capacity
 * Parameters: priority queue pq
 * Returns: capacity of priority queue (as set on creation)
 * Desc: see returns
 *
 * Runtime:   O(1)
 *
 */
int pq_capacity(PQ * pq) {
  return pq->capacity;
}

/**
 * Function: pq_size
 * Parameters: priority queue pq
 * Returns: number of elements currently in queue
 * Desc: see above
 *
 * Runtime:  O(1)
 */
int pq_size(PQ * pq) {
  return pq->size;
}

/**
 * Function: pq_contains
 * Parameters: priority queue pq
 *             element id
 * Returns: 1 if there is an entry in the queue for given id
 *          0 otherwise
 * Desc: see above
 *
 * Runtime:  O(1)
 *
 * Note:  same return value as pq_get_priority
 *
 */
 int pq_contains(PQ * pq, int id) {
   if( id >= 0 && id < pq->capacity && pq->arrID[id] != NULL)
     return 1;
   return 0;
 }
