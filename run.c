#include "h.h"

Value stack[1024];
int stack_len = 0;

void push(Value *value){
	stack[stack_len++] = *value;
}

Value *pop(){
	if(stack_len<=0){
		puts("Stack underflow");
		exit(1);
	}
	return &stack[--stack_len];
}

jmp_buf Error_jump;
char *Error_message = "<no message>";

void Run_init(Instruction *bytecode){
	switch(setjmp(Error_jump)){
	when(0):
		run(bytecode, 0);
	when(1): //runtime error
		puts("Runtime error:");
		puts(Error_message);
	when(2): //program ended
		puts("^^ Finished. ^^");
	}
}

// bytecode must be a pointer to the START of the bytecode array
// It *looks* like you could call this with a pointer, to, say, a function, for recursion, but
// then addresses would be... hmm wait
// actually that might work
// NO it wouldn't because a function still has addresses of OTHER functions
// Which are all absolute !
void run(Instruction *bytecode, Address ip){
	while(1){
		switch(bytecode[ip].opcode){
		when(Op_push):;
			push(&bytecode[ip].value);
		when(Op_print):;
			Value_print(pop());
		when(Op_halt):
			longjmp(Error_jump, 2);
		otherwise:
			Error_message = "Internal error: Invalid opcode";
			longjmp(Error_jump, 1);
		}
		ip++;
	}
exit_run:;
}
