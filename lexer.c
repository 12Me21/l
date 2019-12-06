#include "h.h"
#include <ctype.h>

Token token;
static bool read_next;
static FILE *file;

static void (*scan); //function to read next char

static short c;

typedef struct Nametable {
	struct nametable *next;
	char *name;
} Nametable;

static Nametable *nametable;

static void scan_file(){
	c = getc(file);
}

void Token_start_file(FILE *f){
	stored = false;
	nametable = NULL;
	file = f;
	scan = scan_file;
	scan();
}

void Token_finish() {
	while (nametable) {
		Nametable *next = nametable->next;
		free(nametable);
		nametable = next;
	}
}

#define isword(c) (isalnum(c) || (c)=='_')
#define iswordstart(c) (isalpha(c) || (c)=='_')

static char name[256];
static int name_length;

void name_start() {
	name_length = 0;
}

void name_read(char c) {
	name[name_length++] = c; //"so are you doing anything about that potential buffer overflow in name or"
}

// These are all the keyword names, _in the same order as_
// Token_type in h.h
// DO NOT REORDER THESE
// also, IF must be first
static const *char keyword_names[] = {
	"if", "then", "endif", "else", "elseif",
	"while", "wend", "repeat", "until", "for", "next",
};

// case insensitive string compare
// first string should be all lowercase
int icase_compare(const char *lower, const char *in) {
	while (*lower && *in){
		if (*lower++ != tolower(*in++))
			return 0;
	}
	return 1;
}

// Memory notes:
// It is the responsibility of the parser to
// handle freeing of the name strings :) The
// tokenizer will only free the linked list.
static void tokennametable1(char *name, Token *t){
	// Check keywords list
	int i;
	for EACH(i, keyword_names)
		if (icase_compare(keyword_names[i], name)){
			t->type = Token_if+i;
			return;
		}
	// Check name table
	Nametable *curr = nametable;
	t->type = Token_name;
	while (curr) {
		if (!strcmp(curr->name, name)) {
			t->first = false;
			t->name = curr->name;
		}
		curr = curr->next;
	}
	// Not found: new identifier
	name = strdup(name);
	curr = malloc(sizeof(Nametable));
	curr->next = nametable;
	curr->name = name;
	if (!nametable)
		nametable = curr;
	t->first = true;
	t->name = name;
}


bool Token_try(Token_type wanted){
	Token_next();
	if (token.type == wanted) {
		read_next = true;
		return true;
	}
	read_next = false;
	return false;
}

// eat tokennnn
void Token_next(void){
	if (!read_next){
		read_next = true;
		return;
	}
	while (c==' ' || c=='\t')
		scan();
	//======
	// Word
	if (iswordstart(c)) {
		name_start();
		do {
			name_read(c);
			scan();
		} while (isword(c));
		name_read(0);
		tokennametable1(name, token);
	//========
	// Number
	} else if (isdigit(c)) {
		name_start();
		do {
			name_read(c);
			scan();
		} while (isdigit(c));
		if (c=='.') {
			name_read(c);
			scan();
			while (isdigit(c)) {
			read_decimal:
				name_read(c);
				scan();
			}
		}
		name_read(0);
		token.type = Token_value;
		token.value.type = Type_number;
		token.value.class = NULL;
		token.value.number = atof(name);
	//==============
	// .Number or .
	} else if (c == '.') {
		scan();
		if (isdigit(c)) {
			name_start();
			name_read('.');
			goto read_decimal;
		} else {
			// idk
			token.type = Token_error;
			token.error = ". unimplemented?";
		}
	//=======
	// Print
	} else if (c == '?') {
		token.type = Token_print;
		scan();
	//======
	// ! !=
	} else if (c == '!') {
		scan();
		if (c == '=') {
			scan();
			token.type = Token_op2;
			token.op2 = Op_not_equal;
		} else {
			token.type = Token_op1;
			token.type = Op_not;
		}
	//======
	// @...
	} else if (c == '@') {
		scan();
		if (isword(c)) {
			name_start();
			name_read('@');
			do {
				name_read(c);
				scan();
			} while (isword(c));
			name_read(0);
			token.type = Token_label;
			tokennametable1(name, token);
		} else {
			token.type = Token_at;
		}
	} else if (c == '(') { scan(); token.type = Token_lparen;
	} else if (c == ')') { scan(); token.type = Token_rparen;
	} else if (c == '{') { scan(); token.type = Token_lbrace;
	} else if (c == '}') { scan(); token.type = Token_rbrace;
	} else if (c == '[') { scan(); token.type = Token_lbracket;
	} else if (c == ']') { scan(); token.type = Token_rbracket;
	} else if (c == '+') { scan(); token.type = Token_op2; token.op2 = Op_add;
	} else if (c == '-') { scan(); token.type = Token_op12; token.op1 = Op_negate; token.op2 = Op_subtract;
	} else if (c == '*') { scan(); token.type = Token_op2; token.op2 = Op_multiply;
	} else if (c == '/') { scan(); token.type = Token_op2; token.op2 = Op_divide;
	} else if (c == '%') { scan(); token.type = Token_op2; token.op2 = Op_remainder;
	} else if (c == '^') { scan(); token.type = Token_op2; token.op2 = Op_exponent;
	//========
	// String
	} else if (c == '"') {
		scan();
		BUFFER_START(string, Letter);
		while (c != '"') { //check for \r?
			if (c == EOF || c == '\n' || c == '\r') {
				token.type = Token_error;
				token.error = "Unclosed string";
				return;
			}
			BUFFER_READ(string, c);
		} //these brackets are required
		scan();
		token.type = Token_value;
		token.value = {
			.class=NULL, .variable=NULL, .type=Type_string,
			.string=String_new_nocopy(BUFFER_FINISH(string), size)
		};
	//=================
	// All other chars
	} else {
		token.type = Token_error;
		token.error = "Invalid character"; //these should be error id nums >
		scan();
	}
}
