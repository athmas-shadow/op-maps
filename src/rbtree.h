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
  
  int (*cmp)(const void *n1, const void *n2);
  void (*valcpy)(void **v1, void *v2);
  int (*find)(void *val);

  
};

extern struct rb_tree *rb_create_tree(
    int (*cmp)(const void *n1,const void *n2),
    void (*valcpy)(void **v1, void *v2),
    int (*find)(void *val)
    );
void rb_init_iterator(struct rb_tree *tree);
_bool rb_has_next(struct rb_tree *tree);
void rb_next(struct rb_tree *tree);
extern void rb_insert(struct rb_tree *tree, void *val);
extern void rb_print_tree(struct rb_tree *tree, void (*rb_print_node)(void *val));
#endif
