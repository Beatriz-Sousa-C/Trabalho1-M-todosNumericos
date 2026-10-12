# Organização interna do projeto

Este documento explica a arquitetura e as regras utilizadas no
desenvolvimento. Para instruções de compilação e execução, consulte
o README.md.

## Arquitetura

O projeto utiliza uma organização baseada no padrão MVC:

- `model/`: funções matemáticas e métodos numéricos;
- `view/`: interface;
- `controller/`: comunicação entre View e Model;
- `main.cpp`: criação e inicialização da aplicação.

## Fluxo principal

1. A View recebe os dados informados pelo usuário.
2. O Controller valida e organiza os dados.
3. O Controller seleciona um método numérico.
4. O método realiza o cálculo.
5. O resultado retorna para a View.

## Adicionando um método numérico

Para adicionar um novo método:

1. Crie os arquivos da classe dentro de `model/`.
2. Faça a classe herdar publicamente de `InterfaceMetodo`.
3. Implemente `calcular`.
4. Implemente `nome`.
5. Adicione o novo `.cpp` ao alvo no `CMakeLists.txt`.
6. Registre o método no Controller.
7. Crie testes utilizando valores com resultados conhecidos.

## Models
### InterfaceMetodo

`InterfaceMetodo` define o contrato comum dos métodos numéricos.

Toda classe que representa um método deve:

1. herdar publicamente de `InterfaceMetodo`;
2. implementar `calcular`;
3. implementar `nome`;
4. manter a assinatura definida pela interface.

#### calcular

Executa um método numérico para aproximar uma raiz da função recebida.

Recebe:

- uma `Funcao`, que representa a equação do pêndulo e permite avaliar
  `f(d)` e sua derivada `f'(d)`;
- um objeto `Parametros`, que contém a aproximação inicial, a precisão,
  o limite de iterações e os parâmetros adicionais utilizados pelas
  diferentes versões do método de Newton.

Retorna:

- um `Resultado` contendo o deslocamento aproximado `d`;
- o erro estimado na última iteração;
- o número de iterações realizadas;
- o estado de convergência;
- uma mensagem explicando o resultado ou uma possível falha.

O método não deve alterar a função, os parâmetros recebidos nem o
próprio objeto. Cada implementação deve validar os parâmetros que
utiliza e interromper o cálculo quando o critério de convergência for
atingido ou o limite de iterações for alcançado.

Veja um exemplo de classe que implementa `InterfaceMetodo`:

```cpp
class NewtonOriginal : public InterfaceMetodo {
public:
    Resultado calcular(
        const Funcao& funcao,
        const Parametros& parametros
    ) const override;

    std::string nome() const override;
};
```

O método `NewtonOriginal::calcular` utiliza:

- `funcao.avaliar(d)` para obter `f(d)`;
- `funcao.avaliarDerivada(d)` para obter `f'(d)`;
- `parametros.aproximacaoInicial` como valor inicial de `d`;
- `parametros.precisao` como critério de convergência;
- `parametros.maximoIteracoes` para impedir uma repetição indefinida.

Os campos `lambda` e `passoDerivada` não são utilizados por
`NewtonOriginal`. Eles são destinados, respectivamente, ao método de
Newton com `FL` e ao método de Newton com derivada numérica.

### Funcao

`Funcao` representa uma equação matemática utilizada pelos métodos
numéricos.

Toda classe que representa uma função deve implementar:

- `avaliar(x)`: retorna o valor de `f(x)`;
- `avaliarDerivada(x)`: retorna o valor de `f'(x)`.

Exemplo:

```cpp
class FuncaoPendulo : public Funcao {
public:
    FuncaoPendulo(double a3, double a2);

    double avaliar(double x) const override;
    double avaliarDerivada(double x) const override;

private:
    double a3_;
    double a2_;
};
```

### Parametros

`Parametros` reúne as configurações utilizadas durante um cálculo.

```cpp
struct Parametros {
    double aproximacaoInicial;
    double precisao;
    double lambda;
    double passoDerivada;
    int maximoIteracoes;
};
```
### Resultado

`Resultado` representa a saída comum dos métodos numéricos.

```cpp
struct Resultado {
    double raiz;
    double erro;
    int iteracoes;
    bool convergiu;
    std::string mensagem;
};
```
