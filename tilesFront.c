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
tJugada eliminarWrapper(tJuego * juego);
tJugada columnaWrapper(tJuego * juego);
tJugada hileraWrapper(tJuego * juego);
tJugada martillazoWrapper(tJuego * juego);


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
			juego->conBitacora=1;
			pedirDimensiones(juego);
			pedirNiveles(juego);
			juegoNuevo(juego);/**/
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
	int estadoTablero = PROXIMO_NIVEL;
	tPunto nPunto;
	
	/* Crear un tJuego para el juego nuevo seteando todas sus 
	 * variables a los valores iniciales */
	juego->puntos = 0;
	/* Siendo 1 el primer nivel (aumenta al crear tablero nuevo) */
	juego->nivelActual = 0; 
	juego->movHileras = 1;
	juego->movColumnas = 1;
	juego->movMartillazos = 1;
	
	while (estadoTablero != GAME_OVER) // TODO: Reemplazar 1 por checkearTablero()
	{
		/* Pasar al proximo nivel */
		if (estadoTablero == PROXIMO_NIVEL)
		{
			limpiarConsola();
			
			juego->nivelActual++;
			
			printf("Nivel %d\n", juego->nivelActual);
			juego->tablero = crearTablero(juego);
		}
		
		imprimirTablero(juego);
		puntos(hacerJugada(juego),juego);
		//hacerJugada(juego);
		reacomodarTablero(juego);
		estadoTablero = verificaMatriz(juego);
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

/*char
acciones(int jugadaValidada)
{
	char accion[4] = {"emch"};
	return accion[jugadaValidada-1];
}*/
			
int
hacerJugada(tJuego * juego)
{
	int estado, jugadaValidada, argumentos, azulejosEliminados = 0;
	tJugada jugada;
	
	enum {ERROR, ELIMINAR, MARTILLAZO, COLUMNA, HILERA, UNDO, SAVE, QUIT};

	do
	{
		printf("Ingresar accion:\n");
		jugadaValidada = pedirJugada();
		if (juego->conBitacora)
		{
			char accion[4] = {"emch"};
			printf("%c %d, %d;%d \n", accion[jugadaaValidada-1], jugada.punto.x, jugada.punto.y, jugada.azulejosEliminados);
		}
	} while (jugadaValidada == ERROR);
	
		switch(jugadaValidada)
		{
			case ELIMINAR:
				jugada = eliminarWrapper(juego);
				break;
			case MARTILLAZO:
				jugada = martillazoWrapper(juego);
				break;
			case COLUMNA:
				jugada = columnaWrapper(juego);
				break;
			case HILERA:
				jugada = hileraWrapper(juego);
				break;
			case UNDO:
			
				break;
			case SAVE:
			
				break;
			case QUIT:
				// Retorna
				break;
			default:
				printf("Jugada no reconocida\n");
				break;
		}
	
	printf("Azulejos eliminados: %d\n", jugada.azulejosEliminados);
	
	return jugada.azulejosEliminados;
}

tJugada
eliminarWrapper(tJuego * juego)
{
	int x, y, argumentos = 0, estado, azulejosEliminados;
	
	tJugada jugada;
	
	argumentos = scanf("%d, %d", &y, &x);
	
	if (argumentos == 2) /* TODO: fix validacion */
	{
		estado = validarPunto(x, y, juego);

		switch (estado)
		{
			case PUNTO_VALIDO:
				jugada.punto.x = x;
				jugada.punto.y = y;
				jugada.azulejosEliminados = eliminar(juego->tablero[y][x], jugada.punto, juego);
				break;
			case PUNTO_VACIO:
				printf("El punto (%d, %d) esta vacio\n", x, y);
				break;
			case FUERA_RANGO:
				printf("El punto (%d, %d) no esta dentro del tablero\n",  x, y);
				break;
		}
	}
	
	return jugada;
}

tJugada
hileraWrapper(tJuego * juego)
{
	tJugada jugada;
	int y, argumentos, estado;
	
	argumentos = scanf("%d", &y);
	
	if (argumentos == 1)
	{
		jugada.punto.x = -1; //Indicando que no hay coordenada X 
		jugada.punto.y = y;
		jugada.azulejosEliminados = eliminarHilera(y, juego);
		
		if (jugada.azulejosEliminados == 0)
			printf("Imposible eliminar la fila %d\n", y);
	
	}
	
	return jugada;
}

tJugada
columnaWrapper(tJuego * juego)
{
	tJugada jugada;
	int x, argumentos;
	
	argumentos = scanf("%d", &x);
	
	if (argumentos == 1)
	{
		printf("Antes de columna\n");
		jugada.punto.x = x;
		jugada.punto.y = -1; //Indicando que no hay coordenada y
		jugada.azulejosEliminados = eliminarColumna(x, juego);
		
		if (jugada.azulejosEliminados == 0)
			printf("Imposible eliminar la columna %d\n", x);
	
	}
	
	return jugada;
}

tJugada
martillazoWrapper(tJuego * juego)
{
	int x, y, argumentos = 0, estado, azulejosEliminados;
	tJugada jugada;
	
	argumentos = scanf("%d, %d", &x, &y);
	
			
	if (argumentos == 2)
	{
		estado = validarPunto(x, y, juego);
		
		switch (estado)
		{
			case PUNTO_VALIDO:
				jugada.punto.x = x;
				jugada.punto.y = y;
				jugada.azulejosEliminados = eliminarMartillazo(jugada.punto, juego);
				break;
			case PUNTO_VACIO:
				printf("El punto (%d, %d) esta vacio\n", x, y);
				break;
			case FUERA_RANGO:
				printf("El punto (%d, %d) no esta dentro del tablero\n",  x, y);
				break;
		}
	}
	
	return jugada;
}

void
guardarWrapper(tJuego * juego)
{
	char * nombreArchivo = calloc(MAX_NOMBRE, sizeof(char)), c;
	int i, estado, argumentos;
	
	/* Validacion del nombre del archivo */
	while ((c = getchar()) != '\n')
	{
		if (i > MAX_NOMBRE)
		{
			printf("La longitud maxima del nombre es %d...\n", MAX_NOMBRE);
			free(nombreArchivo);
			
			return;
		}
		
		nombreArchivo[i] = c;
		
		i++;
	}

	/* Guardar el juego */
	estado = guardarJuego(nombreArchivo, juego);
		
	if (estado)
		printf("%s guardado correctamente\n", nombreArchivo);
	else
		printf("Hubo un problema al guardar el juego\n");
		
	free(nombreArchivo);
	
	return;
}

void
limpiarConsola()
{
	int i;
	
	for (i = 0; i < 10; i++)
		putchar('\n');
}
