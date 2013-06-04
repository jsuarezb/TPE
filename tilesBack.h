/*
*
* Funciones del juego Azulejos conteniendo las funciones
* correspondientes al back-end del juego
* 
*/
 
#ifndef _tilesBack_h
#define _tilesBack_h

#include <stdio.h>
#define MAX_CHARS 80

enum {PUNTO_VALIDO, PUNTO_VACIO, FUERA_RANGO};
enum {PROXIMO_NIVEL, SEGUIR_JUGANDO, GAME_OVER, VICTORIA};

typedef struct
{
	int puntos;
	int movHileras;
	int movColumnas;
	int movMartillazos;
	char ** tableroUndo;
} tJuegoUndo;

typedef struct
{
	int ancho;
	int alto;
	int nivelMaximo;
	int nivelActual;
	int puntos;
	int movHileras;
	int movColumnas;
	int movMartillazos;
	int cantJugadas;
	char conBitacora;
	char ** tablero;
	char nombreJuego[MAX_CHARS];
	FILE * bitacora;
	tJuegoUndo juegoUndo;
} tJuego;

typedef struct
{
	int x;
	int y;
} tPunto;

typedef struct
{
	tPunto punto;
	int azulejosEliminados;
} tJugada;


/*
* Funcion que crea un tablero con las especificaciones dadas
* de (alto) x (ancho) dimensiones con nivel colores
* Modo de uso: crearTablero(juego);
*/ 
void crearTablero(tJuego * juego);

void generarTablero(tJuego * juego);

/* Elimina los azulejos adyacentes al punto 'punto' de color '1'
** en el tablero de 'juego'
** Modo de uso:
** eliminar(1, punto, juego);
*/
int eliminar(int color, tPunto punto, tJuego * juego);

/* Elimina la hilera 'hilera' en el tablero de 'juego' si es posible,
** convirtiendo todos los azulejos de la hilera a 0 y devuelve la cantidad
** de azulejos eliminados
** Modo de uso:
** azulejosEliminados = eliminarHilera(2, juego)
*/
int eliminarHilera(int hilera, tJuego * juego);

/* Elimina la columna 'columna' en el tablero de 'juego' si es posible,
** convirtiendo todos los azulejos de la columna a 0 y devuelve la cantidad
** de azulejos eliminados
** Modo de uso:
** azulejosEliminados = eliminarColumna(4, juego)
*/
int eliminarColumna(int columna, tJuego * juego);

/* */
int eliminarMartillazo(tPunto punto, tJuego * juego);

/*
** Devuelve si existe algun azulejo adyacentes al azulejo en 'punto' 
** con el mismo color del azulejo en 'punto'
** Modo de uso:
** if (hayColorAdyacente(punto, juego))
** 		// doSomething
**
*/
int hayColorAdyacente(tPunto punto, tJuego * juego);

void reacomodarTablero(tJuego * juego);

int validarPunto(int x, int y, tJuego * juego);

int verificaMatriz(tJuego * juego);

int calcularPuntos(int azulejosEliminados, tJuego * juego);

int guardarJuego(char * nombreArchivo, tJuego * juego);

void initJuego(tJuego * juego);

void bonus(tJuego * juego);

void recuperarJuego(FILE * partidaGuardada, tJuego * juego);

void undo(tJuego * juego);

void juegoUndo(tJuego * juego);

void liberarTablero(char ** tablero, int alto);

#endif
