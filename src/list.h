#ifndef LIST_H_
#define LIST_H_

#define meow printf("[%d]: meoww\n", __LINE__);

#include <stdint.h>
#include <stdio.h>
#include "log.h"

#define LINEAR_CHECKS 1
#define SIMPLE_CHECKS 0

#define DEBUG_MODE LINEAR_CHECKS
#define TOTAL_DUMP 1

typedef int list_T;
const char T_name[4] = "int";

const int FREE_INDICATION_VAL  = -1;
const list_T POISON_VAL        = 0;
const int FREE_MEM_POINTER_VAL = 7;

const int VAL_SORTED = 1;

const int INCREASE_RATIO = 2;
const int REDUCE_RATIO   = 8;

const int INCREASE_MODE  = 1;
const int REDUCE_MODE    = 0;

const char GVIZ_DOT_NAME[20] = "gviztmp.dot";
const char GVIZ_HTM_NAME[20] = "tmp.htm";
const char LOG_FILE_NAME[10] = "log.htm";

const int CMD_STR_SIZE = sizeof(GVIZ_DOT_NAME) + sizeof(GVIZ_HTM_NAME) + 30;

enum class VERIF_CODE{
    OK,
    CORRUPTED_MEM,
    HEAD_INVALID,
    INVALID_FIRST_ELEM,
    CORRUPTED_CAP,
    CORRUPTED_SIZE,
    INDICATION_TO_EMPTY_ELEM,
    CORRUPTED_INJECTIVITY,
    CORRUPTED_LINK,
    INVALID_FREE_ELEM_POINTER
};

enum class ERR_CODE{
    OK,
    MEM_ALLOC,
    INVALID_POS
};

struct meta_info{
    char* obj_name;
    char* func_name;
    char* file_name;
    uint32_t   n_line;
};

struct node{
    list_T val;
    int next;
    int prev;
};

struct list{
    node* nodes;

    int head;
    int tail;
    int head_free;
    int tail_free;

    int capacity;
    int size;

    int is_sorted;
};

// использовать методы? https://www.quora.com/Can-structs-have-methods
#define LOCATION __FILE__, __FUNCTION__, __LINE__

#define META_PARAMS char const * obj_name, char const * file_name, char const * func_name, int const n_line

#define ListConstructor(obj, cap) _ListConstructor((obj), (cap), #obj, LOCATION)

#define ListDestructor(obj) _ListDestructor((obj), #obj, LOCATION)

#define ListFront(obj) _ListFront((obj), #obj, LOCATION)

#define ListBack(obj) _ListBack((obj), #obj, LOCATION)

#define ListAfter(obj, nod) _ListAfter((obj), (nod), #obj, LOCATION)

#define ListBefore(obj, nod) _ListBefore((obj), (nod), #obj, LOCATION)

#define PushFront(obj, val) _PushFront((obj), (val), #obj, LOCATION)

#define PushBack(obj, val) _PushBack((obj), (val), #obj, LOCATION)

#define SearchByPos(obj, pos) _SearchByPos((obj), (pos), #obj, LOCATION)

#define ListInsert(obj, pos, val) _ListInsert((obj), (pos), (val), #obj, LOCATION)

#define ListRemove(obj, pos) _ListRemove((obj), (pos), #obj, LOCATION)

#define ListRemoveAll(obj) _ListRemoveAll((obj), #obj, LOCATION)




ERR_CODE  _ListConstructor(list* obj, const int capacity, META_PARAMS);

ERR_CODE  _ListDestructor(list* obj, META_PARAMS);

node* _ListFront(list* obj, META_PARAMS);

node* _ListBack(list* obj, META_PARAMS);

node* _ListAfter(list* obj, node* nod, META_PARAMS);

node* _ListBefore(list* obj, node* nod, META_PARAMS);

ERR_CODE _PushFront(list* obj, list_T val, META_PARAMS);

ERR_CODE _PushBack(list* obj, list_T val, META_PARAMS);

int _SearchByPos(list* obj, int pos, META_PARAMS);

ERR_CODE _ListInsert(list* obj, int pos, list_T val, META_PARAMS);

ERR_CODE _ListRemove(list* obj, int pos, META_PARAMS);

ERR_CODE _ListRemoveAll(list* obj, META_PARAMS);

// int is_sorted

#define LIST_DUMP(obj, meta) list_dump((obj), (meta), LOCATION);

#if TOTAL_DUMP == 0
#define LIST_OK(obj)                                \
{                                                   \
    int ver_code_ = (int)list_verification((obj));  \
                                                    \
    if(ver_code_ != (int)VERIF_CODE::OK){           \
        printf("-------  <ERROR> -------\n"         \
               "code:           %d\n"               \
               "file:           %s\n"               \
               "function:       %s(%d)\n"           \
               "lib function:   %s(%d)\n"           \
               "-----------------------\n",         \
               ver_code_, file_name,                \
               func_name, n_line,                   \
               __FUNCTION__, __LINE__);             \
        exit(0);                                    \
    }                                               \
}
#else
#define LIST_OK(obj)                                                    \
{                                                                       \
    int ver_code_ = (int)list_verification((obj));                      \
    meta_info meta = {};                                                \
                                                                        \
    meta.obj_name = (char*)calloc(strlen(obj_name), sizeof(char));      \
    strcpy(meta.obj_name, obj_name);                                    \
                                                                        \
    meta.file_name = (char*)calloc(strlen(file_name), sizeof(char));    \
    strcpy(meta.file_name, file_name);                                  \
                                                                        \
    meta.func_name = (char*)calloc(strlen(func_name), sizeof(char));    \
    strcpy(meta.func_name, func_name);                                  \
                                                                        \
    meta.n_line    = (uint32_t)n_line;                                  \
                                                                        \
    list_dump((obj), &(meta), #obj, LOCATION);                          \
                                                                        \
    free(meta.obj_name);                                                \
    free(meta.file_name);                                               \
    free(meta.func_name);                                               \
                                                                        \
    if(ver_code_ != (int)VERIF_CODE::OK){                               \
        printf("------- <ERROR> ------- \n"                             \
               "code:           %d\n"                                   \
               "file:           %s\n"                                   \
               "function:       %s(%d)\n"                               \
               "lib function:   %s(%d)\n"                               \
               "-----------------------\n",                             \
               ver_code_, file_name,                                    \
               func_name, n_line,                                       \
               __FUNCTION__, __LINE__);                                 \
        close_log_file();                                               \
        exit(0);                                                        \
    }                                                                   \
                                                                        \
}
#endif

#endif
