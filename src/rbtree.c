#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rbtree.h"

#define RED_NODE(node, relative) ((node->colour == RED) && (relative->colour == RED))
#define ROTATE_LEFT1(N, P, GP) do {\
  N->parent = GP;\
  P->parent = N;\
  P->right = N->left;\
  if (N->left != NULL) {\
    (N->left)->parent = P;\
  }\
  N->left = P;\
  GP->left = N;\
} while (0)

#define ROTATE_LEFT2(N, P, GP) do {\
  N->parent = GP;\
  P->parent = N;\
  P->left = N->right;\
  if (N->right != NULL) {\
    (N->right)->parent = P;\
  }\
  N->right = P;\
  GP->right = N;\
} while (0)

// TODO: rotate right both cases.
#define ROTATE_RIGHT1(N, P, GP) do {\
  P->colour = BLACK;\
  GP->colour = RED;\
  GP->left = P->right;\
  if (P->right != NULL) {\
    (P->right)->parent = GP;\
  }\
  P->right = GP;\
} while (0)

#define ROTATE_RIGHT2(N, P, GP) do {\
  P->colour = BLACK;\
  GP->colour = RED;\
  GP->right = P->left;\
  if (P->left != NULL) {\
    (P->left)->parent = GP;\
  }\
  P->left = GP;\
} while (0)


//--------------------------- helper function definitions. ---------------------------------
static struct rb_node_t *get_lm_child(struct rb_node_t *node);
static struct rb_node_t *get_sibling(struct rb_node_t *node);
static _bool is_left_child(struct rb_node_t *pt, struct rb_node_t *chd);
static int icmp(void *v1, void *v2);
static void print(struct rb_tree *tree);
static void rb_insert_at_subtree(struct rb_tree *tree, struct rb_node_t **root, void *val, _bool IS_NODE);

//------------------------------------------------------------------------------------------

static void rebalance_tree(struct rb_tree *tree, struct rb_node_t *node, struct rb_node_t *parent);

//--------------------------- tree rotation functions. -------------------------------------
static _bool rotate_left(struct rb_tree *tree, struct rb_node_t *node, struct rb_node_t *parent);
static _bool rotate_right(struct rb_tree *tree, struct rb_node_t *node, struct rb_node_t *parent);
//------------------------------------------------------------------------------------------

//--------------------------- tree mutation cases. -----------------------------------------
static void c1_rebalance(struct rb_tree *tree, struct rb_node_t *node);
static void c2_rebalance(struct rb_tree *tree, struct rb_node_t *node);
static void c3_rebalance(struct rb_tree *tree, struct rb_node_t *node);
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
  node->colour = (tree->root == NULL) ? BLACK:RED;
  tree->valcpy(&(node->value), val);
  return node;
}

static void rebalance_tree(struct rb_tree *tree, struct rb_node_t *node, struct rb_node_t *parent)
{
  struct rb_node_t *GP, *U, **N = &node, *P;
  
  //LOOP where N is always a red node.
  while (((*N) != NULL)) {
    printf("in loop...\n");
    if ((*N) == tree->root && (*N)->colour == RED) {
      printf("red root.\n");
      (*N)->colour = BLACK;
      return;
    }

    if ((P = (*N)->parent) == NULL) {
      return;
    }
    
    if (P->colour == BLACK) {
      return;
    }
    else if (P == tree->root) {
      printf("parent root.\n");
      P->colour = BLACK;
      return;
    }
    GP = P->parent;
    if (GP == NULL) {
      printf("no gran.\n");
      return;
    }

    U = get_sibling(P);
    if (U != NULL && RED_NODE(P, U)) {
      printf("no unc.\n");
      P->colour = BLACK;
      U->colour = BLACK;
      GP->colour = RED;
      N = &GP;
      printf("case 1.\n");

    }
    else {
      int v1, v2, v3;
      v1 = tree->cmp(*N, P);
      v2 = tree->cmp(*N, GP);
      v3 = tree->cmp(P, GP);

      printf("case 2\n");
      if (v1 > 0 && v2 < 0) {
        printf("left rotation 1.\n");
        ROTATE_LEFT1((*N), P, GP);
        continue;
      } else if (v1 < 0 && v2 > 0) {
        printf("left rotation 2.\n");
        ROTATE_LEFT2((*N), P, GP);
        continue;
      } else if (v1 < 0 && v3 < 0) {
        printf("right rotation 1:\n");
        ROTATE_RIGHT1(*N, P, GP);
        goto rr_gp;

      } else if ( v1 > 0 && v3 > 0) {
        printf("right rotation 2:\n");
        ROTATE_RIGHT2(*N, P, GP);
        goto rr_gp;
      }
rr_gp:
      if (GP->parent != NULL) {
        P->parent = GP->parent;
        if ((GP->parent)->left == GP)
          (GP->parent)->left = P;
        else 
          (GP->parent)->right = P;
        
        GP->parent = P;
      }
      else {
        printf("right rotation >> new root\n\n");
        GP->parent = P;
        tree->root = P;
        P->parent = NULL;
      }
      break;
    }
  }  
  printf("tree-rebalanced.\n");
}


void rb_insert(struct rb_tree *tree, void *val)
{
  rb_insert_at_subtree(tree, &(tree->root), val, 0);  
}


void rb_insert_at_subtree(struct rb_tree *tree, struct rb_node_t **root, void *val, _bool IS_NODE)
{
  struct rb_node_t **cr = root;
  struct rb_node_t *pt = NULL;
  int vl;
  void *v = IS_NODE ? ((struct rb_node_t *)(val))->value : val;

  printf("inserting %d\n",*((int *)val));
  while (*cr != NULL) 
  {
    vl = tree->cmp(v, (*cr)->value);
    printf(" (%d:%d) --> ", (*cr)->colour ,*((int *)(*cr)->value));
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
  rebalance_tree(tree, *cr, pt);
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


static struct rb_node_t *get_sibling(struct rb_node_t *node)
{
  struct rb_node_t *pt = node->parent;

  if (pt == NULL)
    return NULL;

  if (pt->left == node)
    return pt->right;
  else
    return pt->left;
}

/* retreive leftmost child. */
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

