#include "list.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

verif_code list_verification(list* obj);

void list_dump(list* obj, meta_info *meta, char const * file_name, char const * func_name, int const n_line);

void dump_list_data(list* obj);

void print_list(list* obj);

void _ListConstructor(list* obj, size_t const capacity, const char* name, META_PARAMS){

	assert(obj != NULL);

	if(capacity > 0){
		obj->nodes = (node*)calloc(capacity, sizeof(node));
		assert(obj->nodes != NULL);
	}

	for(int n_elem = 1; n_elem < capacity; n_elem++){
		obj->nodes[n_elem].next = n_elem + 1;
		obj->nodes[n_elem].prev = PREV_INVALID_VAL;
	}

	obj->is_sorted = VAL_SORTED;

	obj->head = 0;
	obj->tail = 1;
	obj->size = 0;
	obj->capacity = capacity;

	META_INIT(obj->meta.birth)

	obj->meta.name = (char*)calloc(strlen(name), sizeof(char));
	strcpy(obj->meta.name, name);

	obj->meta.death.file_name = NULL;
	obj->meta.death.func_name = NULL;
	obj->meta.death.n_line    = -1;

	LIST_OK(obj)

	return;
}

void _ListDestructor(list* obj, META_PARAMS){

	LIST_OK(obj)

	free(obj->nodes);
	obj->nodes = (node*)FREE_MEM_POINTER_VAL;

	// DESTRUCTION META INFO CREATION
	META_INIT(obj->meta.death)

	return;
}

verif_code list_verification(list* obj){

	assert(obj != NULL);

	if(obj->nodes == NULL){
		return verif_code::corrupted_mem;
	}
	
	if(obj->tail > obj->capacity){
		return verif_code::tail_invalid;
	}

	if(obj->head >= obj->capacity){
		return verif_code::head_invalid;
	}

	if(obj->meta.birth.func_name == NULL ||
	   obj->meta.birth.file_name == NULL){
		return verif_code::corrupted_meta;
	}

	// TODO: magic number ??
	if(obj->nodes[0].next != 0 || obj->nodes[0].prev != 0){
		return verif_code::invalid_first_elem;
	}

	/*
	#if DEBUG_MODE == LINEAR_CHECKS


	// TODO: запилить линейную проверку
	
	for(uint32_t n_elem = 1; n_elem < obj->capacity; n_elem++){

		if(obj->nodes[n_elem].next)
	}

	#endif
	*/

	return verif_code::ok;
}

void list_dump(list* obj, meta_info *meta, META_PARAMS){

	assert(obj  != NULL);
	assert(meta != NULL);

	to_log("Dump called from function %s(%d)<br>"
		   "That was called from file %s, function %s(%d)<br>"
		   "[#] list %s<%s><br>"
		   "<br>"
		   "function_birth_name: %s<br>"
		   "file_birth_name:     %s<br>"
		   "n_line_birth_name:   %u<br>"
		   "function_death_name: %s<br>"
		   "file_death_name:     %s<br>"
		   "n_line_death_name:   %u<br>"
		   "<br>"
		   "capacity  = %u<br>"
		   "size      = %u<br>"
		   "head      = %u<br>"
		   "tail      = %u<br>"
		   "is_sorted = %d<br>"
		   "<br>"
		   "data = [",
		   func_name, n_line, meta->file_name, meta->func_name, meta->n_line,
		   obj->meta.name, T_name,
		   obj->meta.birth.func_name, obj->meta.birth.file_name, obj->meta.birth.n_line,
		   obj->meta.death.func_name, obj->meta.death.file_name, obj->meta.death.n_line,
		   obj->capacity, obj->size, obj->head, obj->tail, obj->is_sorted);

	dump_list_data(obj);

	to_log("]<br><br>");

	print_list(obj);

	return;
}

void dump_list_data(list* obj){
	
	assert(obj != NULL);

	for(int iter = obj->head; iter != obj->tail; iter = obj->nodes[iter].next){
		to_log("%d, ", obj->nodes[iter].val);
	}

	return;
}

void print_list(list* obj){

	assert(obj != NULL);

	FILE* tmp_file = fopen(GVIZ_TMP_NAME, "w");

	fprintf(tmp_file, "digraph structs {\n"
			          "rankdir=LR;\n");

	// fix node node node node
	for(int n_node = 0; n_node < obj->capacity; n_node++){

		fprintf(tmp_file, "%d [shape=record,label=\"  <main%d> %d | <prev%d> prev | <next%d> next\"];\n", 
			    n_node, n_node, n_node, n_node, n_node);
	}

	fprintf(tmp_file, "{\n"
    				  "node[pin=true];\n"
    				  "edge[style=invis];\n");

	for(int n_node = 0; n_node < obj->capacity - 1; n_node++){

		fprintf(tmp_file, "%d->", n_node);
	}

	fprintf(tmp_file, "%d;\n}\n}\n", obj->capacity - 1);

	// TODO: запилить связи

	fclose(tmp_file);

	// TODO: пофиксить это

	WinExec("dot -Tsvg iopv020vc_zxpq_.dot -o ttmp.svg", 0);

	to_log("<br>"
		   "<img src=%s> <br>", "ttmp.svg");

	return;
}
