#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H
enum colour_t { BLACK, RED };
typedef enum bool_t { FALSE, TRUE } _bool;

struct rb_node_t {
  struct rb_node_t *parent;
  struct rb_node_t *left;
  struct rb_node_t *right;
  enum colour_t colour;
  void *value;
};

struct rb_tree {
  struct rb_node_t *root;

  //current node in iteration.
  struct rb_node_t *current;
  
  int (*cmp)(void *n1, void *n2);
  int (*find)(void *val);
  
};

struct rb_tree *rb_create_tree();
void rb_init_iterator(struct rb_tree *tree);
_bool rb_has_next(struct rb_tree *tree);
void rb_next(struct rb_tree *tree);


#endif
