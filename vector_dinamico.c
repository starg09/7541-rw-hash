#include "vector_dinamico.h"


// Funciones del alumno.

// Revisa si existe espacio reservado para datos. De haberlo libera dicho espacio.
// Posteriormente a esto, libera el espacio del vector en sí.
void vector_destruir(vector_t* vector){
    if (vector->datos != NULL){
        free (vector->datos);
    }
    free (vector);
}

// Si la posición no es válida, devuelve false. De lo contrario, guarda el valor
// ubicado en esa posición del vector en la variable “valor”, y devuelve true.
bool vector_obtener(vector_t* vector, size_t pos, void* valor){
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtype-limits"
    if (( pos < 0 ) || (pos >= vector->tam))
        return false;
    #pragma GCC diagnostic pop
    valor = vector->datos[pos];
    return true;
}

// Si la posición no es válida, devuelve false. De lo contrario, guarda lo almacenado
// en la variable “valor” en el espacio pos del vector, y devuelve true.
bool vector_guardar(vector_t* vector, size_t pos, void* valor){
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtype-limits"
    if (( pos < 0 ) || (pos >= vector->tam))
        return false;
    #pragma GCC diagnostic pop
    vector->datos[pos] = valor;
    return true;
}

//Devuelve el tamaño del vector dinamico entregado.
size_t vector_obtener_tamanio(vector_t* vector){
    return vector->tam;
}

// Funciones implementadas por la catedra.

vector_t* vector_crear(size_t tam) {
    vector_t* vector = malloc(sizeof(vector_t));

    if (vector == NULL) {
	return NULL;
    }
    vector->datos = malloc(tam * sizeof(void*));

    if (tam > 0 && vector->datos == NULL) {
        free(vector);
        return NULL;
    }
    vector->tam = tam;
    return vector;
}

bool vector_redimensionar(vector_t* vector, size_t tam_nuevo) {
    void* datos_nuevo = realloc(vector->datos, tam_nuevo * sizeof(void*));

    // Cuando tam_nuevo es 0, es correcto si se devuelve NULL.
    // En toda otra situación significa que falló el realloc.
    if (tam_nuevo > 0 && datos_nuevo == NULL) {
        return false;
    }

    vector->datos = datos_nuevo;
    vector->tam = tam_nuevo;
    return true;
}
