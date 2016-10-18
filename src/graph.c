#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "hmap.h"
#include "pq.h"
#include "graph.h"

typedef struct lst_node {
  int id;
  double weight;
  struct lst_node *next;
} LST_NODE;

typedef struct vertex_t {
  char *name;
  int id;
  int out_degree;
  LST_NODE *neighbors;
} VERTEX;

struct dijk_rpt {
  GRAPH *g;
  int s;
  double *d;
  int *pred;
};

struct graph {
  int n;              // Size of graph
  VERTEX *vertices;   // Array of vertices
  HMAP_PTR idmap;
};



int g_size(GRAPH *g) {

  return g->n;

}



int g_contains(GRAPH *g, char *name) {
  return hmap_contains(g->idmap, name);
}



static int getNextID(GRAPH *g, char *name, int *i) {
  int ret;
  if(hmap_contains(g->idmap, name)) {
    ret = *(int*)(hmap_get(g->idmap, name)); 
  }
  else {
    ret = *i;
    *i += 1;
    if(hmap_set(g->idmap, name, &(g->vertices[ret].id)) != NULL)
      return -1;
    g->vertices[ret].name = strdup(name);
  }
  return ret;
}

static int getID(GRAPH *g, char *name) {
  int ret;
  if(hmap_contains(g->idmap, name)) {
    ret = *(int*)(hmap_get(g->idmap, name)); 
  }
  else
    ret = -1;
  return ret;
}

GRAPH * g_from_stream(FILE *fp) {
  int n, i;
  char *src, *dest;
  double weight;
  GRAPH *ret;
  LST_NODE *p;

  if(fscanf(fp, "%i", &n) != 1 || n <= 0) {
    fprintf(stderr, "g_from_stream failed\n");
    return NULL;
  }  

  ret = malloc(sizeof(GRAPH));
  ret->n = n;
  ret->vertices = malloc(n*sizeof(VERTEX));
  ret->idmap = hmap_create(n, 0);
  hmap_set_hfunc(ret->idmap, 1);
  for(i = 0; i < n; i++) {
    ret->vertices[i].id = i;
    ret->vertices[i].out_degree = 0;
    ret->vertices[i].name = NULL;
    ret->vertices[i].neighbors = NULL;
  }

  src = malloc(sizeof(char)*(MAX_NAME_LEN+1));
  dest = malloc(sizeof(char)*(MAX_NAME_LEN+1));
  int result;
  i = 0;
  
  while((result = fscanf(fp, "%s %s %lf", src, dest, &weight)) == 3) {
    if(weight > 0 && strcmp(src, dest) != 0) {
      int srcid, destid;
      p = malloc(sizeof(LST_NODE));
      srcid = getNextID(ret, src, &i);
      destid = getNextID(ret, dest, &i);
      if(srcid < 0 || destid < 0) {
	g_free(ret);
   	fprintf(stderr, "g_from_stream failed\n");
	return NULL;
      }
      p->id = destid;
      p->weight = weight;
      p->next = ret->vertices[srcid].neighbors;
      ret->vertices[srcid].neighbors = p;
      ret->vertices[srcid].out_degree++;
      
      p = malloc(sizeof(LST_NODE));
      p->id = srcid;
      p->weight = weight;
      p->next = ret->vertices[destid].neighbors;
      ret->vertices[destid].neighbors = p;
      ret->vertices[destid].out_degree++;
    }
    else {
      if(weight <= 0) 
	fprintf(stderr, "error: non-positive weight. ignoring...\n");
      if(strcmp(src, dest) == 0)
	fprintf(stderr, "error: self-loop. ignoring...\n");
    }
  }
  if(result != EOF) {
    g_free(ret);
    fprintf(stderr, "g_from_stream failed\n");
    return NULL;
  }

  free(src);
  free(dest);
  return ret;
}



void g_free(GRAPH *g)
{
  int i;
  for(i = 0; i<g->n; i++) {
    LST_NODE *cur = g->vertices[i].neighbors;
    while(cur != NULL) {
      LST_NODE *tmp = cur->next;
      free(cur);
      cur = tmp;
    }
    if(g->vertices[i].name != NULL)
      free(g->vertices[i].name);
  }
  hmap_free(g->idmap, 0);
  free(g->vertices);
  free(g);
}



void g_disp(GRAPH *g) {
  int u;
  LST_NODE *p;

  printf("------------\n");
  for(u = 0; u < g->n; u++) {
    printf("%s : < ", g->vertices[u].name);
    p = g->vertices[u].neighbors;
    while(p != NULL) {
      printf("%s %lf ", g->vertices[p->id].name, p->weight);
      p = p->next;
    }
    printf(">\n");
  }
  printf("------------\n");
}



char ** g_get_names(GRAPH *g) {
  int i, n;
  n = g->n;
  char **ret = malloc(sizeof(char*)*n);
  for(i = 0; i < n; i++) {
    if(g->vertices[i].name != NULL) {
      ret[i] = strdup(g->vertices[i].name);
    }
    else
      ret[i] = strdup(" ");
  }
  return ret;
}

PATH_RPT * create_dijk_rpt(GRAPH *g, int s, int n) {
  PATH_RPT *ret = malloc(sizeof(PATH_RPT));
  ret->g = g;
  ret->s = s;
  ret->d = malloc(sizeof(double)*n);
  ret->pred = malloc(sizeof(int)*n);
  return ret;
}

/* should return array that is shortest path from src to dest with the total dist*/
PATH_RPT * g_shortest_path(GRAPH *g, char *src) {
  int u, v, n;
  LST_NODE *p; 
  PATH_RPT *ret;  
  PQ *q = pq_create(g->n, 1);
  n = g->n;
  u = getID(g, src);
  
  if(u == -1) {
    fprintf(stderr, "error: invalid src for shortest path\n");
    pq_free(q);
    return NULL;
  }
  
  ret = create_dijk_rpt(g, u, n);

  for(v = 0; v < n; v++) {
    pq_insert(q, v, DBL_MAX);
    ret->pred[v] = -1;
  }
  
  ret->pred[u] = u;
  pq_change_priority(q, u, 0.0);
  double dist;

  while(pq_size(q) > 0) {
    pq_delete_top(q, &u, &dist);
    ret->d[u] = dist;
    p = g->vertices[u].neighbors;

    while(p != NULL) {
      v = p->id; 
      if(pq_get_priority(q, v, &dist)) {
	if(dist > ret->d[u] + p->weight) {
	  pq_change_priority(q, v, (ret->d[u] + p->weight));
	  ret->pred[v] = u;
	}
      }
      p = p->next;
    }
  }
  pq_free(q);
  return ret;
}



void rpt_free(PATH_RPT *r) {
  free(r->d);
  free(r->pred);
  free(r);
}

/* TODO: get_neighbors (char* src) returns names and weight for each neighbor and how many
 *       get_dist(DIKREP *d, char *name) returns dist from src and best path
 * 
 */

char ** g_get_neighbors(GRAPH *g, char *src, double **weights, int *out_size) {
  int srcid, n;
  LST_NODE *p;
  srcid = getID(g, src);
  if(srcid == -1) {
    fprintf(stderr, "error: invalid src name for get_neighbors\n");
    *out_size = 0;
    (*weights) = NULL;
    return NULL;
  }

  n = g->vertices[srcid].out_degree;
  if(n == 0) {
    *out_size = 0;
    (*weights) = NULL;
    return NULL;
  }

  p = g->vertices[srcid].neighbors;
  char **ret = malloc(sizeof(char*)*n);
  (*weights) = malloc(sizeof(double)*n);
  int i = 0;
  while(p != NULL && i < n) {
    ret[i] = strdup(g->vertices[p->id].name);
    (*weights)[i] = p->weight;
    p = p->next;
    i++;
  }
  if(i != n) {
    fprintf(stderr, "error: out_degree and neighbors do not match\n");
    free(ret);
    free(*weights);
    (*weights) = NULL;
    *out_size = 0;
    return NULL;
  }

  *out_size = i;
  return ret;
}



static char ** rpt_path_r(PATH_RPT *r, int id, int i, int *out_size) {
  char **ret; 
  if(id == r->s || id == -1) {
    if(i == 0)
      return NULL;
    (*out_size) = i;
    ret = malloc(sizeof(char*) * (*out_size));
    return ret;
  }
  ret = rpt_path_r(r, r->pred[id], i+1, out_size);
  if(id >= 0 && r->pred[id] >= 0)
    ret[i] = strdup(r->g->vertices[ r->pred[id] ].name);
  else
    ret[i] = NULL;
  return ret;
}


char ** rpt_path(PATH_RPT *r, char *src, double *out_dist, int *out_size) {
  int n, srcid;
  double dist;

  srcid = getID(r->g, src);
  if(srcid == -1) {
    fprintf(stderr, "error: invalid src name for rpt_path\n");
    *out_size = 0;
    *out_dist = 0;
    return NULL;
  }
  
  *out_dist = r->d[srcid];
  char **ret = rpt_path_r(r, srcid, 1, out_size);
  if(ret != NULL) 
    ret[0] = strdup(r->g->vertices[srcid].name);
  return ret;
}
