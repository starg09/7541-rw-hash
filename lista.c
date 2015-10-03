#include "lista.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct nodo {
	struct nodo* proximo;
	void* dato;
} nodo_t;

struct lista {
	nodo_t* primero;
	nodo_t* ultimo;
	size_t largo;
};

// Primitivas de lista

lista_t* lista_crear(void){
	lista_t* lista = malloc(sizeof(lista_t));
	if (!lista)
		return NULL;
	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->largo = 0;
	return lista;
}

bool lista_esta_vacia(const lista_t *lista){
	return (lista->primero == NULL) ? true : false;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nuevo_elem = malloc (sizeof(nodo_t));
	if (nuevo_elem == NULL)
		return false;
	nuevo_elem->dato = dato;
	if (lista_esta_vacia(lista)){
		nuevo_elem->proximo = NULL;
		lista->ultimo = nuevo_elem;
	} else {
		nuevo_elem->proximo = lista->primero;
	}
	lista->primero = nuevo_elem;
	lista->largo++;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t* nuevo_elem = malloc (sizeof(nodo_t));
	if (nuevo_elem == NULL)
		return false;
	nuevo_elem->dato = dato;
	nuevo_elem->proximo = NULL;

	if (lista_esta_vacia(lista)){
		lista->primero = nuevo_elem;
	} else{
		nodo_t* ult = lista->ultimo;
		ult->proximo = nuevo_elem;
	}
	lista->ultimo = nuevo_elem;
	lista->largo++;
	return true;
}

void *lista_borrar_primero(lista_t *lista){
	if (lista_esta_vacia(lista))
		return NULL;
	nodo_t* elem = lista->primero;
	void* dato = elem->dato;
	lista->primero = elem->proximo;
	if (lista->ultimo == elem)
		lista->ultimo = elem->proximo;
	free(elem);
	lista->largo--;
	return dato;
}

void *lista_ver_primero(const lista_t *lista){
	if (lista_esta_vacia(lista))
		return NULL;
	return lista->primero->dato;
}

size_t lista_largo(const lista_t *lista){
	return lista->largo;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	void* acceso_dato;
	while (!lista_esta_vacia(lista)){
		acceso_dato = lista_borrar_primero(lista);
		if (destruir_dato != NULL)
			destruir_dato(acceso_dato);
	}

	free (lista);
}

// Definición del struct del iterador

struct lista_iter {
	nodo_t* actual;
	nodo_t* anterior;
};

// Primitivas del iterador

lista_iter_t *lista_iter_crear(const lista_t *lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (iter == NULL)
		return NULL;
	iter->actual = lista->primero;
	iter->anterior = NULL;
	return iter;
}

bool lista_iter_al_final(const lista_iter_t *iter){
	if (iter->actual == NULL)
		return true;
	return false;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if (lista_iter_al_final(iter))
		return false;
	iter->anterior = iter->actual;
	iter->actual = iter->actual->proximo;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
	if (lista_iter_al_final(iter))
		return NULL;
	return iter->actual->dato;
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

bool lista_insertar(lista_t *lista, lista_iter_t *iter, void *dato){
	nodo_t* nuevo_elem = malloc(sizeof(nodo_t));
	if (nuevo_elem == NULL)
		return false;

	nuevo_elem->dato = dato;
	nuevo_elem->proximo = iter->actual;

	if (lista_esta_vacia(lista)){
		lista->primero = nuevo_elem;
		lista->ultimo = nuevo_elem;
		iter->anterior = NULL;
	} else {
		if (lista_iter_al_final(iter)){
			iter->anterior = lista->ultimo;
			iter->anterior->proximo = nuevo_elem;
			lista->ultimo = nuevo_elem;
		} else if (iter->anterior != NULL){
			iter->anterior->proximo = nuevo_elem;
		} else {
			lista->primero = nuevo_elem;
		}
	}
	iter->actual = nuevo_elem;

	lista->largo++;
	return true;
}

void *lista_borrar(lista_t *lista, lista_iter_t *iter){
	if (lista_iter_al_final(iter))
		return NULL;
	nodo_t* elem = iter->actual;
	void* dato = elem->dato;

	if (elem == lista->primero)
		lista->primero = elem->proximo;
	else
		iter->anterior->proximo = elem->proximo;

	if (elem == lista->ultimo)
		lista->ultimo = iter->anterior;


	iter->actual = elem->proximo;

	free(elem);
	lista->largo--;
	return dato;

}

// Primitivas de iterador interno

void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra){
	bool seguir = true;
	lista_iter_t* iter = lista_iter_crear(lista);
	while ((seguir) && (!lista_iter_al_final(iter))){
		seguir = visitar(iter->actual->dato, extra);
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
}