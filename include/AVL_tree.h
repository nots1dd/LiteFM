#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct TreeNode {
    char *value;
    struct TreeNode *left;
    struct TreeNode *right;
    int height;
} TreeNode;

TreeNode* insert(TreeNode* node, const char *value);
int search(TreeNode* root, const char *value);
void print_tree(TreeNode *root, const char *type);
void free_tree(TreeNode *root);
int max(int a, int b);
int height(TreeNode *node);
TreeNode* create_node(const char *value);
TreeNode* right_rotate(TreeNode *y);
TreeNode* left_rotate(TreeNode *x);
int get_balance(TreeNode *node);

#endif
