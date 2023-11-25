/*Copyright (C) 
 * 2016 - eda1 dot fiunam at yahoo.com.mx
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

// Modificado el 2018-05-31


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <stdbool.h>
// para usar bool, true y false

#define FILAS 4
#define COLS 4
// TODO: ambos valores deberían ser parte del TDU Tablero, junto con el propio
// tablero


//----------------------------------------------------------------------
//  Tablero ADT
//----------------------------------------------------------------------

enum TIPO_CELDAS 
{
	PARED = 0,
	LIBRE,
	INICIO,
	SALIDA,
	VISITADA,
	CANCELADA
};



struct Tablero_T
{
	int tablero[FILAS][COLS];
	int filas;
	int cols;
};

typedef struct Tablero_T Tablero;


Tablero* Tablero_Ctor (int filas, int cols, int tablero_copia[][COLS])
{
	Tablero* nuevo = (Tablero*) malloc (sizeof (Tablero));
	assert (nuevo);

	nuevo->filas = filas;
	nuevo->cols = cols;

	for (int i = 0; i < filas; ++i) {
		for (int j = 0; j < cols; ++j) {
			nuevo->tablero[i][j] = tablero_copia[i][j];
		}
	}

	return nuevo;
}

void Tablero_Dtor (Tablero* this)
{
	assert (this);
	free (this);
	this = NULL;
}

void Tablero_Dibujar (Tablero* this)
{
	printf ("\n");
	for (int i = 0; i < this->filas; ++i) {
		for (int j = 0; j < this->cols; ++j) {
			int celda = this->tablero[i][j];
			switch (celda) {
				case PARED: printf ("#"); break;
				case LIBRE: printf ("."); break;
				case INICIO: printf ("I"); break;
				case SALIDA: printf ("S"); break;
				case VISITADA: printf ("V"); break;
				case CANCELADA: printf ("X"); break;
				default: break;
			}
		}
		printf ("\n");
	}
	printf ("\n");

}

/**
 * @brief Marca la celda
 *
 * @param this El tablero
 * @param x la fila
 * @param y la columna
 * @param tipo El tipo de celda. \see TIPO_CELDAS
 */
void Tablero_SetCelda (Tablero* this, int x, int y, int tipo)
{
	this->tablero[x][y] = tipo;
}

/**
 * @brief Indica el tipo de celda
 *
 * @param this El tablero
 * @param x la fila
 * @param y la columna
 *
 * @return El tipo de celda según \see TIPO_CELDAS
 */
int Tablero_GetCelda (Tablero* this, int x, int y)
{
	return this->tablero[x][y];
}

//----------------------------------------------------------------------
//  Fin -- Tablero ADT
//----------------------------------------------------------------------

#define UP    0
#define LEFT  1
#define DOWN  2
#define RIGHT 3

const int movs[4] = { UP, LEFT, DOWN, RIGHT };

// esta función se puede mejorar de muchas formas, pero por lo pronto así la voy
// a dejar
bool EsFactible( Tablero* tablero, int fila, int col, int dir )
{
	int celda;

	switch( dir ){
		case UP:
		{
			int fila_tmp = fila - 1;
			if( fila_tmp == -1 ) return false; // está fuera del tablero

			celda = Tablero_GetCelda( tablero, fila_tmp, col );
			break;
		}

		case LEFT:
		{
			int col_tmp = col - 1;
			if( col_tmp == -1 ) return false; // está fuera del tablero

			celda = Tablero_GetCelda( tablero, fila, col_tmp );
			break;
		}

		case DOWN:
		{
			int fila_tmp = fila + 1;
			if( fila_tmp == FILAS ) return false;

			celda = Tablero_GetCelda( tablero, fila_tmp, col );
			break;
		}

		case RIGHT:
		{
			int col_tmp = col + 1;
			if( col_tmp == COLS ) return false; // está fuera del tablero

			celda = Tablero_GetCelda( tablero, fila, col_tmp );
			break;
		}

		default:
			return false;
	}

	return( celda == PARED || celda == VISITADA || celda == CANCELADA ) ? false : true;
}

// se asume que esta función es llamada LUEGO de haberse asegurado (a través de
// EsFactible) que el movimiento es legal
void move( Tablero* tablero, int fila, int col, int dir, int* nueva_fila, int* nueva_col )
{
	switch( dir ){
		case UP: *nueva_fila = fila - 1; break;
		case LEFT: *nueva_col = col - 1; break;
		case DOWN: *nueva_fila = fila + 1; break;
		case RIGHT: *nueva_col = col + 1; break;
#if 0 
		case UP: --(*fila); break;
		case LEFT: --(*col); break;
		case DOWN: ++(*fila); break;
		case RIGHT: ++(*col); break;
#endif  
	}
}

bool EsSolucion( Tablero* tablero, int fila, int col )
{
	return Tablero_GetCelda( tablero, fila, col ) == SALIDA;
}

char dir_2_glypho[4] = { '^', '<', 'v', '>' };

int cont = 0;

bool LaberintoBT( Tablero* tablero, int fila, int col )
{
	printf( "[{%d}->:%d,%d]\n", cont, fila, col );
	++cont;


	for( int dir = UP; dir <= RIGHT; ++dir ){
		printf( " [d:%c]\n", dir_2_glypho[ dir ] );

		if( EsSolucion( tablero, fila, col ) ) {
			return true;
		}
		else{
			if( EsFactible( tablero, fila, col, dir ) ){
				Tablero_SetCelda( tablero, fila, col, VISITADA );
				Tablero_Dibujar( tablero );


				printf( " +[%d,%d]\n", fila, col );

				int nueva_fila = fila, nueva_col = col;

				move( tablero, fila, col, dir, &nueva_fila, &nueva_col );
				// actualiza nueva_fila o nueva_col dependiendo de dir. La otra
				// queda con el valor obtenido en la inicialización

				if( LaberintoBT( tablero, nueva_fila, nueva_col ) == false ){
					Tablero_SetCelda( tablero, fila, col, CANCELADA );
					printf( " -[%d,%d]\n", fila, col );
				}
#if 0 
				else{
					return true;
				}
#endif  
			}

		}
	}

	--cont;
	printf( "[{%d}<-:%d,%d]\n", cont, fila, col );
	return false;
}

#if 0 
// Algoritmo recursivo con backtracking
bool LaberintoBT( Tablero* tablero, int fila, int col )
{
	printf( "[{%d}->:%d,%d]\n", cont, fila, col );
	++cont;

	for( int dir = UP; dir <= RIGHT; ++dir ){
		printf( " [d:%c]\n", dir_2_glypho[ dir ] );

		if( EsFactible( tablero, fila, col, dir ) ){
			Tablero_SetCelda( tablero, fila, col, VISITADA );
			//			Tablero_Dibujar( tablero );

			printf( " +[%d,%d]\n", fila, col );

			if( EsSolucion( tablero, fila, col ) ) 
				return true;
			else{
				int nueva_fila = fila, nueva_col = col;

				move( tablero, fila, col, dir, &nueva_fila, &nueva_col );
				// actualiza nueva_fila o nueva_col dependiendo de dir. La otra
				// queda con el valor obtenido en la inicialización

				if( LaberintoBT( tablero, nueva_fila, nueva_col ) == false ){
					Tablero_SetCelda( tablero, fila, col, CANCELADA );
					printf( " -[%d,%d]\n", fila, col );
				}

			}
		}
	}

	--cont;
	printf( "[{%d}<-:%d,%d]\n", cont, fila, col );
	return false;
}
#endif  

// Función de activación
bool Laberinto( Tablero* tablero, int start_fila, int start_col, int finish_fila, int finish_col )
{
	Tablero_SetCelda( tablero, start_fila, start_col, INICIO );
	Tablero_SetCelda( tablero, finish_fila, finish_col, SALIDA );


	return LaberintoBT( tablero, start_fila, start_col );
}





//----------------------------------------------------------------------
//  Driver program
//----------------------------------------------------------------------
int main(void)
{
	int tablero_prueba[FILAS][COLS] = 
	{
		{1,1,0,1},
		{0,1,1,1},
		{0,1,0,1},
		{1,1,0,1}
	};

	Tablero* tablero = Tablero_Ctor (FILAS, COLS, tablero_prueba);

	if( Laberinto( tablero, 3, 0, 3, 3 ) ){ Tablero_Dibujar( tablero ); }
	else{ printf( "No hubo solución :(\n" ); }

	Tablero_Dtor (tablero);

	return 0;
}

