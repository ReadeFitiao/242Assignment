#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "mylib.h"

#define IS_BLACK(x) ((NULL == (x)) || (BLACK == (x)->colour))
#define IS_RED(x) ((NULL != (x)) && (RED == (x)->colour))

static tree_t tree_type;

int tree_depth(tree t);
void tree_output_dot(tree t, FILE *out);
static tree left_rotate(tree t);
static tree right_rotate(tree t);
static tree tree_fix(tree t);

struct tree_node{
    char *key;
    tree left;
    tree right;
    rbt_colour colour;
    int frequency;
};

tree tree_new(tree_t type){
    tree_type = type;
    return NULL;
}

tree tree_insert(tree t, char *str){
    if(t==NULL){
        t = emalloc(sizeof *t);
        t->key = emalloc((strlen(str)+1)*sizeof str[0]);
        strcpy(t->key,str);
        t->left = tree_new(RBT);
        t->right = tree_new(RBT);
        t->frequency = 1;
        if(tree_type == RBT){
            t->colour = RED;
        }
    }else if(strcmp(t->key,str) == 0){
        t->frequency++;
    }else{
        if(strcmp(t->key,str) > 0){
            t->left = tree_insert(t->left,str);
        }else if(strcmp(t->key,str) < 0){
            t->right = tree_insert(t->right,str);
        }
    }
    if(tree_type == RBT){
        t = tree_fix(t);
    }
    return t;
}

void tree_preorder(tree t, void f(char *str)){
    if(t == NULL){
        return;
    }

    f(t->key);
    tree_preorder(t->left,f);
    tree_preorder(t->right,f);
}

void tree_inorder(tree t, void f(char *str)){
    if(t == NULL){
        return;
    }

    tree_inorder(t->left,f);
    f(t->key);
    tree_inorder(t->right,f);
}

int tree_search(tree t, char *str){
    if(t == NULL){
        return 0;
    }else if(strcmp(t->key,str) == 0){
        return 1;
    }else if(strcmp(t->key,str) > 0){
        return tree_search(t->left,str);
    }else{
        return tree_search(t->right,str);
    }
}

void tree_free(tree t){
    if(t->left != NULL){
        tree_free(t->left);
    }
    if(t->right != NULL){
        tree_free(t->right);
    }
    free(t->key);
    free(t);
}


static tree left_rotate(tree t){
    tree temp = t;
    t = temp->right;
    temp->right = t->left;
    t->left = temp;
    return t;
}

static tree right_rotate(tree t){
    tree temp = t;
    t = temp->left;
    temp->left = t->right;
    t->right = temp;
    return t;
}

static tree tree_fix(tree t){
    if(IS_RED(t->left) && IS_RED(t->left->left)){
        if(IS_RED(t->right)){
            /*Case 1*/
            t->colour = RED;
            t->right->colour = BLACK;
            t->left->colour = BLACK;
        } else {
            /*Case 2*/
            t = right_rotate(t);
            t->colour = BLACK;
            t->right->colour = RED;
        }
    }
    if(IS_RED(t->left) && IS_RED(t->left->right)){
        if(IS_RED(t->right)){
            /*Case 3*/
            t->colour = RED;
            t->right->colour = BLACK;
            t->left->colour = BLACK;
        } else {
            /*Case 4*/
            t->left = left_rotate(t->left);
            t = right_rotate(t);
            t->colour = BLACK;
            t->right->colour= RED;
        }
    }
    if(IS_RED(t->right) && IS_RED(t->right->left)){
        if(IS_RED(t->left)){
            /*Case 5*/
            t->colour = RED;
            t->right->colour = BLACK;
            t->left->colour = BLACK;
        } else {
            /*Case 6*/
            t->right = right_rotate(t->right);
            t = left_rotate(t);
            t->colour = BLACK;
            t->left->colour = RED;
        }
    }
    if(IS_RED(t->right) && IS_RED(t->right->right)){
        if(IS_RED(t->left)){
            /*Case 7*/
            t->colour = RED;
            t->right->colour = BLACK;
            t->left->colour = BLACK;
        } else {
            /*Case 8*/
            t = left_rotate(t);
            t->colour = BLACK;
            t->left->colour = RED;
        }
    }

    return t;
}

int tree_depth(tree t){
    if(t->left == NULL && t->right == NULL){
        return 0;
    }else{
        int leftHeight = (t->left == NULL ? 0 : tree_depth(t->left));
        int rightHeight = (t->right == NULL ? 0 : tree_depth(t->right));
        if(leftHeight>rightHeight){
            return leftHeight + 1;
        }
        else{
            return rightHeight + 1;
        }
    }
}

/**
 * Traverses the tree writing a DOT description about connections, and
 * possibly colours, to the given output stream.
 *
 * @param t the tree to output a DOT description of.
 * @param out the stream to write the DOT output to.
 */
static void tree_output_dot_aux(tree t, FILE *out) {
    if(t->key != NULL) {
        fprintf(out, "\"%s\"[label=\"{<f0>%s:%d|{<f1>|<f2>}}\"color=%s];\n",
                t->key, t->key, t->frequency,
                (RBT == tree_type && RED == t->colour) ? "red":"black");
    }
    if(t->left != NULL) {
        tree_output_dot_aux(t->left, out);
        fprintf(out, "\"%s\":f1 -> \"%s\":f0;\n", t->key, t->left->key);
    }
    if(t->right != NULL) {
        tree_output_dot_aux(t->right, out);
        fprintf(out, "\"%s\":f2 -> \"%s\":f0;\n", t->key, t->right->key);
    }
}

/**
 * Output a DOT description of this tree to the given output stream.
 * DOT is a plain text graph description language (see www.graphviz.org).
 * You can create a viewable graph with the command
 *
 *    dot -Tpdf < graphfile.dot > graphfile.pdf
 *
 * You can also use png, ps, jpg, svg... instead of pdf
 *
 * @param t the tree to output the DOT description of.
 * @param out the stream to write the DOT description to.
 */
void tree_output_dot(tree t, FILE *out) {
    fprintf(out, "digraph tree {\nnode [shape = Mrecord, penwidth = 2];\n");
    tree_output_dot_aux(t, out);
    fprintf(out, "}\n");
}

