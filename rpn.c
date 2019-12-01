#include "h.h"

int Rpn_prec[] = {
	-14522, // invalid
	1, // a + b
};

Opcode Rpn_stack[1000];
StackIndex Rpn_stack_len;
Instruction *Rpn_out;
Instruction *Rpn_out_start;

// Start an rpn expression
void Rpn_start(Instruction *out) {
	Rpn_out = Rpn_out_start = out; //so this is bad because
	// if the buffer fills up, you can't trigger it to expand, I guess
	// actually, the rpn functions can just call a function in the parser
	// which resizes the buffer and gives you a new pointer (based on offset)
}

// finalize
void Rpn_finish(void){
	//TODO: flush stack + check errors

	//TODO: optimize
}

// push op
void Rpn_op(Opcode op){
	while (Rpn_code_len) {
		Opcode top = Rpn_code[Rpn_code_len-1];
		if (Rpn_prec[top] >= Rpn_prec[op]) {
			Rpn_code_len--;
			Instruction *inst = &(Rpn_code[Rpn_code_len++]);
			inst->type = op;
		} else {
			break;
		}
	}
}

//push val
void Rpn_value(Value *value){
	Instruction *inst = &(Rpn_code[Rpn_code_len++]);
	inst->type = Op_push;
	inst->value = *value;
}
