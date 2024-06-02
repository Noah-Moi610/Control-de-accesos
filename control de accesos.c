#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Definición de los estados
#define ESTADO_Entrada 0
#define ESTADO_VALIDACION 1
#define ESTADO_PROFESOR 2
#define ESTADO_AULA 3
#define ESTADO_CALIFICACIONES 4
#define NUM_ESTADOS 5
#define SIMBOLOS 4

// Definición de la estructura de usuario
typedef struct {
    char nombre[50];
    char pin[6];
    int rol;
} Usuario;

// Definición de la estructura de calificaciones
typedef struct {
    char nombreEstudiante[50];
    int calificacion;
} Calificacion;

// Definicióm de base de datos de usuarios
// estudiante = '0', Profesor = '1'
Usuario usuarios[] = {
    {"Jorge", "17378", 1},
    {"Omahar", "54321", 0},
    {"Moi", "12345", 0},
    {"Damian", "78906", 0},
    {"Sebastian", "98541", 0},
    {"Elver", "00000", 0}
};

// Base de datos de calificaciones
Calificacion calificaciones[100]; 
int numCalificaciones = 0;

// Definición de tabla de transiciones
int transiciones[NUM_ESTADOS][SIMBOLOS] = {
    {1, 1, 1, 1},  // ESTADO_Entrada
    {1, 1, 2, 3},  // ESTADO_VALIDACION
    {0, 0, 4, 0},  // ESTADO_PROFESOR
    {0, 0, 3, 3},  // ESTADO_AULA
    {0, 0, 0, 0}   // ESTADO_CALIFICACIONES (profesor)
};

// Tabla de salidas
char salidas[NUM_ESTADOS][SIMBOLOS][50] = {
    {"", "", "", ""},
    {"Validando", "Pin no reconocido", "Validando", "Validando"},
    {"", "", "Bienvenido Profesor", ""},
    {"", "", "Bienvenido Profesor", "Bienvenido Estudiante"},
    {"", "", "Ingresar Calificaciones", ""}
};

// Función índice basado en la entrada
int obtenerIndiceResultados(char entrada[]) {
    if (strcmp(entrada, "pin") == 0)
        return 0;
    if (strcmp(entrada, "no_pin") == 0)
        return 1;
    if (strcmp(entrada, "profesor") == 0)
        return 2;
    if (strcmp(entrada, "estudiante") == 0)
        return 3;
    return -1;
}

int numUsuarios = sizeof(usuarios) / sizeof(usuarios[0]);

// Función de verificación (nombre y pin)
bool validarUsuario(const char *nombre, const char *pin, int *rol) {
    for (int i = 0; i < numUsuarios; i++) {
        if (strcmp(usuarios[i].nombre, nombre) == 0 && strcmp(usuarios[i].pin, pin) == 0) {
            *rol = usuarios[i].rol;
            return true;
        }
    }
    return false;
}

// Función para guardar las calificaciones
void guardarCalificaciones() {
    FILE *archivo = fopen("calificaciones.dat", "wb");
    if (archivo == NULL) {
        printf("Error al abrir el archivo para guardar las calificaciones.\n");
        return;
    }
    fwrite(&numCalificaciones, sizeof(int), 1, archivo);
    fwrite(calificaciones, sizeof(Calificacion), numCalificaciones, archivo);
    fclose(archivo);
}

// Función para cargar las calificaciones
void cargarCalificaciones() {
    FILE *archivo = fopen("calificaciones.dat", "rb");
    if (archivo == NULL) {
        printf("No se encontro un archivo de calificaciones previo. Se creará uno nuevo.\n");
        return;
    }
    fread(&numCalificaciones, sizeof(int), 1, archivo);
    fread(calificaciones, sizeof(Calificacion), numCalificaciones, archivo);
    fclose(archivo);
}

// Función para que el profesor ingrese las calificaciones
void ingresarCalificacionesProfesor() {
    char nombreEstudiante[50];
    int calificacion;

    printf("Ingresar las calificaciones de los estudiantes:\n");

    printf("Ingrese el nombre del estudiante: ");
    scanf("%s", nombreEstudiante);

    printf("Ingrese la calificación: ");
    scanf("%d", &calificacion);

    // Agregar la calificación a la base de datos
    strcpy(calificaciones[numCalificaciones].nombreEstudiante, nombreEstudiante);
    calificaciones[numCalificaciones].calificacion = calificacion;
    numCalificaciones++;

    printf("Calificacion ingresada correctamente.\n");
    guardarCalificaciones(); // Guardar las calificaciones después de ingresarlas
}

// Función para mostrar las calificaciones de un estudiante
void mostrarCalificacionesEstudiante(const char *nombreEstudiante) {
    printf("Calificaciones de %s:\n", nombreEstudiante);
    for (int i = 0; i < numCalificaciones; i++) {
        if (strcmp(calificaciones[i].nombreEstudiante, nombreEstudiante) == 0) {
            printf(" Calificacion: %d\n", calificaciones[i].calificacion);
        }
    }
}

int main() {
    cargarCalificaciones(); 

    char nombre[50];
    char pin[6];
    int rol;

    printf("Ingrese su nombre de usuario: ");
    scanf("%s", nombre);

    printf("Ingrese su PIN de 5 digitos: ");
    scanf("%5s", pin);

    int estadoActual = ESTADO_Entrada;

    // Validar nombre y pin
    bool nombreValido = false;
    bool pinValido = false;
    for (int i = 0; i < numUsuarios; i++) {
        if (strcmp(usuarios[i].nombre, nombre) == 0) {
            nombreValido = true;
        }
        if (strcmp(usuarios[i].pin, pin) == 0) {
            pinValido = true;
        }
        if (nombreValido && pinValido) {
            break;
        }
    }

    if (!nombreValido || !pinValido) {
        estadoActual = ESTADO_VALIDACION;
        int indiceSalida = obtenerIndiceResultados(!nombreValido ? "no_pin" : "pin");
        printf("%s\n", salidas[estadoActual][indiceSalida]);
        return 1;
    }

    if (!validarUsuario(nombre, pin, &rol)) {
        return 1;
    }

    estadoActual = transiciones[estadoActual][ESTADO_VALIDACION];
    printf("%s\n", salidas[estadoActual][rol == 0 ? 3 : 2]);

    if (rol == 0) {
        estadoActual = transiciones[estadoActual][3]; // Transición a ESTADO_AULA
        printf("%s\n", salidas[estadoActual][3]);
        // Mostrar calificaciones del estudiante
        mostrarCalificacionesEstudiante(nombre);
    } else if (rol == 1) {
        estadoActual = transiciones[estadoActual][2]; // Transición a ESTADO_PROFESOR
        printf("%s\n", salidas[estadoActual][2]);

        // Transición adicional para profesores
        estadoActual = transiciones[estadoActual][2]; // Transición a ESTADO_CALIFICACIONES
        printf("%s\n", salidas[estadoActual][2]);

        char configuracion;
        printf("¿Desea configurar las calificaciones? (S/N): ");
        scanf(" %c", &configuracion);

        if (configuracion == 'S' || configuracion == 's') {
            ingresarCalificacionesProfesor(); 
        } else {
            printf("No se configurarán las calificaciones.\n");
        }
    } else {
        printf("Error: Usuario no reconocido.\n");
        return 1;
    }

    return 0;
}
