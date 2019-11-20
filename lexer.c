#include "h.h"
#include <ctype.h>

static bool stored;
static Token next;
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

void Token_finish(){
	while(nametable){
		Nametable *next = nametable->next;
		free(nametable);
		nametable = next;
	}
}

// look at the next token without eating it
void Token_peek(Token *t){
	Token_next(&next);
	stored = true;
	*t = next;
}

#define isword(c) (isalnum(c) || (c)=='_')
#define iswordstart(c) (isalpha(c) || (c)=='_')

static char name[256];
static int name_length;

void name_start(){
	name_length = 0;
}

void name_read(char c){
	name[name_length++] = c; //"so are you doing anything about that potential buffer overflow in name or"
}

// Memory notes:
// It is the responsibility of the parser to
// handle freeing of the name strings :) The
// tokenizer will only free the linked list.
void tokennametable1(char *name, Token *t){
	Nametable *curr = nametable;
	t->type = Token_name;
	while (curr) {
		if (!strcmp(curr->name, name)) {
			t->first = false;
			t->name = curr->name;
		}
		curr = curr->next;
	}
	name = strdup(name);
	curr = malloc(sizeof(Nametable));
	curr->next = nametable;
	curr->name = name;
	if (!nametable)
		nametable = curr;
	t->first = true;
	t->name = name;
}

// eat tokennnn
void Token_next(Token *t){
	if (stored){
		stored = false;
		*t = next;
		return;
	}
	while (c==' ' || c=='\t')
		scan();
	if (iswordstart(c)) {
		name_start();
		do {
			name_read(c);
			scan();
		} while (isword(c));
		name_read(0);
		tokennametable1(name, t);
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
		token->type = Token_value;
		token->value.type = Type_number;
		token->value.class = NULL;
		token->value.number = atof(name);
	} else if (c == '.') {
		scan();
		if (isdigit(c)) {
			name_start();
			name_read('.');
			goto read_decimal;
		} else {
			// idk
			token->type = Token_error;
			token->error = ". unimplemented?";
		}
	} else if (c == '?') {
		token->type = Token_print;
		scan();
	} else {
		token->type = Token_error;
		token->error = "Invalid character"; //these should be error id nums >
		scan();
	}
}
