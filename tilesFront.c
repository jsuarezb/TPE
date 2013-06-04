#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tilesBack.h"
#include "getnum.h"
#include <string.h>
#define MIN_DIM 3
#define MAX_DIM 40
#define MAX_CHARS 80
#define MAX_NOMBRE 50
#define VALIDAR_AUX(x) ((x) != ' ' && (x) != '\n')

/* Enums */
enum {JUEGO_NUEVO = 1, JUEGO_BITACORA, RECUPERAR, TERMINAR};
enum {ERROR, ELIMINAR, MARTILLAZO, COLUMNA, HILERA, UNDO, SAVE, QUIT};

/* Structs */


/* Funciones */
void pedirDimensiones(tJuego * juego);
void pedirNiveles(tJuego * juego);
void imprimirTablero(tJuego * juego);
void analizarOpcion(int opcion, tJuego * juego); // Actua de acuerdo a la opcion pasada
int menuNuevo(); // Crea menu nuevo y devuelve opcion elegida
void comenzarJuego(); // Comienza el juego
int recuperar(tJuego * juego);
int hacerJugada(tJuego * juego);
tJugada eliminarWrapper(tJuego * juego, char args[]);
tJugada columnaWrapper(tJuego * juego, const char args[]);
tJugada hileraWrapper(tJuego * juego, const char args[]);
tJugada martillazoWrapper(tJuego * juego, const char args[]);
void guardarWrapper(tJuego * juego, const char args[]);


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

/* Dada una opcion del menu, determina las opciones correspondientes */
void
analizarOpcion(int opcion, tJuego * juego)
{	
	int i, j;
	
	switch (opcion)
	{
		case JUEGO_NUEVO:
			juego->conBitacora = 0;

			/* Peticion de datos al usuario */
			pedirDimensiones(juego);
			pedirNiveles(juego);
			
			/* Comienzo del juego */
			crearTablero(juego);
			generarTablero(juego);
			/* Juego */
			comenzarJuego(juego);
			/* Fin del juego */
			liberarTablero(juego->tablero, juego->alto);
			liberarTablero(juego->juegoUndo.tableroUndo, juego->alto);
			break;
		case JUEGO_BITACORA:
			juego->conBitacora = 1;
			juego->cantJugadas = 0;
			
			juego->bitacora = fopen("Bitacora.txt", "wt");
			
			/* Peticion de datos al usuario */
			pedirDimensiones(juego);
			pedirNiveles(juego);
			
			crearTablero(juego);
			generarTablero(juego);

			/* Copia el primer tablero a la bitacora */
			for (i = 0; i < juego->alto; i++)
			{
				for (j = 0; j < juego->ancho; j++)
					fprintf(juego->bitacora, "%c", juego->tablero[i][j] + 'A' - 1);
			
				fprintf(juego->bitacora, "\n", NULL);
			}

			comenzarJuego(juego);
			
			liberarTablero(juego->tablero, juego->alto);
			liberarTablero(juego->juegoUndo.tableroUndo, juego->alto);
			break;
		case RECUPERAR:
			/* Lectura de entrada para cargar el archivo indicado
			 * Si fue posible, comienza el juego, si no, continua en el menu */
			if (recuperar(juego))
			{
				comenzarJuego(juego);
				liberarTablero(juego->tablero, juego->alto);
				liberarTablero(juego->juegoUndo.tableroUndo, juego->alto);
			}

			break;
		case TERMINAR:
			printf("Gracias por jugar! Adios!\n");
			break;
		default:
			/* Opcion no valida */
			printf("Opcion invalida\n");
			break;
	}

	return;
}

/* Funcion que valida las dimensiones de un tablero de juego */
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

/* Funcion que valida el numero de niveles a jugar dependiendo
 * del tamaño del tablero */
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

/* Funcion que contiene el game loop, checkea el estado del juego 
 * y determina si se debe seguir jugando o no */
void
comenzarJuego(tJuego * juego)
{
	int estadoTablero = SEGUIR_JUGANDO, pts = 0, resp = 0, i, j;

	/* Validacion del tablero para seguir jugando */
	while (estadoTablero != GAME_OVER)
	{
		pts = calcularPuntos(resp, juego);
		juego->puntos += pts;
		
		/* Impresion pre tablero */
		printf("\nNivel %d\n", juego->nivelActual);
		printf("Puntos: %d\n", pts);	
		printf("Acumulados: %d\n", juego->puntos);	
		printf("h: %d, ", juego->movHileras);
		printf("c: %d, ", juego->movColumnas);
		printf("m: %d\n", juego->movMartillazos);

		estadoTablero = verificaMatriz(juego); /* TODO: ver si no se puede enviar hacia el fondo de la funcion */

		/* Pasar al próximo nivel */
		if (estadoTablero == PROXIMO_NIVEL)
		{	
			/* JUEGO GANADO */
			if (juego->nivelActual >= juego->nivelMaximo)
			{
				if (juego->conBitacora)
					fclose(juego->bitacora);
					
				printf("Felicitaciones, ha ganado!\n");
				printf("Ha completado el ultimo nivel.\n");

				return;
			}
			
			/* PASAJE AL PROXIMO NIVEL */
			bonus(juego);
			
			juego->nivelActual++;
			juego->puntos = 0;
			pts = 0;
			resp = 0;
			
			/* Creacion del nivel */
			generarTablero(juego);

			if (juego->conBitacora)
			{
				juego->cantJugadas = 0;
				
				/* Copia el tablero del siguiente nivel a la bitacora */
				for (i = 0; i < juego->alto; i++)
				{
					for (j = 0; j < juego->ancho; j++)
						fprintf(juego->bitacora, "%c", juego->tablero[i][j] + 'A' - 1);

					fprintf(juego->bitacora, "\n", NULL);
				}
			}

		}
		else if (estadoTablero != GAME_OVER)
		{
			imprimirTablero(juego);
			/* Ejecuta la jugada */
			resp = hacerJugada(juego);

			/* COMANDO QUIT EJECUTADO */
			if (resp == -1)
			{
				if (juego->conBitacora)
					fclose(juego->bitacora);

				return;
			}
			
			reacomodarTablero(juego);
		}	

	}

	/* JUEGO PERDIDO POR FALTA DE MOVIMIENTOS */
	imprimirTablero(juego);
	
	if (juego->conBitacora)
		fclose(juego->bitacora);
		
	printf("Lo siento, no quedan mas movimientos.\nHa perdido.");
		
	return;
}

/* TODO: validar el fopen y fijarse si no es mas efectivo hacerlo con getchar*/
 
int
recuperar(tJuego * juego)
{
	FILE * partidaGuardada = NULL;
	char * nombreArchivoAux, nombreArchivo[MAX_NOMBRE];
	char lineaAux[MAX_CHARS] = {0}, nombreArchivoBit[MAX_NOMBRE + 4];
	int lenArchivo, cantJugadasAux = 0;

	nombreArchivoAux = calloc(MAX_NOMBRE, sizeof(char));

	printf("Ingrese el nombre del archivo: ");

	fgets(nombreArchivoAux, MAX_NOMBRE, stdin);

	lenArchivo = strlen(nombreArchivoAux) - 1;

	/* Quita el "\n" de nombreArchivoAux para su uso en fopen */
	strncpy(nombreArchivo, nombreArchivoAux, lenArchivo);

	/* Recuperar el juego */
	partidaGuardada = fopen(nombreArchivo, "rb");
	/* TODO: mem leak? */
	if (partidaGuardada == NULL)
	{
		printf("No fue posible abrir el archivo \"%s\"\n", nombreArchivo);
		free(nombreArchivoAux);
		return 0;
	}
		
	recuperarJuego(partidaGuardada, juego);
	fclose(partidaGuardada);

	/* Asignacion de variables en modo bitacora */
	if (juego->conBitacora)
	{
		strcpy(nombreArchivoBit, nombreArchivo);
		strcat(nombreArchivoBit, ".txt" );

		/* Abre la bitacora en modo "r" para el correcto uso de fgets */
		juego->bitacora = fopen(nombreArchivoBit, "r");
		if (juego->bitacora == NULL)
		{
			printf("Imposible abrir la bitacora \"%s\"\n", nombreArchivoBit);
			free(nombreArchivoAux);
			return 0;
		}

		/* Obtiene el ultimo cantJugadas de la bitacora */		
		while (!feof(juego->bitacora))
			fgets(lineaAux, MAX_CHARS, juego->bitacora);
		
		sscanf(lineaAux, "%d:", &cantJugadasAux);
		/* TODO: validar el scanf para que no lea cualquier caracter */
		juego->cantJugadas = cantJugadasAux;

		juego->bitacora = fopen(nombreArchivoBit, "a");
	}

	free(nombreArchivoAux);
	return 1;
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

/* Funcion que compara lo ingresado por stdin con comandos[] y devuelve si
 * hubo un matcheo exitoso el indice de la palabra, en caso contrario
 * retorna -1 */
int
pedirJugada(char comandos[][5], size_t cantComandos, char args[])
{

	char * entrada = calloc(MAX_CHARS + 1, sizeof(char)), * e,
	comando[MAX_CHARS] = {0};

	int i, len;

	e = fgets(entrada, MAX_CHARS + 1, stdin);

	/* Validacion de la lectura o...
	 * se supero el limite de 80 caracteres en la lectura */

	if (e == NULL || entrada[MAX_CHARS] != '\0')
	{
		/* TODO: validar la superacion de MAX_CHARS en entrada */ 
		free(entrada);
		return -1;
	}

	sscanf(entrada, "%s", (char *) &comando);

	/* Almacena los argumentos a leer (si es que hay) */
	strncpy(args, entrada + strlen(comando), strlen(entrada) - 2);


	/* Compara el comando leído con los comandos del argumento */
	for (i = 0; i < cantComandos; i++)
	{
		/* strcmp devuelve cero cuando ingresas cualquier cosa y te lo toma como un eliminar */
		if (strcmp(comando, comandos[i]) == 0)
		{
			free(entrada);
			return i;
		}
	}
	
	/* TODO: validacion de comandos vacios */
	free(entrada);
	return -1;
}

/* hacerJugada(tJuego * juego) retorna la cantidad de azulejos eliminados
 * por la jugada elegida o -1 en caso que el usuario ingrese quit*/
int
hacerJugada(tJuego * juego)
{
	char acciones[][5]={"e", "m", "c", "h", "undo", "save", "quit"};
	char accion[4] = {"emch"};
	char args[MAX_CHARS] = {0};
	int jugadaValidada;
	tJugada jugada;

	jugada.azulejosEliminados = 0;

	do
	{
		printf("Ingresar accion:\n");
		/* Se adiciona 1 ya que retorna el indice de la coincidencia */
		jugadaValidada = pedirJugada(acciones, 7, args) + 1;
	} while (jugadaValidada == ERROR);

	if (juego->conBitacora)
	{
		if (jugadaValidada >= ELIMINAR && jugadaValidada <= HILERA)
			fprintf(juego->bitacora, "%d: %c%s", juego->cantJugadas, accion[jugadaValidada - 1], args);

		if (jugadaValidada == UNDO)
			fprintf(juego->bitacora, "%d: undo", juego->cantJugadas);
	}
	
	/* Agrego el \n al final para validar los argumento en los wrapper */
	strcat(args, "\n");

	switch(jugadaValidada)
	{
		case ELIMINAR:
			jugada = eliminarWrapper(juego, args);
			break;
		case MARTILLAZO:
			jugada = martillazoWrapper(juego, args);
			break;
		case COLUMNA:
			jugada = columnaWrapper(juego, args);
			break;
		case HILERA:
			jugada = hileraWrapper(juego, args);
			break;
		case UNDO:
			undo(juego);
			break;
		case SAVE:
			guardarWrapper(juego, args);
			break;
		case QUIT:
			return -1;
		default:
			printf("Jugada no reconocida\n");
			break;
	}

	/* Imprime en bitacora si es una jugada a imprimir */
	if (juego->conBitacora && jugadaValidada >= ELIMINAR && jugadaValidada <= UNDO)
	{
		if(jugada.azulejosEliminados > 0)
			fprintf(juego->bitacora,"; %d \n", jugada.azulejosEliminados);
		else
			fprintf(juego->bitacora,"\n", NULL);
			
		juego->cantJugadas++;
	}

	return jugada.azulejosEliminados;
}

/* Funcion que valida los parametros del comando "e" y
 * ejecuta la funcion eliminar retornando los valores de la jugada hecha */
tJugada
eliminarWrapper(tJuego * juego, char args[])
{
	char c;
	int x, y, argumentos = 0, estado;
	tPunto punto; /* TODO: verificar utilidad de tPunto, es necesario? */
	tJugada jugada;

	jugada.azulejosEliminados = 0;

	argumentos = sscanf(args, "%d, %d%c", &y, &x, &c);
	punto.x = x;
	punto.y = y;

	if (argumentos == 3 && c == '\n')
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
				
				/* Se guarda el juego para el undo sabiendo que eliminara
				 * azulejos del tablero */
				juegoUndo(juego);

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
	else
	{
		printf("Argumentos incorrectos\n");
	}

	return jugada;
}

tJugada
hileraWrapper(tJuego * juego, const char args[])
{
	char c;
	int y, argumentos, hayAzulejoVacio = 1, i;
	tJugada jugada;

	jugada.azulejosEliminados = 0;

	if (juego->movHileras < 1)
		printf("No dispones de suficientes \"hileras\"\n");
	else
	{
		argumentos = sscanf(args, "%d%c", &y, &c);

		if (argumentos == 2 && c == '\n')
		{
			/* Verifica que al menos un azulejo no este vacio en la hilera */
			for (i = 0; i < juego->ancho && hayAzulejoVacio; i++)
			{
				if (juego->tablero[y][i] != 0)
					hayAzulejoVacio = 0;
			}

			if (hayAzulejoVacio)
			{
				printf("Imposible eliminar la hilera %d\n", y);
				return jugada;
			}
				
			/* TODO: fix ejecucion de juegoUndo en caso de no poder eliminar */
			/* Se guarda el juego en caso de undo */
			juegoUndo(juego);
			
			jugada.punto.x = -1; //Indicando que no hay coordenada X 
			jugada.punto.y = y; /** TODO: fix */
			jugada.azulejosEliminados = eliminarHilera(y, juego);

		}
		else
			printf("Argumentos incorrectos\n");
	}

	return jugada;
}

tJugada
columnaWrapper(tJuego * juego, const char args[])
{
	char c;
	int x, argumentos;
	tJugada jugada;

	jugada.azulejosEliminados = 0;

	if (juego->movColumnas < 1)
		printf("No dispones de suficientes \"columnas\"\n");
	else
	{
		argumentos = sscanf(args, "%d%c", &x, &c);

		if (argumentos == 2 && c == '\n')
		{
			/* Verifica que el azulejo inferior no esté vacío.
			* Tener en cuenta que el tablero esta correctamente acomodado */
			if (juego->tablero[juego->alto - 1][x] == 0)
			{
				printf("Imposible eliminar la columna %d\n", x);
				return jugada;
			}
				
			/* Se guarda el juego en caso de undo */
			juegoUndo(juego);
			
			jugada.punto.x = x;
			jugada.punto.y = -1; //Indicando que no hay coordenada y
			jugada.azulejosEliminados = eliminarColumna(x, juego);
		}
		else
		{
			printf("Argumentos incorrectos\n");
		}
	}

	return jugada;
}

tJugada
martillazoWrapper(tJuego * juego, const char args[])
{
	char c;
	int x, y, argumentos = 0, estado;
	tJugada jugada;

	jugada.azulejosEliminados = 0;

	if (juego->movMartillazos < 1)
		printf("No dispones de suficientes \"martillazos\"\n");
	else
	{
		argumentos = sscanf(args, "%d, %d%c", &y, &x, &c);

		if (argumentos == 3 && c == '\n')
		{
			estado = validarPunto(x, y, juego);

			switch (estado)
			{
				case PUNTO_VALIDO:
					/* Se guarda el juego en caso de undo */
					juegoUndo(juego);
					
					/* Se martilla el tablero */
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
		else
		{
			printf("Argumentos incorrectos\n");
		}
	}

	return jugada;
}

void
guardarWrapper(tJuego * juego, const char args[])
{
	char * nombreArchivo = calloc(MAX_CHARS, sizeof(char)), c, val;
	char nombreArchivoBit[MAX_CHARS + 4] = {0};
	int i = 0, estado;

	val = sscanf(args, "%s%c", nombreArchivo, &c);
	
	if (val != 2 || c != '\n')
	{
		printf("Argumentos incorrectos\n");
		free(nombreArchivo);
		return;
	}

	/* Renombre de Bitacora.txt al nuevo nombre */
	/* TODO: renombre del nombre actual al nuevo nombre */
	if (juego->conBitacora)
	{
		strcpy(nombreArchivoBit, nombreArchivo);
		strcat(nombreArchivoBit, ".txt" );

		rename("Bitacora.txt", nombreArchivoBit);
	}

	/* Guardar el juego */
	estado = guardarJuego(nombreArchivo, juego);

	if (estado)
		printf("\"%s\" guardado correctamente\n", nombreArchivo);
	else
		printf("Hubo un problema al guardar el juego, intentelo de nuevo\n");

	free(nombreArchivo);
	return;
}
