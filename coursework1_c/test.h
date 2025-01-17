#ifndef TEST_HEADER
#define TEST_HEADER

#include "./bst.h"
#include "./avl.h"
#include "./brt.h"

void fprint_elapsed_time(FILE *d, char *prefix, long int sec, long int usec);

void bst_insert_test(FILE *d, Bst_Node *root, int *arr, size_t s);
void bst_search_test(FILE *d, Bst_Node *root, int *arr, size_t s);
void bst_delete_test(FILE *d, Bst_Node *root, int *arr, size_t s);

void avl_insert_test(FILE *d, Avl_Node *root, int *arr, size_t s);
void avl_search_test(FILE *d, Avl_Node *root, int *arr, size_t s);
void avl_delete_test(FILE *d, Avl_Node *root, int *arr, size_t s);

void brt_insert_test(FILE *d, Brt_Node **root, int *arr, size_t s);
void brt_search_test(FILE *d, Brt_Node **root, int *arr, size_t s);
void brt_delete_test(FILE *d, Brt_Node **root, int *arr, size_t s);
    
#endif
