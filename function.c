#include "h.h"

//######################
// Local Variable Stack

static Variable *var_stack[1024]; //stack for variables
static VarIndex var_stack_len = 0;

static Variable **level_stack[256];
// This stores pointers to the variable stack
// each item in this is a "depth"
// each FunctionDef has a level which depends on its position in the code
// the global function is level 0
// all functions directly inside that have level 1, etc.
// Functions with higher levels can access variables from outer functions
// and this stack is used to find these nonlocal variables in var_stack

//not sure this should even be a function
static Variable *Nonlocal_lookup(FunctionDef *def, Nonlocal ind) {
	return level_stack[def->level-ind.level][ind.index];
}

// when entering a function, create local variables
Variable **Function_enter(FunctionDef *def) {
	VarIndex c = def->localc;
	VarIndex i;
	Variable **start = var_stack + var_stack_len;
	level_stack[def->level] = start;
	for (i=0; i<c; i++) {
		var_stack[var_stack_len] = Variable_new(NULL, NULL);
		var_stack_len ++;
	}
	return start;
}

// When exiting a function
// You must pass the same `c` value that was passed to _enter
// (maybe in the future I should keep track of that automatically)
void Function_leave(FunctionDef *def) {
	VarIndex c = def->localc;
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

void Varstack_reset(void){
	var_stack_len = 0;
}

static void Function_fin(GC_PTR obj, GC_PTR cd) {
	(void)cd;
	GC_FREE(((Function *)obj)->nonlocals);
}

Function *Function_create(FunctionDef *def) {
	Function *new = GC_MALLOC(sizeof(Function));
	
	new->function = def;
	VarIndex nonlocalc = def->nonlocalc;
	// get nonlocals
	if (nonlocalc) {
		new->nonlocals = GC_MALLOC_UNCOLLECTABLE(sizeof(Variable *) * nonlocalc);
		GC_register_finalizer(new, Function_fin, NULL, NULL, NULL);
		VarIndex i;
		for (i=0; i<nonlocalc; i++)
			new->nonlocals[i] = Nonlocal_lookup(def, def->nonlocals[i]);
	} else
		new->nonlocals = NULL;
	return new;
}

