#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H
enum color_t { BLACK, RED };

struct rb_node_t {
  struct rb_node_t *parent;
  struct rb_node_t *left;
  struct rb_node_t *right;
  void *value;
};

struct rb_tree {
  struct rb_node_t *root;

  int (*cmp)(void *n1, void *n2);
  int (*find)(void *val);

  void (*delete)(void *val);
  void (*insert)(void *val);
};

struct rb_tree *create_tree();


#endif
