#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <setjmp.h>

#include <gc.h>

#define DIM(array) sizeof(array)/sizeof((array)[0])
#define EACH(var, array) ((var)=0; (var)<DIM(array); (var)++)

#define when(x) break;case x
#define otherwise break;default

typedef double Number;
typedef int Address; //address within bytecode
typedef char Boolean;
typedef uint32_t BucketIndex;
typedef int VarIndex;
typedef int Level;
typedef unsigned int StackIndex;
typedef int LineNumber;
//enum = int = too many memory
#define Type_default '\0'
#define Type_number '\1'
#define Type_string '\2'
#define Type_function '\3'
#define Type_table '\4'
#define Type_boolean '\5'
#define Type_null '\6'
typedef char Type;

typedef struct String {
	unsigned int length;
	char *string;
} String;

struct TableNode;

struct Function;

typedef struct Table {
	struct TableNode *shead;
	struct TableNode **buckets;
	BucketIndex size;
} Table;

struct Variable;

//size: 25-32 bytes
typedef struct Value {
	Table *class; //not sure...
	union {
		Number number;
		String *string;
		struct Function *function;
		Table *table;
		Boolean boolean;
	};
	//This is not ideal, and takes up
	//a lot of space. but for now its
	//required for assignment to work
	struct Variable *variable; 
	Type type;
} Value;

typedef struct Variable {
	struct Function *validator;
	Value value; //Not a pointer!
	//Value are never GC: always exist within a variable/the stack/a temporary struct
} Variable;

typedef struct TableNode {
	Value key; //not pointer
	Variable *variable;
	struct TableNode *bnext; //next item in bucket
	struct TableNode *snext; //next item in sorted table
	struct TableNode *sprev; //prev item in sorted table
} TableNode;

Table *Table_new(BucketIndex size);
TableNode *Table_add(Table *tb, Value *key, Variable *var);
TableNode *Table_get(Table *tb, Value *key);
TableNode *Table_remove(Table *tb, Value *key);

void Value_print(Value *value);

String *String_new(char *data, unsigned int length);

#define Value_number(number1, class1) (Value){.type=Type_number, .class=(class1), .number=(number1)}
#define Value_string(data, len, class1) (Value){.type=Type_string, .class=(class1), .string=String_new(data, len)};
void Value_string_copy(Value *dest, Value *value);
String *String_new(char *data, unsigned int length);

//###################
// instructions
typedef enum Opcode {
	Op_invalid,

	//normal ops (these are first to make the rpn precedence list nicer
	Op_add, //------------ x y    | x+y

	// etc.
	Op_push, //-----------        | x
	Op_print, //---------- x      |
	Op_halt, //-----------        |
	Op_jump, //-----------        |
	Op_discard, //-------- x      |

	Op_push_local, //-----        | var
	Op_push_nonlocal, //--        | var
	Op_return, //---------        |
	Op_assign, //--------- var x  |
	Op_create_function, //        | func
	Op_call_function, //-- func   |        //todo: args
	Op_get_method, //----- x name | method
	Op_stack_get, //------        | x

} Opcode;

struct FunctionDef;

typedef struct Instruction {
	Opcode opcode;
	union {
		Value value; //biggest
		Address address;
		VarIndex varindex;
		StackIndex stackindex;
		struct FunctionDef *function;
	};
} Instruction;

// Reference to nonlocal variable used in Function
// level: relative position in the level stack
// index: index in local variable array in its host function
typedef struct Nonlocal {
	VarIndex index;
	Level level; //Relative to current level. 1 = from parent function, 2 = grandparent, etc.
} Nonlocal;

// Function for literal
typedef struct FunctionDef {
	VarIndex localc; // number of local vars
	VarIndex argc; // number of local vars that are arguments
	VarIndex nonlocalc; // number of nonlocal vars
	Instruction *code;
	Nonlocal *nonlocals; // length = `nonlocalc`
	Level level;
	Address *lines; //list of the indexes of the first instruction on each line
	LineNumber first_line;
} FunctionDef;

// Function + upvalues
typedef struct Function {
	FunctionDef *function;
	Variable **nonlocals;
} Function;

void run(Function *function);

void assemble(FILE *file, Instruction *bytecode);
void disassemble(Instruction *bytecode);

void Run_init(Function *);
void Value_copy(Value *dest, Value *value);

// Error handling
extern char *Error_message;
extern jmp_buf Error_jump;

// stack.c
void Stack_reset(void);
Value *Stack_push(void);
Value *Stack_pop(void);
Value *Stack_get(StackIndex);

// function.c
Variable **Function_enter(FunctionDef *def);
void Function_leave(FunctionDef *def);
Function *Function_create(FunctionDef *def);
void Varstack_reset(void);

//Operators:
void Value_add(Value *dest, Value *a, Value *b);

//Variable:
Variable *Variable_new(Value *value, Function *validator);
void Variable_assign(Variable *variable, Value *value);

void Value_check(Value *v);

//Lexer:
typedef unsigned int NameIndex;

typedef enum Token_type {
	Token_error,
	
	Token_name,
	Token_value,
	Token_assign,
	Token_print,
	Token_linebreak,

	Token_lparen,
	Token_rparen,
	
	Token_op1, //prefix op
	Token_op2, //infix op
	Token_op12, //prefix or infix
	// These are all the keyword tokens, _in the same order as_
	// keyword_names in lexer.c
	// DO NOT REORDER THESE
	Token_if, Token_then, Token_endif, Token_else, Token_elseif,
	Token_while, Token_wend, Token_repeat, Token_until,
	Token_for, Token_next,
	
	
} Token_type;

typedef struct Token {
	Token_type type;
	union {
		Value value;
		struct {
			char chr;
			char *error;
		};
		struct {
			char *name; //identical names WILL have == pointers
			bool first;
		};
		Opcode op1;
		Opcode op2;
	};
} Token;

void Token_start_file(FILE *f);
