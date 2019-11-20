#include "h.h"

int main(void){
	GC_INIT();
	/*
	Value testvalue = {.type = Type_null, .class = NULL};
	Value testvalue3 = {.type = Type_boolean, .class = NULL, .boolean = 0};
	Value testvalue2 = Value_number(3.14159265358979, 0);
	Variable testvar = {.value = testvalue};
	Variable testvar2 = {.value = testvalue3};
	Table *test = Table_new(1);
	Table_add(test, &testvalue, &testvar);
	//printf("%p\n",Table_get(test, &testvalue));
	Value v2 = {.type = Type_table, .class = NULL, .table = test};
	//Value_print(&testvalue2);
	Table_add(test, &testvalue2, &testvar);
	Table_add(test, &testvalue2, &testvar2);
	Table_add(test, &testvalue3, &testvar);
	Table_remove(test, &testvalue2);
	Value_print(&v2);
	*/

	//while(1){
	//	String *whatever = String_new("HELLO!", 6);
	//}
	
	puts("vv Running.. vv");
	//while(1)

	Instruction bytecode2[] = {
		//x = true
		{.opcode=Op_push_nonlocal, .varindex=0},
		{.opcode=Op_push, .value={.type=Type_boolean, .boolean=1}},
		{.opcode=Op_assign},
		//x = @
		{.opcode=Op_push_nonlocal, .varindex=0},
		{.opcode=Op_stack_get, .stackindex=0},
		{.opcode=Op_assign},
		//?x
		{.opcode=Op_push_nonlocal, .varindex=0},
		{.opcode=Op_print},

		{.opcode=Op_return}
	};
	Nonlocal nl[1] = {{.index=0,. level=1}};
	FunctionDef ne = {.localc=0, .argc=0, .code=bytecode2, .nonlocals=nl, .level=1, .nonlocalc=1};
	
	Instruction bytecode[] = {
		//{...|...}()
		{.opcode=Op_create_function, .function=&ne},
		{.opcode=Op_call_function},
		//?x
		{.opcode=Op_push_local, .varindex=0},
		{.opcode=Op_print},
		
		{.opcode=Op_return},
	};
	FunctionDef mai = {.localc=1, .argc=0, .code=bytecode, .nonlocals=NULL, .level=0};

	Function *m = Function_create(&mai);
	//Function_enter(&mai);
	//Function *n = Function_create(&ne);
	//while(1)
	Run_init(m);

	FILE *file = fopen("test.prg");
	Token_start_file(file);
	Token_finish();
	
	return 0;
}
