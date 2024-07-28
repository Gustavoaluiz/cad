#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INF 1000000
//#define N 228
#define N 20

typedef struct {
    double* alphas;
    double* scores;
    int* usos;
    int num_amostras;
    int qtd;
} Alphas;

typedef struct {
    float** matriz_custo;
    Alphas alpha;
    int max_iter;
    int cidade_partida;
    int* melhor_solucao;
    float melhor_custo;
} GraspReativo;

void inicializa_alphas(Alphas* alpha, double* alphas, int qtd, int num_amostras) {
    alpha->alphas = alphas;
    alpha->scores = (double*)calloc(qtd, sizeof(double));
    alpha->usos = (int*)calloc(qtd, sizeof(int));
    alpha->num_amostras = num_amostras;
    alpha->qtd = qtd;
}

void inicializa_grasp(GraspReativo* grasp, float** matriz, double* alphas, int qtd, int num_amostras, int max_iter, int cidade_partida) {
    grasp->matriz_custo = matriz;
    inicializa_alphas(&grasp->alpha, alphas, qtd, num_amostras);
    grasp->max_iter = max_iter;
    grasp->cidade_partida = cidade_partida;
    grasp->melhor_solucao = NULL;
    grasp->melhor_custo = INF;
}

void atualizar_alpha_prob(GraspReativo* grasp, int index, float custo_solucao) {
    grasp->alpha.scores[index] += 1.0 / (custo_solucao + 1);
}

int retorna_alpha(GraspReativo* grasp, int iteracao) {
    int index;
    
    if (iteracao >= grasp->alpha.num_amostras * grasp->alpha.qtd) {
        double* prob_alphas = (double*)malloc(grasp->alpha.qtd * sizeof(double));
        double soma_scores = 0;

        for (int i = 0; i < grasp->alpha.qtd; i++) {
            prob_alphas[i] = grasp->alpha.scores[i] / grasp->alpha.usos[i];
            soma_scores += prob_alphas[i];
        }

        for (int i = 0; i < grasp->alpha.qtd; i++) {
            prob_alphas[i] /= soma_scores;
        }

        double random_value = (double)rand() / RAND_MAX;
        double cumulative_probability = 0.0;
        for (index = 0; index < grasp->alpha.qtd; index++) {
            cumulative_probability += prob_alphas[index];
            if (random_value <= cumulative_probability) {
                break;
            }
        }

        grasp->alpha.usos[index]++;
        free(prob_alphas);
    } else {
        index = iteracao % grasp->alpha.qtd;
        grasp->alpha.usos[index]++;
    }
    return index;
}

int* construcao(GraspReativo* grasp, double alpha, int tamanho_matriz) {
    int indice_atual = grasp->cidade_partida;
    int* solucao = (int*)malloc(tamanho_matriz * sizeof(int));
    int* restantes = (int*)malloc(tamanho_matriz * sizeof(int));
    int tam_restantes = tamanho_matriz - 1;
    int solucao_size = 1;

    solucao[0] = indice_atual;
    for (int i = 0, j = 0; i < tamanho_matriz; i++) {
        if (i != indice_atual) {
            restantes[j++] = i;
        }
    }

    while (tam_restantes > 0) {
        int* lista_candidatos = (int*)malloc(tam_restantes * sizeof(int));
        int* candidatos = (int*)malloc(tam_restantes * sizeof(int));

        for (int i = 0; i < tam_restantes; i++) {
            lista_candidatos[i] = grasp->matriz_custo[indice_atual][restantes[i]];
            candidatos[i] = restantes[i];
        }

        int min = INF, max = -INF;
        for (int i = 0; i < tam_restantes; i++) {
            if (lista_candidatos[i] < min) min = lista_candidatos[i];
            if (lista_candidatos[i] > max) max = lista_candidatos[i];
        }

        int limite = min + alpha * (max - min);
        int* rcl = (int*)malloc(tam_restantes * sizeof(int));
        int rcl_size = 0;

        for (int i = 0; i < tam_restantes; i++) {
            if (lista_candidatos[i] <= limite) {
                rcl[rcl_size++] = candidatos[i];
            }
        }

        int candidato_escolhido = rcl[rand() % rcl_size];
        solucao[solucao_size++] = candidato_escolhido;

        int* new_restantes = (int*)malloc((tam_restantes - 1) * sizeof(int));
        for (int i = 0, j = 0; i < tam_restantes; i++) {
            if (restantes[i] != candidato_escolhido) {
                new_restantes[j++] = restantes[i];
            }
        }

        free(restantes);
        free(lista_candidatos);
        free(candidatos);
        free(rcl);

        restantes = new_restantes;
        tam_restantes--;
        indice_atual = candidato_escolhido;
    }

    free(restantes);
    return solucao;
}

int calcula_custo(GraspReativo* grasp, int* solucao, int tamanho_matriz) {
    float custo_total = 0;

    for (int i = 0; i < tamanho_matriz - 1; i++) {
        custo_total += grasp->matriz_custo[solucao[i]][solucao[i + 1]];
    }

    custo_total += grasp->matriz_custo[solucao[tamanho_matriz - 1]][solucao[0]];
    return custo_total;
}

void busca_local(int* solucao, GraspReativo* grasp, int tamanho_matriz, int max_iter) {
    int melhorou = 1;
    int iteracao = 0;

    while (melhorou && iteracao < max_iter) {
        melhorou = 0;

        for (int i = 1; i < tamanho_matriz - 1; i++) {
            for (int j = i + 1; j < tamanho_matriz; j++) {
                // Calcular o custo da solução atual
                float custo_atual = calcula_custo(grasp, solucao, tamanho_matriz);

                // Trocar cidades i e j
                int temp = solucao[i];
                solucao[i] = solucao[j];
                solucao[j] = temp;

                // Calcular o custo da nova solução
                int novo_custo = calcula_custo(grasp, solucao, tamanho_matriz);

                // Verificar se a nova solução é melhor
                if (novo_custo < custo_atual) {
                    melhorou = 1;
                } else {
                    // Reverter a troca se não melhorar
                    temp = solucao[i];
                    solucao[i] = solucao[j];
                    solucao[j] = temp;
                }
            }
        }
        iteracao++;
    }
}

void gerar_solucao(GraspReativo* grasp, int tamanho_matriz, int max_iter_local) {
    for (int iteracao = 0; iteracao < grasp->max_iter; iteracao++) {
        int alpha_index = retorna_alpha(grasp, iteracao);
        double alpha = grasp->alpha.alphas[alpha_index];

        int* solucao_inicial = construcao(grasp, alpha, tamanho_matriz);

        busca_local(solucao_inicial, grasp, tamanho_matriz, max_iter_local);
        float custo_solucao = calcula_custo(grasp, solucao_inicial, tamanho_matriz);

        if (custo_solucao < grasp->melhor_custo) {
            if (grasp->melhor_solucao) {
                free(grasp->melhor_solucao);
            }
            grasp->melhor_solucao = solucao_inicial;
            grasp->melhor_custo = custo_solucao;
        } else {
            free(solucao_inicial);
        }

        atualizar_alpha_prob(grasp, alpha_index, custo_solucao);
    }
}

int main() {
    srand(time(NULL));

    // Exemplo de inicialização da matriz de custo e parâmetros
    int tamanho_matriz = N;

    float matriz_exemplo[20][20] = 
    {
        {0   , 59.6, 198 , 51.0, 260 , 26.7, 124 , 207 , 142 , 21.1, 260 , 175 , 169 , 321 , 281 , 93.8, 208 , 426 , 29.9, 409},
        {59.3, 0   , 159 , 105 , 207 , 82.1, 63.1, 154 , 158 , 69  , 273 , 139 , 220 , 380 , 228 , 79.0, 262 , 489 , 70.2, 377},
        {199 , 159 , 0   , 187 , 115 , 225 , 162 , 60.6, 317 , 193 , 259 , 244 , 249 , 518 , 136 , 104 , 356 , 623 , 228 , 448},
        {51.0, 104 , 187 , 0   , 304 , 77.1, 173 , 252 , 193 , 38.9, 211 , 225 , 121 , 373 , 325 , 82.8, 185 , 477 , 80.3, 458},
        {259 , 205 , 114 , 300 , 0   , 282 , 200 , 57.5, 367 , 261 , 348 , 276 , 363 , 580 , 38.7, 218 , 462 , 684 , 283 , 430},
        {26.9, 81.2, 225 , 77.0, 281 , 0   , 145 , 229 , 137 , 46.8, 286 , 191 , 195 , 315 , 302 , 121 , 232 , 419 , 24.5, 422},
        {124 , 62.8, 160 , 169 , 199 , 146 , 0   , 150 , 169 , 133 , 333 , 82.6, 287 , 444 , 220 , 139 , 326 , 548 , 146 , 345},
        {208 , 154 , 62.0, 249 , 57.3, 233 , 159 , 0   , 314 , 218 , 316 , 206 , 311 , 528 , 78.6, 166 , 413 , 633 , 234 , 416},
        {142 , 159 , 327 , 193 , 365 , 136 , 169 , 317 , 0   , 162 , 402 , 179 , 314 , 379 , 387 , 235 , 348 , 463 , 113 , 382},
        {23.6, 58.0, 193 , 40.3, 269 , 49.8, 133 , 217 , 165 , 0   , 249 , 190 , 159 , 345 , 290 , 84.8, 222 , 450 , 53.0, 423},
        {262 , 269 , 258 , 213 , 347 , 288 , 334 , 315 , 404 , 250 , 0   , 405 , 119 , 497 , 359 , 194 , 206 , 602 , 291 , 643},
        {176 , 139 , 242 , 225 , 275 , 187 , 82.2, 198 , 179 , 189 , 409 , 0   , 344 , 493 , 290 , 215 , 382 , 597 , 164 , 243},
        {169 , 219 , 249 , 120 , 362 , 195 , 285 , 309 , 311 , 157 , 119 , 356 , 0   , 402 , 374 , 145 , 130 , 506 , 193 , 576},
        {321 , 380 , 518 , 372 , 579 , 315 , 449 , 527 , 379 , 343 , 498 , 494 , 404 , 0   , 601 , 414 , 289 , 108 , 338 , 727},
        {281 , 226 , 135 , 322 , 39.0, 304 , 221 , 79.7, 384 , 282 , 355 , 291 , 373 , 601 , 0   , 239 , 483 , 706 , 305 , 445},
        {95.2, 77.4, 105 , 82.7, 218 , 121 , 144 , 164 , 233 , 89.3, 198 , 215 , 145 , 414 , 240 , 0   , 252 , 519 , 124 , 453},
        {209 , 262 , 356 , 186 , 462 , 235 , 331 , 410 , 348 , 220 , 206 , 382 , 130 , 289 , 483 , 252 , 0   , 394 , 236 , 616},
        {425 , 489 , 623 , 477 , 684 , 420 , 553 , 631 , 431 , 448 , 602 , 598 , 508 , 108 , 705 , 519 , 393 , 0   , 443 , 831},
        {29.1, 70.2, 227 , 82.7, 279 , 22.8, 146 , 227 , 114 , 48.9, 292 , 167 , 201 , 336 , 300 , 123 , 235 , 440 , 0   , 401},
        {410 , 377 , 447 , 459 , 430 , 422 , 345 , 409 , 382 , 423 , 643 , 243 , 577 , 726 , 445 , 453 , 616 , 831 , 399 , 0}
    };

    // Converte a matriz estática para um ponteiro de ponteiros
    float** matriz = (float**)malloc(tamanho_matriz * sizeof(float*));
    for (int i = 0; i < tamanho_matriz; i++) {
        matriz[i] = (float*)malloc(tamanho_matriz * sizeof(float));
        for (int j = 0; j < tamanho_matriz; j++) {
            matriz[i][j] = matriz_exemplo[i][j];
        }
    }

    double alphas[] = {0.2, 0.25, 0.3, 0.35, 0.4};
    int qtd_alphas = sizeof(alphas) / sizeof(alphas[0]);
    int num_amostras = 10;
    int max_grasp_iter = 50;
    int max_iter_local = 500;
    int cidade_partida = 0;

    GraspReativo grasp;
    inicializa_grasp(&grasp, matriz, alphas, qtd_alphas, num_amostras, max_grasp_iter, cidade_partida);

    gerar_solucao(&grasp, tamanho_matriz, max_iter_local);

    printf("Melhor solução encontrada com custo %f:\n", grasp.melhor_custo);

    // Libera a memória alocada
    for (int i = 0; i < tamanho_matriz; i++) {
        free(matriz[i]);
    }
    free(matriz);
    free(grasp.melhor_solucao);
    free(grasp.alpha.scores);
    free(grasp.alpha.usos);

    return 0;
}
