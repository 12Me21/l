#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <gc.h>


#define when(x) break;case x
#define otherwise break;default

typedef double Number;
typedef int Address; //address within bytecode
typedef char Boolean;
typedef uint32_t BucketIndex;
//enum = int = too many memory
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

typedef struct Function {
	Address address;
	// TODO: closure stuff
} Function;

struct TableNode;

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
		Function *function;
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
	Function *validator;
	Value value; //Not a pointer!
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

#define Value_number(number1, class1) (Value){.type=Type_number, .class=(class1), .number=(number1)}
void Value_string_copy(Value *dest, Value *value);

//###################
// instructions
typedef enum Opcode {
	Op_invalid,
	Op_push,
	Op_print,
	Op_halt,
} Opcode;

typedef struct Instruction {
	Opcode opcode;
	union {
		Value value;
		Address address;
	};
} Instruction;

void run(Instruction *bytecode, Address ip);

void assemble(FILE *file, Instruction *bytecode);
void disassemble(Instruction *bytecode);

void Run_init(Instruction *bytecode);
