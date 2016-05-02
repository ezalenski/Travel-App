#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "pq.h"

double * rand_arr(int n) {
  int i;
  double *ret = malloc(sizeof(double) * n);
  for(i = 0; i < n; i++) 
    ret[i] =((double)rand()/(double)RAND_MAX)*10;
  return ret;
}

int cmp_double(const void *a, const void *b) {
  double lfa = *(double*)a;
  double lfb = *(double*)b;
  if((lfa - lfb) > 0.0)
    return 1;
  return -1;
}

int reverse_cmp_double(const void *a, const void *b) {
  double lfa = *(double*)a;
  double lfb = *(double*)b;
  if((lfa - lfb) < 0.0)
    return 1;
  return -1;
}

int arr_double_equal(double *a, double *b, int n){
  int i;
  for(i = 0; i < n; i++) 
    if(a[i] != b[i])
      return 0;
  return 1;
}

double * copy_arr(double *a, int n) {
  int i;
  double *ret = malloc(sizeof(double) * n);
  for(i = 0; i < n; i++)
    ret[i] = a[i];
  return ret;
}

void print_arr(double *a, int n) {
  int i;
  printf(" [");
  for(i = 0; i < n; i++)
    printf(" %.2lf ", a[i]);
  printf("]\n");
}

void print_func_info(char *funcname, double *insertorder, double *sortedorder, double *result, int dir, int n) {
  int i;
  printf("\nFUNC: %s\n", funcname);
  if(dir == 1)
    printf("MIN HEAP\n");
  else
    printf("MAX HEAP\n");
  printf("INSERTIONS MADE:");
  print_arr(insertorder, n);
  printf("EXPECTED VALUES:");
  print_arr(sortedorder, n);
  printf("PROGRAM OUTPUT:");
  print_arr(result, n);
}

void swap(double *a, double *b) {
  double tmp = *a;
  *a = *b;
  *b = tmp;
}

void permute_test_delete_top(double *array, int *testtotal, int *testfail, int i, int length) { 
  if (length == i){
    int c;
    PQ *pmin = pq_create(length, 1);
    PQ *pmax = pq_create(length, 0);
    for(c = 0; c < length; c++) {
      pq_insert(pmin, c, array[c]);
      pq_insert(pmax, c, array[c]);
    }

    int id;
    double *result = malloc(sizeof(double) * length);
    double *sorted = copy_arr(array, length);

    qsort(sorted, length, sizeof(double), cmp_double);
    for(c = 0; c < length; c++) {
      pq_delete_top(pmin, &id, &(result[c]));
    }
    if(pq_size(pmin) != 0 || !arr_double_equal(sorted, result, length)) {
      print_func_info("delete_top", array, sorted, result, 1, length);
      (*testfail)++;
    }
    (*testtotal)++;

    qsort(sorted, length, sizeof(double), reverse_cmp_double);
    for(c = 0; c < length; c++) {
      pq_delete_top(pmax, &id, &(result[c]));
    }
    if(pq_size(pmin) != 0 || !arr_double_equal(sorted, result, length)) {
      print_func_info("delete_top", array, sorted, result, 0, length);
      (*testfail)++;
    }
    (*testtotal)++;
    
    free(result);
    free(sorted);
    pq_free(pmin);
    pq_free(pmax);
    return;
  }
  int j = i;
  for (j = i; j < length; j++) { 
    swap(&(array[i]),&(array[j]));
    permute_test_delete_top(array, testtotal, testfail, i+1, length);
    swap(&(array[i]),&(array[j]));
  }
  return;
}

void permute_test_change_priority(double *array, int *testtotal, int *testfail, int i, int length) { 
  if (length == i){
    int c, k;
    PQ *pmin = pq_create(length, 1);
    PQ *pmax = pq_create(length, 0);
    double *rand_doub = rand_arr(length);
    for(k = 0; k < length; k++) {
      double *targ = malloc(sizeof(double) * length);
      for(c = 0; c < length; c++) {
	targ[c] = array[c];
	pq_insert(pmin, c, array[c]);
	pq_insert(pmax, c, array[c]);
      }
      int id;
      double *result = malloc(sizeof(double) * length);
      
      targ[k] = rand_doub[k];
      pq_change_priority(pmin, k, rand_doub[k]);
      
      for(c = 0; c < length; c++) {
	pq_delete_top(pmin, &id, &(result[c]));
      }
      qsort(targ, length, sizeof(double), cmp_double);
      if(pq_size(pmin) != 0 || !arr_double_equal(targ, result, length)) {
	print_func_info("change_priority", array, targ, result, 1, length);
	(*testfail)++;
      }
      (*testtotal)++;
      
      pq_change_priority(pmax, k, rand_doub[k]);
      
      for(c = 0; c < length; c++) {
	pq_delete_top(pmax, &id, &(result[c]));
      }
      qsort(targ, length, sizeof(double), reverse_cmp_double);
      if(pq_size(pmin) != 0 || !arr_double_equal(targ, result, length)) {
	print_func_info("change_priority", array, targ, result, 0, length);
	(*testfail)++;
      }
      (*testtotal)++;
      
      free(result);
      free(targ);
    }
    pq_free(pmin);
    pq_free(pmax);
    free(rand_doub);
    return;
  }
  int j = i;
  for (j = i; j < length; j++) { 
    swap(&(array[i]),&(array[j]));
    permute_test_change_priority(array, testtotal, testfail, i+1, length);
    swap(&(array[i]),&(array[j]));
  }
  return;
}

void permute_test_get_priority(double *array, int *testtotal, int *testfail, int i, int length) { 
  if (length == i){
    int c;
    PQ *pmin = pq_create(length, 1);
    PQ *pmax = pq_create(length, 0);
    for(c = 0; c < length; c++) {
      pq_insert(pmin, c, array[c]);
      pq_insert(pmax, c, array[c]);
    }

    int id;
    double *result = malloc(sizeof(double) * length);
    
    for(c = 0; c < length; c++) {
      pq_get_priority(pmin, c, &(result[c]));
    }
    if(!arr_double_equal(array, result, length)) {
      print_func_info("get_priority", array, array, result, 1, length);
      (*testfail)++;
    }
    (*testtotal)++;

    for(c = 0; c < length; c++) {
      pq_get_priority(pmax, c, &(result[c]));
    }
    if(!arr_double_equal(array, result, length)) {
      print_func_info("get_priority", array, array, result, 0, length);
      (*testfail)++;
    }
    (*testtotal)++;
    
    free(result);
    pq_free(pmin);
    pq_free(pmax);
    return;
  }
  int j = i;
  for (j = i; j < length; j++) { 
    swap(&(array[i]),&(array[j]));
    permute_test_get_priority(array, testtotal, testfail, i+1, length);
    swap(&(array[i]),&(array[j]));
  }
  return;
}

main() {
  int i;

  /*pq_create*/
  PQ *pmin = pq_create(10, 1);
  PQ *pmax = pq_create(10, 0);
  //PQ *pdefault = pq_create(-4, 0);
  assert(pq_capacity(pmin) == 10);
  //assert(pq_capacity(pdefault) == 50);
  assert(pq_insert(pmin, -1, 2.5) == 0);
  assert(pq_insert(pmax, 11, 2.5) == 0);
  assert(pq_insert(pmin, -1, 2.5) == 0);
  assert(pq_insert(pmax, 11, 2.5)  == 0);
  
  double priority;
  assert(pq_get_priority(pmin, -1, &priority) == 0);
  assert(pq_get_priority(pmax, -1, &priority) == 0);
  assert(pq_get_priority(pmin, 11, &priority) == 0);
  assert(pq_get_priority(pmax, 11, &priority) == 0);
  
  
  assert(pq_change_priority(pmin, -1, 2.5) == 0);
  assert(pq_change_priority(pmax, -1, 2.5) == 0);
  assert(pq_change_priority(pmin, 11, 2.5) == 0);
  assert(pq_change_priority(pmax, 11, 2.5) == 0);


  assert(pq_remove_by_id(pmin, -1) == 0);
  assert(pq_remove_by_id(pmax, -1) == 0);
  assert(pq_remove_by_id(pmin, 11) == 0);
  assert(pq_remove_by_id(pmax, 11) == 0);
  

  int id;
  assert(pq_delete_top(pmin, &id, &priority) == 0);
  assert(pq_delete_top(pmax, &id, &priority) == 0);
  


  /*pq_free*/
  pq_free(pmin);
  pq_free(pmax);
  //pq_free(pdefault);
  srand(time(NULL));
  int testt = 0, testf = 0;
  int n = 7;
  double *test = rand_arr(n);
  qsort(test, n, sizeof(double), cmp_double);
  permute_test_delete_top(test, &testt, &testf, 0, n);
  permute_test_change_priority(test, &testt, &testf, 0, n); 
  permute_test_get_priority(test, &testt, &testf, 0, n);
  int testp = testt - testf;
  printf("\n%i/%i PASSED\n", testp, testt);
  free(test);
}
