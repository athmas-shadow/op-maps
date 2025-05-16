#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "rbtree.h"

#define RED_NODE(node, relative) ((node->colour == RED) && (relative->colour == RED))
#define ROTATE_LEFT1(N, P, GP) do {\
  if (N->left != NULL) {\
    (N->left)->parent = P;\
  }\
  P->right = N->left;\
  N->left = P;\
  GP->left = N;\
  N->parent = GP;\
  P->parent = N;\
} while (0)

#define ROTATE_LEFT2(N, P, GP) do {\
  N->parent = GP;\
  P->parent = N;\
  if (N->right != NULL) {\
    (N->right)->parent = P;\
  }\
  P->left = N->right;\
  N->right = P;\
  GP->right = N;\
} while (0)

// TODO: rotate right both cases.
#define ROTATE_RIGHT1(N, P, GP) do {\
  P->colour = BLACK;\
  GP->colour = RED;\
  if (P->right != NULL) {\
    (P->right)->parent = GP;\
  }\
  GP->left = P->right;\
  P->right = GP;\
} while (0)

#define ROTATE_RIGHT2(N, P, GP) do {\
  P->colour = BLACK;\
  GP->colour = RED;\
  if (P->left != NULL) {\
    (P->left)->parent = GP;\
  }\
  GP->right = P->left;\
  P->left = GP;\
} while (0)

struct node_loc {
  struct rb_node_t **addrs;
  struct rb_node_t *prnt;
};

//--------------------------- helper function definitions. ---------------------------------
static struct rb_node_t *get_lm_child(struct rb_node_t *node);
static struct rb_node_t *get_sibling(struct rb_node_t *node);
static bool is_left_child(struct rb_node_t *pt, struct rb_node_t *chd);
static void rb_insert_at_subtree(struct rb_tree *tree, 
    struct rb_node_t **root, void *val);
static void rb_inorder_print(struct rb_node_t *node, 
    void (*rb_print_node)(void *val));
static bool rb_delete_node(struct rb_tree *tree, 
    struct rb_node_t *parent, 
    struct rb_node_t **current);
static struct node_loc get_node_loc(struct rb_tree *tree, 
    struct rb_node_t **root, 
    void *val);
static bool ch_are_blck(struct rb_node_t *node);


//-------------------------- rebalancing functions. ----------------------------------------
static void rebalance_insert(struct rb_tree *tree, 
    struct rb_node_t *node, 
    struct rb_node_t *parent);

static void rebalance_delete(struct rb_tree *tree,
    struct rb_node_t *node,
    struct rb_node_t *parent);


/**
 * allocates memory for node.
 * */
static struct rb_node_t *rb_create_node(struct rb_tree *tree, 
    struct rb_node_t *parent, void *val)
{
  struct rb_node_t *node = (struct rb_node_t *)malloc(sizeof(struct rb_node_t));
  node->parent = parent;
  node->left = NULL;
  node->right = NULL;
  node->colour = (tree->root == NULL) ? BLACK:RED;
  tree->valcpy(&(node->value), val);
  if (tree->root == NULL) 
    tree->root = node;
  return node;
}

static bool rb_delete_node(struct rb_tree *tree, 
    struct rb_node_t *parent,
    struct rb_node_t **current)
{
  struct rb_node_t *node = *current;
  bool is_root = tree->root == node;
  if (node == NULL)
    return false;
  if (node->left == NULL && node->right == NULL) {
    bool is_black = node->colour == BLACK;
    free(node);
    node = NULL;
    return is_black;
  } else if (node->left == NULL) {
    *current = node->right;
    (*current)->parent = node->parent;
    (*current)->colour = BLACK;
    free(node);
  } else if (node->right == NULL) {
    *current = node->left;
    (*current)->parent = node->parent;
    (*current)->colour = BLACK;
    free(node);
  } else {
    node = get_lm_child(node->right);
    (*current)->value = node->value;
    free(node);
  }
  if (is_root) {
    tree->root = *current;
  }
  return false;
}

static void rebalance_delete(struct rb_tree *tree, 
    struct rb_node_t *node, 
    struct rb_node_t *parent)
{
  struct rb_node_t *N = node, *S;
  while (N != NULL) {
    if (N == tree->root) {
      break;
    }
    S = get_sibling(N);
    if (S == NULL)
      return;
    
    if (S->colour == BLACK) {
      if (ch_are_blck(S)) {
        S->colour = RED;
        N = N->parent;
      }
    }

    
  }
}

static void rebalance_insert(struct rb_tree *tree, 
    struct rb_node_t *node, 
    struct rb_node_t *parent)
{
  struct rb_node_t *GP, *U, *N = node, *P = parent;
  //LOOP where N is always a red node.
  while (N != NULL) {
    printf("in loop...\n");
    if (N == tree->root && N->colour == RED) {
      printf("red root.\n");
      N->colour = BLACK;
      break;
    }
    if ((P = N->parent) == NULL) {
      break;
    }
    if (P->colour == BLACK) {
      break;
    }
    else if (P == tree->root) {
      printf("parent root.\n");
      P->colour = BLACK;
      break;
    }
    GP = P->parent;
    if (GP == NULL) {
      printf("no gran.\n");
      break;
    }
    U = get_sibling(P);
    if (U != NULL && RED_NODE(P, U)) {
      P->colour = BLACK;
      U->colour = BLACK;
      GP->colour = RED;
      N = GP;
      printf("recolouring.\n");
    }
    else {
      int v1 =0, v2=0, v3=0;
      int val1 = *((int *)(N)->value), val2 = *((int *)(P)->value),val3 = *((int *)(GP)->value); 
      v1 = tree->cmp(N->value, P->value);
      v2 = tree->cmp(N->value, GP->value);
      v3 = tree->cmp(P->value, GP->value);

      printf("case 2 (%d > %d)=%d; (%d > %d)=%d; (%d > %d)=%d;\n",
          val1, val2, v1, val1, val3, v2, val2, val3, v3);
      if (v1 > 0 && v2 < 0) {
        printf("left rotation 1.\n");
        ROTATE_LEFT1((N), P, GP);
        continue;
      } else if (v1 < 0 && v2 > 0) {
        printf("left rotation 2.\n");
        ROTATE_LEFT2((N), P, GP);
        continue;
      } else if (v1 < 0 && v3 < 0) {
        printf("right rotation 1:\n");
        ROTATE_RIGHT1(N, P, GP);
        goto rr_gp;

      } else if ( v1 > 0 && v3 > 0) {
        printf("right rotation 2:\n");
        ROTATE_RIGHT2(N, P, GP);
        goto rr_gp;
      }
rr_gp:
      if (GP->parent != NULL) {
        printf("right rotation COMPLETE >> Parent found\n");
        P->parent = GP->parent;
        if ((GP->parent)->left == GP)
          (GP->parent)->left = P;
        else 
          (GP->parent)->right = P;
        GP->parent = P;
      }
      else {
        printf("right rotation COMPLETE >> new root\n");
        GP->parent = P;
        tree->root = P;
        P->parent = NULL;
      }
      break;
    }
  }  
  printf("tree-rebalanced.\n\n");
}



struct node_loc get_node_loc(struct rb_tree *tree, struct rb_node_t **root, void *val)
{
  struct rb_node_t **cr = root;
  struct rb_node_t *pt  = NULL;
  struct node_loc oploc = { .addrs=NULL, .prnt=NULL };
  int v1;

  printf("INSERTING %d\n",*((int *)val));
  while (*cr != NULL) {
    v1 = tree->cmp(val, (*cr)->value);
    pt = *cr;
    printf("(%d:%d)-->", pt->colour, *((int *)(pt)->value));
    if (v1 < 0)
      cr = &((*cr)->left);
    else if (v1 > 0)
      cr = &((*cr)->right);
    else 
      return oploc;
  }
  printf("(%d:%d)\n",1, *((int *)val));
  oploc.addrs = cr;
  oploc.prnt = pt;
  return oploc;
}

void rb_delete(struct rb_tree *tree, void *val)
{

}

void rb_delete_at_subtree(struct rb_tree *tree, struct rb_node_t **root, void *val)
{
  struct rb_node_t **cr, *pt;
  struct node_loc op_loc = get_node_loc(tree, root, val);
  bool n_rb;
  cr = op_loc.addrs;
  pt = op_loc.prnt;
  if (cr == NULL) {
    return;
  }
  n_rb = rb_delete_node(tree, pt, cr);
  if (n_rb) {
    rebalance_delete(tree, pt, pt->parent);
  }
}

void rb_insert(struct rb_tree *tree, void *val)
{
  rb_insert_at_subtree(tree, &(tree->root), val);  
}

void rb_insert_at_subtree(struct rb_tree *tree, struct rb_node_t **root, void *val)
{
  struct rb_node_t **cr, *pt;
  struct node_loc op_loc = get_node_loc(tree, root, val);
  cr = op_loc.addrs;
  pt = op_loc.prnt;
  int vl;
  if (cr == NULL) {
    return;
  }
  *cr = rb_create_node(tree, pt, val);
  rebalance_insert(tree, *cr, pt);
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
  _bool lft;
  if (cr == NULL)
    return;
  if (cr->parent == NULL){
    tree->current =get_lm_child(cr->right);
    return;
  }
  lft = is_left_child(cr->parent, cr);
  if (cr->right == NULL) {
    if (lft)
      tree->current = (cr)->parent; 
    else if (cr->right != NULL)
      tree->current = get_lm_child(cr->right);
    else 
      tree->current = NULL;
  }
  else {
    tree->current = get_lm_child(cr->right);
  }
}

struct rb_tree *rb_create_tree(
    int (*cmp)(const void *n1,const void *n2), 
    void (*valcpy)(void **v1, void *v2),
    int (*find)(void *val))
{
  struct rb_tree *tree = (struct rb_tree *)malloc(sizeof(struct rb_tree));
  tree->root = NULL;
  tree->cmp = cmp;
  tree->find = find;
  tree->valcpy = valcpy;
  return tree;
}


void rb_inorder_print(struct rb_node_t *node, void (*rb_print_node)(void *val))
{
  if (node == NULL)
    return;
  rb_inorder_print(node->left, rb_print_node);
  rb_print_node(node->value);
  rb_inorder_print(node->right, rb_print_node);
}

void rb_print_tree(struct rb_tree *tree, void (*rb_print_node)(void *val))
{

  rb_inorder_print(tree->root, rb_print_node);
  printf("END\n");
}

void rb_print_node_int(struct rb_node_t *node)
{
  printf("%d. ",*((int*)(node->value)));
}

/*------------------------------------ helper functions. -------------------------*/

static bool ch_are_blck(struct rb_node_t *node)
{
  if (node == NULL) {
    return false;
  }
  if (node->left == NULL && node->right == NULL) {
    return false;
  }
  else if (node->left == NULL)  {
    struct rb_node_t *r = node->right;
    return r->colour == BLACK;
  }
  else if (node->right == NULL) {
    struct rb_node_t *l = node->left;
    return l->colour == BLACK;
  }
  else {
    struct rb_node_t *l, *r;
    l = node->left;
    r = node->right;
    bool lc = l->colour == BLACK;
    bool rc = r->colour == BLACK;
    return lc && rc;
  }
}

static bool is_left_child(struct rb_node_t * pt, struct rb_node_t *chd)
{
  struct rb_node_t *lft = pt->left;
  return lft == chd;
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
