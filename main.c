#include "h.h"

void main(){
	Value testvalue = {.type = Type_null, .class = NULL};
	Value testvalue3 = {.type = Type_boolean, .class = NULL, .boolean = 0};	
	Value testvalue2 = {.type = Type_number, .class = NULL, .number = 3.1416};
	Variable testvar = {.value = &testvalue};
	Variable testvar2 = {.value = &testvalue3};
	Table *test = Table_new(1);
	Table_add(test, &testvalue, &testvar);
	printf("%p\n",Table_get(test, &testvalue));
	Value v2 = {.type = Type_table, .class = NULL, .table = test};
	//Value_print(&testvalue2);
	Table_add(test, &testvalue2, &testvar);
	Table_add(test, &testvalue2, &testvar2);
	Table_add(test, &testvalue3, &testvar);
	Value_print(&v2);
}
