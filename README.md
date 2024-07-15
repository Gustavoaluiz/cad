1. Nomes e Matrículas dos Integrantes do Grupo:
  Artur Matos Andrade Novais - 202202431
  Daniel Fazzioni - 202202433
  Gustavo Luiz Bueno Pereira - 202202444
  Thiago Pedroso de Jesus - 202202460


2. Problema a ser enfrentado: Para resolver o problema de performance e consumo de memória com grandes datasets, vamos criar um código em C que aproveite o paralelismo. Utilizaremos threads ou bibliotecas como OpenMP para distribuir a carga de trabalho entre múltiplos núcleos de CPU, e frameworks como CUDA para executar milhares de threads em paralelo na GPU. Esse código será capaz de executar uma função similar ao apply do pandas, permitindo definir qual função será aplicada ao dataset. O código deve carregar o dataset, aplicar a função definida e salvar o resultado, aumentando drasticamente a eficiência e a escalabilidade no processamento de grandes volumes de dados.


3. Objetivos:
  a) Implementar uma solução sequencial para o problema;
  b) Implementar uma solução paralela em CPU;
  c) Implementar uma solução paralela em GPU;
  d) Comparar o desempenho da nossa aplicação com a implementação em GPU e CPU, para diferentes cenários;
  e) Comparar o desempenho da nossa aplicação com a biblioteca pandas;
  f) Apresentar o speedup geral, absoluto e relativo, em relação à solução sequencial proposta pelo grupo e à biblioteca pandas;
  g) Calcular a eficiência em CPU.
  h) Comparar o desempenho da nossa aplicação com a implementação de CUDA em python;
