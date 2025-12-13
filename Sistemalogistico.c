#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ESTRUCTURAS DE DATOS//
// Nodo de la cola FIFO (pedidos)
typedef struct Pedido {
    char destino[50];
    int cantidad;
    struct Pedido* siguiente;
} Pedido;

// Nodo del árbol AVL
typedef struct NodoAVL {
    int fecha;              // Clave: AAAAMMDD
    char producto[50];
    int stock;
    Pedido* cabeza;         // Cola FIFO de pedidos
    int altura;
    struct NodoAVL* izquierda;
    struct NodoAVL* derecha;
} NodoAVL;

// ================= FUNCIONES DE LA COLA FIFO =================
// Crear un nuevo pedido
Pedido* crearPedido(const char* destino, int cantidad) {
    Pedido* nuevo = (Pedido*)malloc(sizeof(Pedido));
    strcpy(nuevo->destino, destino);
    nuevo->cantidad = cantidad;
    nuevo->siguiente = NULL;
    return nuevo;
}

// Agregar pedido al final de la cola
void encolar(Pedido** cabeza, const char* destino, int cantidad) {
    Pedido* nuevo = crearPedido(destino, cantidad);
    if (*cabeza == NULL) {
        *cabeza = nuevo;
    } else {
        Pedido* temp = *cabeza;
        while (temp->siguiente != NULL) {
            temp = temp->siguiente;
        }
        temp->siguiente = nuevo;
    }
}

// Eliminar un pedido específico de la cola//
int cancelarPedido(Pedido** cabeza, const char* destino, int cantidad, NodoAVL* nodo) {
    Pedido* actual = *cabeza;
    Pedido* anterior = NULL;

    while (actual != NULL) {
        if (strcmp(actual->destino, destino) == 0 && actual->cantidad == cantidad) {
            if (anterior == NULL) {
                *cabeza = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            nodo->stock += actual->cantidad; // Restaurar stock
            free(actual);
            return 1; // Eliminado
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return 0; // No encontrado
}

// Liberar toda la cola de pedidos//
void liberarCola(Pedido* cabeza) {
    Pedido* temp;
    while (cabeza != NULL) {
        temp = cabeza;
        cabeza = cabeza->siguiente;
        free(temp);
    }
}

// Contar pedidos en la cola
int contarPedidos(Pedido* cabeza) {
    int contador = 0;
    Pedido* temp = cabeza;
    while (temp != NULL) {
        contador++;
        temp = temp->siguiente;
    }
    return contador;
}

// FUNCIONES DEL ÁRBOL AVL
// Función auxiliar para obtener la altura
int altura(NodoAVL* nodo) {
    if (nodo == NULL)
        return 0;
    return nodo->altura;
}

// Función para obtener el máximo de dos enteros//
int maximo(int a, int b) {
    return (a > b) ? a : b;
}

// Crear un nuevo nodo AVL
NodoAVL* crearNodoAVL(int fecha, const char* producto, int stock) {
    NodoAVL* nodo = (NodoAVL*)malloc(sizeof(NodoAVL));
    nodo->fecha = fecha;
    strcpy(nodo->producto, producto);
    nodo->stock = stock;
    nodo->cabeza = NULL;
    nodo->altura = 1;
    nodo->izquierda = NULL;
    nodo->derecha = NULL;
    return nodo;
}

// Rotación simple a la derecha//
NodoAVL* rotarDerecha(NodoAVL* y) {
    NodoAVL* x = y->izquierda;
    NodoAVL* T2 = x->derecha;

    x->derecha = y;
    y->izquierda = T2;

    y->altura = maximo(altura(y->izquierda), altura(y->derecha)) + 1;
    x->altura = maximo(altura(x->izquierda), altura(x->derecha)) + 1;

    return x;
}

// Rotación simple a la izquierda
NodoAVL* rotarIzquierda(NodoAVL* x) {
    NodoAVL* y = x->derecha;
    NodoAVL* T2 = y->izquierda;

    y->izquierda = x;
    x->derecha = T2;

    x->altura = maximo(altura(x->izquierda), altura(x->derecha)) + 1;
    y->altura = maximo(altura(y->izquierda), altura(y->derecha)) + 1;

    return y;
}

// Obtener el factor de balance//
int balanceFactor(NodoAVL* nodo) {
    if (nodo == NULL)
        return 0;
    return altura(nodo->izquierda) - altura(nodo->derecha);
}

// Insertar en el árbol AVL
NodoAVL* insertarAVL(NodoAVL* nodo, int fecha, const char* producto, int stock) {
    if (nodo == NULL)
        return crearNodoAVL(fecha, producto, stock);

    if (fecha < nodo->fecha)
        nodo->izquierda = insertarAVL(nodo->izquierda, fecha, producto, stock);
    else if (fecha > nodo->fecha)
        nodo->derecha = insertarAVL(nodo->derecha, fecha, producto, stock);
    else
        return nodo; // Fecha duplicada (no permitida según requerimiento)

    nodo->altura = 1 + maximo(altura(nodo->izquierda), altura(nodo->derecha));

    int balance = balanceFactor(nodo);

    // Casos de desbalance
    if (balance > 1 && fecha < nodo->izquierda->fecha)
        return rotarDerecha(nodo);

    if (balance < -1 && fecha > nodo->derecha->fecha)
        return rotarIzquierda(nodo);

    if (balance > 1 && fecha > nodo->izquierda->fecha) {
        nodo->izquierda = rotarIzquierda(nodo->izquierda);
        return rotarDerecha(nodo);
    }

    if (balance < -1 && fecha < nodo->derecha->fecha) {
        nodo->derecha = rotarDerecha(nodo->derecha);
        return rotarIzquierda(nodo);
    }

    return nodo;
}

// Encontrar el nodo con la fecha mínima (más próxima a vencer)
NodoAVL* encontrarMinimo(NodoAVL* nodo) {
    NodoAVL* actual = nodo;
    while (actual->izquierda != NULL)
        actual = actual->izquierda;
    return actual;
}

// Eliminar un nodo del árbol AVL
NodoAVL* eliminarNodoAVL(NodoAVL* raiz, int fecha) {
    if (raiz == NULL)
        return raiz;

    if (fecha < raiz->fecha)
        raiz->izquierda = eliminarNodoAVL(raiz->izquierda, fecha);
    else if (fecha > raiz->fecha)
        raiz->derecha = eliminarNodoAVL(raiz->derecha, fecha);
    else {
        // Nodo encontrado: liberar cola y luego el nodo
        liberarCola(raiz->cabeza);

        if (raiz->izquierda == NULL || raiz->derecha == NULL) {
            NodoAVL* temp = raiz->izquierda ? raiz->izquierda : raiz->derecha;
            free(raiz);
            return temp;
        } else {
            NodoAVL* temp = encontrarMinimo(raiz->derecha);
            raiz->fecha = temp->fecha;
            raiz->stock = temp->stock;
            strcpy(raiz->producto, temp->producto);
            raiz->cabeza = temp->cabeza;
            temp->cabeza = NULL; // Evitar que se libere dos veces
            raiz->derecha = eliminarNodoAVL(raiz->derecha, temp->fecha);
        }
    }

    if (raiz == NULL)
        return raiz;

    raiz->altura = 1 + maximo(altura(raiz->izquierda), altura(raiz->derecha));

    int balance = balanceFactor(raiz);

    // Rebalancear
    if (balance > 1 && balanceFactor(raiz->izquierda) >= 0)
        return rotarDerecha(raiz);

    if (balance > 1 && balanceFactor(raiz->izquierda) < 0) {
        raiz->izquierda = rotarIzquierda(raiz->izquierda);
        return rotarDerecha(raiz);
    }

    if (balance < -1 && balanceFactor(raiz->derecha) <= 0)
        return rotarIzquierda(raiz);

    if (balance < -1 && balanceFactor(raiz->derecha) > 0) {
        raiz->derecha = rotarDerecha(raiz->derecha);
        return rotarIzquierda(raiz);
    }

    return raiz;
}

// Buscar un nodo por fecha
NodoAVL* buscarNodo(NodoAVL* raiz, int fecha) {
    if (raiz == NULL || raiz->fecha == fecha)
        return raiz;

    if (fecha < raiz->fecha)
        return buscarNodo(raiz->izquierda, fecha);

    return buscarNodo(raiz->derecha, fecha);
}

// Recorrido In-Order (más próximo a más lejano)
void inOrder(NodoAVL* raiz) {
    if (raiz != NULL) {
        inOrder(raiz->izquierda);
        printf("Fecha: %d | Producto: %s | Stock: %d | Pedidos en cola: %d\n",
               raiz->fecha, raiz->producto, raiz->stock, contarPedidos(raiz->cabeza));
        inOrder(raiz->derecha);
    }
}

// Liberar todo el árbol AVL
void liberarAVL(NodoAVL* raiz) {
    if (raiz != NULL) {
        liberarAVL(raiz->izquierda);
        liberarAVL(raiz->derecha);
        liberarCola(raiz->cabeza);
        free(raiz);
    }
}

// ================= MENÚ PRINCIPAL =================
void mostrarMenu() {
    printf("\n=== SISTEMA LOGISTICO PUERTO BUENAVENTURA ===\n");
    printf("1. Recepcion de Mercancia (Insertar en AVL)\n");
    printf("2. Registrar Pedido de Despacho (Encolar en FIFO)\n");
    printf("3. Cancelacion de Producto (Eliminar nodo AVL)\n");
    printf("4. Cancelar Pedido Especifico\n");
    printf("5. Reporte de Estado (Recorrido In-Order)\n");
    printf("6. Salir\n");
    printf("Seleccione una opcion: ");
}

int main() {
    NodoAVL* raiz = NULL;
    int opcion, fecha, stock, cantidad;
    char producto[50], destino[50];

    do {
        mostrarMenu();
        scanf("%d", &opcion);
        getchar(); // Limpiar buffer

        switch (opcion) {
            case 1:
                printf("Ingrese fecha de vencimiento (AAAAMMDD): ");
                scanf("%d", &fecha);
                printf("Ingrese nombre del producto: ");
                getchar();
                fgets(producto, 50, stdin);
                producto[strcspn(producto, "\n")] = 0;
                printf("Ingrese stock inicial: ");
                scanf("%d", &stock);

                if (buscarNodo(raiz, fecha) != NULL) {
                    printf("Error: Ya existe un producto con esa fecha.\n");
                } else {
                    raiz = insertarAVL(raiz, fecha, producto, stock);
                    printf("Producto registrado exitosamente.\n");
                }
                break;

            case 2:
                if (raiz == NULL) {
                    printf("No hay productos en inventario.\n");
                    break;
                }
                // Buscar la fecha más próxima (mínima)
                NodoAVL* masProximo = encontrarMinimo(raiz);
                printf("Producto mas proximo a vencer: %s (Fecha: %d, Stock: %d)\n",
                       masProximo->producto, masProximo->fecha, masProximo->stock);

                printf("Ingrese destino del pedido: ");
                getchar();
                fgets(destino, 50, stdin);
                destino[strcspn(destino, "\n")] = 0;
                printf("Ingrese cantidad solicitada: ");
                scanf("%d", &cantidad);

                if (cantidad > masProximo->stock) {
                    printf("Error: Stock insuficiente.\n");
                } else {
                    encolar(&(masProximo->cabeza), destino, cantidad);
                    masProximo->stock -= cantidad;
                    printf("Pedido registrado exitosamente.\n");
                }
                break;

            case 3:
                printf("Ingrese fecha del producto a eliminar (AAAAMMDD): ");
                scanf("%d", &fecha);
                if (buscarNodo(raiz, fecha) == NULL) {
                    printf("Error: No existe un producto con esa fecha.\n");
                } else {
                    raiz = eliminarNodoAVL(raiz, fecha);
                    printf("Producto y sus pedidos eliminados exitosamente.\n");
                }
                break;

            case 4:
                printf("Ingrese fecha del producto (AAAAMMDD): ");
                scanf("%d", &fecha);
                NodoAVL* nodoCancelar = buscarNodo(raiz, fecha);
                if (nodoCancelar == NULL) {
                    printf("Error: No existe un producto con esa fecha.\n");
                } else {
                    printf("Ingrese destino del pedido a cancelar: ");
                    getchar();
                    fgets(destino, 50, stdin);
                    destino[strcspn(destino, "\n")] = 0;
                    printf("Ingrese cantidad del pedido a cancelar: ");
                    scanf("%d", &cantidad);

                    if (cancelarPedido(&(nodoCancelar->cabeza), destino, cantidad, nodoCancelar)) {
                        printf("Pedido cancelado exitosamente. Stock restaurado.\n");
                    } else {
                        printf("Error: No se encontro el pedido especificado.\n");
                    }
                }
                break;

            case 5:
                if (raiz == NULL) {
                    printf("No hay productos en inventario.\n");
                } else {
                    printf("\n=== REPORTE DE INVENTARIO ===\n");
                    inOrder(raiz);
                }
                break;

            case 6:
                printf("Saliendo del sistema...\n");
                break;

            default:
                printf("Opcion no valida.\n");
        }
    } while (opcion != 6);

    // Liberar toda la memoria antes de salir
    liberarAVL(raiz);

    return 0;
}#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ESTRUCTURAS DE DATOS//
// Nodo de la cola FIFO (pedidos)
typedef struct Pedido {
    char destino[50];
    int cantidad;
    struct Pedido* siguiente;
} Pedido;

// Nodo del árbol AVL
typedef struct NodoAVL {
    int fecha;              // Clave: AAAAMMDD
    char producto[50];
    int stock;
    Pedido* cabeza;         // Cola FIFO de pedidos
    int altura;
    struct NodoAVL* izquierda;
    struct NodoAVL* derecha;
} NodoAVL;

// ================= FUNCIONES DE LA COLA FIFO =================
// Crear un nuevo pedido
Pedido* crearPedido(const char* destino, int cantidad) {
    Pedido* nuevo = (Pedido*)malloc(sizeof(Pedido));
    strcpy(nuevo->destino, destino);
    nuevo->cantidad = cantidad;
    nuevo->siguiente = NULL;
    return nuevo;
}

// Agregar pedido al final de la cola
void encolar(Pedido** cabeza, const char* destino, int cantidad) {
    Pedido* nuevo = crearPedido(destino, cantidad);
    if (*cabeza == NULL) {
        *cabeza = nuevo;
    } else {
        Pedido* temp = *cabeza;
        while (temp->siguiente != NULL) {
            temp = temp->siguiente;
        }
        temp->siguiente = nuevo;
    }
}

// Eliminar un pedido específico de la cola//
int cancelarPedido(Pedido** cabeza, const char* destino, int cantidad, NodoAVL* nodo) {
    Pedido* actual = *cabeza;
    Pedido* anterior = NULL;

    while (actual != NULL) {
        if (strcmp(actual->destino, destino) == 0 && actual->cantidad == cantidad) {
            if (anterior == NULL) {
                *cabeza = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            nodo->stock += actual->cantidad; // Restaurar stock
            free(actual);
            return 1; // Eliminado
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return 0; // No encontrado
}

// Liberar toda la cola de pedidos//
void liberarCola(Pedido* cabeza) {
    Pedido* temp;
    while (cabeza != NULL) {
        temp = cabeza;
        cabeza = cabeza->siguiente;
        free(temp);
    }
}

// Contar pedidos en la cola
int contarPedidos(Pedido* cabeza) {
    int contador = 0;
    Pedido* temp = cabeza;
    while (temp != NULL) {
        contador++;
        temp = temp->siguiente;
    }
    return contador;
}

// FUNCIONES DEL ÁRBOL AVL
// Función auxiliar para obtener la altura
int altura(NodoAVL* nodo) {
    if (nodo == NULL)
        return 0;
    return nodo->altura;
}

// Función para obtener el máximo de dos enteros//
int maximo(int a, int b) {
    return (a > b) ? a : b;
}

// Crear un nuevo nodo AVL
NodoAVL* crearNodoAVL(int fecha, const char* producto, int stock) {
    NodoAVL* nodo = (NodoAVL*)malloc(sizeof(NodoAVL));
    nodo->fecha = fecha;
    strcpy(nodo->producto, producto);
    nodo->stock = stock;
    nodo->cabeza = NULL;
    nodo->altura = 1;
    nodo->izquierda = NULL;
    nodo->derecha = NULL;
    return nodo;
}

// Rotación simple a la derecha//
NodoAVL* rotarDerecha(NodoAVL* y) {
    NodoAVL* x = y->izquierda;
    NodoAVL* T2 = x->derecha;

    x->derecha = y;
    y->izquierda = T2;

    y->altura = maximo(altura(y->izquierda), altura(y->derecha)) + 1;
    x->altura = maximo(altura(x->izquierda), altura(x->derecha)) + 1;

    return x;
}

// Rotación simple a la izquierda
NodoAVL* rotarIzquierda(NodoAVL* x) {
    NodoAVL* y = x->derecha;
    NodoAVL* T2 = y->izquierda;

    y->izquierda = x;
    x->derecha = T2;

    x->altura = maximo(altura(x->izquierda), altura(x->derecha)) + 1;
    y->altura = maximo(altura(y->izquierda), altura(y->derecha)) + 1;

    return y;
}

// Obtener el factor de balance//
int balanceFactor(NodoAVL* nodo) {
    if (nodo == NULL)
        return 0;
    return altura(nodo->izquierda) - altura(nodo->derecha);
}

// Insertar en el árbol AVL
NodoAVL* insertarAVL(NodoAVL* nodo, int fecha, const char* producto, int stock) {
    if (nodo == NULL)
        return crearNodoAVL(fecha, producto, stock);

    if (fecha < nodo->fecha)
        nodo->izquierda = insertarAVL(nodo->izquierda, fecha, producto, stock);
    else if (fecha > nodo->fecha)
        nodo->derecha = insertarAVL(nodo->derecha, fecha, producto, stock);
    else
        return nodo; // Fecha duplicada (no permitida según requerimiento)

    nodo->altura = 1 + maximo(altura(nodo->izquierda), altura(nodo->derecha));

    int balance = balanceFactor(nodo);

    // Casos de desbalance
    if (balance > 1 && fecha < nodo->izquierda->fecha)
        return rotarDerecha(nodo);

    if (balance < -1 && fecha > nodo->derecha->fecha)
        return rotarIzquierda(nodo);

    if (balance > 1 && fecha > nodo->izquierda->fecha) {
        nodo->izquierda = rotarIzquierda(nodo->izquierda);
        return rotarDerecha(nodo);
    }

    if (balance < -1 && fecha < nodo->derecha->fecha) {
        nodo->derecha = rotarDerecha(nodo->derecha);
        return rotarIzquierda(nodo);
    }

    return nodo;
}

// Encontrar el nodo con la fecha mínima (más próxima a vencer)
NodoAVL* encontrarMinimo(NodoAVL* nodo) {
    NodoAVL* actual = nodo;
    while (actual->izquierda != NULL)
        actual = actual->izquierda;
    return actual;
}

// Eliminar un nodo del árbol AVL
NodoAVL* eliminarNodoAVL(NodoAVL* raiz, int fecha) {
    if (raiz == NULL)
        return raiz;

    if (fecha < raiz->fecha)
        raiz->izquierda = eliminarNodoAVL(raiz->izquierda, fecha);
    else if (fecha > raiz->fecha)
        raiz->derecha = eliminarNodoAVL(raiz->derecha, fecha);
    else {
        // Nodo encontrado: liberar cola y luego el nodo
        liberarCola(raiz->cabeza);

        if (raiz->izquierda == NULL || raiz->derecha == NULL) {
            NodoAVL* temp = raiz->izquierda ? raiz->izquierda : raiz->derecha;
            free(raiz);
            return temp;
        } else {
            NodoAVL* temp = encontrarMinimo(raiz->derecha);
            raiz->fecha = temp->fecha;
            raiz->stock = temp->stock;
            strcpy(raiz->producto, temp->producto);
            raiz->cabeza = temp->cabeza;
            temp->cabeza = NULL; // Evitar que se libere dos veces
            raiz->derecha = eliminarNodoAVL(raiz->derecha, temp->fecha);
        }
    }

    if (raiz == NULL)
        return raiz;

    raiz->altura = 1 + maximo(altura(raiz->izquierda), altura(raiz->derecha));

    int balance = balanceFactor(raiz);

    // Rebalancear
    if (balance > 1 && balanceFactor(raiz->izquierda) >= 0)
        return rotarDerecha(raiz);

    if (balance > 1 && balanceFactor(raiz->izquierda) < 0) {
        raiz->izquierda = rotarIzquierda(raiz->izquierda);
        return rotarDerecha(raiz);
    }

    if (balance < -1 && balanceFactor(raiz->derecha) <= 0)
        return rotarIzquierda(raiz);

    if (balance < -1 && balanceFactor(raiz->derecha) > 0) {
        raiz->derecha = rotarDerecha(raiz->derecha);
        return rotarIzquierda(raiz);
    }

    return raiz;
}

// Buscar un nodo por fecha
NodoAVL* buscarNodo(NodoAVL* raiz, int fecha) {
    if (raiz == NULL || raiz->fecha == fecha)
        return raiz;

    if (fecha < raiz->fecha)
        return buscarNodo(raiz->izquierda, fecha);

    return buscarNodo(raiz->derecha, fecha);
}

// Recorrido In-Order (más próximo a más lejano)
void inOrder(NodoAVL* raiz) {
    if (raiz != NULL) {
        inOrder(raiz->izquierda);
        printf("Fecha: %d | Producto: %s | Stock: %d | Pedidos en cola: %d\n",
               raiz->fecha, raiz->producto, raiz->stock, contarPedidos(raiz->cabeza));
        inOrder(raiz->derecha);
    }
}

// Liberar todo el árbol AVL
void liberarAVL(NodoAVL* raiz) {
    if (raiz != NULL) {
        liberarAVL(raiz->izquierda);
        liberarAVL(raiz->derecha);
        liberarCola(raiz->cabeza);
        free(raiz);
    }
}

// ================= MENÚ PRINCIPAL =================
void mostrarMenu() {
    printf("\n=== SISTEMA LOGISTICO PUERTO BUENAVENTURA ===\n");
    printf("1. Recepcion de Mercancia (Insertar en AVL)\n");
    printf("2. Registrar Pedido de Despacho (Encolar en FIFO)\n");
    printf("3. Cancelacion de Producto (Eliminar nodo AVL)\n");
    printf("4. Cancelar Pedido Especifico\n");
    printf("5. Reporte de Estado (Recorrido In-Order)\n");
    printf("6. Salir\n");
    printf("Seleccione una opcion: ");
}

int main() {
    NodoAVL* raiz = NULL;
    int opcion, fecha, stock, cantidad;
    char producto[50], destino[50];

    do {
        mostrarMenu();
        scanf("%d", &opcion);
        getchar(); // Limpiar buffer

        switch (opcion) {
            case 1:
                printf("Ingrese fecha de vencimiento (AAAAMMDD): ");
                scanf("%d", &fecha);
                printf("Ingrese nombre del producto: ");
                getchar();
                fgets(producto, 50, stdin);
                producto[strcspn(producto, "\n")] = 0;
                printf("Ingrese stock inicial: ");
                scanf("%d", &stock);

                if (buscarNodo(raiz, fecha) != NULL) {
                    printf("Error: Ya existe un producto con esa fecha.\n");
                } else {
                    raiz = insertarAVL(raiz, fecha, producto, stock);
                    printf("Producto registrado exitosamente.\n");
                }
                break;

            case 2:
                if (raiz == NULL) {
                    printf("No hay productos en inventario.\n");
                    break;
                }
                // Buscar la fecha más próxima (mínima)
                NodoAVL* masProximo = encontrarMinimo(raiz);
                printf("Producto mas proximo a vencer: %s (Fecha: %d, Stock: %d)\n",
                       masProximo->producto, masProximo->fecha, masProximo->stock);

                printf("Ingrese destino del pedido: ");
                getchar();
                fgets(destino, 50, stdin);
                destino[strcspn(destino, "\n")] = 0;
                printf("Ingrese cantidad solicitada: ");
                scanf("%d", &cantidad);

                if (cantidad > masProximo->stock) {
                    printf("Error: Stock insuficiente.\n");
                } else {
                    encolar(&(masProximo->cabeza), destino, cantidad);
                    masProximo->stock -= cantidad;
                    printf("Pedido registrado exitosamente.\n");
                }
                break;

            case 3:
                printf("Ingrese fecha del producto a eliminar (AAAAMMDD): ");
                scanf("%d", &fecha);
                if (buscarNodo(raiz, fecha) == NULL) {
                    printf("Error: No existe un producto con esa fecha.\n");
                } else {
                    raiz = eliminarNodoAVL(raiz, fecha);
                    printf("Producto y sus pedidos eliminados exitosamente.\n");
                }
                break;

            case 4:
                printf("Ingrese fecha del producto (AAAAMMDD): ");
                scanf("%d", &fecha);
                NodoAVL* nodoCancelar = buscarNodo(raiz, fecha);
                if (nodoCancelar == NULL) {
                    printf("Error: No existe un producto con esa fecha.\n");
                } else {
                    printf("Ingrese destino del pedido a cancelar: ");
                    getchar();
                    fgets(destino, 50, stdin);
                    destino[strcspn(destino, "\n")] = 0;
                    printf("Ingrese cantidad del pedido a cancelar: ");
                    scanf("%d", &cantidad);

                    if (cancelarPedido(&(nodoCancelar->cabeza), destino, cantidad, nodoCancelar)) {
                        printf("Pedido cancelado exitosamente. Stock restaurado.\n");
                    } else {
                        printf("Error: No se encontro el pedido especificado.\n");
                    }
                }
                break;

            case 5:
                if (raiz == NULL) {
                    printf("No hay productos en inventario.\n");
                } else {
                    printf("\n=== REPORTE DE INVENTARIO ===\n");
                    inOrder(raiz);
                }
                break;

            case 6:
                printf("Saliendo del sistema...\n");
                break;

            default:
                printf("Opcion no valida.\n");
        }
    } while (opcion != 6);

    // Liberar toda la memoria antes de salir
    liberarAVL(raiz);

    return 0;
}
