#include <stdlib.h>
#include <stdio.h>
#include "rbtree.h"


//--------------------------- helper function definitions. ---------------------------------
static struct rb_node_t *get_lm_child(struct rb_node_t *node);
static int cmp(void *v1, void *v2);
static void print(struct rb_tree *tree);

//------------------------------------------------------------------------------------------
/**
 * allocates memory for node.
 * */
static struct rb_node_t *create_node(struct rb_node_t *parent, void *val)
{
  struct rb_node_t *node = (struct rb_node_t *)malloc(sizeof(struct rb_node_t));
  node->parent = parent;
  node->left = NULL;
  node->right = NULL;
  node->colour = RED;
  node->value = val;

  return node;
}


static void insert(struct rb_tree *tree, void *val)
{
  struct rb_node_t **cr = &(tree->root);  
  struct rb_node_t **pt, *n;
  int vl;
  while (*cr != NULL) 
  {
    vl = !tree->cmp(val, (*cr)->value);
    *pt = *cr;
    if (vl < 0)
      cr = &((*cr)->left);
    else if (vl > 0)
      cr = &((*cr)->right);
    else  {
      printf("WARNING: Duplicate value.");
      return;
    }
  }
  *cr = create_node(*pt, val);
}


static void delete(struct rb_tree *tree, void *val)
{

}

void create_inorder_iterator(struct rb_tree *tree)
{
  tree->current = get_lm_child(tree->root);
}

_bool has_next(struct rb_tree *tree)
{
  struct rb_node_t **cr = &(tree->current);
  return (*cr != NULL) && (((*cr)->parent != NULL) || ((*cr)->right != NULL));
}

/*
 *increments current iterator pointer.
 * */ 
void next(struct rb_tree *tree)
{
  struct rb_node_t **cr = &(tree->current);
  void *val = (*cr)->value;
  if ((*cr)->parent != NULL)
    tree->current = (*cr)->parent; 
  else if ( (*cr)->right != NULL)
    tree->current = get_lm_child((*cr)->right);
}

struct rb_tree *rb_create_tree(
    int (*cmp)(void *n1, void *n2), 
    int (*find)(void *val),
    void (*print)())
{
  struct rb_tree *tree = malloc(sizeof(struct rb_tree));
  tree->root = NULL;
  tree->cmp = cmp;
  tree->find = find;
  
  return tree;
}

void rb_print(struct rb_tree *tree, void (*rb_print_node)(struct rb_node_t *node))
{
  while (has_next(tree)) {
    rb_print_node(tree->current);
    next(tree);
  }
}

void rb_print_node_int(struct rb_node_t *node)
{
  printf("%d. ",*((int*)(node->value)));
}

/*------------------------------------ helper functions. -------------------------*/
/* retreive leftmost comparision. */
static struct rb_node_t *get_lm_child(struct rb_node_t *node)
{
  struct rb_node_t **cr = &node;
  struct rb_node_t **pt;
  while (*cr != NULL) {
    *pt = *cr;
    cr = &((*cr)->left);
  }
  return *pt;
}


/* integer comparision */
static int cmp(void *v1, void *v2)
{
  int i1 = *((int*)(v1)), i2 = *((int *)(v2));
  if (i1 < i2)
    return -1;
  return i1 > i2;
}

int main(void) {
  printf("header file works.\n");
  return 0;
}
