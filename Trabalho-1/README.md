# TP1 - Transformações Geométricas 2D e Viewport

**Disciplina:** Computação Gráfica  
**Curso:** Bacharelado em Ciência da Computação  
**Instituição:** Instituto Federal do Norte de Minas Gerais (IFNMG) - Campus Montes Claros  
**Professor:** Wagner Ferreira de Barros  

---

## 1. Integrantes
- Vinicius (Insira o nome completo do integrante / dupla aqui)

---

## 2. Sistema Operacional e Ambiente de Desenvolvimento
- **Sistema Operacional:** Linux (Arch Linux x86_64 / Ubuntu / Debian compatível)
- **Compilador:** `g++` (suporte a C++17 ou superior)
- **Bibliotecas Gráficas:** OpenGL Legacy, FreeGLUT (`freeglut3-dev` / `freeglut`), GLM (OpenGL Mathematics)

---

## 3. Dependências e Instalação

### No Linux (Ubuntu / Debian / Linux Mint):
```bash
sudo apt update
sudo apt install build-essential freeglut3-dev libglm-dev
```

### No Arch Linux / Manjaro:
```bash
sudo pacman -S base-devel freeglut glm
```

*(Nota: O projeto inclui os cabeçalhos da GLM no diretório `include/glm/` para garantir compilação imediata e independente sem depender de permissões de superusuário).*

---

## 4. Instruções de Compilação e Execução

O projeto conta com um `Makefile` configurado para automatizar todo o processo.

### Compilar:
```bash
make
# ou
make all
```

### Executar:
```bash
make run
# ou diretamente:
./tp1
```

### Limpar arquivos gerados:
```bash
make clean
```

---

## 5. Estrutura do Programa e Arquitetura

O projeto foi organizado rigorosamente em classes, separando as responsabilidades geométricas, matemáticas, gerenciais e de renderização:

```text
.
├── include/
│   ├── glm/                # Cabeçalhos da biblioteca matemática GLM
│   ├── scene_types.hpp     # Estruturas PoligonoBase e ObjetoBase (coordenadas do mundo)
│   ├── casa.hpp            # Definição dos polígonos da Casa no mundo
│   ├── barco.hpp           # Definição dos polígonos do Barco no mundo
│   ├── moinho.hpp          # Definição dos polígonos do Moinho no mundo
│   ├── cena_inicial.hpp    # Função criarCenaInicial() que reúne os objetos
│   ├── Vertice.hpp         # Representação de coordenadas no espaço 2D
│   ├── Poligono.hpp        # Polígono com cor RGB e lista ordenada de vértices no mundo
│   ├── Objeto2D.hpp        # Agrupador de polígonos, matriz acumulada M e centro geométrico
│   ├── Transformacoes.hpp  # Fábrica de matrizes homogêneas 3x3 (GLM)
│   ├── Viewport.hpp        # Mapeamento explícito Mundo -> Viewport com inversão do eixo Y
│   ├── Cena.hpp            # Gerenciador da coleção de objetos e objeto ativo
│   └── Aplicacao.hpp       # Loop da aplicação, callbacks GLUT, interface e terminal
├── src/
│   ├── Vertice.cpp
│   ├── Poligono.cpp
│   ├── Objeto2D.cpp
│   ├── Transformacoes.cpp
│   ├── Viewport.cpp
│   ├── Cena.cpp
│   ├── Aplicacao.cpp
│   └── main.cpp
├── Makefile
└── README.md
```

### 5.1 Arquivos Iniciais e Modelo da Cena
O diretório `include/` incorpora a descrição da cena inicial do TP1 em coordenadas do mundo:
- `scene_types.hpp`: define as estruturas `PoligonoBase` (cor RGB e lista de vértices `glm::vec2`) e `ObjetoBase` (nome e lista de `PoligonoBase`).
- `casa.hpp`, `barco.hpp` e `moinho.hpp`: definem as figuras coloridas em coordenadas cartesianas do mundo.
- `cena_inicial.hpp`: disponibiliza a função `criarCenaInicial()`, que instancia e reúne os três objetos iniciais.

As estruturas usam `glm::vec2` para vértices e `glm::vec3` para cores RGB. Os vértices permanecem no sistema do mundo; a conversão explícita para a viewport é responsabilidade da implementação.

Exemplo de uso:
```cpp
#include "cena_inicial.hpp"

std::vector<ObjetoBase> cena = criarCenaInicial();
```
A classe `Cena` utiliza essa interface via método `carregarDeCenaBase(...)`, permitindo que o professor modifique, amplie ou substitua a cena para criar variações de avaliação sem alterações manuais no restante da arquitetura.

### 5.2 Principais Classes e Responsabilidades:
1. **`Vertice`**: Encapsula coordenadas $(x, y)$ usando `glm::vec2`.
2. **`Poligono`**: Armazena a cor `glm::vec3` e uma lista `std::vector<Vertice>` no sistema de coordenadas do mundo. Possui construtor e conversão direta com `PoligonoBase` de `scene_types.hpp`.
3. **`Objeto2D`**:
   - Mantém os vértices originais **imutáveis**.
   - Construtível diretamente a partir de `ObjetoBase` de `scene_types.hpp`.
   - Armazena a **matriz acumulada $M$** (`glm::mat3`), inicializada com a identidade.
   - Calcula o centro geométrico $(C_x, C_y)$ pela média aritmética dos vértices originais e rastreia o centro transformado atual no mundo.
4. **`Transformacoes`**:
   - Gera matrizes de translação, escala (uniforme e não uniforme), rotação em torno da origem, rotação em torno de ponto arbitrário ($M = T(C) \cdot R \cdot T(-C)$), reflexões e cisalhamento.
5. **`Viewport`**:
   - Implementa o cálculo explícito de mapeamento do sistema cartesiano do mundo para as coordenadas de pixel da viewport.
   - **Inversão do Eixo Y**: Realiza a inversão necessária ($Y$ cresce para cima no mundo e para baixo na tela/viewport).
6. **`Cena`**:
   - Carrega dinamicamente a cena através do include oficial (`#include "cena_inicial.hpp"` e `criarCenaInicial()`), gerenciando a lista de objetos e o objeto ativo.
7. **`Aplicacao`**:
   - Configura as callbacks do GLUT, desenha a viewport delimitada, desenha eixos cartesianos com graduações, desenha o painel lateral de status e processa entradas do usuário.

---

## 6. Comandos de Interação

A interação pode ser realizada tanto por **atalhos de teclado na janela gráfica** quanto por **comandos digitados no terminal**:

### 6.1 Atalhos de Teclado (na Janela Gráfica)
| Tecla | Ação |
| :--- | :--- |
| `1` | Seleciona a **Casa** como objeto ativo |
| `2` | Seleciona o **Barco** como objeto ativo |
| `3` | Seleciona o **Moinho** como objeto ativo |
| `Setas` ($\leftarrow, \rightarrow, \uparrow, \downarrow$) | Transladar objeto no eixo X e Y |
| `R` / `r` | Rotacionar em torno do centro geométrico do objeto ($\pm 5^\circ$) |
| `O` / `o` | Rotacionar em torno da origem cartesiana $(0, 0)$ ($\pm 5^\circ$) |
| `+` / `-` | Escala uniforme (+10% / -10%) |
| `]` / `[` | Escala não uniforme (estica/encolhe em X) |
| `H` / `h` | Cisalhamento em X e Y |
| `X` / `Y` | Reflexão em relação aos eixos X e Y |
| `0` (zero) | **Reset:** Restaura a matriz acumulada do objeto ativo para a Identidade |
| `D` | Alterna para o **Modo de Demonstração da Ordem das Transformações** ($T \cdot S$ vs $S \cdot T$) |
| `Clique + Arraste` | Seleciona e translada o objeto diretamente com o mouse |
| `Q` ou `Esc` | Encerra a aplicação |

### 6.2 Comandos Interativos no Terminal
Com a aplicação em execução, você pode digitar comandos no terminal onde o programa foi aberto:
- `selecionar <1|2|3|casa|barco|moinho>`
- `transladar <dx> <dy>` ou `mover <dx> <dy>`
- `rotacionar <graus>` (em torno do centro geométrico)
- `rotacionar_origem <graus>` (em torno da origem do mundo)
- `escala <sx> [sy]` (escala uniforme ou não uniforme)
- `cisalhar <hx> <hy>`
- `espelhar <x|y>`
- `reset` (reinicia o objeto ativo)
- `reset_todos` (reinicia todos os objetos da cena)
- `demo` (ativa/desativa a demonstração de não-comutatividade)
- `status` (exibe a matriz acumulada $M$ e o centro no terminal)
- `ajuda` (imprime o manual de comandos)
- `sair` (fecha a aplicação)

---

## 7. Conformidade com as Restrições Técnicas
- **Sem funções proibidas:** Nenhuma função de transformação do OpenGL (`glTranslatef`, `glRotatef`, `glScalef`, `glViewport`) é usada como substituto das operações exigidas. O OpenGL recebe unicamente as coordenadas finais já mapeadas em pixels.
- **Vértices imutáveis:** Os vértices originais permanecem inalterados; toda manipulação é realizada via matriz acumulada homogênea $3 \times 3$ (`glm::mat3`).
- **Mapeamento Explícito:** A conversão Mundo $\to$ Viewport com inversão vertical do eixo $Y$ é calculada matematicamente pela classe `Viewport`.

---

## 8. Fontes Consultadas
- HEARN, Donald; BAKER, M. Pauline; CARITHERS, Warren. *Computer Graphics with OpenGL*. 4. ed. Pearson, 2014.
- Documentação oficial da GLM (OpenGL Mathematics): [glm.g-truc.net](https://glm.g-truc.net)
- Documentação da especificação OpenGL e FreeGLUT: [freeglut.sourceforge.net](https://freeglut.sourceforge.net/)
