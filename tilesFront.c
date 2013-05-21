#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tilesBack.h"
#include "getnum.h"
#define MIN_DIM 3

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
	nPunto.x = 0;
	nPunto.y = 0;
	imprimirTablero(juego);
	printf("Azulejos eliminados: %d\n", eliminar(juego->tablero[nPunto.y][nPunto.x], nPunto, juego));
	imprimirTablero(juego);
	
	//freeMemory(juego);
	
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
		printf("%d) ", i);
		for (j = 0; j < juego->ancho; j++)
			putchar((juego->tablero[i][j] > 0) ? juego->tablero[i][j] + 'A' : ' ');
			
		putchar('\n');
	}
}

void
limpiarConsola()
{
	int i;
	
	for (i = 0; i < 10; i++)
		putchar('\n');
}
