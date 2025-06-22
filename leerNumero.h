#include <stdio.h>

// Declaración de la función limpiarBuffer
void limpiarBuffer();

// Función para leer un número entero
int leerNumero() {
    int numero;
    while (scanf("%d", &numero) != 1) {  // Mientras no lea un número válido
        printf("Error: Ingrese un número válido: ");
        limpiarBuffer();                  // Limpia la entrada incorrecta
    }
    return numero;
}

// Función para leer un número decimal
float leerFloat() {
    float numero;
    while (scanf("%f", &numero) != 1) {  // Mientras no lea un número válido
        printf("Error: Ingrese un número válido: ");
        limpiarBuffer();                  // Limpia la entrada incorrecta
    }
    return numero;
}
