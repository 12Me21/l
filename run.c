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
}

// bytecode must be a pointer to the START of the bytecode array
// It *looks* like you could call this with a pointer, to, say, a function, for recursion, but
// then addresses would be... hmm wait
// actually that might work
// NO it wouldn't because a function still has addresses of OTHER functions
// Which are all absolute !

// Idea to improve efficiency of functions:
// each function is generated in its own instruction array, and function pointers
// are a pointer to this array, rather than an address in the main array.
// This way, we can avoid the GOTO @SKIP <function code> @SKIP trick.
// Which is ugly and may become problematic with the extensive use of functions
// for parsing: just read the function code into the main Instructions buffer,
// Then at the end of a function, copy that function's data into a new allocated array, and then return the instruction insert pointer to the start of the newly finished function, and continue.!
void run(Function *function){
	Address ip = 0;
	Instruction *bytecode = function->function->code;
	VarIndex localc = function->function->localc;
	Variable **locals = Varstack_enter(localc);
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
		otherwise:
			Error_message = "Internal error: Invalid opcode";
			longjmp(Error_jump, 1);
		}
		ip++;
	}
func_return:
	Varstack_leave(localc);
}
