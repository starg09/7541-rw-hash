#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdio.h>


// ******************************************************************
//                  DEFINICION DE LOS TIPOS DE DATOS
// ******************************************************************

// La lista está planteada como una lista de punteros genéricos.

struct lista;
typedef struct lista lista_t;


// ******************************************************************
//                    PRIMITIVAS DE LA LISTA
// ******************************************************************


// Crea una lista.
// Post: Devuelve una lista vacía.
lista_t *lista_crear(void);

// Devuelve verdadero o falso, según si la lista tiene o no elementos en ella.
// Pre: La lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento al comienzo de la lista. Devuelve falso en caso de error.
// Coloca en la última posición de ser necesario.
// Pre: La lista fue creada.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra en la primer ubicación.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento al comienzo de la lista. Devuelve falso en caso de error.
// Coloca en la primer posición de ser necesario.
// Pre: La lista fue creada.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra en la última ubicación.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Borra el primer elemento de la lista. Si la lista tiene elementos, se quita el
// que se encuentre en la primera ubicación, y se devuelve su valor.
// Si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista contiene un elemento menos, si la lista no estaba vacía.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista, de tener esta elementos.
// Si no tiene, devuelve NULL.
// Pre: La lista fue creada.
// Post: Cuando la lista no esté vacía, devuelve el primer elemento.
void *lista_ver_primero(const lista_t *lista);

// Devuelve la cantidad de elementos de la lista.
// Pre: La lista fue creada.
// Post: Se devuelve la cantidad de elementos actualmente en la lista.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// se le aplica a los datos ubicados en cada elemento de la lista, antes de borrar el nodo.
// Pre: La lista fue creada. destruir_dato es capaz de destruir los datos de esta,
// o es NULL en caso de que no se use.
// Post: Se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));




// ******************************************************************
//                    PRIMITIVAS DE LA LISTA
// ******************************************************************


//Defino la estructura que contendrá al iterador.
typedef struct lista_iter lista_iter_t;

// Crea un iterador para la lista pasada, ubicado al inicio de esta.
// Pre: La lista fue creada.
// Post: El iterador fue creado, y se ubica al comienzo de la lista.
lista_iter_t *lista_iter_crear(const lista_t *lista);

// Devuelve un booleano dependiendo de si se encuentra al final de la lista iterada o no.
// Pre: La lista y el iterador fueron creados, y ninguno destruido.
bool lista_iter_al_final(const lista_iter_t *iter);

// Hace que el iterador avance a través de la lista en la que trabaja una posición.
// Pre: El iterador fue creado. La lista existe todavía.
// Post: Devuelve verdadero de haber avanzado sin problemas, o falso de no ser posible hacer esto. (Por ejemplo, si ya se encuentra al final)
bool lista_iter_avanzar(lista_iter_t *iter);

// Pasa el dato del nodo en el que se ubica actualmente el iterador.
// Pre: El iterador fue creado. La lista existe todavía.
// Post: Se devuelve el puntero al nodo en el que se ubica actualmente el iterador, o NULL de haber llegado al final.
void *lista_iter_ver_actual(const lista_iter_t *iter);


// Destruye el iterador.
// Pre: el iterador fue creado.
void lista_iter_destruir(lista_iter_t *iter);




// ******************************************************************
//                    PRIMITIVAS DE LISTAS JUNTO CON ITERADOR
// ******************************************************************


// Inserta un elemento en la lista, en la posición en la que se encuentra actualmente el iterador.
// Pre: El iterador y la lista fueron creados, y ninguno destruido.
// Post: Devuelve verdadero luego de insertar el nuevo elemento, o falso de haber un problema.
bool lista_insertar(lista_t *lista, lista_iter_t *iter, void *dato);

// Borra de la lista al elemento en el que se ubica actualmente el iterador, y acomoda la lista conforme a esto.
// Devuelve el dato almacenado en el elemento borrado.
// Pre: El iterador y la lista fueron creados, y ninguno destruido.
void *lista_borrar(lista_t *lista, lista_iter_t *iter);




// ******************************************************************
//                    PRIMITIVAS DE ITERADOR INTERNO
// ******************************************************************


// Itera por el vector mientras la función pasada devuelva true, o hasta que se llegue al final del iterador.
// Pre: La lista fue creada. extra es un puntero apuntando a algo accesible.
void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra);




/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
// (esto es, las pruebas no pueden acceder a los miembros del struct lista o iter).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_alumno(void);

#endif // LISTA_H
