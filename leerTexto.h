#include <stdio.h>
#include <string.h>

// Declaración de la función limpiarBuffer
void limpiarBuffer();

// Función para leer una línea de texto
void leerTexto(char texto[], int tamaño) {
    limpiarBuffer();                    // llamada a la función limpiarBuffer
    fgets(texto, tamaño, stdin);        // Lee la línea completa
    
    // Luego de la llamada se hace quita el \n y se remplaza por \0 para visualizar toda la cadena guardada
    int len = strlen(texto);
    if (len > 0 && texto[len-1] == '\n') {
        texto[len-1] = '\0';
    }
}
