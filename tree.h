#ifndef TREE_H_
#define TREE_H_

typedef struct tree_node *tree;
typedef enum { RED, BLACK } rbt_colour;
typedef enum tree_e { BST, RBT } tree_t;

extern void  tree_free(tree t);
extern void  tree_inorder(tree t, void f(char *str));
extern tree  tree_insert(tree t, char *str);
extern tree  tree_new(tree_t type);
extern void  tree_preorder(tree t, void f(char *str));
extern int   tree_search(tree t, char *str);

#endif
