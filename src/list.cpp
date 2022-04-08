#include "list.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <sys\stat.h> 

//----------------------LOCAL-FUNCTIONS-DECLARATION-----------------------//

#define list_resize(obj, mode) _list_resize((obj), (mode), #obj, LOCATION)

#define  extract_free_ind(obj) _extract_free_ind((obj), #obj, LOCATION)

ERR_CODE _list_resize(list* obj, int mode, META_PARAMS);

int _extract_free_ind(list* obj, META_PARAMS);

int node_cmp(const void * node1, const void * node2);

VERIF_CODE list_verification(list* obj);

void list_dump(list* obj, meta_info *meta, META_PARAMS);

void dump_nodes(list* obj);

void print_list(list* obj);

int get_file_size(const char* name);

// checker that node is in list
inline int node_in_list(list* obj, node* nod){
	if(nod >= obj->nodes + obj->size || nod < obj->nodes){
		return 0;
	}
	return 1;
}

typedef unsigned int uint;

// вставляем в любом случае за единицу
// в любом случае реализую is_sorted, сортировку, сверхбыструю адресацию по логическим адресам
// так как поиск энивей за O(n), будем сортить предподсчётом за O(n), нужно прописать в доку, что юзается много памяти

// TODO: может быть ошибка при вставке на позицию size
// TODO: слишком много ифов в методах, нужна оптимизация
// TODO: более продуманые ресайзы
// TODO: аккуратно проставить is_sorted = UNSORTED в методах вставки/удаления

//----------------------PUBLIC-FUNCTIONS-DEFINITIONS----------------------//

// OK
ERR_CODE _ListConstructor(list* obj, const size_t capacity, META_PARAMS){

	assert(obj != NULL);

	if(capacity == 0) obj = NULL;

	obj->nodes = (node*)calloc(capacity, sizeof(node));

	if(obj->nodes == NULL){
		return ERR_CODE::MEM_ALLOC;
	}

	// создаем односвязный список свободных элементов
	// индикатор незанятости элемента - FREE_MATE в prev
	for(int n_node = 0; n_node < capacity; n_node++){
		obj->nodes[n_node].val  = POISON_VAL;
		obj->nodes[n_node].next = n_node + 1;
		obj->nodes[n_node].prev = FREE_MATE;
	}

	obj->nodes[capacity - 1].next = POINTER_TO_VOID;

	obj->is_sorted 	= SORTED;

	obj->head 		= FREE_MATE;
	obj->tail 		= FREE_MATE;

	obj->head_free 	= 0;
	obj->tail_free 	= capacity - 1;

	obj->size 		= 0;
	obj->capacity 	= capacity;

	LIST_OK(obj)

	return ERR_CODE::OK;
}
//----------------------------------------------------------------------------------------//

// OK
ERR_CODE _ListDestructor(list* obj, META_PARAMS){

	assert(obj != NULL);
	LIST_OK(obj)

	free(obj->nodes);
	obj->nodes = (node*)FREE_MEM;

	obj->head 		= FREE_MATE;
	obj->tail 		= FREE_MATE;
	obj->head_free  = FREE_MATE;
	obj->tail_free  = FREE_MATE;
	obj->capacity 	= 0;
	obj->size 		= 0;
	obj->is_sorted  = 1;

	return ERR_CODE::OK;
}
//----------------------------------------------------------------------------------------//

// OK
node* _ListFront(list* obj, META_PARAMS){

	assert(obj != NULL);
	LIST_OK(obj)

	if(obj->size > 0) return obj->nodes + obj->head;
	else return NULL;
}
//----------------------------------------------------------------------------------------//

// OK
node* _ListBack(list* obj, META_PARAMS){

	assert(obj != NULL);
	LIST_OK(obj)

	if(obj->size > 0) return obj->nodes + obj->tail;
	else return NULL;
}
//----------------------------------------------------------------------------------------//

// OK
node* _ListAfter(list* obj, const node* nod, META_PARAMS){

	assert(obj  != NULL);
	assert(nod  != NULL);
	LIST_OK(obj)

	if(!node_in_list(obj, nod)) return NULL;

	// после любого элемента кроме хвоста есть следующий элемент
	if(nod != obj->nodes + obj->tail){
		return obj->nodes + nod->next;
	}
	else return NULL;
}
//----------------------------------------------------------------------------------------//

// OK
node* _ListBefore(list* obj, const node* nod, META_PARAMS){

	assert(obj  != NULL);
	assert(nod  != NULL);
	LIST_OK(obj)	

	if(!node_in_list(obj, nod)) return NULL;

	// после любого элемента кроме головы есть предыдущий элемент	
	if(nod != obj->nodes + obj->head){
		return obj->nodes + nod->prev];
	}
	else return NULL;
}
//----------------------------------------------------------------------------------------//

// OK

// TODO: если я захочу, чтобы массив всегда был отсортирован - тогда тут лучше сделать специальную функцию для сортировки,
// а в pushback'e просто в конец массива вставлять
ERR_CODE _PushFront(list* obj, const list_T val, META_PARAMS){

	assert(obj != NULL);
	LIST_OK(obj)

	if(obj->size >= obj->capacity){
		int resize_rez = (int)list_resize(obj, INCREASE_MODE);
		
		if(resize_rez != (int)ERR_CODE::OK){
			return (ERR_CODE)resize_rez;
		}
	}

	int new_head_pos = extract_free_ind(obj);

	obj->nodes[new_head_pos].next = obj->head;
	obj->nodes[new_head_pos].prev = FREE_MATE;

	if(obj->head != FREE_MATE){
		obj->nodes[obj->head].prev = new_head_pos;
	}

	obj->head = new_head_pos;
	obj->nodes[obj->head].val = val;

	if(obj->size == 0){
		obj->tail = new_head_pos;
	}

	obj->size++;

	LIST_OK(obj)
	return ERR_CODE::OK;
}
//----------------------------------------------------------------------------------------//

// OK

ERR_CODE _PushBack(list* obj, const list_T val, META_PARAMS){

	assert(obj != NULL);
	LIST_OK(obj)

	if(obj->size >= obj->capacity){
		int resize_rez = (int)list_resize(obj, INCREASE_MODE);
		
		if(resize_rez != (int)ERR_CODE::OK){
			return (ERR_CODE)resize_rez;
		}
	}

	int new_tail_pos = extract_free_ind(obj);

	obj->nodes[new_tail_pos].next = FREE_MATE;
	obj->nodes[new_tail_pos].prev = obj->tail;

	if(obj->tail != FREE_MATE){
		obj->nodes[obj->tail].next = new_tail_pos;
	}

	obj->tail = new_tail_pos;
	obj->nodes[obj->tail].val = val;

	if(obj->size == 0){
		obj->head = new_tail_pos;
	}
	obj->size++;

	LIST_OK(obj)

	return ERR_CODE::OK;
}
//----------------------------------------------------------------------------------------//

// OK
// да! будем сортить сразу после вставки/удаления. Тогда операция вставки будет за O(n * const) (radix), или за O(n) (count_sort)
ERR_CODE _ListInsert(list* obj, const uint pos, const list_T val, META_PARAMS){

	assert(obj != NULL);
	LIST_OK(obj);

	// если = obj->size, то вставляем в конец как pushback'e
	if(pos < 0 || pos > obj->size){
		return ERR_CODE::INVALID_POS;
	}

	if(obj->size >= obj->capacity){
		int resize_rez = (int)list_resize(obj, INCREASE_MODE);
		
		if(resize_rez != (int)ERR_CODE::OK){
			return (ERR_CODE)resize_rez;
		}
	}

	// в SearchByPos должна вернуть сделать массив отсортированным, если он уже не отсортирован
	int pos_of_prev = SearchByPos(obj, pos);

	int new_node_pos = extract_free_ind(obj);

	obj->nodes[new_node_pos].next = obj->nodes[pos_of_prev].next;
	obj->nodes[new_node_pos].prev = pos_of_prev;
	obj->nodes[new_node_pos].val  = val;

	obj->nodes[pos_of_prev].next 					= new_node_pos;
	obj->nodes[obj->nodes[new_node_pos].next].prev 	= new_node_pos;
	obj->size++;

	LIST_OK(obj)
	return ERR_CODE::OK;
}
//----------------------------------------------------------------------------------------//

// OK
ERR_CODE _ListRemove(list* obj, const uint pos, META_PARAMS){

	assert(obj != NULL);

	LIST_OK(obj);

	if(pos < 0 || pos >= obj->size){
		return ERR_CODE::INVALID_POS;
	}

	if((obj->capacity >> REDUCE_RATIO) >= obj->size){
		int resize_rez = (int)list_resize(obj, REDUCE_MODE);

		if(resize_rez != (int)ERR_CODE::OK){
			return (ERR_CODE)resize_rez;
		}
	}

	int physical_pos = SearchByPos(obj, pos);

	int pos_next = obj->nodes[physical_pos].next;
	int pos_prev = obj->nodes[physical_pos].prev;

	// не уверен, нужен тест
	if(pos_prev != FREE_MATE){
		obj->nodes[pos_prev].next = pos_next;
	}
	else{
		obj->head = pos_next;
	}

	if(pos_next != FREE_MATE){
		obj->nodes[pos_next].prev = pos_prev;
	}
	else{
		obj->tail = pos_prev;
	}

	if(obj->tail_free != FREE_MATE){
		obj->nodes[obj->tail_free].next = physical_pos;
		obj->nodes[physical_pos].prev   = obj->tail_free;
	}
	else{
		head_free                     = physical_pos;
		obj->nodes[physical_pos].prev = FREE_MATE;
	}

	obj->tail_free                = physical_pos;
	obj->nodes[physical_pos].val  = POISON_VAL;
	obj->nodes[physical_pos].next = FREE_MATE;

	obj->size--;

	LIST_OK(obj)
	return ERR_CODE::OK;
}
//----------------------------------------------------------------------------------------//

// OK
ERR_CODE _ListRemoveAll(list* obj, META_PARAMS){

	assert(obj != NULL);
	LIST_OK(obj)

	for(int n_node = 0; n_node < capacity; n_node++){
		obj->nodes[n_node].val  = POISON_VAL;
		obj->nodes[n_node].next = n_node + 1;
		obj->nodes[n_node].prev = FREE_MATE;
	}

	obj->nodes[obj->capacity - 1].next = POINTER_TO_VOID;

	obj->size = 0;
	obj->head_free = 0;
	obj->tail_free = obj->capacity - 1;

	obj->head = FREE_MATE;
	obj->tail = FREE_MATE;

	obj->is_sorted = UNSORTED;

	LIST_OK(obj)
	
	return ERR_CODE::OK;
}
//----------------------------------------------------------------------------------------//
//
int _SearchByPos(list* obj, int pos, META_PARAMS){
	
	assert(obj != NULL);
	LIST_OK(obj);

	int counter = 0;

	int n_node = obj->head;

	while((counter < pos) && (n_node != 0)){
		counter++;
		n_node = obj->nodes[n_node].next;
	}

	return n_node;
}
//----------------------------------------------------------------------------------------//

//----------------------LOCAL-FUNCTIONS-DEFINITIONS----------------------//

int _extract_free_ind(list* obj, META_PARAMS){

	assert(obj != NULL);
	LIST_OK(obj);

	int ind = obj->head_free;

	if(obj->nodes[ind].next != 0){
		int head_free_next = obj->nodes[ind].next;
		obj->head_free = head_free_next;
	}

	return ind;
}

ERR_CODE _list_resize(list* obj, int mode, META_PARAMS){

	assert(obj != NULL);
	LIST_OK(obj)

	if(mode == INCREASE_MODE){

		int new_capacity = obj->capacity * INCREASE_RATIO;
		
		node* p_new_mem = (node*)calloc(new_capacity, sizeof(node));

		if(p_new_mem == NULL){
			return ERR_CODE::MEM_ALLOC;
		}

		memcpy(p_new_mem, obj->nodes, obj->capacity * sizeof(node));

		free(obj->nodes);

		obj->nodes    = p_new_mem;
		p_new_mem     = NULL;

		for(int n_node = obj->capacity; n_node < new_capacity ; n_node++){
			
			obj->nodes[n_node].next = n_node + 1;
			obj->nodes[n_node].prev = FREE_INDICATION_VAL;
		}
		
		if(obj->nodes[obj->capacity - 1].prev == FREE_INDICATION_VAL){
			obj->nodes[obj->capacity - 1].next = obj->capacity;
		}

		if(obj->nodes[new_capacity - 1].prev == FREE_INDICATION_VAL){
			obj->nodes[new_capacity - 1].next = 0;
		}

		obj->nodes[obj->tail_free].next = obj->capacity;
		obj->tail_free 						= new_capacity - 1;

		obj->capacity = new_capacity;

	}

	/*
	else if(mode == REDUCE_MODE){
		
		SortList(obj);

		int new_capacity = obj->capacity / REDUCE_RATIO;
		
		node* p_new_mem = (node*)calloc(new_capacity, sizeof(node));
	
		if(p_new_mem == NULL){
			return ERR_CODE::MEM_ALLOC;
		}

		memcpy(p_new_mem, obj->nodes, new_capacity * sizeof(node));

		free(obj->nodes);

		obj->nodes = p_new_mem;
		obj->capacity = new_capacity;
		p_new_mem = NULL;

		if(obj->nodes[new_capacity - 1].prev == FREE_INDICATION_VAL){
			obj->nodes[new_capacity - 1].next = 0;
		}
	}
	*/
	LIST_OK(obj)
	return ERR_CODE::OK;
}


int node_cmp(const void * node1, const void * node2){

	assert(node1 != NULL);
	assert(node2 != NULL);
	assert(node1 != node2);

	return (int)((uint32_t)(((node*)node1)->prev)) - (int)((uint32_t)(((node*)node2)->prev));
}

void nodes_swap(node* a, node* b){
	
	assert(a != NULL);
	assert(b != NULL);
	assert(a != b);

	node c;

	memcpy(&c, a, sizeof(node));
	memcpy(a, b, sizeof(node));
	memcpy(b, &c, sizeof(node));	

	return;
}
/*
void _SortList(list* obj, META_PARAMS){

	assert(obj != NULL);

	int *swap_arr = (int*)calloc(obj->capacity, sizeof(int));

	swap_arr[0] = 0;

	int counter = 1;

	for(int n_node = obj->head; n_node != 0; n_node = obj->nodes[n_node].next, counter++){
		swap_arr[counter] = n_node;
	}

	for(int n_node = 0; n_node < counter; n_node++){

	}

	qsort(obj->nodes + 1, obj->capacity, sizeof(node), node_cmp);

	return;
}
*/

VERIF_CODE list_verification(list* obj){

	assert(obj != NULL);

	if(obj->nodes == NULL){
		return VERIF_CODE::CORRUPTED_MEM;
	}

	if((obj->head < 0) || (obj->head >= obj->capacity)){
		return VERIF_CODE::HEAD_INVALID;
	}
	
	if(obj->capacity <= obj->size){
		return VERIF_CODE::CORRUPTED_SIZE;
	}

	if(obj->nodes[0].next != 0 || obj->nodes[0].prev != 0){
		return VERIF_CODE::INVALID_FIRST_ELEM;
	}

	#if DEBUG_MODE == LINEAR_CHECKS

	char* visited = (char*)calloc(obj->capacity, sizeof(char));

	if(obj->size > 0){
		for(int n_node = obj->head; n_node != 0; visited[n_node] = 1, n_node = obj->nodes[n_node].next){

			int next_ind = obj->nodes[n_node].next;

			if(next_ind >= obj->capacity){
				return VERIF_CODE::CORRUPTED_LINK;
			}

			if(visited[n_node]){
				return VERIF_CODE::CORRUPTED_INJECTIVITY;
			}

			if(obj->nodes[n_node].prev == -1){
				return VERIF_CODE::INDICATION_TO_EMPTY_ELEM;
			}

			if(next_ind != 0){
				if(n_node != obj->nodes[next_ind].prev){
					return VERIF_CODE::CORRUPTED_LINK;
				}
			}
		}
	}

	for(int n_node = 1; n_node < obj->capacity; n_node++){

		if(visited[n_node]) continue;

		int next_ind = obj->nodes[n_node].next;
		
		if(next_ind >= obj->capacity){
			return VERIF_CODE::CORRUPTED_LINK;
		}

		if(visited[next_ind]){
			return VERIF_CODE::INVALID_FREE_ELEM_POINTER;
		}
	}
	#endif

	return VERIF_CODE::OK;
}

void list_dump(list* obj, meta_info *meta, META_PARAMS){

	assert(obj  != NULL);
	assert(meta != NULL);

	to_log("<br><br>Dump called from function %s(%d)<br>"
		   "That was called from file %s, function %s(%d)<br>"
		   "[#] list %s<%s><br>"
		   "<br>"
		   "capacity  = %d<br>"
		   "size      = %d<br>"
		   "head      = %d<br>"
		   "tail 	  = %d<br>"
		   "head_free = %d<br>"
		   "tail_free = %d<br>"
		   "is_sorted = %d<br>"
		   "<br>",
		   func_name, n_line, meta->file_name, meta->func_name, meta->n_line,
		   meta->obj_name, T_name,
		   obj->capacity, obj->size, obj->head, obj->tail, obj->head_free, obj->tail_free, obj->is_sorted);

	dump_nodes(obj);

	to_log("<br><br>");
	print_list(obj);
	to_log("<br><br>");
	
	return;
}

void dump_nodes(list* obj){
	
	assert(obj != NULL);

	to_log("data = [");
	for(int n_node = 0; n_node < obj->capacity; n_node++){
		to_log("%5d, ", obj->nodes[n_node].val);
	}
	to_log("]<br>");

	to_log("next = [");
	for(int n_node = 0; n_node < obj->capacity; n_node++){
		to_log("%5d, ", obj->nodes[n_node].next);
	}
	to_log("]<br>");

	to_log("prev = [");
	for(int n_node = 0; n_node < obj->capacity; n_node++){
		to_log("%5d, ", obj->nodes[n_node].prev);
	}
	to_log("]<br>");

	return;
}

void print_list(list* obj){

	assert(obj != NULL);

	FILE* tmp_file = fopen(GVIZ_DOT_NAME, "w");

	fprintf(tmp_file, "digraph structs {\n"
			          "rankdir=LR;\n");

	if(obj->capacity > 0){
		fprintf(tmp_file, "0 [style=\"filled\", fillcolor=\"#BB8FCE\", shape=record,label=\"  <main0> 0 | %d | { %d | %d }\"];\n", 
			    obj->nodes[0].val, obj->nodes[0].prev, obj->nodes[0].next);
	}
	for(int n_node = 1; n_node < obj->capacity; n_node++){

		fprintf(tmp_file, "%d [style=\"filled\", fillcolor=\"%s\", shape=record,label=\"  <main%d> %d | %d | { %d | %d }\"];\n", 
			    n_node, (obj->nodes[n_node].prev == FREE_INDICATION_VAL) ? "#AED6F1" : "#82E0AA",
			    n_node, n_node, obj->nodes[n_node].val, obj->nodes[n_node].prev, obj->nodes[n_node].next);
	}

	fprintf(tmp_file, "{\n"
    				  "node[pin=true];\n"
    				  "edge[style=invis];\n");

	for(int n_node = 0; n_node < obj->capacity - 1; n_node++){
		fprintf(tmp_file, "%d->", n_node);
	}

	fprintf(tmp_file, "%d;\n}\n\n", obj->capacity - 1);

	for(int n_node = obj->head; obj->nodes[n_node].next != 0; n_node = obj->nodes[n_node].next){
		fprintf(tmp_file, "%d->%d;\n", n_node, obj->nodes[n_node].next);
	}

	for(int n_node = obj->head_free; obj->nodes[n_node].next != 0; n_node = obj->nodes[n_node].next){
		fprintf(tmp_file, "%d->%d[color=\"blue\"];\n", n_node, obj->nodes[n_node].next);
	}

	fprintf(tmp_file, "\n}\n");
	fclose(tmp_file);

	char* cmd_str = (char*)calloc(CMD_STR_SIZE, sizeof(char));

	// TODO: придумать как исправить этот костыль
	sprintf(cmd_str, "dot -Tsvg %s -o %s", GVIZ_DOT_NAME, GVIZ_HTM_NAME);
	WinExec(cmd_str, 0);
	Sleep(80);
	// КОСТЫЛЬ

    int buflen = get_file_size(GVIZ_HTM_NAME);

	tmp_file = fopen(GVIZ_HTM_NAME, "r");
 	
	char buffer[buflen];

	fread(buffer, sizeof(char), buflen, tmp_file);

	fclose(tmp_file);
	to_log(buffer);

	return;
}

int get_file_size(const char* name){
	
	assert(name != NULL);

	struct stat st;
    stat(name, &st);

    return st.st_size;	
}
