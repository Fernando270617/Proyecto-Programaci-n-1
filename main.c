#include <stdio.h>
#include <string.h>
#include <stdlib.h> //Solo para no generar error en clear
#include "mostrarMenu.h"
#include "leerTexto.h" 
#include "leerNumero.h"

#define COLOR_BLANCO_FONDO "\033[47m"    // Fondo blanco (código 47)
#define COLOR_TEXTO_NEGRO "\033[30m"     // Texto negro (código 30)
#define COLOR_COMPLETO "\033[47;30m"     // Fondo blanco + texto negro

// Constantes
#define MAX_ZONAS 5
#define MAX_NOMBRE 50
#define MAX_HISTORICOS 10  // Reducido de 30 a 10
#define ARCHIVO_DATOS "datos.txt"

// Límites OMS
#define LIMITE_CO2 400
#define LIMITE_SO2 20
#define LIMITE_NO2 40
#define LIMITE_PM25 15

// Estructura para zona
struct Zona {
    int id;
    char nombre[MAX_NOMBRE];
    float co2, so2, no2, pm25;           
    float historicos[MAX_HISTORICOS][4]; // CO2, SO2, NO2, PM25
    float temp, viento, humedad;         // Clima
    int num_historicos;
    int alerta;
};

// Variables globales
struct Zona zonas[MAX_ZONAS];
int total_zonas = 0;

// Función auxiliar
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Función con punteros 1: Calcular promedio de array
void calcularPromedio(float valores[], int cantidad, float *resultado) {
    float suma = 0;
    for (int i = 0; i < cantidad; i++) {
        suma += valores[i];
    }
    *resultado = suma / cantidad;  // Guardar resultado usando puntero
}

// Buscar zona por ID
int buscarZona(int id) {
    for (int i = 0; i <= total_zonas; i++)
    {
        if (zonas[i].id == id) 
        {
        return i;
        }
    }
    return -1;
}

// Calcular índice de contaminación
float calcularIndice(struct Zona zona) {
    return ((zona.co2/LIMITE_CO2 + zona.so2/LIMITE_SO2 + 
             zona.no2/LIMITE_NO2 + zona.pm25/LIMITE_PM25) * 100) / 4.0;
}

// Función predecir contaminación
void predecir(struct Zona *z) {
    if (z->num_historicos < 3) {
        printf("Necesita más datos históricos.\n");
        return;
    }
    
    float prom[4] = {0, 0, 0, 0}; // CO2, SO2, NO2, PM25
    
    // Promedio de últimos 5 días (o los que tenga)
    int dias;
    if (z->num_historicos > 5) 
    {
    dias = 5;
    } 
    else
    {
    dias = z->num_historicos;
    }
    
    // Calcular promedio y ajustar por clima
    float factor;
    if (z->viento > 10) {
    factor = 0.8;
    } else {
    factor = 1.2;
    }

    if (z->temp > 25) {
    factor = factor * 1.1;
    }

    printf("Predicción \n ");
    printf("CO2: %.1f \n", (prom[0]/dias) * factor);
    printf("SO2: %.1f \n", (prom[1]/dias) * factor);
    printf("NO2: %.1f \n", (prom[2]/dias) * factor);
    printf("PM2.5: %.1f \n", (prom[3]/dias) * factor);
}

// Agregar zona
void agregarZona() {
    if (total_zonas >= MAX_ZONAS) {
        printf("Máximo %d zonas.\n", MAX_ZONAS);
        return;
    }
    
    int id;
    do {
        printf("ID zona: ");
        id = leerNumero();
        if (buscarZona(id) != -1) 
        printf("ID %d ya existe.\n", id);
    } while (buscarZona(id) != -1);
    
    zonas[total_zonas].id = id;
    printf("Nombre zona: ");
    leerTexto(zonas[total_zonas].nombre, MAX_NOMBRE);
    zonas[total_zonas].num_historicos = 0;
    zonas[total_zonas].alerta = 0;
    total_zonas++;
    printf("Zona agregada.\n");
}

// Registrar datos
void registrarDatos() {
    if (total_zonas == 0) {
        printf("No hay zonas.\n");
        return;
    }
    
    printf("ID zona: ");
    int pos = buscarZona(leerNumero());
    if (pos == -1) {
        printf("Zona no encontrada.\n");
        return;
    }
    
    printf("CO2 (ppm): ");
    zonas[pos].co2 = leerNumero();
    printf("SO2 (ug/m3): ");
    zonas[pos].so2 = leerNumero();
    printf("NO2 (ug/m3): ");
    zonas[pos].no2 = leerNumero();
    printf("PM2.5 (ug/m3): ");
    zonas[pos].pm25 = leerNumero();
    
    printf("Temperatura (°C): ");
    zonas[pos].temp = leerNumero();
    printf("Viento (km/h): ");
    zonas[pos].viento = leerNumero();
    printf("Humedad (%%): ");
    zonas[pos].humedad = leerNumero();
    
    // Agregar a históricos
    if (zonas[pos].num_historicos < MAX_HISTORICOS) {
        int n = zonas[pos].num_historicos;
        zonas[pos].historicos[n][0] = zonas[pos].co2;
        zonas[pos].historicos[n][1] = zonas[pos].so2;
        zonas[pos].historicos[n][2] = zonas[pos].no2;
        zonas[pos].historicos[n][3] = zonas[pos].pm25;
        zonas[pos].num_historicos++;
    } else {
        // ERROR: Desplazar sin verificar límites
        for (int i = 0; i < MAX_HISTORICOS; i++) {  // Debería ser MAX_HISTORICOS-1
            for (int j = 0; j < 4; j++) {
                zonas[pos].historicos[i][j] = zonas[pos].historicos[i+1][j];
            }
        }
        // Agregar nuevo dato al final
        zonas[pos].historicos[MAX_HISTORICOS-1][0] = zonas[pos].co2;
        zonas[pos].historicos[MAX_HISTORICOS-1][1] = zonas[pos].so2;
        zonas[pos].historicos[MAX_HISTORICOS-1][2] = zonas[pos].no2;
        zonas[pos].historicos[MAX_HISTORICOS-1][3] = zonas[pos].pm25;
    }
    
    // Verificar alertas
    zonas[pos].alerta = (zonas[pos].co2 > LIMITE_CO2 || zonas[pos].so2 > LIMITE_SO2 ||
                        zonas[pos].no2 > LIMITE_NO2 || zonas[pos].pm25 > LIMITE_PM25);
    
    printf("Datos registrados\n");
}

// Mostrar estado actual
void mostrarEstado() {
    if (total_zonas == 0) {
        printf("No hay zonas.\n");
        return;
    }
    
    printf("\n      ESTADO ACTUAL       \n");
    printf("ID  Zona                 CO2    SO2    NO2    PM2.5  Indice  Alerta\n");
    
    for (int i = 0; i < total_zonas; i++) {
        float indice = calcularIndice(zonas[i]);
        printf("%-3d %-20s %-6.1f %-6.1f %-6.1f %-6.1f %-7.1f %s\n",
               zonas[i].id, zonas[i].nombre, zonas[i].co2, zonas[i].so2,
               zonas[i].no2, zonas[i].pm25, indice, 
               zonas[i].alerta ? "SI" : "NO");
    }
}

// Generar predicciones
void generarPredicciones() {
    if (total_zonas == 0) {
        printf("No hay zonas.\n");
        return;
    }
    
    printf("\n      PREDICCIONES        \n");
    for (int i = 0; i < total_zonas; i++) {
        printf("\nZona: %s (ID: %d)\n", zonas[i].nombre, zonas[i].id);
        predecir(&zonas[i]);
    }
}

// Calcular promedios usando función con punteros
void calcularPromedios() {
    printf("\n      PROMEDIOS HISTORICOS  \n");
    
    for (int i = 0; i < total_zonas; i++) {
        if (zonas[i].num_historicos == 0) {
            printf("Zona %s: Sin datos.\n", zonas[i].nombre);
            continue;
        }
        
        // Usar función con punteros para calcular promedios
        float valores_co2[MAX_HISTORICOS], valores_so2[MAX_HISTORICOS];
        float valores_no2[MAX_HISTORICOS], valores_pm25[MAX_HISTORICOS];
        
        // Extraer valores de cada contaminante
        for (int j = 0; j < zonas[i].num_historicos; j++) {
            valores_co2[j] = zonas[i].historicos[j][0];
            valores_so2[j] = zonas[i].historicos[j][1];
            valores_no2[j] = zonas[i].historicos[j][2];
            valores_pm25[j] = zonas[i].historicos[j][3];
        }
        
        float prom_co2, prom_so2, prom_no2, prom_pm25;
        
        // Calcular promedios usando punteros
        calcularPromedio(valores_co2, zonas[i].num_historicos, &prom_co2);
        calcularPromedio(valores_so2, zonas[i].num_historicos, &prom_so2);
        calcularPromedio(valores_no2, zonas[i].num_historicos, &prom_no2);
        calcularPromedio(valores_pm25, zonas[i].num_historicos, &prom_pm25);
        
        printf("\n%s - CO2: %.1f, SO2: %.1f, NO2: %.1f, PM2.5: %.1f\n",
               zonas[i].nombre, prom_co2, prom_so2, prom_no2, prom_pm25);
    }
}

// Recomendaciones
void mostrarRecomendaciones() {
    printf("\n      RECOMENDACIONES         \n");
    int hay_alertas = 0;
    
    for (int i = 0; i < total_zonas; i++) {
        if (zonas[i].alerta) {
            hay_alertas = 1;
            printf("\nZONA CRITICA: %s\n", zonas[i].nombre);
            if (zonas[i].co2 > LIMITE_CO2) printf("- Reducir trafico vehicular\n");
            if (zonas[i].so2 > LIMITE_SO2) printf("- Revisar emisiones industriales\n");
            if (zonas[i].no2 > LIMITE_NO2) printf("- Promover transporte publico\n");
            if (zonas[i].pm25 > LIMITE_PM25) printf("- Evitar actividades al aire libre\n");
        }
    }
    
    if (!hay_alertas) {
        printf("Sin alertas. Recomendaciones generales:\n");
        printf("- Mantenimiento vehicular\n- Transporte sostenible\n- Mas areas verdes\n");
    }
}

// Guardar datos
void guardarDatos() {
    FILE *f = fopen(ARCHIVO_DATOS, "w");
    if (!f) {
        printf("Error al guardar.\n");
        return;
    }
    
    fprintf(f, "%d\n", total_zonas);
    for (int i = 0; i < total_zonas; i++) {
        fprintf(f, "%d %s %.2f %.2f %.2f %.2f %d\n",
                zonas[i].id, zonas[i].nombre, zonas[i].co2, zonas[i].so2,
                zonas[i].no2, zonas[i].pm25, zonas[i].num_historicos);
    }
    fclose(f);
    printf("Datos guardados.\n");
}

// Cargar datos
void cargarDatos() {
    FILE *f = fopen(ARCHIVO_DATOS, "r");
    if (!f) {
        printf("Sin archivo previo.\n");
        return;
    }
    
    fscanf(f, "%d", &total_zonas);
    for (int i = 0; i < total_zonas; i++) {
        fscanf(f, "%d %s %f %f %f %f %d",
               &zonas[i].id, zonas[i].nombre, &zonas[i].co2, &zonas[i].so2,
               &zonas[i].no2, &zonas[i].pm25, &zonas[i].num_historicos);
        zonas[i].alerta = 0;
    }
    fclose(f);
    printf("Datos cargados.\n");
}

// Función principal
int main() {
    int opcion;
    printf(COLOR_COMPLETO);
    printf("        SISTEMA DE CONTAMINACION                            \n");
    cargarDatos();
    
    do {
        mostrarMenu();
        opcion = leerNumero();
        
        switch (opcion) {
            case 1: 
            system("clear");
            agregarZona(); break;
            case 2:
            system("clear");
            registrarDatos(); break;
            case 3: 
            system("clear");
            mostrarEstado(); break;
            case 4: 
            system("clear");
            generarPredicciones(); break;
            case 5: 
            system("clear");
            calcularPromedios(); break;
            case 6: 
            system("clear");
            mostrarRecomendaciones(); break;
            case 7: 
            system("clear");
            guardarDatos(); break;
            case 8: 
            system("clear");
            cargarDatos(); break;
            case 9: 
            system("clear");
            guardarDatos(); 
            printf("Adios!\n"); break;
            default: printf("Opcion invalida.\n");
        }
    } while (opcion != 9);
    
    return 0;
}
