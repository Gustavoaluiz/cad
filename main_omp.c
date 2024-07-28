#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define INF 1000000
#define N 228

typedef struct {
    float** matriz_custo;  // Matriz com os custos de ir de uma cidade para outra
    float alpha;
    int max_iter;
    int cidade_partida;
    int* melhor_solucao;
    float melhor_custo;
    omp_lock_t lock;  // Lock para sincronizar acesso à melhor solução e custo
} GraspReativo;


float** ler_matriz_custo(const char* nome_arquivo, int tamanho_matriz) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(1);
    }

    float** matriz = (float**)malloc(tamanho_matriz * sizeof(float*));
    for (int i = 0; i < tamanho_matriz; i++) {
        matriz[i] = (float*)malloc(tamanho_matriz * sizeof(float));
    }

    for (int i = 0; i < tamanho_matriz; i++) {
        for (int j = 0; j < tamanho_matriz; j++) {
            if (fscanf(arquivo, "%f", &matriz[i][j]) != 1) {
                fprintf(stderr, "Erro ao ler o arquivo %s\n", nome_arquivo);
                exit(1);
            }
        }
    }

    fclose(arquivo);
    return matriz;
}

void inicializa_grasp(GraspReativo* grasp, float** matriz, float alpha, int max_iter, int cidade_partida) {
    grasp->matriz_custo = matriz;
    grasp->alpha = alpha;
    grasp->max_iter = max_iter;
    grasp->cidade_partida = cidade_partida;
    grasp->melhor_solucao = NULL;
    grasp->melhor_custo = INF;
    omp_init_lock(&grasp->lock);
}

int* construcao(GraspReativo* grasp, int tamanho_matriz) {
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

        int limite = min + grasp->alpha * (max - min);
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
    #pragma omp parallel
    {
        #pragma omp for
        for (int iteracao = 0; iteracao < grasp->max_iter; iteracao++) {
            printf("Iteração %d\n", iteracao);

            int* solucao_inicial = construcao(grasp, tamanho_matriz);

            busca_local(solucao_inicial, grasp, tamanho_matriz, max_iter_local);
            float custo_solucao = calcula_custo(grasp, solucao_inicial, tamanho_matriz);

            #pragma omp critical
            {
                printf("Thread %d entrando na seção crítica\n", omp_get_thread_num());
                printf("Melhor custo local: %f, Melhor custo global antes: %f\n", custo_solucao, grasp->melhor_custo);
                if (custo_solucao < grasp->melhor_custo) {
                    if (grasp->melhor_solucao) {
                        free(grasp->melhor_solucao);
                    }
                    grasp->melhor_solucao = solucao_inicial;
                    grasp->melhor_custo = custo_solucao;
                } else {
                    free(solucao_inicial);
                }
            }
        }
    }
}

int main() {
    srand(time(NULL));
    clock_t inicio, fim;
    double tempoGasto;
    inicio = clock();  

    // Exemplo de inicialização da matriz de custo e parâmetros
    int tamanho_matriz = N;
    const char* nome_arquivo = "data.txt";

    // Converte a matriz estática para um ponteiro de ponteiros
    float** matriz = ler_matriz_custo(nome_arquivo, tamanho_matriz);

    float alpha = 0.2;
    int max_grasp_iter = 50;
    int max_iter_local = 1000;
    int cidade_partida = 0;

    GraspReativo grasp;
    inicializa_grasp(&grasp, matriz, alpha, max_grasp_iter, cidade_partida);

    gerar_solucao(&grasp, tamanho_matriz, max_iter_local);

    fim = clock();
    tempoGasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("Melhor solução encontrada com custo %f:\n", grasp.melhor_custo);
    printf("Tempo gasto: %.2fs\n", tempoGasto);
    // Libera a memória alocada
    for (int i = 0; i < tamanho_matriz; i++) {
        free(matriz[i]);
    }
    free(matriz);
    free(grasp.melhor_solucao);

    return 0;
}
