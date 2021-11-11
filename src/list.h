#ifndef LIST_H_
#define LIST_H_

#define meow printf("meoww\n");

#include <stdint.h>
#include <stdio.h>
#include "log.h"

#define LINEAR_CHECKS 1
#define SIMPLE_CHECKS 0

#define DEBUG_MODE SIMPLE_CHECKS
// #define ALWAYS_DUMP 0

typedef int list_T;

const char T_name[4] = "int";

const int PREV_INVALID_VAL = -1;

const int FREE_MEM_POINTER_VAL = 7;
const int VAL_SORTED = 1;

const char GVIZ_TMP_NAME[30] = "iopv020vc_zxpq_.dot";
const char LOG_FILE_NAME[10] = "log.htm";

enum verif_code{
	ok,
	corrupted_mem,
	corrupted_meta,
	tail_invalid,
	head_invalid,
	invalid_first_elem
};

struct meta_info{
	char* func_name;
	char* file_name;
	uint32_t   n_line;
};

struct list_meta_info{
	char* name;
	meta_info birth;

	meta_info death;
};

struct node{
	list_T val;
	int next;
	int prev;
};

// TODO: защита?
struct list{
	node* nodes;

	list_meta_info meta;

	uint32_t head;
	uint32_t tail;
	
	uint32_t capacity;
	uint32_t size;

	int is_sorted;
};

// использовать методы? https://www.quora.com/Can-structs-have-methods
#define LOCATION __FILE__, __FUNCTION__, __LINE__

#define META_PARAMS char const * file_name, char const * func_name, int const n_line

#define META_INIT(meta)   													\
{																			\
	meta.file_name = (char*)calloc(strlen(file_name), sizeof(char));		\
	strcpy(meta.file_name, file_name);										\
																			\
	meta.func_name = (char*)calloc(strlen(func_name), sizeof(char));		\
	strcpy(meta.func_name, func_name);										\
																			\
	meta.n_line = (uint32_t)n_line;											\
}

#define ListConstructor(obj, cap) _ListConstructor((obj), (cap), #obj, LOCATION)
// TODO: fix long parameters
void _ListConstructor(list* obj, size_t const capacity, const char* name, META_PARAMS);

#define ListDestructor(obj) _ListDestructor((obj), LOCATION)

void _ListDestructor(list* obj, META_PARAMS);

#define LIST_DUMP(obj, meta) list_dump((obj), (meta), LOCATION);

// если не exit, то что?
#if TOTAL_DUMP == 0
#define LIST_OK(obj)								\
{													\
	int ver_code_ = list_verification((obj));		\
													\
	if(ver_code_ != (int)verif_code::ok){			\
		printf("<ERROR>: %d\n"						\
			   "file:     %s(%d)\n"					\
			   "function: %s\n",					\
			   ver_code_, file_name,				\
			   n_line, func_name);					\
		exit(0);									\
	}												\
}
#else
#define LIST_OK(obj)								\
{													\
	int ver_code_ = list_verification((obj));		\
													\
	if(ver_code_ != (int)verif_code::ok){			\
		printf("<ERROR>: %d\n"						\
			   "file:     %s(%d)\n"					\
			   "function: %s\n",					\
			   ver_code_, file_name,				\
			   n_line, func_name);					\
		exit(0);									\
	}												\
	list_dump((obj), &(meta), LOCATION);			\
}
#endif

#endif
