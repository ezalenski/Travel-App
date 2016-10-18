#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "graph.h"

static void trim_string(char *s) {
  int i, j, skip;

  j = strlen(s)-1;
  while(j>=0 && isspace(s[j])){
    s[j] = '\0';
    j--;
  }
  skip = 0;
  while(s[skip] != '\0' && isspace(s[skip]))
    skip++;

  if(skip > 0) {
    for(i = 0, j = skip; s[j] != '\0'; i++,j++) {
      s[i] = s[j];
      s[j] = '\0';
    }
  }
}

void names_free(char **names, int n) {
  int i;
  for(i = 0; i < n; i++)
    free(names[i]);
  free(names);
}

void names_print(char **names, int n) {
  int i;
  for(i = 0; i < n; i++) {
    printf("%s ", names[i]);
  }
}

char * get_next_move(PATH_RPT *r, char *loc, double *dist) {
  char **path;
  int npath, i;
  path = rpt_path(r, loc, dist, &npath);
  char *ret = path[1];
  free(path[0]);
  for(i = 2; i < npath; i++) {
    free(path[i]);
  }
  free(path);
  return ret;
}

double start_travel(GRAPH *g, PATH_RPT *r, char *loc, char *dest) {
  double dist, ret;
  int j, recnum, nNeighbors, choice;
  double *wNeighbors;
  char *recmove;
  ret = 0;
  while(strcmp(loc, dest) != 0) {
    recmove = get_next_move(r, loc, &dist);
    printf("CURRENT LOCATION:\t%s\n", loc);
    printf("DESTINATION     :\t%s\n", dest);
    printf("MINIMUM DISTANCE TO DESTINATION:\t%.2lf\n\n", dist);
    printf("POSSIBLES MOVES:\n\n");
    
    char **locNeighbors = g_get_neighbors(g, loc, &wNeighbors, &nNeighbors);
    printf("\t0. I give up!\n");
    for(j = 0; j < nNeighbors; j++) {
      if(strcmp(locNeighbors[j], recmove) == 0)
	recnum = j+1;
      printf("\t%d. %s (%.2lf units)\n", (j+1), locNeighbors[j], wNeighbors[j]);
    }
    
    printf("RECOMMENDED MOVE:\t%d. %s\n\n", recnum, recmove);
    printf("SELECT A MOVE (ENTER A NUMBER):\t");
    while(scanf("%d", &choice) != 1 || choice < 0 || choice > nNeighbors) {
      printf("ERROR: INVALID CHOICE\n");
      printf("SELECT A MOVE (ENTER A NUMBER):\t");
      scanf("%*[^\n]");
    } 
    choice--;
    if(choice < 0) {
      free(loc);
      free(recmove);
      free(wNeighbors);
      names_free(locNeighbors, nNeighbors);
      return -1;
    }
    free(loc);
    loc = strdup(locNeighbors[choice]);
    ret += wNeighbors[choice];
    printf("\nTOTAL DISTANCE TRAVELED:\t%.2lf units\n", ret);
    free(recmove);
    free(wNeighbors);
    names_free(locNeighbors, nNeighbors);
  }
  free(loc);
  return ret;
}

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("usage:  travel <graph_file>\n");
    return 0;
  }

  FILE *fp = fopen(argv[1], "r");
  GRAPH *g = g_from_stream(fp);
  char **names = g_get_names(g);
  char *loc, *dest;
  int i;

  loc = malloc(sizeof(char)*(MAX_NAME_LEN+1));
  dest = malloc(sizeof(char)*(MAX_NAME_LEN+1));
  
  printf("Welcome to travel planner.\n\n");
  printf("Vertices:\n\n");
  printf("\t");
  names_print(names, g_size(g));
  printf("\n\n");
  printf("SELECT YOUR CURRECT LOCATION:\t");
  fgets(loc, MAX_NAME_LEN+1, stdin);
  trim_string(loc);
  while(!g_contains(g, loc)) {
    printf("INVALID LOCATION\n");
    printf("SELECT YOUR CURRECT LOCATION:\t");
    fgets(loc, MAX_NAME_LEN+1, stdin);
    trim_string(loc);
  }
  printf("SELECT YOUR DESTINATION     :\t");
  fgets(dest, MAX_NAME_LEN+1, stdin);
  trim_string(dest);
  while(!g_contains(g, dest)) {
    printf("INVALID LOCATION\n");
    printf("SELECT YOUR DESTINATION     :\t");
    fgets(dest, MAX_NAME_LEN+1, stdin);
    trim_string(dest);
  }
  printf("\n");



  PATH_RPT *dijk = g_shortest_path(g, dest);
  double dist;
  int npath; 
  char **path = rpt_path(dijk, loc, &dist, &npath);
  if(dist < DBL_MAX) {
    printf("You can reach your destination in %.2lf units.\n\n", dist);
    printf("SHORTEST PATH:\n");
    for(i = 0; i < npath-1; i++) {
      printf("\t%s ->\n", path[i]);
    }
    printf("\t%s\n\n", path[i]);
    printf("TIME TO TRAVEL!\n");
    double totaldist = start_travel(g, dijk, loc, dest);
    
    if(totaldist >= 0) {
      printf("\nYOU MADE IT.\n");
      printf("(OPTIMAL DISTANCE: %.2lf)\n", dist);
      printf("GOODBYE.\n");
    }
    else
      printf("THAT'S OK.\nGOODBYE.\n");
  }
  else {
    printf("Your destination is unreachable.\nGOODBYE.\n");
    free(loc);
  }
  for(i = 0; i < npath; i++) {
    if(path[i] != NULL)
      free(path[i]);
  }
  free(path);
  free(dest);
  rpt_free(dijk);
  names_free(names, g_size(g));
  g_free(g);
  fclose(fp);
}
