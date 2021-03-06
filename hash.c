#include "lista.h"
#include "cola.h"
#include "vector_dinamico.h"
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define LARGO_INICIAL 100
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
	size_t cantidad_actual;
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

// Método de hasheo elegido.
size_t hashear_clave(size_t tam, const char* clave) { 

    unsigned char* tempstr;
    tempstr = (unsigned char*)clave;
    unsigned long hash = 5381;
    unsigned int c;

    while ((c = *tempstr++) != 0) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % tam;
}

bool hash_redimensionar(hash_t* hash, size_t tam);
bool hash_revisar_redimensionamiento(hash_t* hash);


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if (hash == NULL)
		return NULL;
	
	vector_t* vector = vector_crear(LARGO_INICIAL);
	if (vector == NULL){
		free (hash);
		return NULL;
	}

	for (size_t i = 0; i < vector_obtener_tamanio(vector); i++){
		vector_guardar(vector, i, NULL);
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

	size_t pos = hashear_clave(vector_obtener_tamanio(hash->vector), clave);

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

	size_t pos = hashear_clave(vector_obtener_tamanio(hash->vector), clave);

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
			hash_revisar_redimensionamiento(hash);
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
			hash_revisar_redimensionamiento(hash);
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

	size_t pos = hashear_clave(vector_obtener_tamanio(hash->vector), clave);

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
				hash_revisar_redimensionamiento(hash);
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

	size_t pos = hashear_clave(vector_obtener_tamanio(hash->vector), clave);

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
	hiter->cantidad_actual = hash->cantidad;

	//Se inicializa de tener elementos
	if (hiter->cantidad_actual > 0) {
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

	if (lista_iter_al_final(iter->act)) {

		size_t i = iter->pos_actual + 1;
		lista_iter_t* tempact = NULL;

		if (hash_iter_avance_interno(iter, &i, &tempact)) {
			lista_iter_destruir(iter->act);
			iter->act = tempact;
			iter->pos_actual = i;
			return true;
		} else {
			return false;
		}
	} else if (lista_iter_avanzar(iter->act)) {
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
		} else {
			return true;
		}

	} else {
		return false;
	}
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
	if ((iter->cantidad_actual == 0) || (iter->cantidad_actual < iter->recorridos))
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



// Redimensiona el vector del hash, y recoloca todos los valores guardados en este.
// Pre: Hash creado. No hay ningún iterador en uso.
// Pos: Devuelve true de poder redimensionar, o false en caso contrario.

bool hash_redimensionar(hash_t* hash, size_t tam){
	if (hash->cantidad == 0)
		return vector_redimensionar(hash->vector, tam);

	size_t i = 0;
	size_t clave_hasheada_temp;
	size_t pos;
	lista_iter_t* temp_liter = NULL;
	lista_t* nueva_lista_temp = NULL;
	nodo_hash_t* nodo_temp = NULL;

	// Cola de los nodos a reubicar en el nuevo vector.
	cola_t* cola_datos = cola_crear();
	if (cola_datos == NULL)
		return false;

	// Cola de posiciones ocupadas del vector original, para limpiar mas rápido después
	cola_t* cola_pos = cola_crear();
	if (cola_pos == NULL){
		cola_destruir(cola_datos, NULL);
		return false;
	}

	// Cola de posiciones ocupadas del nuevo vector, para deshacer en caso de error.
	cola_t* cola_undo = cola_crear();
	if (cola_undo == NULL){
		cola_destruir(cola_datos, NULL);
		cola_destruir(cola_pos, NULL);
		return false;
	}

	hash_iter_t* iter = hash_iter_crear(hash);
	if (iter == NULL){
		cola_destruir(cola_datos, NULL);
		cola_destruir(cola_pos, NULL);
		cola_destruir(cola_undo, free);
		return false;
	}

	temp_liter = iter->act;
	pos = iter->pos_actual;

	while (!hash_iter_al_final(iter)){
		if (iter->act != NULL)
			cola_encolar(cola_datos, lista_iter_ver_actual(iter->act));

		hash_iter_avanzar(iter);
		i++;

		if (iter->act != temp_liter){
			// Usada ubicación directa porque con función no salía. Revisar que funcione bien
			cola_encolar(cola_pos, hash->vector->datos[pos]);
			pos = iter->pos_actual;
			temp_liter = iter->act;
		}
	}

	hash_iter_destruir(iter);


	vector_redimensionar(hash->vector, tam);
	for (i=0; i<tam; i++){
		hash->vector->datos[i] = NULL;
	}

	while (!cola_esta_vacia(cola_datos)){
		nodo_temp = cola_desencolar(cola_datos);
		clave_hasheada_temp = hashear_clave(tam, nodo_temp->clave);
		nueva_lista_temp = hash->vector->datos[clave_hasheada_temp];
		if (nueva_lista_temp == NULL){
			nueva_lista_temp = lista_crear();
			if (nueva_lista_temp == NULL){
				cola_destruir(cola_datos, NULL);
				cola_destruir(cola_pos, NULL);
				cola_destruir(cola_undo, free);
				return false;
			}
			vector_guardar(hash->vector, clave_hasheada_temp, nueva_lista_temp);
		}
		lista_insertar_ultimo(nueva_lista_temp, nodo_temp);
	}
	cola_destruir(cola_datos, NULL);

	while(!cola_esta_vacia(cola_pos)){
		lista_destruir(cola_desencolar(cola_pos), NULL);
	}
	cola_destruir(cola_pos, NULL);
	cola_destruir(cola_undo, free);

	return true;
}

bool hash_revisar_redimensionamiento(hash_t* hash){
	size_t elementos = hash->cantidad;
	size_t capacidad = vector_obtener_tamanio(hash->vector);
	size_t lugar_usado = (elementos) * 100 / (capacidad);
	if (lugar_usado >= UMBRAL){
		if (hash_redimensionar (hash, capacidad * VARIACION)){
			return true;
		} else {
			fprintf(stderr, "No se pudo redimensionar el hash. Esto puede producir errores a corto plazo.\n");
			return false;
		}
	} else if ((capacidad > LARGO_INICIAL) && (lugar_usado <= (100-UMBRAL))){
		if (hash_redimensionar (hash, (capacidad / VARIACION))){
			return true;
		} else {
			fprintf(stderr, "No se pudo redimensionar el hash. Esto puede producir errores a corto plazo.\n");
			return false;
		}
	} else {
		return true;
	}
}
