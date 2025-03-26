#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rbtree.h"

#define RED_NODE(node, relative) ((node->colour == RED) && (relative->colour == RED))
#define ROTATE_LEFT1(N, P, GP) do {\
  N->parent = GP;\
  P->parent = N;\
  N->left = P;\
  GP->left = N;\
  P->right = NULL;\
} while (0)

#define ROTATE_LEFT2(N, P, GP) do {\
  N->parent = GP;\
  P->parent = N;\
  N->right = P;\
  GP->right = N;\
  P->left = NULL;\
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
  node->colour = (tree->root == NULL) ? BLACK : RED;
  tree->valcpy(&(node->value), val);
  return node;
}

static void rebalance_tree(struct rb_tree *tree, struct rb_node_t *node, struct rb_node_t *parent)
{
  struct rb_node_t *GP, *U, *N = node, *P;
  
  //LOOP where N is always a red node.
  while ((N != NULL) && ((P = N->parent) != NULL)) {
    printf("in loop...\n");
    if (N == tree->root && N->colour == RED) return;
    if (P->colour == BLACK) {
      return;
    }
    else if (P == tree->root) {
      P->colour = BLACK;
      return;
    }
    GP = P->parent;
    if (GP == NULL) return;

    U = get_sibling(P);
    if (U == NULL) return;

    if (RED_NODE(P, U)) {
      P->colour = BLACK;
      U->colour = BLACK;
      GP->colour = RED;
      N = GP;
      printf("case 1.\n");

    }
    else if (U->colour == BLACK) {
      int v1, v2;
      v1 = tree->cmp(N, P);
      v2 = tree->cmp(N, GP);

      if (v1 > 0 && v2 < 0) {
        printf("left rotation 1.\n");
        ROTATE_LEFT1(N, P, GP);
      }
      else if (v1 < 0 && v2 > 0) {
        printf("left rotation 2.\n");
        ROTATE_LEFT2(N, P, GP);
      }
      else {

      }
    }
    
  }  
}

static _bool rotate_left(struct rb_tree *tree, struct rb_node_t *node, struct rb_node_t *parent)
{
  struct rb_node_t *p = parent, *gp;
  int v1, v2;
  _bool x = 0;

  if (p == NULL) return 0;
  gp = p->parent;
  if (gp == NULL) return 0;

  /*valid conditions to warrant a left rotation 
   * 1.) parent < node < granparent
   * 2.) gran parent < node < parent
   * */
  v1 = tree->cmp(node, p);
  v2 = tree->cmp(node, gp); 

  if (!RED_NODE(p, node))
    return 0;

  node->parent = gp;
  p->parent    = node;
  if (v1 > 0 && v2 < 0) {
    node->left = p;
    p->right = NULL;
    gp->left = node;
    x = 1;

  } else if (v1 < 0 && v2 > 0) {
    node->right = p;
    p->left = NULL;
    gp->right = node;
    x = 1;
  }

  return 1;
}

static _bool rotate_right(struct rb_tree *tree, struct rb_node_t *node, struct rb_node_t *parent)
{
  struct rb_node_t *p = parent, *gp;
  int v1, v2;
  _bool x;

  if (p == NULL) return 0;
  gp = p->parent;
  if (gp == NULL) return 0;
  
  v1 = tree->cmp(p, gp);
  v2 = tree->cmp(p, node);

  if (!RED_NODE(p, node))
    return 0;
  /**
   * valid conditions to warrant a right rotation.
   * 1.) node(red) < parent(red) < gran parent(black) 
   * 2.) granparent(black) > parent(red) > node(red)
   * */ 
  gp->parent = p;
  gp->colour = RED;
  if (v1 < 0 && v2 > 0) {
    rb_insert_at_subtree(tree, &p, gp, 1); 
    x = 1;
  }
  else if (v1 > 0 && v2 < 0) {
    rb_insert_at_subtree(tree, &p, gp, 1);
    x = 1;
  }
  p->colour = BLACK;
  if (gp->parent == NULL)
    return x;
  rb_insert_at_subtree(tree, &(gp->parent), p, 1);
  return x;
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

  if (pt != NULL)
    return NULL;

  if (is_left_child(pt, node))
    return pt->left;
  else
    return pt->right;
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

