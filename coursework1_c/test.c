#include "./test.h"

#include <sys/time.h>

void fprint_elapsed_time(FILE *d, char* prefix, long int sec, long int usec) {
    fprintf(d, "%s %ld.%06ld\n", prefix, sec, usec);
}

void bst_insert_test(FILE *d, Bst_Node *root, int *arr, size_t s) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    for (int i = 0; i < s; ++i) {
        root = bst_insert(root, arr[i]);
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    fprint_elapsed_time(d, "", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void bst_search_test(FILE *d, Bst_Node *root, int *arr, size_t s) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    for (int i = 0; i < s; ++i) {
        bst_search(root, arr[i]);
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    fprint_elapsed_time(d, "", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void bst_delete_test(FILE *d, Bst_Node *root, int *arr, size_t s) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    for (int i = 0; i < s; ++i) {
        root = bst_delete_node(root, arr[i]);
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    fprint_elapsed_time(d, "", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}


void avl_insert_test(FILE *d, Avl_Node *root, int *arr, size_t s) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    for (int i = 0; i < s; ++i) {
        root = avl_insert(root, arr[i]);
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    fprint_elapsed_time(d, "", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void avl_search_test(FILE *d, Avl_Node *root, int *arr, size_t s) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    for (int i = 0; i < s; ++i) {
        avl_search(root, arr[i]);
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    fprint_elapsed_time(d, "", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void avl_delete_test(FILE *d, Avl_Node *root, int *arr, size_t s) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    for (int i = 0; i < s; ++i) {
        root = avl_delete_node(root, arr[i]);
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    fprint_elapsed_time(d, "", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void brt_insert_test(FILE *d, Brt_Node **root, int *arr, size_t s) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    for (int i = 0; i < s; ++i) {
        brt_insert(root, arr[i]);
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    fprint_elapsed_time(d, "", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void brt_search_test(FILE *d, Brt_Node **root, int *arr, size_t s) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    for (int i = 0; i < s; ++i) {
        brt_search(*root, arr[i]);
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    fprint_elapsed_time(d, "", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void brt_delete_test(FILE *d, Brt_Node **root, int *arr, size_t s) {
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
    for (int i = 0; i < s; ++i) {
        brt_delete_by_data(root, arr[i]);
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    
    fprint_elapsed_time(d, "", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}
