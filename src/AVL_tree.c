
#include "../AVL_tree.h"

int height(TreeNode *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// Function to get maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Function to create a new tree node
TreeNode* create_node(const char *value) {
    TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
    new_node->value = strdup(value);
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->height = 1;  // New node is initially added at leaf
    return new_node;
}

// Right rotate subtree rooted with y
TreeNode* right_rotate(TreeNode *y) {
    TreeNode *x = y->left;
    TreeNode *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

// Left rotate subtree rooted with x
TreeNode* left_rotate(TreeNode *x) {
    TreeNode *y = x->right;
    TreeNode *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

// Get balance factor of node
int get_balance(TreeNode *node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

// Function to insert a value into the AVL tree
TreeNode* insert(TreeNode* node, const char *value) {
    // 1. Perform the normal BST insert
    if (node == NULL)
        return create_node(value);

    if (strcmp(value, node->value) < 0)
        node->left = insert(node->left, value);
    else if (strcmp(value, node->value) > 0)
        node->right = insert(node->right, value);
    else  // Equal values are not allowed in BST
        return node;

    // 2. Update height of this ancestor node
    node->height = 1 + max(height(node->left), height(node->right));

    // 3. Get the balance factor of this ancestor node to check whether
    // this node became unbalanced
    int balance = get_balance(node);

    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && strcmp(value, node->left->value) < 0)
        return right_rotate(node);

    // Right Right Case
    if (balance < -1 && strcmp(value, node->right->value) > 0)
        return left_rotate(node);

    // Left Right Case
    if (balance > 1 && strcmp(value, node->left->value) > 0) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    // Right Left Case
    if (balance < -1 && strcmp(value, node->right->value) < 0) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    // Return the (unchanged) node pointer
    return node;
}

// Function to search a value in the AVL tree
int search(TreeNode* root, const char *value) {
    if (root == NULL)
        return 0;
    if (strcmp(value, root->value) == 0)
        return 1;
    if (strcmp(value, root->value) < 0)
        return search(root->left, value);
    return search(root->right, value);
}

// Function to free the tree
void free_tree(TreeNode *root) {
    if (root != NULL) {
        free_tree(root->left);
        free(root->value);
        free_tree(root->right);
        free(root);
    }
}
