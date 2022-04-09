#include <stdio.h>
#include "list.h"

int main(){
	
	open_log_file(LOG_FILE_NAME);

	list lst = {};

	ListConstructor(&lst, 10);

	for(int i = 0; i < 6; i++){
		PushBack(&lst, i + 1);
	}
	close_log_file();
	return 0;
}
