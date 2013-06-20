#include "queue.h"
#include <stdlib.h>
#include <stdio.h>


int compare (void *objecta, void *objectb) {
  int *a = (int *) objecta;
  int *b = (int *) objectb;
  if (*a == *b)
    return 1;
  else
    return 0;
}

int main(int argc, char *argv[]) {
  void *l = queue_init();
  //set_print(l);
  int *a = malloc(sizeof(int)), *b = malloc(sizeof(int)), *c = malloc(sizeof(int)), *d = malloc(sizeof(int));
  *a = 2;
  *b = 6;
  *c = 5;
  *d = 5;
  printf("adding 2\n");
  queue_add(l, a);
  queue_print(l);
  
  printf("adding 6\n");
  queue_add(l, b);
  queue_print(l);

  printf("removing\n");
  int *removed = queue_remove(l);
  queue_print(l);
  printf("removed %d\n", *removed);

  printf("add to queue for 6 2\n");
  queue_add(l,a);
  queue_print(l);

  printf("removing twice\n");
  queue_remove(l);
  queue_remove(l);
  queue_print(l);

  printf("adding 2\n");
  queue_add(l, a);
  queue_print(l);

  printf("removing\n");  
  queue_remove(l);
  printf("removing again(should be illegal)\n");
  queue_remove(l);
  queue_print(l);

  printf("adding 2\n");
  queue_add(l, a);
  queue_print(l);
 
  printf("adding 6\n");
  queue_add(l, b);
  queue_print(l); 

  printf("adding 2\n");
  queue_add(l, a);
  queue_print(l);
 
  printf("adding 5\n");
  queue_add(l, c);
  queue_print(l);

  printf("removing twice\n");
  queue_remove(l);
  queue_remove(l);
  queue_print(l);

  printf("adding 6\n");
  queue_add(l, b);
  queue_print(l); 

  printf("removing twice\n");
  queue_remove(l);
  queue_remove(l);
  queue_print(l);

  printf("removing\n");  
  queue_remove(l);
  queue_print(l);
  
  printf("removing again(should be illegal)\n");
  queue_remove(l);
  queue_print(l);

  printf("adding 2\n");
  queue_add(l, a);
  queue_print(l);
 
  printf("adding 6\n");
  queue_add(l, b);
  queue_print(l); 

  printf("adding 2\n");
  queue_add(l, a);
  queue_print(l);
 
  printf("adding 5\n");
  queue_add(l, c);
  queue_print(l);

  printf("adding 5b\n");
  queue_add(l, d);
  queue_print(l);

  printf("remove all 6\n");
  queue_remove_all_obj(l, b, compare);
  queue_print(l);

  printf("remove all 2\n");
  queue_remove_all_obj(l, a, compare);
  queue_print(l);

  printf("remove all 5\n");
  queue_remove_all_obj(l, d, compare);
  queue_print(l);

  return 0;
}
