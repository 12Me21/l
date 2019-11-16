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
	if (!variable) {
		Error_message = "Tried to assign to something that isn't a variable";
		longjmp(Error_jump, 1);
	}
	variable->value = *value;
	variable->value.variable = variable;
}
