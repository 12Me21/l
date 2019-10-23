#include "h.h"

void Run_init(Instruction *bytecode){
	Stack_reset();
	switch(setjmp(Error_jump)){
	when(0):
		run(bytecode, 0);
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
void run(Instruction *bytecode, Address ip){
	while(1){
		switch(bytecode[ip].opcode){
		when(Op_push):;
			Value_copy(Stack_push(), &bytecode[ip].value);
		when(Op_print):;
			Value_print(Stack_pop());
		when(Op_halt):
			longjmp(Error_jump, 2);
		when(Op_jump):
			ip = bytecode[ip].address-1;
		when(Op_discard):
			Stack_pop();
		when(Op_add):;
			Value *a = Stack_pop();
			Value *b = Stack_pop();
			Value_add(Stack_push(), a, b);
		when(Op_variable):;
			*(Stack_push()) = Variable_new(NULL, NULL)->value;
		otherwise:
			Error_message = "Internal error: Invalid opcode";
			longjmp(Error_jump, 1);
		}
		ip++;
	}
}
