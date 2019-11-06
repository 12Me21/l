#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <gc.h>

#define DIM(array) sizeof(array)/sizeof((array)[0])

#define when(x) break;case x
#define otherwise break;default

typedef double Number;
typedef int Address; //address within bytecode
typedef char Boolean;
typedef uint32_t BucketIndex;
typedef int VarIndex;
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
	Op_push, // .value
	Op_print,
	Op_halt,
	Op_jump, // .address
	Op_discard,
	Op_add, //
	Op_push_local, // .varindex
	Op_push_nonlocal, // .varindex
	Op_push_function_literal, // .function
	Op_return,
} Opcode;

typedef struct Instruction {
	Opcode opcode;
	union {
		Value value; //biggest
		Address address;
		VarIndex varindex;
		struct Function *function;
	};
} Instruction;

// Reference to nonlocal variable used in Function
// level: relative position in the level stack
// index: index in local variable array in its host function
typedef struct Nonlocal {
	VarIndex index;
	int level;
} Nonlocal;

// Function for literal
typedef struct FunctionDef {
	VarIndex localc; // number of local vars
	VarIndex argc; // number of local vars that are arguments
	Instruction *code;
	Nonlocal *nonlocals;
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
void Callstack_push(Address n);
Address Callstack_pop(void);
Variable **Varstack_enter(VarIndex c);
void Varstack_leave(VarIndex c);

//Operators:
void Value_add(Value *dest, Value *a, Value *b);

//Variable:
Variable *Variable_new(Value *value, Function *validator);

