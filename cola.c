#include "cola.h"
#include <stdlib.h>
#include <stdio.h>


typedef struct nodo_cola {
	void* dato;
	struct nodo_cola* proximo;
} nodo_cola_t;


struct cola {
	nodo_cola_t* primero;
	nodo_cola_t* ultimo;
};

//Primitivas

cola_t* cola_crear(void){
	cola_t* cola = malloc(sizeof(cola_t));
	if (cola == NULL)
		return NULL;
	cola->primero = NULL;
	cola->ultimo = NULL;
	return cola;
}

void cola_destruir(cola_t *cola, void destruir_dato(void*)){
	void* acceso_dato;
	while (!cola_esta_vacia(cola)){
		acceso_dato = cola_desencolar(cola);
		if (destruir_dato != NULL)
			destruir_dato(acceso_dato);
	}

	free (cola);
}

bool cola_esta_vacia(const cola_t *cola){
	return (cola->primero == NULL);
}

bool cola_encolar(cola_t *cola, void* valor){
	nodo_cola_t* nuevo_nodo = malloc(sizeof(nodo_cola_t));
	if (nuevo_nodo == NULL)
		return false;
	nuevo_nodo->dato = valor;
	nuevo_nodo->proximo = NULL;
	if (cola->primero == NULL){
		cola->primero = nuevo_nodo;
	}
	if (cola->ultimo != NULL){
		nodo_cola_t* ultimo_nodo = cola->ultimo;
		ultimo_nodo->proximo = nuevo_nodo;
	}
	cola->ultimo = nuevo_nodo;

	return true;
}

void* cola_ver_primero(const cola_t *cola){
	if (cola_esta_vacia(cola)){
		return NULL;
	} else {
		nodo_cola_t* temp = cola->primero;
		return temp->dato;
	}
}

void* cola_desencolar(cola_t *cola){
	if (cola_esta_vacia(cola))
		return NULL;
	
	nodo_cola_t* nodo_temp = cola->primero;
	void* dato = nodo_temp->dato;
	cola->primero = nodo_temp->proximo;
	if (cola->ultimo == nodo_temp)
		cola->ultimo = NULL;
	
	free(nodo_temp);

	return dato;
}
