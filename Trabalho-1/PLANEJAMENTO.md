# Planejamento & Guia de Domínio - TP1: Transformações 2D e Viewport

Este documento serve como roteiro de implementação e guia de estudo para a apresentação do trabalho prático de Computação Gráfica. Cada etapa adiciona código ao projeto e consolida exatamente os conceitos que o professor pode cobrar em uma arguição.

---

## 📊 Status Geral do Projeto

- [x] **Etapa 1:** Estrutura OO & Modelo da Cena
- [x] **Etapa 1:** Estrutura OO & Modelo da Cena
- [x] **Etapa 2:** Mapeamento Mundo $\to$ Viewport (Inversão do Eixo Y)
- [ ] **Etapa 3:** Transformações Geométricas 2D (Matrizes Homogêneas 3×3)
- [ ] **Etapa 4:** Demonstração da Não-Comutatividade ($T \cdot S \neq S \cdot T$)
- [ ] **Etapa 5:** Interatividade, Painel Lateral, Documentação & Entrega

---

## 🔹 Etapa 1: Estrutura Orientada a Objetos & Modelo da Cena
**Objetivo de Aprendizagem:** Compreender a representação computacional de formas geométricas 2D no espaço contínuo do mundo, o encapsulamento de responsabilidades e a imutabilidade dos vértices originais.

### Checklist de Atividades:
- [x] Criar classe `Vertice` (encapsulando coordenadas no mundo via `glm::vec2`).
- [x] Criar classe `Poligono` (cor RGB `glm::vec3`, lista ordenada de vértices no mundo e compatibilidade com `PoligonoBase`).
- [x] Criar classe `Objeto2D` (agrupador de polígonos, armazenamento da matriz acumulada $M = I_{3\times3}$, cálculo do centro geométrico original e atual, compatibilidade com `ObjetoBase`).
- [x] Criar classe `Cena` (gerenciamento da coleção de objetos, seleção do objeto ativo e carregamento dinâmico via `cena_inicial.hpp` / `criarCenaInicial()`).
- [x] Configurar o `Makefile` básico para compilação com `g++ -std=c++17 -Wall -Wextra -O2 -Iinclude`.

### 💡 O que responder na Arguição (30 segundos):
> **Pergunta provável:** *"Como os objetos são modelados e por que os vértices originais não são alterados após uma transformação?"*  
> **Resposta:** *"Os vértices originais definem a geometria de repouso no sistema do mundo e permanecem imutáveis na memória para evitar acúmulo de erros numéricos de ponto flutuante e permitir reset instantâneo. Cada objeto mantém apenas uma matriz homogênea acumulada $M$ (iniciada como identidade $3 \times 3$). Quando a cena é desenhada, aplicamos $P_{\text{mundo}} = M \cdot P_{\text{original}}$ em tempo de execução."*

---

## 🔹 Etapa 2: Mapeamento Mundo $\rightarrow$ Viewport & Inversão do Eixo Y
**Objetivo de Aprendizagem:** Compreender o pipeline de visualização 2D e implementar a conversão matemática explícita de coordenadas reais (mundo) para coordenadas de pixel da janela sem recorrer a funções automáticas proibidas do OpenGL (`glViewport`, `gluOrtho2D` no desenho das formas).

### Checklist de Atividades:
- [x] Criar estruturas auxiliares `JanelaMundo` ($x_{\min}, x_{\max}, y_{\min}, y_{\max}$) e `RegiaoViewport` ($x_{\min}, x_{\max}, y_{\min}, y_{\max}$ na tela).
- [x] Criar classe `Viewport` com os métodos de conversão explícita:
  - `mundoParaViewport(glm::vec2)`: aplicando escala e inversão do eixo $Y$.
  - `viewportParaMundo(glm::vec2)`: conversão inversa para futuros cliques de mouse.
- [x] Configurar a janela básica com GLUT (`glutInit`, `glutCreateWindow`, callbacks `display` e `reshape`).
- [x] Desenhar a delimitação visual da viewport gráfica na tela (`GL_LINE_LOOP`).
- [x] Desenhar os eixos cartesianos com setas, graduações (*ticks*) e valores numéricos no mundo.
- [x] Renderizar os objetos estáticos da cena dentro da viewport.

### 💡 O que responder na Arguição (30 segundos):
> **Pergunta provável:** *"Por que é necessário inverter o eixo Y e qual é a fórmula de mapeamento?"*  
> **Resposta:** *"No sistema cartesiano do mundo, o eixo $Y$ cresce para cima. No sistema de coordenadas de tela e eventos de janelas/mouse, a origem fica no canto superior esquerdo e o eixo $Y$ cresce para baixo. Por isso, invertemos o eixo na fórmula:  
> $x_v = x_{v\min} + \frac{x_w - x_{w\min}}{x_{w\max} - x_{w\min}} \cdot (x_{v\max} - x_{v\min})$  
> $y_v = y_{v\min} + \frac{y_{w\max} - y_w}{y_{w\max} - y_{w\min}} \cdot (y_{v\max} - y_{v\min})$  
> O OpenGL recebe exclusivamente os pontos finais calculados em pixels."*

---

## 🔹 Etapa 3: Transformações Geométricas 2D (Matrizes Homogêneas 3×3)
**Objetivo de Aprendizagem:** Dominar a formulação matemática de matrizes afins homogêneas $3 \times 3$, a convenção de vetores-coluna da GLM e a composição em torno de centros arbitrários.

### Checklist de Atividades:
- [ ] Criar classe de utilitários `Transformacoes`:
  - Translação: $T(\Delta x, \Delta y)$
  - Escala: $S(s_x, s_y)$ (uniforme e não-uniforme)
  - Rotação em torno da origem: $R(\theta)$
  - Rotação em torno do centro geométrico: $M = T(C) \cdot R(\theta) \cdot T(-C)$
  - Escala em torno do centro geométrico: $M = T(C) \cdot S(s_x, s_y) \cdot T(-C)$
  - Cisalhamento: $H_x(h)$ e $H_y(h)$
  - Reflexão: em relação ao eixo X e ao eixo Y
- [ ] Implementar a acumulação matricial à esquerda no `Objeto2D`: $M = T_{\text{nova}} \cdot M$.
- [ ] Implementar a função `resetar()` que restaura $M = I_{3 \times 3}$.

### 💡 O que responder na Arguição (30 segundos):
> **Pergunta provável:** *"Por que usamos coordenadas homogêneas 3x3 para 2D e como você implementou a rotação no centro?"*  
> **Resposta:** *"Usamos coordenadas homogêneas $[x, y, 1]^T$ porque a translação não é linear em 2D; ao adicionar a dimensão homogênea $W=1$, a translação vira uma multiplicação matricial, permitindo compor qualquer sequência em uma única matriz 3x3. Para rotacionar em torno do centro $C$, primeiro transladamos o objeto para a origem com $T(-C)$, aplicamos a rotação $R(\theta)$ na origem, e depois devolvemos o objeto à sua posição com $T(C)$, resultando em $M = T(C) \cdot R(\theta) \cdot T(-C)$."*

---

## 🔹 Etapa 4: Composição & Não-Comutatividade ($T \cdot S \neq S \cdot T$)
**Objetivo de Aprendizagem:** Demonstrar geometricamente e algebricamente que o produto de transformações lineares e afins não é comutativo.

### Checklist de Atividades:
- [ ] Criar modo especial de exibição dividida (duas viewports simultâneas independentes).
- [ ] Lado Esquerdo: aplicar Escala seguida de Translação ($M = T \cdot S$).
- [ ] Lado Direito: aplicar Translação seguida de Escala ($M = S \cdot T$).
- [ ] Desenhar o objeto original em aramado (*wireframe*) para referência visual da origem.
- [ ] Adicionar legendas explicativas superiores e eixos cartesianos locais.

### 💡 O que responder na Arguição (30 segundos):
> **Pergunta provável:** *"Por que $T \cdot S$ resulta em uma posição diferente de $S \cdot T$?"*  
> **Resposta:** *"A multiplicação matricial não é comutativa. Em $T \cdot S$, o objeto é escalado na origem e depois deslocado exatamente pelo vetor de translação $(\Delta x, \Delta y)$. Em $S \cdot T$, o objeto é transladado primeiro e, quando a escala é aplicada a seguir, ela multiplica inclusive as coordenadas de translação $(s_x \cdot \Delta x, s_y \cdot \Delta y)$, afastando ou aproximando o objeto da origem."*

---

## 🔹 Etapa 5: Interatividade, Interface, Documentação & Entrega
**Objetivo de Aprendizagem:** Conectar a arquitetura ao loop de eventos do usuário (teclado, mouse e painel de controle) e preparar todos os artefatos de entrega conforme o edital do trabalho.

### Checklist de Atividades:
- [ ] Desenhar painel lateral fora da viewport com primitivas OpenGL (`GL_QUADS`, `GL_LINE_LOOP`) e texto (`glutBitmapCharacter`).
- [ ] Exibir no painel o nome do objeto ativo, coordenadas do centro no mundo e a matriz acumulada $M$ formatada em tempo real ($3 \times 3$).
- [ ] Configurar atalhos de teclado para todas as transformações (setas, R, O, +, -, ], [, H, X, Y, 0, D).
- [ ] Implementar seleção de objetos por clique na lista do painel e por clique na viewport.
- [ ] Implementar seleção e translação direta por arraste com o mouse (convertendo $\Delta\text{pixel} \to \Delta\text{mundo}$).
- [ ] Gerar as capturas de tela obrigatórias (`captura_transformacao_composta.png` e `captura_ordem_transformacoes.png`).
- [ ] Criar o `README.md` completo com as instruções de compilação, requisitos, dados dos integrantes e referências.

### 💡 O que responder na Arguição (30 segundos):
> **Pergunta provável:** *"Como funciona o arraste do mouse para mover o objeto no mundo?"*  
> **Resposta:** *"O evento de movimento do mouse fornece posições em pixels da janela. Capturamos o ponto anterior e o ponto atual do mouse e os convertemos para coordenadas do mundo usando a função inversa da viewport (`viewportParaMundo`). A diferença $(\Delta x, \Delta y)$ resultante já está no sistema cartesiano do mundo, e então multiplicamos uma matriz de translação $T(\Delta x, \Delta y)$ na matriz acumulada $M$ do objeto ativo."*

---
