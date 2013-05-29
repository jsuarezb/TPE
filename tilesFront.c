#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tilesBack.h"
#include "getnum.h"
#include <string.h>
#define MIN_DIM 3
#define MAX_DIM 40
#define MAX_NOMBRE 50
#define VALIDAR_AUX(x) ((x) != ' ' && (x) != '\n')

/* Enums */
enum {JUEGO_NUEVO = 1, JUEGO_BITACORA, RECUPERAR, TERMINAR};
enum {ERROR, ELIMINAR, MARTILLAZO, COLUMNA, HILERA, UNDO, SAVE, QUIT};
/* TODO: lograr enviar el QUIT a -1 */

/* Structs */


/* Funciones */
void limpiarConsola(); // Limpia la consola
void pedirDimensiones(tJuego * juego);
void pedirNiveles(tJuego * juego);
void imprimirTablero(tJuego * juego);
void analizarOpcion(int opcion, tJuego * juego); // Actua de acuerdo a la opcion pasada
int menuNuevo(); // Crea menu nuevo y devuelve opcion elegida
void comenzarJuego(); // Comienza el juego
void recuperar(tJuego * juego);
int hacerJugada(tJuego * juego);
tJugada eliminarWrapper(tJuego * juego);
tJugada columnaWrapper(tJuego * juego);
tJugada hileraWrapper(tJuego * juego);
tJugada martillazoWrapper(tJuego * juego);
void guardarWrapper(tJuego * juego);


int
main()
{
	int o;
	tJuego juego;
	
	srand(time(NULL));
	
	do
	{
		initJuego(&juego);
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
	
	printf("\n\n");
	printf("######################\n");
	printf("------ AZULEJOS ------\n");
	printf("######################\n");
	printf("\n\n");
	
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
			juego->conBitacora = 0;
			
			pedirDimensiones(juego);
			pedirNiveles(juego);
			juego->tablero = crearTablero(juego);
			comenzarJuego(juego);
			break;
		case JUEGO_BITACORA:
			
			juego->conBitacora = 1;
			juego->cantJugadas = 0;
			juego->bitacora = fopen("Bitacora.txt", "wt");
			int i, j;
			
			pedirDimensiones(juego);
			pedirNiveles(juego);
			juego->tablero = crearTablero(juego);
			
			/* Copia el primer tablero a la bitacora */
			for (i = 0; i < juego->alto; i++)
			{
				for (j = 0; j < juego->ancho; j++)
					fprintf(juego->bitacora, "%d", juego->tablero[i][j]);		
				fprintf(juego->bitacora, "\n", NULL);
			}
			
			comenzarJuego(juego);
			break;
		case RECUPERAR:
			recuperar(juego);
			comenzarJuego(juego);
			break;
		case TERMINAR:
			printf("Gracias por jugar! Adios!\n");
			break;
		default:
			printf("Opcion invalida\n");
			break;
	}
	
	return;
}

void
pedirDimensiones(tJuego * juego)
{
	int ancho, alto;
	
	ancho = getint("Ingresar el ancho del tablero: ");
	while (ancho < MIN_DIM || ancho > MAX_DIM)
	{
		printf("Dimension invalida.\n");
		ancho = getint("Volver a ingresar: ");
	}
	
	alto = getint("Ingresar el alto del tablero: ");
	while (alto < MIN_DIM || ancho > MAX_DIM)
	{
		printf("Dimension invalida.\n");
		alto = getint("Volver a ingresar: ");
	}
		
	juego->ancho = ancho;
	juego->alto = alto;
}

void
pedirNiveles(tJuego * juego)
{
	int niveles;
	int azulejos = juego->ancho * juego->alto / 2;
	
	niveles = getint("Ingresar niveles: ");
	
	while (niveles < 2 || niveles > 8 || niveles >= azulejos)
	{
		printf("Nivel invalido.\n");
		niveles = getint("Volver a ingresar niveles: ");
	}
		
	juego->nivelMaximo = niveles;
}

void
comenzarJuego(tJuego * juego)
{
	int estadoTablero = SEGUIR_JUGANDO, pts = 0, resp = 0;

	while (estadoTablero != GAME_OVER)
	{		
				
		
		printf("\nNivel %d\n", juego->nivelActual);
			
		pts = calcularPuntos(resp, juego);
		juego->puntos += pts;
		
		printf("Puntos: %d\n", pts);	
		printf("Acumulados: %d\n", juego->puntos);	
		printf("h: %d, ", juego->movHileras);
		printf("c: %d, ", juego->movColumnas);
		printf("m: %d\n", juego->movMartillazos);

		

		estadoTablero = verificaMatriz(juego);

		
		/* Pasar al proximo nivel */
		if (estadoTablero == PROXIMO_NIVEL)
		{	
			bonus(juego);
			juego->nivelActual++;
			
			juego->puntos = 0;
			pts = 0;
			resp = 0;
			
			juego->cantJugadas = 0;
			
			/* FIN DEL JUEGO (ganado) */
			if (juego->nivelActual > juego->nivelMaximo)
			{
				printf("Felicitaciones, ha ganado!\n");
				printf("Ha completado el nivel %d.\n", juego->nivelMaximo);
				if (juego->conBitacora)
					fclose(juego->bitacora);
				
				return;
			}
			

			juego->tablero = crearTablero(juego);
			
			/* Copia el tablero del siguiente nivel a la bitacora */
			if (juego->conBitacora)
			{
				int i, j;
				for (i = 0; i < juego->alto; i++)
				{
					for (j = 0; j < juego->ancho; j++)
						fprintf(juego->bitacora, "%d", juego->tablero[i][j]);
						
					fprintf(juego->bitacora, "\n", NULL);
				}
			}
			
		}
		else if (estadoTablero != GAME_OVER)
		{
			imprimirTablero(juego);
			
			resp = hacerJugada(juego);
			
			reacomodarTablero(juego);
			
			
			
			if (resp == -1) /* FIN DEL JUEGO (perdido) */
			{
				if (juego->conBitacora)
					fclose(juego->bitacora);
				return;
			}
		}	

	}
	
	printf("Lo siento, no quedan mas movimientos.\nHa perdido.");
	if (juego->conBitacora)
		fclose(juego->bitacora);
	return;
}

/* TODO: validar el fopen y fijarse si no es mas efectivo hacerlo con getchar*/
 
void
recuperar(tJuego * juego)
{	
	FILE * partidaGuardada;
	char * nombreArchivoAux;
	char nombreArchivo[MAX_NOMBRE];
	int lenArchivo;
	
	
	nombreArchivoAux = malloc(MAX_NOMBRE*sizeof(char));

	printf("Ingrese el nombre del archivo: ");
	
	fgets(nombreArchivoAux, MAX_NOMBRE, stdin);
	
	lenArchivo = (strlen(nombreArchivoAux))-1;
	
	/*Quita el "\n" de nombreArchivoAux para su uso en fopen*/

	strncpy(nombreArchivo, nombreArchivoAux, lenArchivo);

	partidaGuardada = fopen(nombreArchivo, "rb");	
	
	leerArchivo(partidaGuardada, juego);
	
	if (juego->conBitacora)
	{
		char nombreArchivoBit[lenArchivo + 4];
		strcpy(nombreArchivoBit, nombreArchivo);
		strcat(nombreArchivoBit, ".txt" );
		juego->bitacora = fopen(nombreArchivoBit, "a");
		/* Ver q onda con el juego->cantJugadas, se guarda? */
	}
	
	return;
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
			printf("\033[%dm %c", juego->tablero[i][j] + 30,
		(juego->tablero[i][j] > 0) ? 178 : ' ');
			
		printf("\033[0m\n");
	}

}

int
pedirJugada()
{
	char accionAux[6] = {0}, esIgual;	
	char acciones[][5]={"e", "m", "c", "h", "undo", "save", "quit"};
	int len = 0, j, c, i = 0;

	/* c contiene siempre el último caracter */
	while ((c = getchar()) != ' ' && c != '\n')
	{	
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
	
	/* Si se llega hasta este punto la funcion no reconocio el comando */
	return 0;
}

/* hacerJugada(tJuego * juego) retorna la cantidad de azulejos eliminados
 * por la jugada elegida o -1 en caso que el usuario ingrese quit*/
int
hacerJugada(tJuego * juego)
{
	char accion[4] = {"emch"};
	int jugadaValidada;
	tJugada jugada;
	
	jugada.azulejosEliminados = 0;
	
	do
	{
		printf("Ingresar accion:\n");
		jugadaValidada = pedirJugada();
	} while (jugadaValidada == ERROR);
	
	/* Crea una copia auxiliar del juego */

	if (jugadaValidada >= ELIMINAR && jugadaValidada <= HILERA)
		juegoUndo(juego);
	
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
			undo(juego);
			break;
		case SAVE:
			guardarWrapper(juego);
			break;
		case QUIT:
			return -1;
		default:
			printf("Jugada no reconocida\n");
			break;
	}
	
	if (juego->conBitacora)
	{	
		if (jugadaValidada == 5)
		{
			fprintf(juego->bitacora, "%d: ", juego->cantJugadas);
			fprintf(juego->bitacora, "undo \n", NULL);
		}
		
		if (jugadaValidada >= 1 && jugadaValidada <= 4)
		{
			fprintf(juego->bitacora, "%d: ", juego->cantJugadas);
			fprintf(juego->bitacora, "%c ", accion[jugadaValidada - 1]);
			/* Si es necesario imprimir dos coordenadas */
			if (jugada.punto.x != -1 && jugada.punto.y != -1)
				fprintf(juego->bitacora, "%d, %d", jugada.punto.y, jugada.punto.x);
			if (jugada.punto.x == -1)
				fprintf(juego->bitacora, "%d", jugada.punto.y);
			if (jugada.punto.y == -1)
				fprintf(juego->bitacora, "%d", jugada.punto.x);
				
			if(jugada.azulejosEliminados > 0)
				fprintf(juego->bitacora,"; %d \n", jugada.azulejosEliminados);
		}
		
		juego->cantJugadas++;
			
	}
	
	getchar(); /* Come el \n al final del buffer */

	return jugada.azulejosEliminados;
}



tJugada
eliminarWrapper(tJuego * juego)
{
	int x, y, argumentos = 0, estado;
	tPunto punto;
	tJugada jugada;
	
	jugada.azulejosEliminados = 0;
		
	argumentos = scanf("%d, %d", &y, &x);
	punto.x = x;
	punto.y = y;
	
	if (argumentos == 2) /* TODO: fix validacion */
	{		
		estado = validarPunto(x, y, juego);

		switch (estado)
		{
			case PUNTO_VALIDO:
				jugada.punto.x = x;
				jugada.punto.y = y;
		
				if (!hayColorAdyacente(punto, juego))
				{
					printf("El punto (%d, %d) no tiene colores ady.\n", x, y);
					return jugada;
				}
				
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
	int y, argumentos;
	tJugada jugada;
	
	jugada.azulejosEliminados = 0;
	
	if (juego->movHileras < 1)
		printf("No dispones de suficientes \"hileras\"\n");
	else
	{
		argumentos = scanf("%d", &y);
		
		if (argumentos == 1)
		{
			jugada.punto.x = -1; //Indicando que no hay coordenada X 
			jugada.punto.y = y; /** TODO: fix */
			jugada.azulejosEliminados = eliminarHilera(y, juego);
			
			if (jugada.azulejosEliminados == 0)
				printf("Imposible eliminar la hilera %d\n", y);

		}
	}
	
	return jugada;
}

tJugada
columnaWrapper(tJuego * juego)
{
	int x, argumentos;
	tJugada jugada;
	
	jugada.azulejosEliminados = 0;
	
	if (juego->movColumnas < 1)
		printf("No dispones de suficientes \"columnas\"\n");
	else
	{
		argumentos = scanf("%d", &x);
		
		if (argumentos == 1)
		{
			jugada.punto.x = x;
			jugada.punto.y = -1; //Indicando que no hay coordenada y
			jugada.azulejosEliminados = eliminarColumna(x, juego);
			
			if (jugada.azulejosEliminados == 0)
				printf("Imposible eliminar la columna %d\n", x);
		
		}
	}
	
	return jugada;
}

tJugada
martillazoWrapper(tJuego * juego)
{
	int x, y, argumentos = 0, estado;
	tJugada jugada;
	
	jugada.azulejosEliminados = 0;
	
	if (juego->movMartillazos < 1)
		printf("No dispones de suficientes \"martillazos\"\n");
	else
	{
		argumentos = scanf("%d, %d", &y, &x);
		
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
	}
	
	return jugada;
}

void
guardarWrapper(tJuego * juego)
{
	char * nombreArchivo = calloc(MAX_NOMBRE, sizeof(char)), c;
	int i = 0, estado;
	int validarRename = 0;
	
	/* Validacion del nombre del archivo */
	while ((c = getchar()) != '\n')
	{
		if (i > MAX_NOMBRE)
		{
			printf("La longitud maxima del nombre es %d: \n", MAX_NOMBRE);
			free(nombreArchivo);
			
			return;
		}
		
		nombreArchivo[i] = c;
		
		i++;
	}
		
	
	if (juego->conBitacora)
	{
		
		char nombreArchivoBit[i + 4];
		
		strcpy(nombreArchivoBit, nombreArchivo);
		strcat(nombreArchivoBit, ".txt" );
		
		validarRename = rename("Bitacora.txt", nombreArchivoBit);
	}
	
	/* Guardar el juego */
	
	estado = guardarJuego(nombreArchivo, juego);
		
	if (estado)
		printf("\"%s\" guardado correctamente\n", nombreArchivo);
	else
		printf("Hubo un problema al guardar el juego\n");
	if (validarRename != 0)
		printf("La bitacora no pudo ser guardada. \n");
		
		
	
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
