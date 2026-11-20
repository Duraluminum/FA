#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "brackets.h"


static char *copy_part(const char *s, size_t n) {
    char *p = (char *)malloc(n + 1);
    if (!p) return NULL;

    memcpy(p, s, n);
    p[n] = '\0';

    return p;
}

void free_tree(TreeNode *r) {
    while(r) {
        TreeNode *n = r->next;
        free_tree(r->children);
        free(r->value);
        free(r);
        r = n;
    }
}

static void skip_spaces(const char **p) {
    while (isspace((unsigned char)**p)) (*p)++;
}

static TreeStatus parse_node(const char **p, TreeNode **result) {
    skip_spaces(p);
    const char *start = *p;
    while (**p && (**p != '(') && (**p != ')') && (**p != ',') && !isspace((unsigned char)**p))
        (*p)++;

    if (*p == start) return TREE_VALUE;

    TreeNode *n = (TreeNode *)calloc(1, sizeof(TreeNode));
    if (!n) return TREE_MEMORY;

    n->value = copy_part(start, (size_t)(*p - start));
    if (!n->value) {
        free(n);
        return TREE_MEMORY;
    }

    skip_spaces(p);

    if (**p == '(') {
        (*p)++;
        TreeNode **tail = &n->children;
        skip_spaces(p);

        while(**p && (**p != ')')) {
            TreeNode *child = NULL;
            TreeStatus status = parse_node(p, &child);
            if (status != TREE_OK) {
                free_tree(n);
                return status;
            }

            *tail = child;
            while (*tail) tail = &(*tail)->next;

            skip_spaces(p);
            
            if (**p == ',') {
                (*p)++;
                skip_spaces(p);
            }
            else if (**p != ')') {
                free_tree(n);
                return TREE_VALUE;
            }
        }

        if (**p != ')') {
            free_tree(n);
            return TREE_VALUE;
        }

        (*p)++;
    }

    *result = n;
    return TREE_OK;
}

TreeStatus parse_tree(const char *text, TreeNode **root) {
    if (!text || !root) return TREE_ARGUMENT;

    *root = NULL;
    const char *p = text;
    TreeStatus status = parse_node(&p, root);

    skip_spaces(&p);

    if (status != TREE_OK || *p) {
        free_tree(*root);
        *root = NULL;
        return status == TREE_OK ? TREE_VALUE : status;
    }

    return TREE_OK;
}

void print_tree(FILE *f, const TreeNode *r, unsigned int depth) {
    for(; r; r = r->next) {
        for (unsigned int i = 0; i < depth; i++)
            fprintf(f, "    ");

        fprintf(f, "%s\n", r->value);
        print_tree(f, r->children, depth + 1);
    }
}