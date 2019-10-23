#include "h.h"

Variable *Variable_new(Value *value, Function *validator){
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


//ok but now, how are functions going to reference variables?
//you can create them, but then how to retrieve references later?
// mmmmmm
