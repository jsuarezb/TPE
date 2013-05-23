/*
*
* Funciones del juego Azulejos conteniendo las funciones
* correspondientes al back-end del juego
* 
*/
 
#ifndef _tilesBack_h
#define _tilesBack_h

enum {PUNTO_VALIDO, PUNTO_VACIO, FUERA_RANGO};

typedef struct
{
  int ancho;
	int alto;
	int niveles;
	int nivelMaximo;
	int nivelActual;
	int puntos;
	int movHileras;
	int movColumnas;
	int movMartillazos;
	char conBitacora;
	char ** tablero;
} tJuego;

typedef struct
{
	int x;
	int y;
} tPunto;

/*
* Funcion que crea un tablero con las especificaciones dadas
* de (alto) x (ancho) dimensiones con nivel colores
* Modo de uso: crearTablero(5, 5, 2, tablero)
*/ 
char ** crearTablero(tJuego * juego, int nivel);

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

/*
** Crea un arreglo de puntos adyacentes al punto dado
** Modo de uso:
** crearAdyacentes(punto, puntos[4]);
** crea los puntos:
** puntos[0] = {puntos.x - 1, punto.y}
** puntos[1] = {puntos.x, punto.y - 1}
** puntos[2] = {puntos.x, punto.y + 1}
** puntos[3] = {puntos.x + 1, punto.y}
*/
void crearAdyacentes(tPunto punto, tPunto adyacentes[]);

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

static void correrColumna(int posAnterior, int posNueva, tJuego * juego);

int validarPunto(int x, int y, tJuego * juego);

int verificaMatriz(tJuego * juego);

int hayPoderes(tJuego * juego);

int puntos(int azulejosEliminados, tJuego * juego);

#endif
