// For the management of stacks
#include "h.h"

//#############
// Value Stack

static Value stack[1024];
static unsigned stack_len = 0;

Value *Stack_push(void){
	if(stack_len >= DIM(stack)){
		Error_message = "Stack overflow";
		longjmp(Error_jump, 1);
	}
	return &stack[stack_len++];
}

Value *Stack_pop(void){
	if(stack_len<=0){
		Error_message = "Internal error: Stack underflow";
		longjmp(Error_jump, 1);
	}
	return &stack[--stack_len];
}

void Stack_reset(void){
	memset(stack, 0, sizeof stack);
	stack_len = 0;
}

//############
// Call Stack

static Address callstack[256];
static unsigned callstack_len = 0;

void Callstack_push(Address n){
	if(callstack_len >= DIM(stack)){
		Error_message = "Call stack overflow";
		longjmp(Error_jump, 1);
	}
	callstack[callstack_len++] = n;
}

Address Callstack_pop(void){
	if(stack_len<=0){
		Error_message = "Internal error: Call stack underflow";
		longjmp(Error_jump, 1);
	}
	return callstack[--callstack_len];
}

// note: be careful when using malloc instead of GC_malloc
// because I think pointers inside these objects will not be tracked
// if something has pointers and should not be GC, I think there's a GC_ function for that
