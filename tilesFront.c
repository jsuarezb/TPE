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
	while (1 == 1)
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
	
	printf("El tablero es el siguiente: \n");
	printf("Ancho: %d\nAlto: %d\n", juego->ancho, juego->alto);
	
	for (i = 0; i < juego->alto; i++)
	{
		printf("\033[0m%d) ", i);
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
	int x, y, estado, jugada;
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
			/* Validacion de unicamente 2 coordenadas sin mas caracteres */
			if (scanf("%d, %d", &x, &y) == 2) /* TODO: fix validacion */
			{
				estado = validarPunto(x, y, juego);

				switch (estado)
				{
					case PUNTO_VALIDO:
						punto.x = x;
						punto.y = y;
						eliminar(juego->tablero[y][x], punto, juego);
						break;
					case PUNTO_VACIO:
						printf("El punto elegido esta vacio\n");
						break;
					case FUERA_RANGO:
						printf("El punto elegido no esta dentro del tablero\n");
						break;
				}
			}

			break;
	}
}


void
limpiarConsola()
{
	int i;
	
	for (i = 0; i < 10; i++)
		putchar('\n');
}
