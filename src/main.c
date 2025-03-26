#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rbtree.h"

/**
 * A simple random number generator implementation in C
 * Provides functions for different types of random numbers
 */

// Initialize the random seed
void init_random() {
    srand((unsigned int)time(NULL));
}

// Generate a random integer between min and max (inclusive)
int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Generate a random float between min and max
float random_float(float min, float max) {
    float scale = rand() / (float) RAND_MAX; /* 0.0 to 1.0 */
    return min + scale * (max - min);
}

// Generate a random double between min and max
double random_double(double min, double max) {
    double scale = rand() / (double) RAND_MAX; /* 0.0 to 1.0 */
    return min + scale * (max - min);
}

int integer_cmp(void *v1, void *v2)
{
  int i1 = *((int *)v1), i2 = *((int *)v2);
  if (i1 < i2)
    return -1;
  return i1 > i2;
}
void integer_cpy(void **v1, void *v2)
{
  *v1 = (void *)malloc(sizeof(int));
  **((int **)v1) = *((int *)v2);
}
void integer_print(void *v1)
{
  printf("%d -> ", *((int *)v1));
}


int main(void) {
  init_random();
  struct rb_tree *tree = rb_create_tree(integer_cmp, integer_cpy, NULL);
  int nums[10] = {90, 844, 1000, 884, 997, 310, 341, 183, 333, 694};
  int data[7] = {0, 1, 2, 3, 4, 5, 6};
  for (int i =0; i < 7; i++) {
    int ri = random_int(0, 1000); 
    rb_insert(tree, &data[i]);
  }
  if (tree->root == NULL)
    printf("insertion: failure!");
  else
    printf("root: %d",*((int *)tree->root->value));
  printf("\n");
  rb_print_tree(tree, integer_print);
}

