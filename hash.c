#include "lista.h"
#include "vector_dinamico.h"
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LARGO_INICIAL 27
#define VARIACION 2
#define UMBRAL 70
#define LARGO_MAX_CLAVE 50

//Hash elegido: Abierto

struct hash {
	vector_t* vector;
	size_t cantidad;
	hash_destruir_dato_t func_destruct;
};

typedef struct nodo_hash {
	char clave[LARGO_MAX_CLAVE];
	void* dato;
} nodo_hash_t;

struct hash_iter {
	lista_iter_t* act;
	size_t pos_actual;
};

// Método de hasheo elegido. Se suman todos los valores del string hasta el octavo caracter
// (o el último de medir la clave menos), y se devuelve el módulo de esa suma con el tamaño del vector que almacena los valores.
int hashear_clave(const hash_t* hash, char* clave){
	int num = 0, i, len, tam;

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsign-conversion" // Igual se va a hacer positivo despues
	len = strlen(clave);
	tam = vector_obtener_tamanio(hash->vector);

	for (i=0; (i<8) && (i<len); i++){
		num += clave[i];
	}
	#pragma GCC diagnostic pop

	if (num < 0)
		num *= (-1);

	num %= tam;

	return num;
}


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if (hash == NULL)
		return NULL;
	
	vector_t* vector = vector_crear(LARGO_INICIAL);
	if (vector == NULL){
		free (hash);
		return NULL;
	}

	for (int i = 0; i < vector_obtener_tamanio(vector); i++){
		vector->datos[i] = NULL;
	}
	
	hash->vector = vector;
	hash->cantidad = 0;
	hash->func_destruct = destruir_dato;

	return hash;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	if (hash->cantidad == 0)
		return false;

	char clave_truncada[LARGO_MAX_CLAVE];
	strncpy(clave_truncada, clave, LARGO_MAX_CLAVE);
	int pos = hashear_clave(hash, clave_truncada);

	if (hash->vector->datos[pos] == NULL)
		return false;
	else {
		lista_iter_t* iter = lista_iter_crear(hash->vector->datos[pos]);
		if (iter == NULL){
			return false;
		}
		nodo_hash_t* nodo_actual;
		while (!lista_iter_al_final(iter)){
			nodo_actual = lista_iter_ver_actual(iter);
			if (strcmp(nodo_actual->clave, clave_truncada) == 0){
				lista_iter_destruir(iter);
				return true;
			} else {
				lista_iter_avanzar(iter);
			}
		}
		lista_iter_destruir(iter);
		return false;
	}

}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	char clave_truncada[LARGO_MAX_CLAVE];
	strncpy(clave_truncada, clave, LARGO_MAX_CLAVE);
	int pos = hashear_clave(hash, clave_truncada);
	if (hash->vector->datos[pos] == NULL){
		hash->vector->datos[pos] = lista_crear();
		if (hash->vector->datos[pos] == NULL){
			return false;
		}
		nodo_hash_t* nuevo_dato = malloc(sizeof(nodo_hash_t));
		if (nuevo_dato == NULL)
			return false;

		strcpy(nuevo_dato->clave, clave_truncada);
		nuevo_dato->dato = dato;

		if (lista_insertar_ultimo(hash->vector->datos[pos], nuevo_dato)){
			return true;
		} else {
			free(nuevo_dato);
			return false;
		}

	} else {
		lista_iter_t* iter = lista_iter_crear(hash->vector->datos[pos]);
		if (iter == NULL){
			return false;
		}

		bool encontrado = false;
		nodo_hash_t* nodo_actual;

		while ((!encontrado) && (!lista_iter_al_final(iter))){
			nodo_actual = lista_iter_ver_actual(iter);
			if (strcmp(nodo_actual->clave, clave_truncada) == 0){
				encontrado = true;
				nodo_actual->dato = dato;
			} else {
				lista_iter_avanzar(iter);
			}
		}

		if(!encontrado){
			nodo_hash_t* nuevo_dato = malloc(sizeof(nodo_hash_t));
			if (nuevo_dato == NULL){
				lista_iter_destruir(iter);
				return false;
			}
			strcpy(nuevo_dato->clave, clave_truncada);
			nuevo_dato->dato = dato;
			lista_insertar_ultimo(hash->vector->datos[pos], nuevo_dato);
		}
		lista_iter_destruir(iter);
		return true;
	}
}

void *hash_borrar(hash_t *hash, const char *clave){

	char clave_truncada[LARGO_MAX_CLAVE];
	strncpy(clave_truncada, clave, LARGO_MAX_CLAVE);
	int pos = hashear_clave(hash, clave_truncada);

	if (hash->vector->datos[pos] == NULL)
		return NULL;
	else {
		lista_iter_t* iter = lista_iter_crear(hash->vector->datos[pos]);
		if (iter == NULL){
			return NULL;
		}
		nodo_hash_t* nodo_actual;
		while (!lista_iter_al_final(iter)){
			nodo_actual = lista_iter_ver_actual(iter);
			if (strcmp(nodo_actual->clave, clave_truncada) == 0){
				nodo_hash_t* elem = (lista_borrar(hash->vector->datos[pos], iter));
				lista_iter_destruir(iter);
				void* dato = elem->dato;
				free(elem);
				return dato;
			} else {
				lista_iter_avanzar(iter);
			}
		}

		lista_iter_destruir(iter);
		return NULL;
	}
}

void *hash_obtener(const hash_t *hash, const char *clave){
	//...
	return NULL;
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
