#ifndef LIST_H_
#define LIST_H_

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include "log.h"

#define SUCCESSS 1

typedef unsigned int uint;

// TODO: insert after/be4

// при верификации проверяются все элементы списка
#define LINEAR_CHECKS 1
// при верификации проверяется тоолько основная иф=нформаиця списка
#define SIMPLE_CHECKS 0

#define DEBUG_MODE LINEAR_CHECKS

/**
 * @brief макрос для контроля того, как много данных мы хотим получить при дампе
 * 
 */
#ifndef TOTAL_DUMP
    #define TOTAL_DUMP 1
#endif

#ifndef ENABLE_SORT
    #define ENABLE_SORT 0
#endif

#
typedef int list_T;
//const char T_name[4] = "int";

// значит, что нет указателя на какого-то "соседа"
const int FREE_MATE            = -1;

// обозначение конца последовательности
const int POINTER_TO_VOID      = -2;
const list_T POISON_VAL        =  0;

// значит, пойзон освобожденной памяти
const int FREE_MEM             =  0xDEADB1AF;

#if ENABLE_SORT
// метка на то, что список отсортирован
const uint SORTED   = 1;
const uint UNSORTED = 0;
#endif

const int INCREASE_RATIO = 1;
const int REDUCE_RATIO   = 3;

// режим для resize
const int INCREASE_MODE  = 1;
const int REDUCE_MODE    = 0;

// default file names
const char GVIZ_DOT_NAME[20] = "gviztmp.dot";
const char GVIZ_HTM_NAME[20] = "tmp.htm";
const char LOG_FILE_NAME[10] = "log.htm";

// TODO: это что?
const int CMD_STR_SIZE = sizeof(GVIZ_DOT_NAME) + sizeof(GVIZ_HTM_NAME) + 30;

// list condition codes
enum class VERIF_CODE{
    OK,
    CORRUPTED_MEM,
    HEAD_INVALID,
    TAIL_INVALID,
    HEAD_FREE_CORRUPTED,
    TAIL_FREE_CORRUPTED,
    //INVALID_FIRST_ELEM,
    CORRUPTED_CAP,
    CORRUPTED_SIZE,
    PREV_CORRUPTED,
    CORRUPTED_INJECTIVITY,
    CORRUPTED_LINK,
    INVALID_FREE_ELEM_POINTER,
    FREE_CORRUPTED,
    VOID_ELEMENT                       // means that we have element in list that is not free and not busy
};

enum class ERR_CODE{
    OK,
    MEM_ALLOC,
    INVALID_POS
};

// meta for debugging
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

    // индекс головы списка
    int head;

    // индекс последнего элемента в списке
    int tail;

    // индекс головы подсписка свободныъ элементов
    int head_free;
    int tail_free;

    size_t capacity;
    size_t size;

#if ENABLE_SORT
    uint8_t is_sorted;
#endif
};
//
// использовать методы? https://www.quora.com/Can-structs-have-methods
// скорее всего нет

#define LOCATION __FILE__, __FUNCTION__, __LINE__

#define META_PARAMS char const * obj_name, char const * file_name, char const * func_name, int const n_line

#define ListConstructor(obj, cap)       _ListConstructor((obj), (cap), #obj, LOCATION)
#define ListDestructor(obj)             _ListDestructor((obj), #obj, LOCATION)
#define ListFront(obj)                  _ListFront((obj), #obj, LOCATION)
#define ListBack(obj)                   _ListBack((obj), #obj, LOCATION)
#define ListAfter(obj, nod)             _ListAfter((obj), (nod), #obj, LOCATION)
#define ListBefore(obj, nod)            _ListBefore((obj), (nod), #obj, LOCATION)
#define PushFront(obj, val)             _PushFront((obj), (val), #obj, LOCATION)
#define PushBack(obj, val)              _PushBack((obj), (val), #obj, LOCATION)
#define ListInsert(obj, pos, val)       _ListInsert((obj), (pos), (val), #obj, LOCATION)
#define ListRemove(obj, pos)            _ListRemove((obj), (pos), #obj, LOCATION)
#define ListRemoveAll(obj)              _ListRemoveAll((obj), #obj, LOCATION)


ERR_CODE  _ListConstructor(list* obj, const size_t capacity, META_PARAMS);

ERR_CODE  _ListDestructor(list* obj, META_PARAMS);

/**
 * @brief возвращает указатель на голову списка
 * 
 * @param obj - указатель на список
 * @return указатель на голову списка
 */
node* _ListFront(list* obj, META_PARAMS);

/**
 * @brief возвращает указатель на хвост списка
 * 
 * @param obj указатель на список
 * @return указатель на хвост списка
 */
node* _ListBack(list* obj, META_PARAMS);

/**
 * @brief возвращает следующий элемент после указанного
 * 
 * @param obj указатель на список
 * @param nod указатель на узел
 * @return node* node->next указатель
 */
node* _ListAfter(list* obj, node* nod, META_PARAMS);

/**
 * @brief возвращает предыдущий элемент после указанного
 * 
 * @param obj указатель на список
 * @param nod указатель на узел
 * @return node* node->prev указатель
 */
node* _ListBefore(list* obj, node* nod, META_PARAMS);

/**
 * @brief вставляет элемент в голову списка
 * 
 * @param obj указатель на список
 * @param val значение элемента
 * @return код ошибки
 */
ERR_CODE _PushFront(list* obj, list_T val, META_PARAMS);

/**
 * @brief вставляет элемент в конец списка
 * 
 * @param obj указатель на список
 * @param val значение элемента
 * @return код ошибки
 */
ERR_CODE _PushBack(list* obj, list_T val, META_PARAMS);

/**
 * @brief ищет физический индекс pos с помощью метлда SearchBYPos
 * 
 * @param obj 
 * @param pos 
 * @param val 
 * @return ERR_CODE 
 */
ERR_CODE _ListInsert(list* obj, int pos, list_T val, META_PARAMS);

// insert after
// insert before

ERR_CODE _ListRemove(list* obj, int pos, META_PARAMS);

ERR_CODE _ListRemoveAll(list* obj, META_PARAMS);

// int is_sorted

#define LIST_DUMP(obj, meta) list_dump((obj), (meta), #obj, LOCATION);

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
#endif //TOTAL_DUMP

#endif //LIST_H_
