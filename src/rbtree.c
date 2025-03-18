#include <stdlib.h>
#include <stdio.h>
#include "rbtree.h"


//--------------------------- helper function definitions. ---------------------------------
static struct rb_node_t *get_lm_child(struct rb_node_t *node);
static _bool is_left_child(struct rb_node_t *pt, struct rb_node_t *chd);
static int icmp(void *v1, void *v2);
static void print(struct rb_tree *tree);

//------------------------------------------------------------------------------------------
/**
 * allocates memory for node.
 * */
static struct rb_node_t *rb_create_node(struct rb_tree *tree, struct rb_node_t *parent, void *val)
{
  struct rb_node_t *node = (struct rb_node_t *)malloc(sizeof(struct rb_node_t));
  node->parent = parent;
  node->left = NULL;
  node->right = NULL;
  node->colour = RED;
  //node->value = (void *)malloc(sizeof(int));
  //*((int *)node->value) = *((int *)val);
  tree->valcpy(&(node->value), val);
  return node;
}

void rb_insert(struct rb_tree *tree, void *val)
{
  struct rb_node_t **cr = &(tree->root);  
  struct rb_node_t *pt = NULL, *n;
  int vl;
  printf("inserting %d\n",*((int *)val));
  while (*cr != NULL) 
  {
    vl = icmp(val, (*cr)->value);
    printf("at %d --> ",*((int *)(*cr)->value));
    pt = *cr; 
    if (vl < 0)
      cr = &((*cr)->left);
    else if (vl > 0)
      cr = &((*cr)->right);
    else
      return;
  }
  printf("done...\n");
  *cr = rb_create_node(tree, pt, val);
}


static void delete(struct rb_tree *tree, void *val)
{

}

void rb_init_iterator(struct rb_tree *tree)
{
  tree->current = get_lm_child(tree->root);
}

_bool has_next(struct rb_tree *tree)
{
  struct rb_node_t *cr = tree->current;
  _bool lft;
  if (cr == NULL)
    return FALSE;
  if ((cr)->parent == NULL)
    return (cr)->right != NULL;
  lft = is_left_child((cr)->parent, cr);
  if (lft)
    return TRUE;
  else
    return (cr->left != NULL) || (cr->right != NULL); 
}

/*
 *increments current iterator pointer.
 * */ 
void next(struct rb_tree *tree)
{
  struct rb_node_t *cr = (tree->current);
  void *val = (cr)->value;
  _bool lft;
  if (cr == NULL)
    return;
  if (cr->parent == NULL){
    tree->current =get_lm_child(cr->right);
    return;
  }
  lft = is_left_child(cr->parent, cr);
  if (lft)
    tree->current = (cr)->parent; 
  else if (cr->right != NULL)
    tree->current = get_lm_child(cr->right);
  else 
    tree->current = NULL;
}

struct rb_tree *rb_create_tree(
    int (*cmp)(void *n1, void *n2), 
    void (*valcpy)(void **v1, void *v2),
    int (*find)(void *val))
{
  struct rb_tree *tree = malloc(sizeof(struct rb_tree));
  tree->root = NULL;
  tree->cmp = cmp;
  tree->find = find;
  tree->valcpy = valcpy;
  return tree;
}

void rb_print_tree(struct rb_tree *tree, void (*rb_print_node)(void *val))
{
  rb_init_iterator(tree);
  if (tree->current == NULL)
    printf("current not present: failure!");
  else
    printf("smallest: %d\n",*((int *)(tree->current)->value));
  while (has_next(tree)) {
    void *v = (tree->current)->value;
    rb_print_node(v);
    next(tree);
  }
  printf("END\n");
}

void rb_print_node_int(struct rb_node_t *node)
{
  printf("%d. ",*((int*)(node->value)));
}

/*------------------------------------ helper functions. -------------------------*/
static _bool is_left_child(struct rb_node_t * pt, struct rb_node_t *chd)
{
  return pt->left == chd;
}
/* retreive leftmost comparision. */
static struct rb_node_t *get_lm_child(struct rb_node_t *node)
{
  struct rb_node_t **cr = &node;
  struct rb_node_t *pt = *cr;
  while (*cr != NULL) {
    pt = *cr;
    cr = &((*cr)->left);
  }
  return pt;
}


/* integer comparision */
static int icmp(void *v1, void *v2)
{
  int i1 = *((int*)(v1)), i2 = *((int *)(v2));
  if (i1 < i2)
    return -1;
  return i1 > i2;
}

