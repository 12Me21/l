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

//todo: after popping a value, it is left on the stack
// so it won't be garbage collected
// how to fix?
Value *Stack_pop(void){
	if(stack_len<=0){
		Error_message = "Internal error: Stack underflow";
		longjmp(Error_jump, 1);
	}
	//memset(&stack[stack_len], 0, sizeof(Value)); //clear the value ABOVE the one we're popping
	// this is to prevent pointers staying on the stack forever, preventing GC
	// It's not a perfect fix (there will always be 1 extra item on the stack, because this function returns a pointer to it)
	// But it works for now
	// UM no it doens't. why does the memset break aaa
	stack_len--;
	return &stack[stack_len];
}

void Stack_reset(void){
	memset(stack, 0, sizeof stack);
	stack_len = 0;
}

// note: be careful when using malloc instead of GC_malloc
// because I think pointers inside these objects will not be tracked by the GCer
// if something has pointers and should not be GCed, I think there's a GC_ function for that
