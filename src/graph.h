#ifndef GRAPH_H
#define GRAPH_H
#define MAX_NAME_LEN 10

typedef struct graph GRAPH;

typedef struct dijk_rpt PATH_RPT;

extern GRAPH * g_from_stream(FILE *fp);

extern void g_disp(GRAPH *g);

extern int g_contains(GRAPH *g, char *name);

extern void g_free(GRAPH *g);

extern int g_size(GRAPH *g);

extern char ** g_get_names(GRAPH *g);

extern PATH_RPT *  g_shortest_path(GRAPH *g, char *src);

extern void rpt_free(PATH_RPT *r);

extern char ** g_get_neighbors(GRAPH *g, char *src, double **weights, int *out_size);

extern char ** rpt_path(PATH_RPT *r, char *src, double *out_dist, int *out_size);

#endif
