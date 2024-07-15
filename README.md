# Projeto de Paralelismo para Processamento de Grandes Datasets

## Integrantes do Grupo
- **Artur Matos Andrade Novais** - 202202431
- **Daniel Fazzioni** - 202202433
- **Gustavo Luiz Bueno Pereira** - 202202444
- **Thiago Pedroso de Jesus** - 202202460

## Problema a Ser Enfrentado
Para resolver o problema de performance e consumo de memória com grandes datasets, vamos criar um código em C que aproveite o paralelismo. Utilizaremos threads ou bibliotecas como OpenMP para distribuir a carga de trabalho entre múltiplos núcleos de CPU, e frameworks como CUDA para executar milhares de threads em paralelo na GPU. Esse código será capaz de executar uma função similar ao `apply` do pandas, permitindo definir qual função será aplicada ao dataset. O código deve carregar o dataset, aplicar a função definida e salvar o resultado, aumentando drasticamente a eficiência e a escalabilidade no processamento de grandes volumes de dados.

## Objetivos
1. **Implementar uma solução sequencial para o problema;**
2. **Implementar uma solução paralela em CPU;**
3. **Implementar uma solução paralela em GPU;**
4. **Comparar o desempenho da nossa aplicação com a implementação em GPU e CPU, para diferentes cenários;**
5. **Comparar o desempenho da nossa aplicação com a biblioteca pandas;**
6. **Apresentar o speedup geral, absoluto e relativo, em relação à solução sequencial proposta pelo grupo e à biblioteca pandas;**
7. **Calcular a eficiência em CPU;**
8. **Comparar o desempenho da nossa aplicação com a implementação de CUDA em Python.**

## Instruções de Uso
### Dependências
- OpenMP
- CUDA Toolkit

### Compilação
Para compilar a versão sequencial:
```sh
gcc -o sequencial sequencial.c
