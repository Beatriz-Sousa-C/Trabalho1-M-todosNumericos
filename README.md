## Objetivo

O objetivo deste projeto é implementar e comparar métodos numéricos
para encontrar raízes de equações. O programa permite informar os
parâmetros do problema, executar os métodos disponíveis e visualizar
as aproximações, os erros e a quantidade de iterações.

## Tecnologias utilizadas

- C++17
- CMake
- Linux

## Requisitos

Para compilar o projeto, é necessário possuir:

- Compilador compatível com C++17
- CMake versão 3.16 ou superior

## Compilação

Na raiz do projeto, execute:

```bash
cmake -S . -B build
cmake --build build
```

## Execução

Após a compilação, execute:

```bash
./build/main.exe
```