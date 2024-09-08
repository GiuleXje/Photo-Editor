//314CA Pal Roberto Giulio

#include "func.h"

void swap(int *a, int *b)
{
	int aux = *a;
	*a = *b;
	*b = aux;
}

unsigned char **matrix_aloc(int n, int m)
{
	unsigned char **a = (unsigned char **)malloc(n * sizeof(unsigned char *));
	if (!a) {
		fprintf(stderr, "malloc() failed!\n");
		return NULL;
	}
	for (int i = 0 ; i < n ; i++) {
		a[i] = (unsigned char *)malloc(m * sizeof(unsigned char));
		if (!a[i]) {
			fprintf(stderr, "malloc() failed!");
			while (--i >= 0)
				free(a[i]);
			free(a);
			return NULL;
		}
	}
	return a;
}

void free_matrix(unsigned char **a, int n)
{
	for (int i = 0 ; i < n ; i++)
		free(a[i]);
	free(a);
}

void print_matrix(unsigned char **a, int n, int m)
{
	FILE *out = fopen("output.out", "w");
	fprintf(out, "%d %d\n", n, m);
	for (int i = 0 ; i < n ; i++) {
		for (int j = 0 ; j < m ; j++)
			fprintf(out, "%hhu ", a[i][j]);
		fprintf(out, "\n");
	}
	fclose(out);
}

void debug(void)
{
	printf("ok\n");
}

int clamp(double x)
{
	int y = round(x);
	if (y < 0)
	    return 0;
	else if (y > 255)
	    return 255;
	else
		return y;
}

int APPLY_EDGE(unsigned char **a, int i, int j)
{
	double x;
	x = a[i][j] * 8 - a[i + 1][j]
		- a[i - 1][j] - a[i][j + 3]
		- a[i][j - 3] - a[i + 1][j - 3]
		- a[i + 1][j + 3] - a[i - 1][j + 3]
		- a[i - 1][j - 3];
	int y;
	y = clamp(x);
	return y;
}

int APPLY_SHARPEN(unsigned char **a, int i, int j)
{
	double x;
	x = a[i][j] * 5 - a[i + 1][j]
		- a[i - 1][j] - a[i][j - 3]
		- a[i][j + 3];
	int y;
	y = clamp(x);
	return y;
}

int APPLY_BLUR(unsigned char **a, int i, int j)
{
	double x;
	x = a[i][j] / 9.0 + a[i + 1][j] / 9.0
		+ a[i - 1][j] / 9.0 + a[i][j + 3] / 9.0
		+ a[i][j - 3] / 9.0 + a[i - 1][j - 3] / 9.0
		+ a[i - 1][j + 3] / 9.0 + a[i + 1][j - 3] / 9.0
		+ a[i + 1][j + 3] / 9.0;
	int y;
	y = clamp(x);
	return y;
}

int APPLY_GAUSSIAN_BLUR(unsigned char **a, int i, int j)
{
	double x;
	x = (4.0 * a[i][j]) / 16.0 + (2.0 * a[i][j + 3]) / 16.0
		+ (2.0 * a[i][j - 3]) / 16.0 + (2.0 * a[i + 1][j]) / 16.0
		+ (2.0 * a[i - 1][j]) / 16.0 + a[i - 1][j - 3] / 16.0
		+ a[i + 1][j - 3] / 16.0 + a[i - 1][j + 3] / 16.0
		+ a[i + 1][j + 3] / 16.0;
	int y;
	y = clamp(x);
	return y;
}

void copy_matrix(unsigned char **a, unsigned char **b, int n, int m)
{
	for (int i = 0 ; i < n ; i++) {
		for (int j = 0 ; j < m ; j++)
			b[i][j] = a[i][j];
	}
}

void ROTATE_ALL(editor_foto *poza)
{
	int height, width;
	height = poza->height;
	width = poza->width;
	if (poza->RGB) {
		height *= 3;
		unsigned char **mat = matrix_aloc(width, height);
		int column = 0, line = 0;
		for (int i = poza->height - 1 ; i >= 0 ; i--) {
			line = 0;
			for (int j = 0 ; j < poza->width * 3 ; j += 3) {
				mat[line][column] = poza->poza_curenta[i][j];
				mat[line][column + 1] = poza->poza_curenta[i][j + 1];
				mat[line][column + 2] = poza->poza_curenta[i][j + 2];
				line++;
			}
			column += 3;
		}
		free_matrix(poza->poza_curenta, poza->height);
		poza->poza_curenta = mat;
		swap(&poza->height, &poza->width);
		swap(&poza->x2, &poza->y2);
		swap(&poza->x1, &poza->y1);
	} else {
		unsigned char **mat = matrix_aloc(width, height);
		int column = 0, line = 0;
		for (int i = poza->height - 1 ; i >= 0 ; i--) {
			line = 0;
			for (int j = 0 ; j < poza->width ; j++)
				mat[line++][column] = poza->poza_curenta[i][j];
			column++;
		}
		free_matrix(poza->poza_curenta, poza->height);
		poza->poza_curenta = mat;
		swap(&poza->height, &poza->width);
		swap(&poza->x2, &poza->y2);
		swap(&poza->x1, &poza->y1);
	}
}

void ROTATE_SQUARE(editor_foto *poza)
{
	int height = poza->x2 - poza->x1;
	unsigned char **mat = matrix_aloc(height, height);
	int line = 0, column = 0;
	for (int i = poza->y2 - 1 ; i >= poza->y1 ; i--) {
		column = 0;
		for (int j = poza->x1 ; j < poza->x2 ; j++)
			mat[line][column++] = poza->poza_curenta[i][j];
		line++;
	}
	line = 0;
	column = 0;
	for (int i = poza->y1 ; i < poza->y2 ; i++) {
		line = 0;
		for (int j = poza->x1 ; j < poza->x2 ; j++)
			poza->poza_curenta[i][j] = mat[line++][column];
		column++;
	}
	free_matrix(mat, height);
}

void ROTATE90(editor_foto *poza)
{
	int height, width, line, column;
	height = poza->y2 - poza->y1;
	width = poza->x2 - poza->x1;
	unsigned char **mat = matrix_aloc(height, width * 3);
	line = 0;
	column = 0;
	for (int i = poza->y2 - 1 ; i >= poza->y1 ; i--) {
		line = 0;
		for (int j = 3 * poza->x1 ; j < 3 * poza->x2 ; j += 3) {
			mat[line][column] = poza->poza_curenta[i][j];
			mat[line][column + 1] = poza->poza_curenta[i][j + 1];
			mat[line][column + 2] = poza->poza_curenta[i][j + 2];
			line++;
		}
		column += 3;
	}
	line = 0;
	for (int i = poza->y1 ; i < poza->y2 ; i++) {
		column = 0;
		for (int j = 3 * poza->x1 ; j < 3 * poza->x2 ; j++)
			poza->poza_curenta[i][j] = mat[line][column++];
		line++;
	}
	free_matrix(mat, height);
}

void set_false(editor_foto *poza)
{
	poza->RGB = false;
	poza->GRAYSCALE = false;
	poza->ascii = false;
	poza->binary = false;
	poza->height = 0;
}

void escape_comments(int *c, FILE *in)
{
	while ((*c = fgetc(in)) != EOF && *c == '#')
		while ((*c = fgetc(in)) != EOF && *c != '\n')
			;
	ungetc(*c, in);
}

void read_again(int *c, FILE *in)
{
	char magic_number[3];
	while ((*c = fgetc(in)) != EOF && *c == '#')
		while ((*c = fgetc(in)) != EOF && *c != '\n')
			;
	ungetc(*c, in);
	fread(magic_number, sizeof(char), 2, in);
	while ((*c = fgetc(in)) != EOF && *c == '#')
		while ((*c = fgetc(in)) != EOF && *c != '\n')
			;
	ungetc(*c, in);
	int a, b;
	fscanf(in, "%d %d", &a, &b);
	while ((*c = fgetc(in)) != EOF && *c == '#')
		while ((*c = fgetc(in)) != EOF && *c != '\n')
			;
	ungetc(*c, in);
	fscanf(in, "%d", &a);
	while ((*c = fgetc(in)) != EOF && *c == '#')
		while ((*c = fgetc(in)) != EOF && *c != '\n')
			;
}

void read_ascii_GRAYSCALE(editor_foto *poza, FILE *in)
{
	int line, column, x;
	line = 0;
	column = 0;
	for (int i = 0 ; i < poza->width * poza->height ; i++) {
		fscanf(in, "%d", &x);
		poza->poza_curenta[line][column++] = x;
		if (column == poza->width) {
			column = 0;
			line++;
		}
	}
}

void read_ascii_RGB(editor_foto *poza, FILE *in)
{
	int line, column, x;
	line = 0;
	column = 0;
	for (int i = 0 ; i < 3 * poza->width * poza->height ; i++) {
		fscanf(in, "%d", &x);
		poza->poza_curenta[line][column++] = x;
		if (column == 3 * poza->width) {
			column = 0;
			line++;
		}
	}
}

void read_binary_GRAYSCALE(editor_foto *poza, FILE *in)
{
	UC *pixels = (UC *)malloc(poza->height * poza->width * sizeof(UC));
	fread(pixels, sizeof(UC), poza->height * poza->width, in);
	int line = 0, column = 0;
	for (int i = 0 ; i < poza->height * poza->width ; i++) {
		poza->poza_curenta[line][column++] = pixels[i];
		if (column == poza->width) {
			column = 0;
			line++;
		}
	}
	free(pixels);
}

void read_binary_RGB(editor_foto *poza, FILE *in)
{
	int RGB_pixels = poza->height * 3 * poza->width;
	UC *pixels = (UC *)malloc(RGB_pixels * sizeof(UC));
	fread(pixels, sizeof(UC), RGB_pixels, in);
	int line = 0, column = 0;
	for (int i = 0 ; i < RGB_pixels ; i++) {
		poza->poza_curenta[line][column++] = pixels[i];
		if (column == 3 * poza->width) {
			column = 0;
			line++;
		}
	}
	free(pixels);
}
