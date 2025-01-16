#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "./test.h"

struct Root {
    Bst_Node *bst;
    Avl_Node *avl;
    Brt_Node *brt;
};

FILE *open_file_for_writing(const char* filename) {
    FILE *fd = fopen(filename, "w");
    
    if (!fd) {
        perror("FATAL: файл не удалось открыть");
        exit(-1);
    }

    return fd;
}

int* generate_arr(size_t s, int range) {
    int* arr = (int*)malloc(sizeof(int) * s);
    if (!arr) {
        perror("FATAL: массив не сгенерирован");
        exit(-1);
    }
    
    for (int i = 0; i < s; ++i) {
        arr[i] = rand() % range;
    }

    return arr;
}

// Арифметическая програессия
int* iota_arr(size_t s, size_t initial_range, size_t common_diff) {
    int* arr = (int*)malloc(sizeof(int) * s);
    if (!arr) {
        perror("FATAL: массив не сгенерирован");
        exit(-1);
    }

    int initial_val = rand() % initial_range;
    int temper = initial_val;
    for (int i = 0; i < s; ++i) {
        arr[i] = temper;
        temper += common_diff;
    }

    return arr;
}

void print_help(const char *prog_name) {
    printf("Usage %s [OPTION] [TREE NAME] [METHOD] [OUTPUT NAME]\n", prog_name);
    printf("\nOutput name:");
    printf("\n\tstdout - write to console buffer");
    printf("\n\t\"any string\" - outputs file with that name");
    printf("\nOptions:");
    printf("\n\t--arr-size=ARRAY-SIZE - specifies array size, size must be positive number");
    printf("\n\t--rand-range=RANDOM-RANGE - specifies random range from 0 to (RANDOM-RANGE - 1), range must be positive number");
    printf("\n\t--test-count=COUNT - specifies how much test must be proceed, count must be positive number");
    printf("\n\t--type-test={iota, rand} - specifies test type (enum).");
    printf("\n\t\tiota - arithmetic sequencce array, rand - array generated using rand()");
    printf("\nAvailable tree names:");
    printf("\n\tbst - binary search tree");
    printf("\n\tavl - AVL tree");
    printf("\n\tbrt - Black-red tree");
    printf("\nAvailable methods:");
    printf("\n\tinsert");
    printf("\n\tsearch");
    printf("\n\tdelete");
    
    printf("\n");
}

void bst_case(const char *method, Bst_Node *root, FILE *d, int *arr, size_t arr_size) {
    if (strcmp(method, "insert") == 0) {
        bst_insert_test(d, root, arr, arr_size);
    } else if (strcmp(method, "search") == 0) {
        for (int i = 0; i < arr_size; ++i) {
            root = bst_insert(root, arr[i]);
        }
        
        bst_search_test(d, root, arr, arr_size);
    } else if (strcmp(method, "delete") == 0) {
        for (int i = 0; i < arr_size; ++i) {
            root = bst_insert(root, arr[i]);
        }
        
        bst_delete_test(d, root, arr, arr_size);
    }
}

void avl_case(const char *method, Avl_Node *root, FILE *d, int *arr, size_t arr_size) {
    if (strcmp(method, "insert") == 0) {
        avl_insert_test(d, root, arr, arr_size);
    } else if (strcmp(method, "search") == 0) {
        for (int i = 0; i < arr_size; ++i) {
            root = avl_insert(root, arr[i]);
        }
        
        avl_search_test(d, root, arr, arr_size);
    } else if (strcmp(method, "delete") == 0) {
        for (int i = 0; i < arr_size; ++i) {
            root = avl_insert(root, arr[i]);
        }
        
        avl_delete_test(d, root, arr, arr_size);
    }
}

void brt_case(const char *method, Brt_Node *root, FILE *d, int *arr, size_t arr_size) {
    if (strcmp(method, "insert") == 0) {
        brt_insert_test(d, &root, arr, arr_size);
    } else if (strcmp(method, "search") == 0) {
        for (int i = 0; i < arr_size; ++i) {
            brt_insert(&root, arr[i]);
        }
        
        brt_search_test(d, &root, arr, arr_size);
    } else if (strcmp(method, "delete") == 0) {
        for (int i = 0; i < arr_size; ++i) {
            brt_insert(&root, arr[i]);
        }
        
        brt_delete_test(d, &root, arr, arr_size);
    }
}

void proceed_cases(const char *tree, const char *method, struct Root *root, FILE *d, int *arr, size_t arr_size) {
    if (strcmp(tree, "bst") == 0) {
        bst_case(method, root->bst, d, arr, arr_size);
    } else if (strcmp(tree, "avl") == 0) {
        avl_case(method, root->avl, d, arr, arr_size);
    } else if (strcmp(tree, "brt") == 0) {
        brt_case(method, root->brt, d, arr, arr_size);
    }    
}

void execute_standart_case(const char *tree, const char *method, const char *type, FILE *d, size_t arr_size, size_t rand_range) {
    struct Root root = {0};

    srand(time(NULL));
    
    int *arr;
    if (strcmp(type, "iota") == 0) {
        arr = iota_arr(arr_size, rand_range, 10);
    } else {
        arr = generate_arr(arr_size, rand_range);
    }

    proceed_cases(tree, method, &root, d, arr, arr_size);
    
    free(arr);
}

void execute(const char *tree, const char *method, const char *output, const char *type, size_t arr_size, size_t rand_range, size_t test_count) {
    FILE *file;
    if (strcmp(output, "stdout") == 0) {
        file = stdout;
    } else {
        file = open_file_for_writing(output);
    }

    for (int i = 0; i < test_count; ++i) {
        execute_standart_case(tree, method, type, file, arr_size, rand_range);
    }

    fclose(file);
}

enum {
    ARR_SIZE = 0,
    RAND_RANGE,
    TEST_COUNT,
    TYPE_TEST,
} typedef option;

bool check_valid(const char *opt, char **arg, option *opt_enum, bool *is_num) {
    size_t len = strlen(opt);
    if (len < 2) {
        perror("FATAL: ошибка аргумента");
        exit(-1);
    }

    *arg = (char*)malloc(sizeof(char) * 255);
    char param[255] = {0}; char offs_param = 0, offs_arg = 0;
    for (int i = 0; i < len; ++i) {
        char c = opt[i];
        switch (c) {
        case '-':
            continue;
        case '=':
            for (int j = i + 1; j < len; ++j)
                (*arg)[offs_arg++] = opt[j];
            goto end_for;
        default:
            param[offs_param++] = c;
        }
    }
  end_for:

    param[offs_param] = '\0';
    (*arg)[offs_arg] = '\0';

    if (strcmp(param, "arrsize") == 0) {
        *opt_enum = ARR_SIZE;
        *is_num = true;
        return true;
    } else if (strcmp(param, "randrange") == 0) {
        *opt_enum = RAND_RANGE;
        *is_num = true;
        return true;
    } else if (strcmp(param, "testcount") == 0) {
        *opt_enum = TEST_COUNT;
        *is_num = true;
        return true;
    } else if (strcmp(param, "typetest") == 0) {

        if ((strcmp(*arg, "iota") != 0) && (strcmp(*arg, "rand") != 0)) {
            perror("FATAL: неверное предоставленный enum для type-test");
            exit(-1);
        }
        
        *opt_enum = TYPE_TEST;
        *is_num = false;
        return true;
    }

    return false;
}

struct optional {
    int val;
    char *str;
    bool exist;
} typedef opt_t;

void parse_argv(int argc, const char **argv) {
    opt_t arr_size = {0}, rand_range = {0}, test_count = {0}, type_test = {0};
    
    for (int i = 1; i < argc - 3; ++i) {
        char *arg; bool is_num; option opt;
        if (check_valid(argv[i], &arg, &opt, &is_num)) {
            switch (opt) {
            case ARR_SIZE:
                arr_size.val = atoi(arg);
                arr_size.exist = true;
                break;
            case RAND_RANGE:
                rand_range.val = atoi(arg);
                rand_range.exist = true;
                break;
            case TEST_COUNT:
                test_count.val = atoi(arg);
                test_count.exist = true;
                break;
            case TYPE_TEST:
                type_test.str = arg;
                type_test.exist = true;
                break;
            }
        } else {
            perror("FATAL: неверный аргумент");
            exit(-1);
        }
    }

    if ((arr_size.exist && arr_size.val <= 0)
        || (rand_range.exist && rand_range.val <= 0)
        || (test_count.exist && test_count.val <= 0)) {
        perror("FATAL: было передано отрицательное число либо значение не является числом");
        exit(-1);
    }
    
    execute(argv[argc - 3], argv[argc - 2], argv[argc - 1]
            , type_test.exist ? type_test.str : "iota", arr_size.exist ? arr_size.val : 15000
            , rand_range.exist ? rand_range.val : 2000, test_count.exist ? test_count.val : 100);
}

int main(int argc, const char **argv) {
    if (argc == 4) {
        execute(argv[1], argv[2], argv[3], "iota", 15000, 1000, 100);
        return 0;
    } else if (argc > 4) {
        parse_argv(argc, argv);
        return 0;
    }
 
    print_help(argv[0]);
    
    return 0;
}
