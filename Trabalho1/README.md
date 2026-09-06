# TP1 - Transformações Geométricas 2D e Viewport

**Disciplina:** Computação Gráfica  
**Curso:** Bacharelado em Ciência da Computação  
**Instituição:** Instituto Federal do Norte de Minas Gerais (IFNMG) - Campus Montes Claros  
**Professor:** Wagner Ferreira de Barros

---

## 1. Integrantes

- Vinicius Macedo e Ícaro Vinicius

---

## 2. Sistema Operacional e Ambiente de Desenvolvimento

- **Sistema Operacional:** Linux (x86_64 compatível com Ubuntu, Debian, Arch Linux)
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

_(Nota: O projeto inclui os cabeçalhos da GLM no diretório `include/glm/` para garantir compilação imediata e independente)._

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
│   ├── Transformacoes.hpp  # Fábrica de matrizes afins homogêneas 3x3 (GLM)
│   ├── Viewport.hpp        # Mapeamento explícito Mundo -> Viewport com inversão do eixo Y
│   ├── Cena.hpp            # Gerenciador da coleção de objetos e objeto ativo
│   └── Aplicacao.hpp       # Loop da aplicação, callbacks GLUT, interface e terminal
├── src/
│   ├── Vertice.cpp
│   ├── Poligono.cpp
│   ├── Objeto2D.cpp
│   ├── Viewport.cpp
│   ├── Cena.cpp
│   ├── Aplicacao.cpp
│   └── main.cpp
├── Makefile
├── PLANEJAMENTO.md
└── README.md
```

### 5.1 Arquivos Iniciais e Modelo da Cena

O diretório `include/` incorpora a descrição da cena inicial do TP1 em coordenadas do mundo:

- `scene_types.hpp`: define as estruturas `PoligonoBase` (cor RGB e lista de vértices `glm::vec2`) e `ObjetoBase` (nome e lista de `PoligonoBase`).
- `casa.hpp`, `barco.hpp` e `moinho.hpp`: definem as figuras coloridas em coordenadas cartesianas do mundo.
- `cena_inicial.hpp`: instancia e posiciona a Casa, o Barco e o Moinho em coordenadas estáticas do mundo.

---

## 6. Mapeamento Mundo -> Viewport & Inversão do Eixo Y

Em conformidade com as regras do trabalho prático, as coordenadas dos vértices transformados são calculadas explicitamente pelo programa antes de serem enviadas ao OpenGL. Funções proibidas de transformação e viewport automáticos (`glViewport`, `gluOrtho2D`, `glTranslatef`, `glRotatef`, `glScalef`) não são utilizadas para desenhar as formas.

### Fórmulas Matemáticas Utilizadas:

Para mapear um ponto do mundo $P_w = (x_w, y_w)$ para a viewport da janela $P_v = (x_v, y_v)$:

$$x_v = x_{v\min} + \frac{x_w - x_{w\min}}{x_{w\max} - x_{w\min}} \cdot (x_{v\max} - x_{v\min})$$

$$y_v = y_{v\min} + \frac{y_{w\max} - y_w}{y_{w\max} - y_{w\min}} \cdot (y_{v\max} - y_{v\min})$$

> **Nota sobre o eixo Y:** No sistema cartesiano do mundo, $Y$ cresce para cima. No sistema de pixels de tela/eventos da janela, a origem $(0, 0)$ fica no canto superior esquerdo e $Y$ cresce para baixo. A inversão vertical é garantida pelo termo $(y_{w\max} - y_w)$.

---

## 7. Transformações Geométricas 2D & Coordenadas Homogêneas 3×3

Cada objeto mantém sua matriz acumulada $M = I_{3 \times 3}$. Todas as transformações são representadas por matrizes afins homogêneas $3 \times 3$ com a convenção de vetores-coluna (padrão GLM/OpenGL):

$$\begin{bmatrix} x' \\ y' \\ 1 \end{bmatrix} = M \cdot \begin{bmatrix} x \\ y \\ 1 \end{bmatrix}$$

A composição de uma nova transformação $T_{\text{nova}}$ sobre a matriz acumulada é feita à esquerda (ordem cronológica padrão):
$$M_{\text{acumulada}} \leftarrow T_{\text{nova}} \cdot M_{\text{acumulada}}$$

---

## 8. Principais Fórmulas Matemáticas das Operações

Abaixo estão as matrizes afins homogêneas $3 \times 3$ implementadas para cada operação geométrica:

### 8.1 Translação $T(\Delta x, \Delta y)$
Desloca os vértices horizontalmente por $\Delta x$ e verticalmente por $\Delta y$:
$$T(\Delta x, \Delta y) = \begin{bmatrix} 1 & 0 & \Delta x \\ 0 & 1 & \Delta y \\ 0 & 0 & 1 \end{bmatrix}$$

### 8.2 Escala na Origem $S(s_x, s_y)$
Modifica as proporções dos vértices em relação à origem $(0,0)$. Se $s_x = s_y$, a escala é uniforme; caso contrário, é não-uniforme:
$$S(s_x, s_y) = \begin{bmatrix} s_x & 0 & 0 \\ 0 & s_y & 0 \\ 0 & 0 & 1 \end{bmatrix}$$

### 8.3 Rotação na Origem $R(\theta)$
Gira os vértices por um ângulo $\theta$ (em radianos) no sentido anti-horário em torno de $(0,0)$:
$$R(\theta) = \begin{bmatrix} \cos(\theta) & -\sin(\theta) & 0 \\ \sin(\theta) & \cos(\theta) & 0 \\ 0 & 0 & 1 \end{bmatrix}$$

### 8.4 Rotação em Torno de um Centro Arbitrário $C = (x_c, y_c)$
Para rotacionar o objeto ao redor do seu próprio centro geométrico sem deslocá-lo para a origem:
1. Translada o centro para a origem: $T(-x_c, -y_c)$
2. Aplica a rotação: $R(\theta)$
3. Devolve o objeto à posição original: $T(x_c, y_c)$

$$M_{R_C} = T(x_c, y_c) \cdot R(\theta) \cdot T(-x_c, -y_c)$$

$$\begin{bmatrix}
\cos(\theta) & -\sin(\theta) & x_c(1 - \cos\theta) + y_c \sin\theta \\
\sin(\theta) & \cos(\theta) & y_c(1 - \cos\theta) - x_c \sin\theta \\
0 & 0 & 1
\end{bmatrix}$$

### 8.5 Escala em Torno de um Centro Arbitrário $C = (x_c, y_c)$
Para redimensionar o objeto preservando a posição do seu centro geométrico:
$$M_{S_C} = T(x_c, y_c) \cdot S(s_x, s_y) \cdot T(-x_c, -y_c)$$

$$\begin{bmatrix}
s_x & 0 & x_c(1 - s_x) \\
0 & s_y & y_c(1 - s_y) \\
0 & 0 & 1
\end{bmatrix}$$

### 8.6 Cisalhamento (Shear) $H_x(h_x)$ e $H_y(h_y)$
Distorce a geometria inclinando-a linearmente ao longo de um eixo em função da coordenada do outro:
- **Cisalhamento ao longo do eixo X:**
  $$H_x(h_x) = \begin{bmatrix} 1 & h_x & 0 \\ 0 & 1 & 0 \\ 0 & 0 & 1 \end{bmatrix} \implies \begin{cases} x' = x + h_x \cdot y \\ y' = y \end{cases}$$

- **Cisalhamento ao longo do eixo Y:**
  $$H_y(h_y) = \begin{bmatrix} 1 & 0 & 0 \\ h_y & 1 & 0 \\ 0 & 0 & 1 \end{bmatrix} \implies \begin{cases} x' = x \\ y' = y + h_y \cdot x \end{cases}$$

### 8.7 Reflexão (Espelhamento)
Inverte as coordenadas em relação aos eixos cartesianos:
- **Reflexão no eixo X** (inverte verticalmente o sinal de $y$):
  $$Ref_x = \begin{bmatrix} 1 & 0 & 0 \\ 0 & -1 & 0 \\ 0 & 0 & 1 \end{bmatrix}$$

- **Reflexão no eixo Y** (inverte horizontalmente o sinal de $x$):
  $$Ref_y = \begin{bmatrix} -1 & 0 & 0 \\ 0 & 1 & 0 \\ 0 & 0 & 1 \end{bmatrix}$$

---

## 9. Não-Comutatividade das Transformações (Tecla D)

O programa demonstra geometricamente e algebricamente a não-comutatividade do produto matricial ($A \cdot B \neq B \cdot A$):

- **Modo Normal:** $M = T_{\text{nova}} \cdot M$ (a operação atual atua sobre o estado e centro atuais do objeto).
- **Modo Ordem Contrária (Tecla D):** calcula $M = M \cdot T_{\text{nova}}$ (pré-multiplicando as operações na base antes das translações anteriores).
  Ao alternar a tecla **`D`**, a tela ganha destaque avermelhado e renderiza toda a cena com a matriz de ordem inversa, permitindo visualizar órbitas distorcidas causadas pela troca da ordem, com um aramado de referência da posição correta.

---

## 10. Controles e Atalhos

### Teclado (Janela Gráfica):

| Tecla                     | Ação                                                             |
| :------------------------ | :--------------------------------------------------------------- |
| **`1`**, **`2`**, **`3`** | Seleciona o objeto ativo (Casa, Barco, Moinho)                   |
| **Setas direcionais**     | Translação nos eixos X e Y                                       |
| **`R` / `r`**             | Rotação horária / anti-horária em torno do **centro geométrico** |
| **`O` / `o`**             | Rotação horária / anti-horária em torno da **origem $(0, 0)$**   |
| **`+` / `-`**             | Escala uniforme (+10% / -10%) em torno do centro                 |
| **`]` / `[`**             | Escala não-uniforme no eixo X (+10% / -10%)                      |
| **`H` / `h`**             | Cisalhamento (_shear_) nos eixos Y / X                           |
| **`X` / `Y`**             | Reflexão / Espelhamento no eixo X / eixo Y                       |
| **`0` (zero)**            | Reset da matriz acumulada $M$ para a Identidade                  |
| **`D`**                   | Alterna entre o modo Normal e o modo de Ordem Contrária          |
| **`Q` / `ESC`**           | Sair da aplicação                                                |

### Mouse:

- **Clique com botão esquerdo na Viewport:** seleciona o objeto mais próximo do cursor.
- **Clique e arraste com o botão esquerdo:** move diretamente o objeto ativo pelo mundo, convertendo $\Delta\text{pixel} \to \Delta\text{mundo}$.
- **Clique na lista do Painel Lateral:** alterna o objeto ativo.
