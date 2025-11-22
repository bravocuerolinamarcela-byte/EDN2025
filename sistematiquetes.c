#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definición de la estructura del nodo Pasajero
typedef struct Pasajero {
    int documento;
    char destino[50];
    char tipo_pasaje[20];
    struct Pasajero *izq;
    struct Pasajero *der;
} Pasajero;

// Función para crear un nuevo nodo Pasajero
Pasajero* crearPasajero(int documento, char destino[], char tipo_pasaje[]) {
    Pasajero *nuevo = (Pasajero*)malloc(sizeof(Pasajero));
    if (nuevo == NULL) {
        printf("Error de memoria.\n");
        return NULL;
    }
    nuevo->documento = documento;
    strcpy(nuevo->destino, destino);
    strcpy(nuevo->tipo_pasaje, tipo_pasaje);
    nuevo->izq = NULL;
    nuevo->der = NULL;
    return nuevo;
}

// Función para insertar en el ABB
Pasajero* insertar(Pasajero *raiz, int documento, char destino[], char tipo_pasaje[]) {
    if (raiz == NULL) {
        return crearPasajero(documento, destino, tipo_pasaje);
    }
    if (documento < raiz->documento) {
        raiz->izq = insertar(raiz->izq, documento, destino, tipo_pasaje);
    } else if (documento > raiz->documento) {
        raiz->der = insertar(raiz->der, documento, destino, tipo_pasaje);
    } else {
        printf("Documento %d ya existe. No se inserta.\n", documento);
    }
    return raiz;
}

// Recorrido Inorden
void inorden(Pasajero *raiz) {
    if (raiz != NULL) {
        inorden(raiz->izq);
        printf("Documento: %d, Destino: %s, Tipo: %s\n", raiz->documento, raiz->destino, raiz->tipo_pasaje);
        inorden(raiz->der);
    }
}

// Recorrido Preorden
void preorden(Pasajero *raiz) {
    if (raiz != NULL) {
        printf("Documento: %d, Destino: %s, Tipo: %s\n", raiz->documento, raiz->destino, raiz->tipo_pasaje);
        preorden(raiz->izq);
        preorden(raiz->der);
    }
}

// Recorrido Postorden
void postorden(Pasajero *raiz) {
    if (raiz != NULL) {
        postorden(raiz->izq);
        postorden(raiz->der);
        printf("Documento: %d, Destino: %s, Tipo: %s\n", raiz->documento, raiz->destino, raiz->tipo_pasaje);
    }
}

// Función para contar el total de pasajeros
int contarPasajeros(Pasajero *raiz) {
    if (raiz == NULL) {
        return 0;
    }
    return 1 + contarPasajeros(raiz->izq) + contarPasajeros(raiz->der);
}

// Función para encontrar el nodo mínimo (para eliminación)
Pasajero* encontrarMinimo(Pasajero *raiz) {
    Pasajero *actual = raiz;
    while (actual && actual->izq != NULL) {
        actual = actual->izq;
    }
    return actual;
}

// Función para eliminar un pasajero por documento
Pasajero* eliminar(Pasajero *raiz, int documento) {
    if (raiz == NULL) {
        return raiz;
    }
    if (documento < raiz->documento) {
        raiz->izq = eliminar(raiz->izq, documento);
    } else if (documento > raiz->documento) {
        raiz->der = eliminar(raiz->der, documento);
    } else {
        // Caso 1: Nodo sin hijos o con un hijo
        if (raiz->izq == NULL) {
            Pasajero *temp = raiz->der;
            free(raiz);
            return temp;
        } else if (raiz->der == NULL) {
            Pasajero *temp = raiz->izq;
            free(raiz);
            return temp;
        }
        // Caso 2: Nodo con dos hijos
        Pasajero *temp = encontrarMinimo(raiz->der);
        raiz->documento = temp->documento;
        strcpy(raiz->destino, temp->destino);
        strcpy(raiz->tipo_pasaje, temp->tipo_pasaje);
        raiz->der = eliminar(raiz->der, temp->documento);
    }
    return raiz;
}

// Función para liberar toda la memoria del árbol
void liberarArbol(Pasajero *raiz) {
    if (raiz != NULL) {
        liberarArbol(raiz->izq);
        liberarArbol(raiz->der);
        free(raiz);
    }
}

int main() {
    Pasajero *raiz = NULL;
    int opcion, documento;
    char destino[50], tipo_pasaje[20];
    char destinos[4][50] = {"Timbiquí", "Juanchaco", "Tumaco", "Guapi"};

    do {
        printf("\n--- Sistema de Gestión de Tiquetes ---\n");
        printf("1. Registrar pasajero\n");
        printf("2. Listar pasajeros (Inorden)\n");
        printf("3. Listar pasajeros (Preorden)\n");
        printf("4. Listar pasajeros (Postorden)\n");
        printf("5. Contar pasajeros\n");
        printf("6. Eliminar pasajero\n");
        printf("7. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Ingrese el documento: ");
                scanf("%d", &documento);
                printf("Seleccione el destino (1.Timbiquí, 2.Juanchaco, 3.Tumaco, 4.Guapi): ");
                int dest;
                scanf("%d", &dest);
                if (dest < 1 || dest > 4) {
                    printf("Destino no válido.\n");
                    break;
                }
                printf("Seleccione el tipo de pasaje (1. Ida, 2. Ida y Regreso): ");
                int tipo;
                scanf("%d", &tipo);
                if (tipo == 1) {
                    strcpy(tipo_pasaje, "Ida");
                } else if (tipo == 2) {
                    strcpy(tipo_pasaje, "Ida y Regreso");
                } else {
                    printf("Tipo no válido.\n");
                    break;
                }
                raiz = insertar(raiz, documento, destinos[dest-1], tipo_pasaje);
                break;
            case 2:
                printf("Listado Inorden:\n");
                inorden(raiz);
                break;
            case 3:
                printf("Listado Preorden:\n");
                preorden(raiz);
                break;
            case 4:
                printf("Listado Postorden:\n");
                postorden(raiz);
                break;
            case 5:
                printf("Total de pasajeros: %d\n", contarPasajeros(raiz));
                break;
            case 6:
                printf("Ingrese el documento a eliminar: ");
                scanf("%d", &documento);
                raiz = eliminar(raiz, documento);
                break;
            case 7:
                printf("Saliendo...\n");
                liberarArbol(raiz);
                break;
            default:
                printf("Opción no válida.\n");
        }
    } while (opcion != 7);

    return 0;
}
