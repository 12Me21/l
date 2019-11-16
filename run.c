#include "h.h"

void Run_init(Function *func){
	Stack_reset();
	switch(setjmp(Error_jump)){
	when(0):
		run(func);
	when(1): //runtime error
		puts("Runtime error:");
		puts(Error_message);
	when(2):; //program ended
		//puts("^^ Finished. ^^");
	}
	Stack_reset();
	Varstack_reset();
	GC_gcollect();
}

// for parsing: just read the function code into the main Instructions buffer,
// Then at the end of a function, copy that function's data into a new allocated array, and then return the instruction insert pointer to the start of the newly finished function, and continue.!

// Execute a function
// supports recursion
// Error_jump should be set to a point before the first call to this
// consider adding support for exceptions
void run(Function *function){
	
	Address ip = 0;
	Instruction *bytecode = function->function->code;
	Variable **locals = Function_enter(function->function);
	Variable **nonlocals = function->nonlocals;
	while(1){
		Instruction inst = bytecode[ip];
		switch(inst.opcode){
		when(Op_push):;
			Value_copy(Stack_push(), &inst.value);
		when(Op_print):;
			Value_print(Stack_pop());
		when(Op_halt):
			longjmp(Error_jump, 2);
		when(Op_jump):
			ip = inst.address-1;
		when(Op_discard):
			Stack_pop();
		when(Op_add):;
			Value *a = Stack_pop();
			Value *b = Stack_pop();
			Value_add(Stack_push(), a, b);
		when(Op_push_local):;
			*(Stack_push()) = locals[inst.varindex]->value;
		when(Op_push_nonlocal):;
			*(Stack_push()) = nonlocals[inst.varindex]->value;
		when(Op_return):;
			goto func_return;
		when(Op_assign):;
			a = Stack_pop();
			Variable_assign(Stack_pop()->variable, a);
		when(Op_create_function):;
			Function *func = Function_create(inst.function);
			*(Stack_push()) = (Value){.class=NULL, .type=Type_function, .variable=NULL, .function=func};
		when(Op_call_function):; //no args yet
			a = Stack_pop();
			if (a->type != Type_function){
				Error_message = "Tried to call something that isn't a function";
				longjmp(Error_jump, 1);
			}
			run(a->function);
		when(Op_get_method):;
			// - get metatable
			// - look up field
			// - if function, set `this` upvalue maybe
			// - push
		otherwise:
			Error_message = "Internal error: Invalid opcode";
			longjmp(Error_jump, 1);
		}
		ip++;
	}
func_return:
	Function_leave(function->function);
}
