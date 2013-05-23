#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tilesBack.h"
#include "getnum.h"
#define MIN_DIM 3
#define VALIDAR_AUX(x) ((x) != ' ' && (x) != '\n')

/* Enums */
enum {JUEGO_NUEVO = 1, JUEGO_BITACORA, RECUPERAR, TERMINAR};

/* Structs */


/* Funciones */
void limpiarConsola(); // Limpia la consola
void analizarOpcion(int opcion, tJuego * juego); // Actua de acuerdo a la opcion pasada
void pedirDimensiones(tJuego * juego);
void pedirNiveles(tJuego * juego);
void imprimirTablero(tJuego * juego);
int menuNuevo(); // Crea menu nuevo y devuelve opcion elegida
int comenzarJuego(); // Comienza el juego
int juegoNuevo();
int juegoBitacora();
int recuperar();

int
main()
{
  int o;
	tJuego juego;
	
	srand(time(NULL));
	
	do
	{
		o = menuNuevo();
		analizarOpcion(o, &juego);
	} while (o != TERMINAR); 
		
	return 0;
}

int
menuNuevo()
{
	int o, i;
	char * txt[] = {
		"Bienvenido!\n",
		"Seleccione una de las siguientes opciones\n",
		"(1) Juego nuevo\n",
		"(2) Juego con bitacora\n",
		"(3) Recuperar juego\n",
		"(4) Terminar\n"
	};
	
	limpiarConsola();
	
	printf("######################\n");
	printf("------ AZULEJOS ------\n");
	printf("######################\n");
	putchar('\n');
	
	/* Impresion de menu */
	for (i = 0; i < 6; i++)
		printf("%s", txt[i]);

	/* Seleccion de opcion */
	o = getint("Ingrese el numero de la opcion elegida: ");
	while (o < 1 || o > 4)
		o = getint("Opcion no valida, ingrese la opcion nuevamente: ");
	
	return o;
}

void
analizarOpcion(int opcion, tJuego * juego)
{
	switch (opcion)
	{
		case JUEGO_NUEVO:
			pedirDimensiones(juego);
			pedirNiveles(juego);
			juegoNuevo(juego);
			break;
		case JUEGO_BITACORA:
			juegoBitacora();
			break;
		case RECUPERAR:
			recuperar();
			break;
		case TERMINAR:
			// Permite continuar el programa para finalizar
			break;
		default:
			printf("Opcion invalida\n");
			break;
	}
}

void
pedirDimensiones(tJuego * juego)
{
	int ancho, alto;
	
	ancho = getint("Ingresar el ancho del tablero: ");
	while (ancho <= MIN_DIM)
		ancho = getint("Volver a ingresar el ancho del tablero: ");
	
	alto = getint("Ingresar el alto del tablero: ");
	while (alto <= MIN_DIM)
		alto = getint("Volver a ingresar el alto del tablero: ");
		
	juego->ancho = ancho;
	juego->alto = alto;
}

void
pedirNiveles(tJuego * juego)
{
	int niveles;
	
	niveles = getint("Ingresar niveles: ");
	while (niveles >= (juego->alto * juego->ancho / 2) || niveles <= 8)
		niveles = getint("Volver a ingresar niveles: ");
		
	juego->niveles = niveles;
}

int
juegoNuevo(tJuego * juego)
{
	tPunto nPunto;
	
	juego->tablero = crearTablero(juego, 1);
	
	imprimirTablero(juego);
	
	while (1) // TODO: Reemplazar 1 por checkearTablero()
	{
		verificarJugada(juego);
		reacomodarTablero(juego);
		imprimirTablero(juego);
	}
	
	return 0;
}

int
juegoBitacora()
{
	return 0;
}

int
recuperar()
{
	return 0;
}

void
imprimirTablero(tJuego * juego)
{
	int i, j;
	
	printf("Tablero: \n");
	
	/* Imprime la hilera de las posiciones de las columnas */
	printf("  ");
	
	for (i = 0; i < juego->ancho; i++)
	{
		if (i % 2 == 0)
			(i < 10) ? printf(" %d", i) : printf("%d", i);
		else
			printf("  ");
	}
	
	putchar('\n');
	
	for (i = 0; i < juego->alto; i++)
	{
		/* Imprime la columna de las posiciones de las hileras */
		(i < 10) ? printf("\033[0m %d", i) : printf("\033[0m%d", i);
		
		/* Imprime los valores del tablero */
		for (j = 0; j < juego->ancho; j++)
			printf("\033[%dm %c", juego->tablero[i][j] + 31, juego->tablero[i][j] + '0');
			
		printf("\033[0m\n");
	}
}

int
pedirJugada()
{
	char accionAux[6] = {0}, i = 0, j, c, esIgual, len = 0;	
	char acciones[][5]={"e", "m", "c", "h", "undo", "save", "quit"};

	/* c contiene siempre el último caracter */
	while ((c = getchar()) != ' ' && c != '\n')
	{	
		/* */
		if (len > 4 && VALIDAR_AUX(c))
			return 0;	
	
		accionAux[len++] = c;
	}

	/* El argumento leído no tiene longitud 1 ni 4 */
	if (len != 1 && len != 4)
		return 0;

	/* Comparación del argumento leído con las acciones válidas */
	for (i = 0; i < 7; i++)
	{
		esIgual = 1;
		
		for (j = 0; accionAux[j] != '\0' && esIgual; j++)
		{
			/* Basta con un caracter distinto para descartar una opcion */
			if (acciones[i][j] != accionAux[j])
				esIgual = 0;
		}
		
		if (esIgual)
		{
			/* Validando el save*/
			if (i == 5 && c != ' ')
				return 0;

			/* Validando undo y quit */
			if ((i == 4 || i == 6) && c != '\n')
				return 0;
		
			return i + 1;
		}
	}

}
			
int
verificarJugada(tJuego * juego)
{
	int estado, jugada, argumentos, azulejosEliminados = 0;
	tPunto punto;
	
	enum {ERROR, ELIMINAR, MARTILLAZO, COLUMNA, HILERA, UNDO, SAVE, QUIT};

	do
	{
		printf("Ingresar accion:\n");
		jugada = pedirJugada();
	} while (jugada == ERROR);
	
	switch(jugada)
	{
		case ELIMINAR:
			azulejosEliminados = eliminarWrapper(juego);
			
			break;
		case MARTILLAZO:
			azulejosEliminados = martillazoWrapper(juego);
			
			break;
		case COLUMNA:
			azulejosEliminados = columnaWrapper(juego);
			break;
		case HILERA:
			azulejosEliminados = hileraWrapper(juego);
			break;
		case UNDO:
			azulejosEliminados = columnaWrapper(juego);
			break;
		case SAVE:
		
			break;
		case QUIT:
			
			break;
		default:
			printf("Jugada no reconocida\n");
			break;
	}
	
	printf("Azulejos eliminados: %d\n", azulejosEliminados);
}

int
eliminarWrapper(tJuego * juego)
{
	int x, y, argumentos = 0, estado, azulejosEliminados;
	tPunto punto;
	
	argumentos = scanf("%d, %d", &x, &y);
	
	if (argumentos == 2) /* TODO: fix validacion */
	{
		estado = validarPunto(x, y, juego);

		switch (estado)
		{
			case PUNTO_VALIDO:
				punto.x = x;
				punto.y = y;
				azulejosEliminados = eliminar(juego->tablero[y][x], punto, juego);
				break;
			case PUNTO_VACIO:
				printf("El punto (%d, %d) esta vacio\n", x, y);
				break;
			case FUERA_RANGO:
				printf("El punto (%d, %d) no esta dentro del tablero\n",  x, y);
				break;
		}
	}
	
	return azulejosEliminados;
}

int
hileraWrapper(tJuego * juego)
{
	int y, argumentos, azulejosEliminados = 0, estado;
	
	argumentos = scanf("%d", &y);
	
	if (argumentos == 1)
	{
		azulejosEliminados = eliminarHilera(y, juego);
		
		if (azulejosEliminados == 0)
			printf("Imposible eliminar la fila %d\n", y);
	
	}
	
	return azulejosEliminados;
}

int
columnaWrapper(tJuego * juego)
{
	int x, argumentos, azulejosEliminados = 0;
	
	argumentos = scanf("%d", &x);
	
	if (argumentos == 1)
	{
		printf("Antes de columna\n");
		azulejosEliminados = eliminarColumna(x, juego);
		
		if (azulejosEliminados == 0)
			printf("Imposible eliminar la columna %d\n", x);
	
	}
	
	return azulejosEliminados;
}

int
martillazoWrapper(tJuego * juego)
{
	int x, y, argumentos = 0, estado, azulejosEliminados;
	tPunto punto;
	
	argumentos = scanf("%d, %d", &x, &y);
	
			
	if (argumentos == 2)
	{
		estado = validarPunto(x, y, juego);
		
		switch (estado)
		{
			case PUNTO_VALIDO:
				punto.x = x;
				punto.y = y;
				azulejosEliminados = eliminarMartillazo(punto, juego);
				break;
			case PUNTO_VACIO:
				printf("El punto (%d, %d) esta vacio\n", x, y);
				break;
			case FUERA_RANGO:
				printf("El punto (%d, %d) no esta dentro del tablero\n",  x, y);
				break;
		}
	}
	
	return azulejosEliminados;
}

void
limpiarConsola()
{
	int i;
	
	for (i = 0; i < 10; i++)
		putchar('\n');
}
