#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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


//--------------------------- helper function definitions. ---------------------------------
static struct rb_node_t *get_lm_child(struct rb_node_t *node);
static struct rb_node_t *get_sibling(struct rb_node_t *node);
static _bool is_left_child(struct rb_node_t *pt, struct rb_node_t *chd);
static void rb_insert_at_subtree(struct rb_tree *tree, struct rb_node_t **root, void *val);
static void rb_inorder_print(struct rb_node_t *node, void (*rb_print_node)(void *val));
//------------------------------------------------------------------------------------------

static void rebalance_tree(struct rb_tree *tree, struct rb_node_t *node, struct rb_node_t *parent);

//--------------------------- tree rotation functions. -------------------------------------
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
  if (tree->root == NULL) 
    tree->root = node;
  return node;
}

static void rebalance_tree(struct rb_tree *tree, struct rb_node_t *node, struct rb_node_t *parent)
{
  struct rb_node_t *GP, *U, *N = node, *P = parent;
  
  //LOOP where N is always a red node.
  while (((N) != NULL)) {
    printf("in loop...\n");
    if ((N) == tree->root && (N)->colour == RED) {
      printf("red root.\n");
      (N)->colour = BLACK;
      break;
    }

    if ((P = (N)->parent) == NULL) {
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


void rb_insert(struct rb_tree *tree, void *val)
{
  rb_insert_at_subtree(tree, &(tree->root), val);  
}


void rb_insert_at_subtree(struct rb_tree *tree, struct rb_node_t **root, void *val)
{
  struct rb_node_t **cr = root;
  struct rb_node_t *pt = NULL;
  int vl;
  //void *v = IS_NODE ? ((struct rb_node_t *)(val))->value : val;

  printf("INSERTING %d\n",*((int *)val));
  while (*cr != NULL) 
  {
    vl = tree->cmp(val, (*cr)->value);
    pt = *cr; 
    printf("(%d:%d)-->", pt->colour, *((int *)(pt)->value));
    if (vl < 0)
      cr = &((*cr)->left);
    else if (vl > 0)
      cr = &((*cr)->right);
    else
      return;
  }
  printf("(%d:%d)\n",1, *((int *)val));
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


/* integer comparision 
static int icmp(void *v1, void *v2)
{
  int i1 = *((int*)(v1)), i2 = *((int *)(v2));
  if (i1 < i2)
    return -1;
  return i1 > i2;
}
*/
