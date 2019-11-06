#include "h.h"

static Variable *Variable_new(Value *value, Function *validator){
	Variable *new = GC_MALLOC(sizeof(Variable));
	if (value)
		new->value = *value;
	else
		new->value.type = Type_default;
	new->validator = validator;
	new->value.variable = new;
	return new;
}

void Variable_assign(Variable *variable, Value *value){
	//if(variable->validator)
		//Stack_push(value);
	variable->value = *value;
	variable->value.variable = variable;
}

//######################
// Local Variable Stack

static Variable *var_stack[1024]; //stack for variables
static VarIndex var_stack_len = 0;

// when entering a function, create local variables
Variable **Varstack_enter(VarIndex c){
	VarIndex i;
	Variable **start = var_stack + var_stack_len;
	for(i=0;i<c;i++){
		var_stack[var_stack_len] = Variable_new(NULL, NULL);
		var_stack_len ++;
	}
	return start;
}

// When exiting a function
// You must pass the same `c` value that was passed to _enter
// (maybe in the future I should keep track of that automatically)
void Varstack_leave(VarIndex c){
	if (var_stack_len < c) {
		Error_message = "Internal error: variable stack underflow";
		longjmp(Error_jump, 1);
	}
	var_stack_len -= c;
	// we can't free vars here because they might be used by closures
	// so they need to be GC
	// (potentially optimization: mark which vars are used in closures, and
	// then this can free the others)
}
