#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef double Number;
typedef int Address; //address within bytecode
typedef char Boolean;
typedef uint32_t BucketIndex;
typedef enum Type {
	Type_number, Type_string, Type_function, Type_table, Type_boolean, Type_null
} Type;

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

typedef struct Value {
	Type type;
	Table *class; //not sure...
	union {
		Number number;
		String *string;
		Function *function;
		Table *table;
		Boolean boolean;
	};
} Value;

typedef struct Variable {
	Function *validator;
	Value *value;
} Variable;

typedef struct TableNode {
	Value *key;
	Variable *variable;
	struct TableNode *bnext; //next item in bucket
	struct TableNode *snext; //next item in sorted table
	struct TableNode *sprev; //prev item in sorted table
} TableNode;

Table *Table_new(BucketIndex size);
TableNode *Table_add(Table *tb, Value *key, Variable *var);
TableNode *Table_get(Table *tb, Value *key);
void Value_print(Value *value);
