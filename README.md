# Algoritmo Evolutivo aplicado ao Problema do Percurso do Cavalo

A Computação Bioinspirada é uma área de pesquisa que se baseia em conceitos biológicos para o desenvolvimento de algoritmos, sendo geralmente aplicados na solução de problemas de alta complexidade. Estes problemas podem ser resolvidos por meio de métodos determinísticos, porém, à medida que o tamanho da instância cresce, o custo computacional pode aumentar drasticamente. Neste sentido, os Algoritmos Evolutivos (AEs) são técnicas de otimização que têm se destacado.

AEs se baseiam fortemente nas teorias evolucionistas de Darwin, por exemplo, a Seleção Natural, para otimização de problemas. Os primeiros trabalhos sobre eles datam da década de 1960, quando (HOLLAND, 1992) começou suas pesquisas sobre o assunto e publicou posteriormente o livro ``Adaptation in Natural and Artificial Systems``, definindo as bases para a Computação Evolutiva. (GOLDBERG, 1987), aluno de Holland, foi o primeiro a obter sucesso na aplicação industrial de AEs.

Esses algoritmos oferecem a vantagem de trabalharem com múltiplas soluções para um problema, diferente de métodos determinísticos que buscam uma única solução ótima. Ainda que haja diversas abordagens de AEs, o princípio básico é o mesmo: dado uma população de indivíduos (i.e. um conjunto de soluções), pressões do ambiente desencadeiam a seleção natural, em que indivíduos mais aptos são privilegiados (neste caso, as soluções mais adequadas) que constituirão uma nova população. O processo se repete até que um número de iterações seja atingido ou a solução mais qualificada seja encontrada.

Neste trabalho de Conclusão de Curso, o Algoritmo Evolutivo será aplicado a um famoso problema combinatório denominado o Problema do Percurso do Cavalo -- PPC, em inglês ``Knight's Tour``, no tabuleiro de xadrez. Esse problema pode ser definido da seguinte forma:
\\

**Dado um tabuleiro $n$ $x$ $m$ qualquer, determine uma sequência legal de movimentos do cavalo de modo que esta peça passe por todas as casas uma única vez, a partir de qualquer casa do tabuleiro.**
\\

A implementação do Algoritmo Evolutivo foi baseada nos trabalhos encontrados na literatura, com a inicialização da população seguindo a implementação de (PINTO, 2013), a avaliação de (GORDON; SLOCUM, 2004) e (AL-GHARAIBEH; QAWAGNEH; ALZAHAWI, 2007), a seleção dos indivíduos pelo torneio, o cruzamento pela recombinação uniforme e a mutação 1-ponto. Em concordância com os objetivos propostos, os operadores torneio dos dissimilares, mutação dos vizinhos e inicialização central foram desenvolvidos, melhorando os resultados encontrados em um tabuleiro de dimensões 20x20.

## Descrição

O código ``main.c`` estuda os operadores de reprodução no contexto de um problema simples.

**Dado uma frase alvo e uma população de frases cópias (indivíduos) gerados aleatóriamente
o programa precisa gerar um indivíduo de fitness zero, ou seja, a frase objetivo.**

O código ``PPC.c``, baseado no código desenvolvido no ``main.c``, é uma solução para o clássico
Problema Combinatório do Percurso do Cavalo.

## Instalação

O código foi desenvolvido em linguagem C, sistema operacional Windows com auxílio da 
ferramenta VSCode. As extensões usadas são descritas nos links abaixo:

```
Name: C/C++
Id: ms-vscode.cpptools
Description: C/C++ IntelliSense, debugging, and code browsing.
Version: 1.17.2
Publisher: Microsoft
VS Marketplace Link: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
```

```
Name: C/C++ Extension Pack
Id: ms-vscode.cpptools-extension-pack
Description: Popular extensions for C++ development in Visual Studio Code.
Version: 1.3.0
Publisher: Microsoft
VS Marketplace Link: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack
```

```
Name: Code Runner
Id: formulahendry.code-runner
Description: Run C, C++, Java, JS, PHP, Python, Perl, Ruby, Go, Lua, Groovy, PowerShell, CMD, BASH, F#, C#, VBScript, TypeScript, CoffeeScript, Scala, Swift, Julia, Crystal, OCaml, R, AppleScript, Elixir, VB.NET, Clojure, Haxe, Obj-C, Rust, Racket, Scheme, AutoHotkey, AutoIt, Kotlin, Dart, Pascal, Haskell, Nim, 
Version: 0.12.0
Publisher: Jun Han
VS Marketplace Link: https://marketplace.visualstudio.com/items?itemName=formulahendry.code-runner
```
