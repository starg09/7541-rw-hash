#include "lista.h"
#include "vector_dinamico.h"
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>

//Hash elegido: Abierto

struct hash {
	vector_t* vector;
	size_t cantidad;
	hash_destruir_dato_t func_destruct;
};

struct hash_iter {
	lista_iter_t* act;
	size_t pos_actual;
};

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	//...
	return NULL;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	//...
	return false;
}

void *hash_borrar(hash_t *hash, const char *clave){
	//...
	return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	//...
	return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	//...
	return false;
}

size_t hash_cantidad(const hash_t *hash){
	//...
	return 0;
}

void hash_destruir(hash_t *hash){
	//...
	return;
}

hash_iter_t *hash_iter_crear(const hash_t *hash){
	//...
	return NULL;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	//...
	return false;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	//...
	return NULL;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	//...
	return false;
}

void hash_iter_destruir(hash_iter_t* iter){
	//...
	return;
}
