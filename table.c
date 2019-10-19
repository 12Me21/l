#include "h.h"

static BucketIndex hash_pointer(void *ptr){
	return (size_t)ptr;
}

static BucketIndex Table_hash(Value *value){
	Type type = value->type;
	if(type == Type_number)
		return value->number;
	if(type == Type_string){
		unsigned int i;
		BucketIndex hash = 5381;
		for(i=0;i<value->string->length;i++)
			hash = hash*33 + value->string->string[i];
		return hash;
	}
	if(type == Type_boolean)
		return value->boolean ? 69420 : 14522;
	if(type == Type_function)
		return hash_pointer(value->function);
	if(type == Type_table)
		return hash_pointer(value->table);
	if(type == Type_null)
		return 10112019;
	exit(1);
}

//compare values for table keys
static int Value_compare(Value *v1, Value *v2){
	Type type = v1->type;
	if (type != v2->type)
		return 0;
	if (v1->class != v2->class)
		return 0;
	if (type == Type_number)
		return v1->number == v2->number;
	if (type == Type_string)
		return v1->string->length == v2->string->length && !memcmp(v1->string->string, v2->string->string, v1->string->length);
	if (type == Type_boolean)
		return v1->boolean == v2->boolean;
	if (type == Type_null)
		return 1;
	if (type == Type_function)
		return v1->function == v2->function;
	if (type == Type_table)
		return v1->table == v2->table;
	exit(1);
}

Table *Table_new(BucketIndex size){
	Table *new = GC_MALLOC(sizeof(Table));
	new->shead = NULL;
	new->size = size;
	new->buckets = GC_MALLOC(size * sizeof(TableNode *)); //should be handled by Table finalizer
	BucketIndex i;
	for(i=0;i<size;i++)
		new->buckets[i] = NULL;
	return new;
}

TableNode *Table_get(Table *tb, Value *key){
	BucketIndex index = Table_hash(key) % tb->size; //someday I'm going to forget to % ...
	TableNode *bucket = tb->buckets[index];
	while(bucket){
		if (Value_compare(&bucket->key, key))
			return bucket;
		bucket = bucket->bnext;
	}
	return NULL;
}

TableNode *Table_add(Table *tb, Value *key, Variable *var){
	BucketIndex index = Table_hash(key) % tb->size;
	TableNode *bucket = tb->buckets[index];	
	while(bucket){
		if (Value_compare(&bucket->key, key)){
			bucket->variable = var;
			return bucket;
		}
		bucket = bucket->bnext;
	}
	bucket = GC_MALLOC(sizeof(TableNode)); // should be handled by finalizer
	bucket->bnext = tb->buckets[index];
	if (key->type != Type_string)
		bucket->key = *key;
	else {
		//Strings are special, as they are both
		//mutable, AND compared by value, so...
		//must be copied, otherwise it would be
		//possible to modify a key in a table,:
		//causing it to be in the wrong bucket!
		Value_string_copy(&bucket->key, key);
	}
	bucket->variable = var;
	tb->buckets[index] = bucket;
	if(!(tb->shead)){
		tb->shead = bucket;
		bucket->snext = NULL;
		bucket->sprev = bucket;
	}else{
		bucket->sprev = tb->shead->sprev;
		bucket->snext = NULL;
		tb->shead->sprev->snext = bucket;
		tb->shead->sprev = bucket;
	}
	return bucket;
}

TableNode *Table_remove(Table *tb, Value *key){
	BucketIndex index = Table_hash(key) % tb->size;
	TableNode *prev;
	TableNode *node = tb->buckets[index];
	while(node){
		if (Value_compare(&node->key, key))
			goto found;
		prev = node;
		node = node->bnext;
	}
	return NULL;
found:
	// remove node from bucket:
	if (prev) //is not first node
		prev->bnext = node->bnext;
	else //first
		tb->buckets[index] = node->bnext;

	// Remove node from sorted list
	
	if(node == tb->shead){ //is the first node
		tb->shead = node->snext;
		// update pointer to last node
		if(node->snext) //not the last node
			node->snext->sprev = node->sprev;
	}else{ //not first node
		node->sprev->snext = node->snext;
		
		if (node->snext) //is not last node
			node->snext->sprev = node->sprev;
		else //last node
			tb->shead->sprev = node;
	}
	return node;
}
