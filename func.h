//Copyright Pal Roberto Giulio 2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

#pragma once

#define UC unsigned char

//structura in care vom memora toate datele despre poza incarcata in memorie
typedef struct {
	bool GRAYSCALE, RGB, ascii, binary;
	int height, width, max_value, x1, y1, x2, y2;
	unsigned char **poza_curenta;
} editor_foto;

//interschimbarea a doua valori
void swap(int *x, int *y);

//alocare matrice
unsigned char **matrix_aloc(int n, int m);

//eliberare matrice
void free_matrix(unsigned char **a, int n);

//afisare matrice
void print_matrix(unsigned char **a, int n, int m);

//functie de debuging
void debug(void);

//clamp
int clamp(double x);

//aplicare edge
int APPLY_EDGE(unsigned char **a, int i, int j);

//aplicare sharpen
int APPLY_SHARPEN(unsigned char **a, int i, int j);

//aplicare blur
int APPLY_BLUR(unsigned char **a, int i, int j);

//aplicare gaussian blur
int APPLY_GAUSSIAN_BLUR(unsigned char **a, int i, int j);

//copiere matrice
void copy_matrix(unsigned char **a, unsigned char **b, int n, int m);

//rotire patrat
void ROTATE_SQUARE(editor_foto *poza);

//rotire intreaga imagine
void ROTATE_ALL(editor_foto *poza);

//rotire pentru imagini RGB
void ROTATE90(editor_foto *poza);

//cazul in care nu avem niciun fisier incarcat
void set_false(editor_foto *poza);

//ignorare linii ce contin comentarii
void escape_comments(int *c, FILE *in);

//citim din nou datele cand redeschidem fisierul binar, dar de aceasta data
//ignoram tot ceea ce vom citi pana la matricea cu pixeli
void read_again(int *c, FILE *in);

//citirea unei matrice GRAYSCALE dintr-un fisier ascii
void read_ascii_GRAYSCALE(editor_foto *poza, FILE *in);

//citirea unei matrice RGB dintr-un fisier ascii
void read_ascii_RGB(editor_foto *poza, FILE *in);

//citirea unei matrice GRAYSCALE dintr-un fisier binar
void read_binary_GRAYSCALE(editor_foto *poza, FILE *in);

//citirea unei matrice RGB dintr-un fisier binar
void read_binary_RGB(editor_foto *poza, FILE *in);

