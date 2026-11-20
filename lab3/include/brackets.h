#ifndef BRACKETS_H
#define BRACKETS_H

#include <stdio.h>


typedef enum {
    TREE_OK = 0,
    TREE_ARGUMENT,
    TREE_MEMORY,
    TREE_VALUE,
    TREE_FILE
} TreeStatus;

typedef struct TreeNode {
    char *value; 
    struct TreeNode *children, *next;
} TreeNode;

TreeStatus parse_tree(const char *text,TreeNode **root);
void print_tree(FILE *file,const TreeNode *root,unsigned int depth);
void free_tree(TreeNode *root);


#endif