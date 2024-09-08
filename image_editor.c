//#Copyright Pal Roberto-Giulio 2024

#include "func.h"

void LOAD(editor_foto *poza, char input[50])
{
	if (poza->height > 0)
		free_matrix(poza->poza_curenta, poza->height);
	set_false(poza);
	char magic_number[3] = "";
	FILE *in = fopen(input + 5, "r");
	if (!in) {
		printf("Failed to load %s\n", input + 5);
		if (poza->height > 0) {
			free_matrix(poza->poza_curenta, poza->height);
			poza->height = 0;
		}
		return;
	}
	int c;
	escape_comments(&c, in);
	fread(magic_number, sizeof(char), 2, in);
	if (strcmp(magic_number, "P2") == 0) {
		poza->ascii = true;
		poza->GRAYSCALE = true;
	} else if (strcmp(magic_number, "P3") == 0) {
		poza->ascii = true;
		poza->RGB = true;
	} else if (strcmp(magic_number, "P5") == 0) {
		poza->binary = true;
		poza->GRAYSCALE = true;
	} else if (strcmp(magic_number, "P6") == 0) {
		poza->binary = true;
		poza->RGB = true;
	}
	escape_comments(&c, in);
	fscanf(in, "%d %d", &poza->width, &poza->height);
	escape_comments(&c, in);
	fscanf(in, "%d", &poza->max_value);
	while ((c = fgetc(in)) != EOF && c == '#')
		while ((c = fgetc(in)) != EOF && c != '\n')
			;

	if (poza->RGB)
		poza->poza_curenta = matrix_aloc(poza->height, 3 * poza->width);
	else
		poza->poza_curenta = matrix_aloc(poza->height, poza->width);
	if (poza->ascii) {
		if (poza->GRAYSCALE)
			read_ascii_GRAYSCALE(poza, in);
		else
			read_ascii_RGB(poza, in);
	} else if (poza->binary) {
		fclose(in);
		in = fopen(input + 5, "rb");
		read_again(&c, in);
		if (poza->GRAYSCALE)
			read_binary_GRAYSCALE(poza, in);
		else
			read_binary_RGB(poza, in);
	}

	fclose(in);
	printf("Loaded %s\n", input + 5);
	poza->x1 = 0;
	poza->x2 = poza->width;
	poza->y1 = 0;
	poza->y2 = poza->height;
}

void SELECT_ALL(editor_foto *poza)
{
	if (!poza->RGB && !poza->GRAYSCALE) {
		printf("No image loaded\n");
		return;
	}
	poza->x1 = 0;
	poza->y1 = 0;
	poza->x2 = poza->width;
	poza->y2 = poza->height;
	printf("Selected ALL\n");
}

void SELECT(editor_foto *poza, char input[50])
{
	if (!poza->RGB && !poza->GRAYSCALE) {
		printf("No image loaded\n");
		return;
	}
	int x1, x2, y1, y2, nr, k;
	nr = 0;
	k = 0;
	for (int i = 7; input[i] ; i++) {
		if (isdigit(input[i])) {
			if (!nr)
				k++;
			nr = nr * 10 + (input[i] - '0');
		} else if (input[i] == ' ') {
			if (k == 1)
				x1 = nr;
			else if (k == 2)
				y1 = nr;
			else
				x2 = nr;
			nr = 0;
		} else {
			if (input[i] == '-')
				printf("Invalid set of coordinates\n");
			else
				printf("Invalid command\n");
			return;
		}
	}
	if (k != 4) {
		printf("Invalid command\n");
		return;
	}
	y2 = nr;
	if (x1 > poza->width || x2 > poza->width ||
		y1 > poza->height || y2 > poza->height || y1 == y2 || x1 == x2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1 > x2)
		swap(&x1, &x2);
	if (y1 > y2)
		swap(&y1, &y2);
	poza->x1 = x1;
	poza->x2 = x2;
	poza->y1 = y1;
	poza->y2 = y2;
	printf("Selected %d %d %d %d\n", poza->x1, poza->y1, poza->x2, poza->y2);
}

void CROP(editor_foto *poza)
{
	if (!poza->RGB && !poza->GRAYSCALE) {
		printf("No image loaded\n");
		return;
	}
	int width, height;
	width = poza->x2 - poza->x1;
	height = poza->y2 - poza->y1;
	int to_alloc_width = width;
	if (poza->RGB)
		to_alloc_width *= 3;
	UC **mat = matrix_aloc(height, to_alloc_width);
	int line, column;
	line = 0;
	column = 0;
	if (poza->GRAYSCALE) {
		for (int i = poza->y1 ; i < poza->y2 ; i++) {
			for (int j = poza->x1 ; j < poza->x2 ; j++)
				mat[line][column++] = poza->poza_curenta[i][j];
			column = 0;
			line++;
		}
	} else {
		for (int i = poza->y1 ; i < poza->y2 ; i++) {
			for (int j = 3 * poza->x1 ; j < 3 * poza->x2 ; j++)
				mat[line][column++] = poza->poza_curenta[i][j];
			column = 0;
			line++;
		}
	}
	free_matrix(poza->poza_curenta, poza->height);
	poza->poza_curenta = mat;
	poza->x1 = 0;
	poza->x2 = width;
	poza->y1 = 0;
	poza->y2 = height;
	poza->height = height;
	poza->width = width;
	printf("Image cropped\n");
}

void HISTOGRAM(editor_foto *poza, char input[50])
{
	if (!poza->RGB && !poza->GRAYSCALE) {
		printf("No image loaded\n");
		return;
	}
	int bins = 0, stars = 0, nr = 0, k = 0;
	for (int i = 10 ; input[i] ; i++) {
		if (isdigit(input[i])) {
			nr = nr * 10 + (input[i] - '0');
		} else if (input[i] == ' ') {
			k++;
			stars = nr;
			nr = 0;
		} else {
			printf("Invalid command\n");
			return;
		}
	}
	if (k != 1) {
		printf("Invalid command\n");
		return;
	}
	if (poza->RGB) {
		printf("Black and white image needed\n");
		return;
	}
	bins = nr;
	if (stars == 0 || bins == 0 || bins > poza->max_value + 1) {
		printf("Invalid set of parameters\n");
		return;
	}
	int *frequency = (int *)calloc(sizeof(int), poza->max_value + 1);
	for (int i = 0 ; i < poza->height ; i++) {
		for (int j = 0 ; j < poza->width ; j++)
			frequency[poza->poza_curenta[i][j]]++;
	}
	int max_frequency = 0;
	int step = (poza->max_value + 1) / bins;
	for (int i = 0 ; i <= poza->max_value ; i += step) {
		int sum = 0;
		for (int j = i ; j < i + step && j <= poza->max_value ; j++)
			sum = sum + frequency[j];
		if (sum > max_frequency)
			max_frequency = sum;
	}
	for (int i = 0 ; i <= poza->max_value ; i += step) {
		int sum = 0;
		for (int j = i ; j < i + step && j <= poza->max_value ; j++)
			sum = sum + frequency[j];
		int y = (sum * stars) / max_frequency;
		printf("%d\t|\t", y);
		for (int j = 0 ; j < y ; j++)
			printf("*");
		printf("\n");
	}
}

void EQUALIZE(editor_foto *poza)
{
	if (!poza->RGB && !poza->GRAYSCALE) {
		printf("No image loaded\n");
		return;
	}
	if (poza->RGB) {
		printf("Black and white image needed\n");
		return;
	}
	int *frequency = (int *)calloc(sizeof(int), 256);
	for (int i = 0 ; i < poza->height ; i++) {
		for (int j = 0 ; j < poza->width ; j++)
			frequency[poza->poza_curenta[i][j]]++;
	}
	for (int i = 1 ; i < 256 ; i++)
		frequency[i] += frequency[i - 1];
	int area = poza->width * poza->height;
	int *function = (int *)malloc(sizeof(int) * 256);
	for (int i = 0 ; i < 256 ; i++) {
		double x = 255.0 / area * frequency[i];
		int y = round(x);
		function[i] = clamp(y);
	}
	for (int i = 0 ; i < poza->height ; i++) {
		for (int j = 0 ; j < poza->width ; j++)
			poza->poza_curenta[i][j] = function[poza->poza_curenta[i][j]];
	}
	free(frequency);
	free(function);
	printf("Equalize done\n");
}

void APPLY(editor_foto *poza, char input[50])
{
	if (!poza->GRAYSCALE && !poza->RGB) {
		printf("No image loaded\n");
		return;
	}
	if (strcmp("APPLY", input) == 0) {
		printf("Invalid command\n");
		return;
	}
	if (poza->GRAYSCALE) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	char command[50];
	int k = 0;
	for (int i = 6 ; input[i] ; i++)
		command[k++] = input[i];
	command[k] = '\0';
	int x1, x2, y1, y2;
	x1 = poza->x1;
	x2 = poza->x2;
	y1 = poza->y1;
	y2 = poza->y2;
	UC **mat = matrix_aloc(poza->height, 3 * poza->width);
	copy_matrix(poza->poza_curenta, mat, poza->height, 3 * poza->width);
	if (x1 == 0)
		x1++;
	if (x2 == poza->width)
		x2--;
	if (y1 == 0)
		y1++;
	if (y2 == poza->height)
		y2--;
	if (strcmp(command, "EDGE") == 0) {
		for (int i = y1 ; i < y2 ; i++) {
			for (int j = 3 * x1; j < 3 * x2 ; j++)
				poza->poza_curenta[i][j] = APPLY_EDGE(mat, i, j);
		}
	} else if (strcmp(command, "SHARPEN") == 0) {
		for (int i = y1 ; i < y2 ; i++) {
			for (int j = 3 * x1 ; j < 3 * x2 ; j++)
				poza->poza_curenta[i][j] = APPLY_SHARPEN(mat, i, j);
		}
	} else if (strcmp(command, "BLUR") == 0) {
		for (int i = y1 ; i < y2 ; i++) {
			for (int j = 3 * x1 ; j < 3 * x2 ; j++)
				poza->poza_curenta[i][j] = APPLY_BLUR(mat, i, j);
		}
	} else if (strcmp(command, "GAUSSIAN_BLUR") == 0) {
		for (int i = y1 ; i < y2 ; i++) {
			for (int j = 3 * x1 ; j < 3 * x2 ; j++)
				poza->poza_curenta[i][j] = APPLY_GAUSSIAN_BLUR(mat, i, j);
		}
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}
	free_matrix(mat, poza->height);
	printf("APPLY %s done\n", command);
}

void ROTATE(editor_foto *poza, char input[50])
{
	if (!poza->RGB && !poza->GRAYSCALE) {
		printf("No image loaded\n");
		return;
	}
	int angle = 0, sign = 1, rotates;
	for (int i = 7 ; input[i] ; i++) {
		if (isdigit(input[i])) {
			angle = angle * 10 + (input[i] - '0');
		} else if (input[i] == '-') {
			sign *= -1;
		} else {
			printf("Invalid command\n");
			return;
		}
	}
	angle *= sign;
	if ((angle % 90 != 0 && angle != 0) || angle > 360 || angle < -360) {
		printf("Unsupported rotation angle\n");
		return;
	}
	if (!(poza->x2 - poza->x1 == poza->y2 - poza->y1) &&
		!(poza->x2 - poza->x1 == poza->width &&
		poza->y2 - poza->y1 == poza->height)) {
		printf("The selection must be square\n");
		return;
	}
	if (angle != 0) {
		if (angle > 0)
			rotates = angle / 90;
		else
			rotates = 4 + angle / 90;
		if (poza->x2 - poza->x1 == poza->width &&
			poza->y2 - poza->y1 == poza->height &&
			poza->height != poza->width) {
			for (int i = 0 ; i < rotates ; i++)
				ROTATE_ALL(poza);
		} else if (poza->GRAYSCALE) {
			for (int i = 0 ; i < rotates ; i++)
				ROTATE_SQUARE(poza);
		} else {
			for (int i = 0 ; i < rotates ; i++)
				ROTATE90(poza);
		}
	}
	printf("Rotated %d\n", angle);
}

void SAVE(editor_foto *poza, char input[50])
{
	if (!poza->RGB && !poza->GRAYSCALE) {
		printf("No image loaded\n");
		return;
	}
	char file_name[50];
	int k, mode;
	k = 0;
	mode = 0;
	for (int i = 5 ; input[i] ; i++) {
		if (input[i] != ' ') {
			file_name[k++] = input[i];
		} else {
			if (strncmp(input + i + 1, "ascii", 5) == 0)
				mode++;
			break;
		}
	}
	file_name[k] = '\0';
	//mode = 0 pentru binar, mode = 1 pentru ascii
	if (!mode) {
		FILE *out = fopen(file_name, "wb");
		if (!out) {
			printf("No file to save\n");
			return;
		}
		if (poza->RGB)
			fprintf(out, "%s\n", "P6");
		else
			fprintf(out, "%s\n", "P5");
		fprintf(out, "%d %d\n", poza->width, poza->height);
		fprintf(out, "%d\n", poza->max_value);
		if (poza->GRAYSCALE) {
			for (int i = 0 ; i < poza->height ; i++)
				fwrite(poza->poza_curenta[i], sizeof(UC), poza->width, out);
		} else {
			for (int i = 0 ; i < poza->height ; i++)
				fwrite(poza->poza_curenta[i], sizeof(UC), poza->width * 3, out);
		}
		fclose(out);
		printf("Saved %s\n", file_name);
	} else {
		FILE *out = fopen(file_name, "wt");
		if (!out) {
			printf("No file to save\n");
			return;
		}
		if (poza->GRAYSCALE)
			fprintf(out, "%s\n", "P2");
		else
			fprintf(out, "%s\n", "P3");
		fprintf(out, "%d %d\n", poza->width, poza->height);
		fprintf(out, "%d\n", poza->max_value);
		if (poza->GRAYSCALE) {
			for (int i = 0 ; i < poza->height ; i++) {
				for (int j = 0 ; j < poza->width ; j++)
					fprintf(out, "%d ", poza->poza_curenta[i][j]);
				fprintf(out, "\n");
			}
		} else {
			for (int i = 0 ; i < poza->height ; i++) {
				for (int j = 0 ; j < 3 * poza->width ; j++)
					fprintf(out, "%d ", poza->poza_curenta[i][j]);
				fprintf(out, "\n");
			}
		}
		fclose(out);
		printf("Saved %s\n", file_name);
	}
}

void EXIT(editor_foto *poza)
{
	if (!poza->RGB && !poza->GRAYSCALE)
		printf("No image loaded\n");
	if (poza->height > 0)
		free_matrix(poza->poza_curenta, poza->height);
	free(poza);
	exit(0);
}

int main(void)
{
	char input[50];
	editor_foto *poza = (editor_foto *)malloc(sizeof(editor_foto));
	if (!poza) {
		fprintf(stderr, "malloc() failed!\n");
		return 0;
	}
	set_false(poza);//initializam variabilele din struct

	while (1) {
		fgets(input, sizeof(input), stdin);
		if (input[strlen(input) - 1] == '\n')
			input[strlen(input) - 1] = '\0';
		if (strncmp(input, "LOAD", 4) == 0)
			LOAD(poza, input);
		else if (strncmp(input, "SELECT ALL", 10) == 0)
			SELECT_ALL(poza);
		else if (strncmp(input, "SELECT", 6) == 0)
			SELECT(poza, input);
		else if (strncmp(input, "HISTOGRAM", 9) == 0)
			HISTOGRAM(poza, input);
		else if (strncmp(input, "EQUALIZE", 8) == 0)
			EQUALIZE(poza);
		else if (strncmp(input, "ROTATE", 6) == 0)
			ROTATE(poza, input);
		else if (strncmp(input, "CROP", 4) == 0)
			CROP(poza);
		else if (strncmp(input, "APPLY", 5) == 0)
			APPLY(poza, input);
		else if (strncmp(input, "SAVE", 4) == 0)
			SAVE(poza, input);
		else if (strncmp(input, "EXIT", 4) == 0)
			EXIT(poza);
		else
			printf("Invalid command\n");
	}
	return 0;
}
