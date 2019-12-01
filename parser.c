#include "h.h"

extern Token token;

static jmp_buf Parse_error;

void Parse_start_string() {
	
}

void Parse_start_file(FILE *f) {
	Token_start_file(f);
	Parse_start();
}

static Token t;

void Parse_start() {
	Rpn_start();
	Token_next();
	switch(setjmp(Parse_error)){
	when(0):
		Parse();
	otherwise:
		puts("parse error :(");
	}
}

// parse an expression
bool Parse_expression(FunctionDef *func) {
	if (token.type == Token_linebreak)
		Token_next();
	switch (token.type) {
	// open paren
	when(Token_lparen):
		Rpn_group();
		Parse_expression();
		if (!Token_try(Token_rparen)) {
			longjmp(Parse_error);
		}
		Rpn_flush();
	// prefix operator
	when(Token_op1):
	case Token_op12:
		Rpn_op(token.op1);
	when(Token_value):
		Rpn_value(token.value);
	when(Token_name):
		
	}
}

// parse a function
void Parse() {
	FunctionDef func;
	Instruction bytecode[1000];
	Address ip = 0;
	func.code = bytecode;
	
	while(1){
		switch (token.type) {
		when(Token_error):
			longjmp(Error_jump, 1);
		when(Token_print):
			Token_next(t);
			Parse_expression();
			bytecode[ip++] = Op_print;
			
		when(Token_name):
		case Token_value:
		case Token_op1:
		case Token_op12:
			Parse_expression(bytecode);
			bytecode[ip++] = Op_discard;
		
			
			
			
		when(Token_linebreak):
			Token_next();
		}
	}
}

void Parse_expression(bytecode){
	switch (t.type) {
	when(Token_op1):
	case Token_op12:
		//
		when(Token_
	}
	
}
