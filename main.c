#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ROWS 1000
#define MAX_COLS 100

typedef struct {
    char headers[MAX_COLS][50]; // Armazena cabeçalhos de colunas
    float data[MAX_ROWS][MAX_COLS]; // Armazena apenas floats
    int rows;
    int cols;
} CSVData;



int isFloat(const char *str) {
    char *endptr;
    strtof(str, &endptr);
    // Verifica se endptr não aponta para o final da string
    return (*endptr == '\0' && endptr != str);
}





void loadCSV(const char *filename, CSVData *csvData) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return;
    }

    char line[1024];
    int row = 0;

    // Lê o cabeçalho
    if (fgets(line, sizeof(line), file)) {
        int col = 0;
        char *token = strtok(line, ",");
        while (token && col < MAX_COLS) {
            // Remove caracteres de nova linha do token
            char *newline = strchr(token, '\n');
            if (newline) *newline = '\0';
            
            strcpy(csvData->headers[col], token);
            token = strtok(NULL, ",");
            col++;
        }
        csvData->cols = col; // Atualiza o número de colunas
    }

    // Lê os dados
    while (fgets(line, sizeof(line), file) && row < MAX_ROWS) {
        int col = 0;
        char *token = strtok(line, ",");
        while (token && col < MAX_COLS) {
            // Remove caracteres de nova linha do token
            char *newline = strchr(token, '\n');
            if (newline) *newline = '\0';

            // Verifica se o token é um float antes de armazenar
            if (isFloat(token)) {
                csvData->data[row][col] = strtof(token, NULL);
                col++;
            }
            token = strtok(NULL, ",");
        }
        // Verifica se há um valor na última coluna sem vírgula
        if (token == NULL && line[strlen(line) - 1] != '\n' && isFloat(line)) {
            csvData->data[row][col] = strtof(line, NULL);
            col++;
        }
        row++;
    }

    csvData->rows = row;
    fclose(file);
}



void printRow(int cols, char headers[][50], float *row) {
    for (int i = 0; i < cols; i++) {
        printf("%s: %f ", headers[i], row[i]);
    }
    printf("\n");
}


int main() {
    CSVData csvData = { .rows = 0, .cols = 0 };

    loadCSV("arquivo.csv", &csvData);

    // Imprime cabeçalhos
    for (int j = 0; j < csvData.cols; j++) {
        printf("%s ", csvData.headers[j]);
    }
    printf("\n");


    // Imprime os dados
    for (int i = 0; i < csvData.rows; i++) {
        for (int j = 0; j < csvData.cols; j++) {
            printf("%f ", csvData.data[i][j]);
        }
        printf("\n");
    }

        // Imprime os dados usando a nova função
    for (int i = 0; i < csvData.rows; i++) {
        printRow(csvData.cols, csvData.headers, csvData.data[i]);
    }

    return 0;
}
