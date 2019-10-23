#include "h.h"

void Value_add(Value *dest, Value *a, Value *b){
	if(a->type != Type_number || b->type != Type_number){
		Error_message = "Type mismatch in add";
		longjmp(Error_jump, 1);
	}
	*dest = Value_number(a->number + b->number, NULL);
}
