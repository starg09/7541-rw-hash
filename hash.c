#include "lista.h"
#include "vector_dinamico.h"
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define LARGO_INICIAL 27
#define VARIACION 2
#define UMBRAL 70

//Hash elegido: Abierto

struct hash {
	vector_t* vector;
	size_t cantidad;
	hash_destruir_dato_t func_destruct;
};

typedef struct nodo_hash {
	char* clave;
	void* dato;
} nodo_hash_t;

struct hash_iter {
	const hash_t* hash;
	lista_iter_t* act;
	size_t pos_actual;
	size_t recorridos;
};

//Duplica una clave, o devuelve NULL de no poder hacerlo.
char* strdup (const char* s) {
	char* dup = malloc(strlen (s) + 1);
	if (dup == NULL)
		return NULL;
	strcpy (dup,s);
	return dup;
}

// Método de hasheo elegido. Se suman todos los valores del string hasta el octavo caracter
// (o el último de medir la clave menos), y se devuelve el módulo de esa suma con el tamaño del vector que almacena los valores.
unsigned int hashear_clave(const hash_t* hash, const char* clave){

	unsigned int num, tam, tempval;
	int i;
	
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsign-conversion" // Igual va a ser positivo.
	
	num = 0;
	tam = vector_obtener_tamanio(hash->vector);
	i = 0;
	
	while ((i < 50) && (i < strlen(clave))){
		tempval = (clave[i] < 0) ? -clave[i] : clave[i];
		num += tempval;
		i++;
	}
	#pragma GCC diagnostic pop

	return num % tam;
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

	unsigned int pos = hashear_clave(hash, clave);

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
			if (strcmp(nodo_actual->clave, clave) == 0){
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

	unsigned int pos = hashear_clave(hash, clave);

	if (hash->vector->datos[pos] == NULL){
		
		hash->vector->datos[pos] = lista_crear();
		
		if (hash->vector->datos[pos] == NULL){
			return false;
		}


		nodo_hash_t* nuevo_dato = malloc(sizeof(nodo_hash_t));
		if (nuevo_dato == NULL)
			return false;


		nuevo_dato->clave = strdup(clave);
		if (nuevo_dato->clave == NULL){
			free(nuevo_dato);
			return false;
		}


		nuevo_dato->dato = dato;


		if (lista_insertar_ultimo(hash->vector->datos[pos], nuevo_dato)){
			hash->cantidad++;
			return true;
		} else {
			free(nuevo_dato->clave);
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
			if (strcmp(nodo_actual->clave, clave) == 0){
				encontrado = true;
				if (hash->func_destruct != NULL)
					hash->func_destruct(nodo_actual->dato);
				nodo_actual->dato = dato;
				lista_iter_destruir(iter);
				return true;
			} else {
				lista_iter_avanzar(iter);
			}
		}
		
		lista_iter_destruir(iter);

		//Esto ocurre solo si no encontró al elemento

		nodo_hash_t* nuevo_dato = malloc(sizeof(nodo_hash_t));
		if (nuevo_dato == NULL)
			return false;


		nuevo_dato->clave = strdup(clave);
		if (nuevo_dato->clave == NULL){
			free(nuevo_dato);
			return false;
		}


		nuevo_dato->dato = dato;


		if (lista_insertar_ultimo(hash->vector->datos[pos], nuevo_dato)){
			hash->cantidad++;
			return true;
		} else {
			free(nuevo_dato);
			return false;
		}
	}
}

void *hash_borrar(hash_t *hash, const char *clave){
	if (hash->cantidad == 0)
		return NULL;

	unsigned int pos = hashear_clave(hash, clave);

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
			if (strcmp(nodo_actual->clave, clave) == 0){
				nodo_hash_t* elem = (lista_borrar(hash->vector->datos[pos], iter));
				lista_iter_destruir(iter);
				void* dato = elem->dato;
				free(elem->clave);
				free(elem);
				hash->cantidad--;
				if (lista_esta_vacia(hash->vector->datos[pos])){
					lista_destruir(hash->vector->datos[pos], NULL);
					hash->vector->datos[pos] = NULL;
				}
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
	if (hash->cantidad == 0)
		return NULL;

	unsigned int pos = hashear_clave(hash, clave);

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
			if (strcmp(nodo_actual->clave, clave) == 0){
				lista_iter_destruir(iter);
				return nodo_actual->dato;
			} else {
				lista_iter_avanzar(iter);
			}
		}
		lista_iter_destruir(iter);
		return NULL;
	}
}

//Avanza el iterador del hash a través del vector, hasta encontrar o bien ĺa próxima lista existente, o bien el final.
bool hash_iter_avance_interno(hash_iter_t* iter, size_t* i, lista_iter_t** tempact){
	while ( ( *tempact == NULL ) && ( *i < ( iter->hash->vector->tam ) ) ) {
		if (iter->hash->vector->datos[*i] != NULL){
			*tempact = lista_iter_crear(iter->hash->vector->datos[*i]);
			if (*tempact == NULL)
				return false;
			if (lista_iter_al_final(*tempact)) {
				lista_iter_destruir(*tempact);
				*tempact = NULL;
			}
		} else {
			(*i)++;
		}
	}
	return true;
}

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* hiter = malloc(sizeof(hash_iter_t));
	if (hiter == NULL)
		return NULL;

	//Valores caso base, hash vacío
	hiter->hash = hash;
	hiter->act = NULL;
	hiter->pos_actual = 0;
	hiter->recorridos = 0;

	//Se inicializa de tener elementos
	if (hash->cantidad > 0) {
		size_t i = 0;
		lista_iter_t* tempact = NULL;

		if (!hash_iter_avance_interno(hiter, &i, &tempact)){
			free(hiter);
			return NULL;
		} else {
			hiter->act = tempact;
			hiter->pos_actual = i;
			hiter->recorridos++;
		}
	}

	return hiter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if (hash_iter_al_final(iter))
		return false;
	if (!lista_iter_avanzar(iter->act))
		return false;
	iter->recorridos++;
	if (lista_iter_al_final(iter->act)) {

		size_t i = iter->pos_actual + 1;
		lista_iter_t* tempact = NULL;

		if (hash_iter_avance_interno(iter, &i, &tempact)){
			lista_iter_destruir(iter->act);
			iter->act = tempact;
			iter->pos_actual = i;
			return true;
		} else {
			return false;
		}
	}
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if (hash_iter_al_final(iter)) {
		return NULL;
	} else {
		lista_iter_t* iter_act = iter->act;
		nodo_hash_t* nodo_act = lista_iter_ver_actual(iter_act);
		return nodo_act->clave;
	}
}

bool hash_iter_al_final(const hash_iter_t *iter){
	if ((iter->hash->cantidad == 0) || (iter->hash->cantidad < iter->recorridos))
		return true;
	return false;
}

void hash_iter_destruir(hash_iter_t* iter){
	if (iter->act != NULL)
		lista_iter_destruir(iter->act);
	free(iter);
}




void hash_destruir(hash_t *hash){
	size_t elems_a_borrar = hash->cantidad;
	lista_t* lista_actual;
	nodo_hash_t* elem_actual;
	int i = 0;
	while ( ( elems_a_borrar > 0 ) && ( i < (hash->vector->tam) ) ) {
		lista_actual = hash->vector->datos[i];
		if (lista_actual != NULL){
			while (!lista_esta_vacia(lista_actual)){
				elem_actual = lista_borrar_primero(lista_actual);
				free(elem_actual->clave);
				if (hash->func_destruct != NULL)
					hash->func_destruct(elem_actual->dato);
				free(elem_actual);
				elems_a_borrar--;
			}
			lista_destruir(lista_actual, NULL); //No uso solo esto con una función auxiliar así puedo detectar si ya borré todos los elementos.
		}
		i++;
	}
	vector_destruir(hash->vector);
	free(hash);
}