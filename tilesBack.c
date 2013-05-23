#include <stdio.h>
#include <stdlib.h>
#include "tilesBack.h"
#define PUNTO_FUERA(x, y) ((x) < 0 || (x) >= (y))
#define PasaDeNivel 0
#define SigueJugando 1
#define GAMEOVER 2

/* Enums */
enum {E, H, C, M, UNDO, SAVE, QUIT};

/* Funciones */

char **
crearTablero(tJuego * juego, int nivel)
{
  int i, j;

	printf("Creando tablero de %dx%d con %d colores...\n\n", juego->alto, juego->ancho, nivel+1);
	/* Creacion de filas */
	juego->tablero = malloc(juego->alto * sizeof(char *));
	
	for (i = 0; i < juego->alto; i++)
	{
		/* Creacion de columnas */
		juego->tablero[i] = malloc(juego->ancho * sizeof(char));
		
		for (j = 0; j < juego->ancho; j++)
			juego->tablero[i][j] = rand() % (nivel + 3) + 1;

	}
	
	return juego->tablero;
	
}

/* eliminar(int color, tPunto punto, tJuego * juego)
** Modo de uso:
** azulejosEliminados = eliminar(1, punto, juego);
** Elimina los azulejos adyacentes al punto 'punto' de color '1'
** en el tablero de 'juego' y devuelve cantidad de azulejos eliminados
*/

int
eliminar(int color, tPunto punto, tJuego * juego)
{
	int i, j = 0;
	tPunto puntosAdyacentes[4];
	
	/* Validacion del punto */
	if (validarPunto(punto.x, punto.y, juego) != PUNTO_VALIDO)
		return 0;
	
	/* Eliminacion */
	if (juego->tablero[punto.y][punto.x] != color)
		return 0;
	else
	{
		juego->tablero[punto.y][punto.x] = 0;
		crearAdyacentes(punto, puntosAdyacentes);
	
		/* Eliminar si corresponde los puntos
		* izquierda - arriba - abajo - derecha */
		for (i = 0; i < 4; i++)
			j += eliminar(color, puntosAdyacentes[i], juego);
			
		return 1 + j;
	}
}

/*
**
*/

int
validarPunto(int x, int y, tJuego * juego)
{
	if (PUNTO_FUERA(x, juego->ancho) || PUNTO_FUERA(y, juego->alto))
		return FUERA_RANGO;

	if (juego->tablero[y][x] == 0)
		return PUNTO_VACIO;

	return PUNTO_VALIDO;
}

/* Elimina la hilera 'hilera' en el tablero de 'juego' y devuelve la 
** cantidad de azulejos eliminados
*/

int
eliminarHilera(int hilera, tJuego * juego)
{
	int i, hayAzulejoVacio = 1, azulejos = 0;
	
	/* Verifica que al menos un azulejo no este vacio en la hilera */
	for (i = 0; i < juego->ancho && hayAzulejoVacio; i++)
	{
		if (juego->tablero[hilera][i] == 1)
			hayAzulejoVacio = 0;
			
	}
	
	if (hayAzulejoVacio)
		return 0;
		
		
	/* Ejecuta la eliminacion de la hilera */
	for (i = 0; i < juego->ancho; i++)
	{
		if (juego->tablero[hilera][i] != 0)
		{
			juego->tablero[hilera][i] = 0;
			azulejos++;
		}
	}

	/*
	** TODO: Disminuir movimientos eliminarHilera
	*/
	
	return azulejos;
}

/* Elimina la columna 'columna' en el tablero de 'juego' */
int
eliminarColumna(int columna, tJuego * juego)
{
	int i, azulejos = 0;
	
	/* Verifica que el azulejo inferior no esté vacío.
	 * Tener en cuenta que ya se aplicó la gravedad al tablero
	 * pasado el movimiento anterior. */
	printf("Valor de (%d, %d) = %d\n", columna, juego->alto - 1, juego->tablero[juego->alto - 1][columna]);
	
	if (juego->tablero[juego->alto - 1][columna] == 0)
			return 0;
		
	/* Ejecuta la eliminacion de la hilera.
	 * Es necesario recorrerla toda, ya que el azulejo más
	 * a la derecha puede ser el único no vacío. */
	for (i = juego->alto - 1; i >= 0 && juego->tablero[i][columna] != 0; i--)
	{
		/* Eliminación de los azulejos no vacios hasta el primer vacío */
		juego->tablero[i][columna] = 0;
		azulejos++;
	}

	/*
	** TODO: Disminuir movimientos eliminarHilera
	*/
	
	return azulejos;
}

/*
** Elimina los alrededores del parametro 'punto' si es posible
*/
int
eliminarMartillazo(tPunto punto, tJuego * juego)
{
	int i, j, azulejosEliminados = 0;
	
	/* Se vacian los alrededores y el centro */
	for (i = punto.y - 1; i <= punto.y + 1; i++)
	{
		if (!PUNTO_FUERA(i, juego->alto))
		{
			for (j = punto.x - 1; j <= punto.x + 1; j++)
			{
				if (!PUNTO_FUERA(j, juego->ancho) && juego->tablero[i][j] != 0)
				{
					juego->tablero[i][j] = 0;
					azulejosEliminados++;
				}
			}
		}
	}
	
	/*
	** TODO: Disminuir en uno los movimientos martillazo
	*/
	
	return azulejosEliminados;
}

/*
**
*/
int
hayColorAdyacente(tPunto punto, tJuego * juego)
{
	int color, i, hayAdyacente = 0, x, y;
	tPunto adyacentes[4];
	
	if (juego->tablero[punto.y][punto.x] == 0
	|| PUNTO_FUERA(punto.x, juego->ancho) || PUNTO_FUERA(punto.y, juego->alto))
	{
		printf("La coordenada (%d, %d) no es valida. \n", punto.x, punto.y);
		return 0;
	}
	
	color = juego->tablero[punto.y][punto.x];
	
	crearAdyacentes(punto, adyacentes);
	
	for (i = 0; i < 4 && !hayAdyacente; i++)
	{
		x = adyacentes[i].x;
		y = adyacentes[i].y;
		
		/* Validacion del punto cuando si no esta dentro del tablero */
		if (PUNTO_FUERA(x, juego->ancho) && PUNTO_FUERA(y, juego->alto))
			continue;
		
		if (juego->tablero[y][x] == color)
			hayAdyacente = 1;
	}
	
	return hayAdyacente;
}

/* crea 4 puntos adyacentes a 'punto' y los deja en 'adyacentes' */
void
crearAdyacentes(tPunto punto, tPunto adyacentes[])
{
	int i;

	for (i = 0; i < 4; i++)
	{
		/* Simplificacion de las posiciones de x: {x-1, x, x, x+1} */
		adyacentes[i].x = punto.x + (i+1) / 2 - 1;
		
		/* Posiciones de y: {x, x-1, x+1, x} */
		switch (i)
		{
			case 0:
			case 3:
				adyacentes[i].y = punto.y;
				break;
			case 1:
				adyacentes[i].y = punto.y - 1;
				break;
			case 2:
				adyacentes[i].y = punto.y + 1;
				break;
		}
	}
}


void 
reacomodarTablero( tJuego * juego )
{
	int i, j;
	int ultimaPosX = -1;
	int ultimaPosY = juego->alto;

	/*
	** TODO: Hacerlo mas eficiente
	*/

	for (j = 0; j < juego->ancho; j++) 
	{
		ultimaPosY = juego->alto - 1;
		
		/* Reacomoda los Azulejos para abajo.*/
		for (i = juego->alto - 1; i >= 0; i--)
		{
			if ( juego->tablero[i][j] != 0)
			{	
				if (i + 1 < juego->alto && juego->tablero[i+1][j] == 0)
				{
					/* Caida de azulejo */
					juego->tablero[ultimaPosY][j] = juego->tablero[i][j];
					juego->tablero[i][j] = 0;
				}
			
				ultimaPosY--;
			}
		}
		
		/* Movimiento hacia la izquierda de las columnas */
		if (juego->tablero[juego->alto - 1][j] != 0)
		{
			if (j - 1 >= 0 && juego->tablero[juego->alto - 1][j - 1] == 0)
				correrColumna(j, ultimaPosX + 1, juego);

			ultimaPosX++;
		}
	}
}
/* Modo de uso: 
** Reacomoda una columna a la derecha de 
** la ultima columna no vacia.
*/ 
static void 
correrColumna(int posAnterior, int posNueva, tJuego * juego)
{
	int i;

	for (i = juego->alto-1; i >= 0 && juego->tablero[i][posAnterior] != 0; i--)
	{
		juego->tablero[i][posNueva] = juego->tablero[i][posAnterior];
		juego->tablero[i][posAnterior] = 0;
	}
}	

int
verificaMatriz(tJuego * juego)
{
	int i, j;
	tPunto punto;	

	
	if( juego->tablero[juego->alto - 1][0] == 0)
		return PasaDeNivel;

	for( j = 0; j < juego->ancho; j++)
	{
		for( i = juego->alto-1; i >= 0; i--)
		{
			punto.x = i;
			punto.y = j;
		
			if(  hayColorAdyacente( punto, juego ) )
			{
				printf("Se Puede Seguir Jugando");
				return SigueJugando;
			}
			//validar columnas martillos e hileras
		}
	}
	if ( hayPoderes(juego) )
		return SigueJugando;
	else 
		return GAMEOVER;
}

int 
hayPoderes(tJuego * juego)
{
	int estado = 0;

	if( juego->movHileras != 0 || juego->movColumnas != 0 || juego->movMartillazos != 0)
		estado = 1;
	
	return estado;
}

