#include "Aplicacao.hpp"
#include "Transformacoes.hpp"
#include <GL/freeglut.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <unistd.h>
#include <poll.h>

Aplicacao* Aplicacao::instancia = nullptr;

Aplicacao::Aplicacao(int largura, int altura, const std::string& titulo)
    : larguraJanela(largura), alturaJanela(altura), tituloJanela(titulo),
      cena(), modoDemonstracaoOrdem(false),
      mousePressionado(false), mouseUltimoX(0), mouseUltimoY(0) {
    instancia = this;

    // Configuração inicial da viewport principal e janela do mundo
    // Viewport gráfica ocupa o lado esquerdo e central
    float vpXMin = 30.0f;
    float vpXMax = 720.0f;
    float vpYMin = 40.0f;
    float vpYMax = alturaJanela - 30.0f;

    // Janela do mundo padrão de [-120, 120] em X e Y
    JanelaMundo mundo(-120.0f, 120.0f, -120.0f, 120.0f);
    RegiaoViewport regiao(vpXMin, vpXMax, vpYMin, vpYMax);
    viewportPrincipal = Viewport(mundo, regiao);
}

Aplicacao::~Aplicacao() {
    instancia = nullptr;
}

void Aplicacao::inicializar(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(larguraJanela, alturaJanela);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(tituloJanela.c_str());

    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);

    // Callbacks do GLUT
    glutDisplayFunc(callbackDisplay);
    glutReshapeFunc(callbackReshape);
    glutKeyboardFunc(callbackKeyboard);
    glutSpecialFunc(callbackSpecial);
    glutMouseFunc(callbackMouse);
    glutMotionFunc(callbackMotion);
    glutIdleFunc(callbackIdle);

    exibirAjudaTerminal();
    std::cout << "\n[CG-Terminal]> " << std::flush;
}

void Aplicacao::executar() {
    glutMainLoop();
}

void Aplicacao::callbackDisplay() {
    if (instancia) {
        instancia->desenharCena();
    }
}

void Aplicacao::callbackReshape(int w, int h) {
    if (instancia) {
        instancia->larguraJanela = w;
        instancia->alturaJanela = h;

        // Projeção 2D de tela: X de 0 a w, Y de 0 a h (0 no topo, h na base)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, static_cast<double>(w), static_cast<double>(h), 0.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Atualiza a área da viewport gráfica mantendo o painel lateral sem sobreposição
        float painelLargura = 330.0f;
        if (static_cast<float>(w) < 700.0f) {
            painelLargura = static_cast<float>(w) * 0.45f;
        }
        float vpXMax = static_cast<float>(w) - painelLargura - 25.0f;
        if (vpXMax < 60.0f) vpXMax = 60.0f;

        RegiaoViewport regiao(25.0f, vpXMax, 40.0f, static_cast<float>(h) - 30.0f);
        instancia->viewportPrincipal.setRegiao(regiao);

        glViewport(0, 0, w, h);
        glutPostRedisplay();
    }
}

void Aplicacao::callbackKeyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    if (!instancia) return;

    Objeto2D* obj = instancia->cena.getObjetoAtivo();

    switch (key) {
        // Seleção de Objetos
        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': {
            int idx = key - '1';
            if (idx < static_cast<int>(instancia->cena.getObjetos().size())) {
                instancia->cena.setIndiceObjetoAtivo(idx);
                std::cout << "\nObjeto ativo: " << instancia->cena.getObjetoAtivo()->getNome() << "\n[CG-Terminal]> " << std::flush;
            }
            break;
        }

        // Rotação em torno do centro geométrico
        case 'r':
            if (obj) obj->rotacionarCentro(glm::radians(5.0f));
            break;
        case 'R':
            if (obj) obj->rotacionarCentro(glm::radians(-5.0f));
            break;

        // Rotação em torno da origem do mundo
        case 'o':
            if (obj) obj->rotacionarOrigem(glm::radians(5.0f));
            break;
        case 'O':
            if (obj) obj->rotacionarOrigem(glm::radians(-5.0f));
            break;

        // Escala uniforme
        case '+':
        case '=':
            if (obj) obj->escalarCentro(1.1f, 1.1f);
            break;
        case '-':
        case '_':
            if (obj) obj->escalarCentro(0.9f, 0.9f);
            break;

        // Escala não uniforme
        case ']':
            if (obj) obj->escalarCentro(1.1f, 1.0f);
            break;
        case '[':
            if (obj) obj->escalarCentro(0.9f, 1.0f);
            break;

        // Cisalhamento (Shear)
        case 'h':
            if (obj) obj->cisalhar(0.1f, 0.0f);
            break;
        case 'H':
            if (obj) obj->cisalhar(0.0f, 0.1f);
            break;

        // Reflexão em X e Y
        case 'x':
        case 'X':
            if (obj) obj->espelharX();
            break;
        case 'y':
        case 'Y':
            if (obj) obj->espelharY();
            break;

        // Reset da matriz acumulada
        case '0':
            if (obj) {
                obj->resetar();
                std::cout << "\nMatriz do objeto " << obj->getNome() << " reiniciada para a Identidade.\n[CG-Terminal]> " << std::flush;
            }
            break;

        // Modo Demonstração da ordem de transformações (T*S vs S*T)
        case 'd':
        case 'D':
            instancia->toggleModoDemonstracao();
            std::cout << "\nModo demonstracao: " << (instancia->isModoDemonstracao() ? "ATIVADO" : "DESATIVADO") << "\n[CG-Terminal]> " << std::flush;
            break;

        // Captura de tela
        case 'p':
        case 'P':
            instancia->salvarCaptura("captura_tela.png");
            break;

        // Sair
        case 27: // ESC
        case 'q':
        case 'Q':
            exit(0);
            break;

        default:
            break;
    }

    glutPostRedisplay();
}

void Aplicacao::callbackSpecial(int key, int x, int y) {
    (void)x;
    (void)y;
    if (!instancia) return;
    Objeto2D* obj = instancia->cena.getObjetoAtivo();
    if (!obj) return;

    float passo = 5.0f;
    switch (key) {
        case GLUT_KEY_LEFT:
            obj->transladar(-passo, 0.0f);
            break;
        case GLUT_KEY_RIGHT:
            obj->transladar(passo, 0.0f);
            break;
        case GLUT_KEY_UP:
            obj->transladar(0.0f, passo);
            break;
        case GLUT_KEY_DOWN:
            obj->transladar(0.0f, -passo);
            break;
        default:
            break;
    }

    glutPostRedisplay();
}

void Aplicacao::callbackMouse(int button, int state, int x, int y) {
    if (!instancia) return;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            instancia->mousePressionado = true;
            instancia->mouseUltimoX = x;
            instancia->mouseUltimoY = y;

            // Se clicou na viewport, seleciona o objeto mais próximo
            if (instancia->viewportPrincipal.contemPonto(x, y)) {
                glm::vec2 pMundo = instancia->viewportPrincipal.viewportParaMundo(glm::vec2(x, y));
                float menorDist = 1e9f;
                int melhorIdx = -1;
                for (size_t i = 0; i < instancia->cena.getObjetos().size(); ++i) {
                    glm::vec2 c = instancia->cena.getObjetos()[i].calcularCentroAtual();
                    float dist = glm::distance(pMundo, c);
                    if (dist < menorDist) {
                        menorDist = dist;
                        melhorIdx = static_cast<int>(i);
                    }
                }
                if (melhorIdx != -1 && menorDist < 50.0f) {
                    instancia->cena.setIndiceObjetoAtivo(melhorIdx);
                    std::cout << "\nObjeto selecionado por clique: " << instancia->cena.getObjetoAtivo()->getNome() << "\n[CG-Terminal]> " << std::flush;
                    glutPostRedisplay();
                }
            } else {
                // Se clicou no painel lateral, verifica se clicou na lista de objetos
                float xMinPainel = instancia->viewportPrincipal.getRegiao().xMax + 15.0f;
                float xMaxPainel = static_cast<float>(instancia->larguraJanela) - 15.0f;
                if (x >= xMinPainel && x <= xMaxPainel) {
                    float yListaInicio = 128.0f;
                    for (size_t i = 0; i < instancia->cena.getObjetos().size(); ++i) {
                        float itemY = yListaInicio + static_cast<float>(i) * 18.0f;
                        if (y >= itemY - 14.0f && y <= itemY + 4.0f) {
                            instancia->cena.setIndiceObjetoAtivo(static_cast<int>(i));
                            std::cout << "\nObjeto selecionado por clique no painel: "
                                      << instancia->cena.getObjetoAtivo()->getNome() << "\n[CG-Terminal]> " << std::flush;
                            glutPostRedisplay();
                            break;
                        }
                    }
                }
            }
        } else if (state == GLUT_UP) {
            instancia->mousePressionado = false;
        }
    }
}

void Aplicacao::callbackMotion(int x, int y) {
    if (!instancia || !instancia->mousePressionado) return;
    Objeto2D* obj = instancia->cena.getObjetoAtivo();
    if (!obj) return;

    // Converte delta de viewport para delta no mundo
    glm::vec2 pAntMundo = instancia->viewportPrincipal.viewportParaMundo(glm::vec2(instancia->mouseUltimoX, instancia->mouseUltimoY));
    glm::vec2 pAtualMundo = instancia->viewportPrincipal.viewportParaMundo(glm::vec2(x, y));

    float dx = pAtualMundo.x - pAntMundo.x;
    float dy = pAtualMundo.y - pAntMundo.y;

    obj->transladar(dx, dy);

    instancia->mouseUltimoX = x;
    instancia->mouseUltimoY = y;

    glutPostRedisplay();
}

void Aplicacao::callbackIdle() {
    if (instancia) {
        instancia->verificarEntradaTerminal();
    }
}

void Aplicacao::desenharCena() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (modoDemonstracaoOrdem) {
        desenharDemonstracaoOrdem();
    } else {
        desenharViewportNormal();
    }

    desenharPainelLateral();

    glutSwapBuffers();
}

void Aplicacao::desenharViewportNormal() {
    const RegiaoViewport& regiao = viewportPrincipal.getRegiao();

    // 1. Fundo da Viewport Gráfica (Preto suave)
    desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.02f, 0.02f, 0.04f), true);

    // 2. Borda externa da Viewport
    desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.4f, 0.6f, 0.8f), false);

    // 3. Título da Viewport
    glColor3f(0.8f, 0.8f, 0.9f);
    desenharTexto(regiao.xMin + 10.0f, regiao.yMin - 10.0f, "VIEWPORT GRAFICA (SISTEMA DE COORDENADAS DO MUNDO)", GLUT_BITMAP_HELVETICA_12);

    // Habilita teste de tesoura (scissor test) para garantir que nada transborde da viewport
    glEnable(GL_SCISSOR_TEST);
    glScissor(static_cast<GLint>(regiao.xMin),
              static_cast<GLint>(alturaJanela - regiao.yMax),
              static_cast<GLsizei>(regiao.getLargura()),
              static_cast<GLsizei>(regiao.getAltura()));

    // 4. Eixos Cartesianos do Mundo com marcações
    desenharEixosCartesianos(viewportPrincipal, 25.0f);

    // 5. Desenhar Objetos da Cena
    const auto& objs = cena.getObjetos();
    for (size_t i = 0; i < objs.size(); ++i) {
        // Se for o objeto ativo, podemos desenhar seu centro ou indicar
        desenharObjeto(objs[i], viewportPrincipal, false);

        // Se estiver ativo, desenha uma cruz discreta no centro geométrico atual
        if (static_cast<int>(i) == cena.getIndiceObjetoAtivo()) {
            glm::vec2 centroMundo = objs[i].calcularCentroAtual();
            glm::vec2 centroVp = viewportPrincipal.mundoParaViewport(centroMundo);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
            glVertex2f(centroVp.x - 5.0f, centroVp.y);
            glVertex2f(centroVp.x + 5.0f, centroVp.y);
            glVertex2f(centroVp.x, centroVp.y - 5.0f);
            glVertex2f(centroVp.x, centroVp.y + 5.0f);
            glEnd();
        }
    }

    glDisable(GL_SCISSOR_TEST);
}

void Aplicacao::desenharDemonstracaoOrdem() {
    float x1 = viewportPrincipal.getRegiao().xMin;
    float x2 = viewportPrincipal.getRegiao().xMax;
    float y1 = 40.0f;
    float y2 = static_cast<float>(alturaJanela) - 30.0f;
    float larguraTotal = x2 - x1;
    float meioX = x1 + larguraTotal * 0.5f;

    // Fundo geral do modo demo
    desenharRetangulo(x1, y1, x2, y2, glm::vec3(0.02f, 0.02f, 0.03f), true);
    desenharRetangulo(x1, y1, x2, y2, glm::vec3(0.3f, 0.7f, 0.9f), false);

    // Cabeçalho Central
    glColor3f(1.0f, 1.0f, 0.4f);
    desenharTexto(x1 + (larguraTotal * 0.5f) - 130.0f, y1 + 25.0f, "A ordem das transformacoes importa", GLUT_BITMAP_HELVETICA_18);

    // Divisória vertical central
    glColor3f(0.3f, 0.3f, 0.4f);
    glBegin(GL_LINES);
    glVertex2f(meioX, y1 + 45.0f);
    glVertex2f(meioX, y2 - 20.0f);
    glEnd();

    // Viewport da Esquerda (T * S)
    JanelaMundo jmLocal(-4.0f, 4.0f, -4.0f, 4.0f);
    RegiaoViewport vpEsq(x1 + 15.0f, meioX - 15.0f, y1 + 75.0f, y2 - 20.0f);
    Viewport viewportEsquerda(jmLocal, vpEsq);

    // Viewport da Direita (S * T)
    RegiaoViewport vpDir(meioX + 15.0f, x2 - 15.0f, y1 + 75.0f, y2 - 20.0f);
    Viewport viewportDireita(jmLocal, vpDir);

    // Textos explicativos superiores
    glColor3f(0.9f, 0.9f, 0.9f);
    desenharTexto(vpEsq.xMin + (vpEsq.getLargura() * 0.5f) - 75.0f, y1 + 45.0f, "1. Escala -> 2. Translacao", GLUT_BITMAP_HELVETICA_12);
    glColor3f(0.4f, 0.8f, 1.0f);
    desenharTexto(vpEsq.xMin + (vpEsq.getLargura() * 0.5f) - 15.0f, y1 + 62.0f, "T . S", GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.9f, 0.9f, 0.9f);
    desenharTexto(vpDir.xMin + (vpDir.getLargura() * 0.5f) - 75.0f, y1 + 45.0f, "1. Translacao -> 2. Escala", GLUT_BITMAP_HELVETICA_12);
    glColor3f(0.4f, 0.8f, 1.0f);
    desenharTexto(vpDir.xMin + (vpDir.getLargura() * 0.5f) - 15.0f, y1 + 62.0f, "S . T", GLUT_BITMAP_HELVETICA_12);

    // Objeto base para a demonstração (Barco simplificado centrado na origem)
    Objeto2D barcoDemo("BarcoDemo");
    {
        // Casco
        Poligono casco(glm::vec3(0.15f, 0.55f, 0.75f));
        casco.adicionarVertice(-1.0f, -0.6f);
        casco.adicionarVertice(1.0f, -0.6f);
        casco.adicionarVertice(1.4f, -0.1f);
        casco.adicionarVertice(-1.4f, -0.1f);
        barcoDemo.adicionarPoligono(casco);

        // Mastro
        Poligono mastro(glm::vec3(0.6f, 0.4f, 0.2f));
        mastro.adicionarVertice(-0.05f, -0.1f);
        mastro.adicionarVertice(0.05f, -0.1f);
        mastro.adicionarVertice(0.05f, 0.9f);
        mastro.adicionarVertice(-0.05f, 0.9f);
        barcoDemo.adicionarPoligono(mastro);

        // Vela
        Poligono vela(glm::vec3(1.0f, 0.55f, 0.1f));
        vela.adicionarVertice(0.05f, 0.0f);
        vela.adicionarVertice(0.9f, 0.0f);
        vela.adicionarVertice(0.05f, 0.8f);
        barcoDemo.adicionarPoligono(vela);
    }

    // Matrizes para a demonstração:
    // Escala S (sx=0.5, sy=0.5) e Translação T (dx=1.8, dy=1.8)
    glm::mat3 S = Transformacoes::escala(0.5f, 0.5f);
    glm::mat3 T = Transformacoes::translacao(1.8f, 1.8f);

    glm::mat3 M_TS = T * S; // 1. Escala, depois Translação: T * S
    glm::mat3 M_ST = S * T; // 1. Translação, depois Escala: S * T

    // ================= Desenho no Lado Esquerdo =================
    glEnable(GL_SCISSOR_TEST);
    glScissor(static_cast<GLint>(vpEsq.xMin),
              static_cast<GLint>(alturaJanela - vpEsq.yMax),
              static_cast<GLsizei>(vpEsq.getLargura()),
              static_cast<GLsizei>(vpEsq.getAltura()));

    desenharEixosCartesianos(viewportEsquerda, 1.0f);

    // Desenha original em aramado na origem
    barcoDemo.resetar();
    desenharObjeto(barcoDemo, viewportEsquerda, true);

    // Desenha objeto transformado por T * S
    barcoDemo.setMatrizAcumulada(M_TS);
    desenharObjeto(barcoDemo, viewportEsquerda, false);

    // ================= Desenho no Lado Direito =================
    glScissor(static_cast<GLint>(vpDir.xMin),
              static_cast<GLint>(alturaJanela - vpDir.yMax),
              static_cast<GLsizei>(vpDir.getLargura()),
              static_cast<GLsizei>(vpDir.getAltura()));

    desenharEixosCartesianos(viewportDireita, 1.0f);

    // Desenha original em aramado na origem
    barcoDemo.resetar();
    desenharObjeto(barcoDemo, viewportDireita, true);

    // Desenha objeto transformado por S * T
    barcoDemo.setMatrizAcumulada(M_ST);
    desenharObjeto(barcoDemo, viewportDireita, false);

    glDisable(GL_SCISSOR_TEST);
}

void Aplicacao::desenharEixosCartesianos(const Viewport& vp, float passo) {
    const JanelaMundo& jm = vp.getMundo();

    // 1. Linhas dos eixos principais (X=0 e Y=0)
    glm::vec2 pXInicio = vp.mundoParaViewport(glm::vec2(jm.xMin, 0.0f));
    glm::vec2 pXFim    = vp.mundoParaViewport(glm::vec2(jm.xMax, 0.0f));
    glm::vec2 pYInicio = vp.mundoParaViewport(glm::vec2(0.0f, jm.yMin));
    glm::vec2 pYFim    = vp.mundoParaViewport(glm::vec2(0.0f, jm.yMax));

    glColor3f(0.45f, 0.45f, 0.50f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
    // Eixo X
    glVertex2f(pXInicio.x, pXInicio.y);
    glVertex2f(pXFim.x, pXFim.y);
    // Eixo Y
    glVertex2f(pYInicio.x, pYInicio.y);
    glVertex2f(pYFim.x, pYFim.y);
    glEnd();

    // Setas nos eixos
    // Seta X
    glBegin(GL_LINES);
    glVertex2f(pXFim.x, pXFim.y);
    glVertex2f(pXFim.x - 6.0f, pXFim.y - 4.0f);
    glVertex2f(pXFim.x, pXFim.y);
    glVertex2f(pXFim.x - 6.0f, pXFim.y + 4.0f);

    // Seta Y (aponta para cima no mundo, que é para cima na tela/viewport)
    glVertex2f(pYFim.x, pYFim.y);
    glVertex2f(pYFim.x - 4.0f, pYFim.y + 6.0f);
    glVertex2f(pYFim.x, pYFim.y);
    glVertex2f(pYFim.x + 4.0f, pYFim.y + 6.0f);
    glEnd();

    desenharTexto(pXFim.x - 15.0f, pXFim.y - 6.0f, "x", GLUT_BITMAP_HELVETICA_12);
    desenharTexto(pYFim.x + 8.0f, pYFim.y + 12.0f, "y", GLUT_BITMAP_HELVETICA_12);

    // 2. Graduações (Ticks) e números no Eixo X
    for (float x = std::ceil(jm.xMin / passo) * passo; x <= jm.xMax; x += passo) {
        if (std::abs(x) < 0.001f) continue; // Pula o 0
        glm::vec2 pt = vp.mundoParaViewport(glm::vec2(x, 0.0f));

        glColor3f(0.5f, 0.5f, 0.55f);
        glBegin(GL_LINES);
        glVertex2f(pt.x, pt.y - 4.0f);
        glVertex2f(pt.x, pt.y + 4.0f);
        glEnd();

        std::ostringstream ss;
        if (passo < 1.0f) ss << std::fixed << std::setprecision(1) << x;
        else ss << static_cast<int>(x);

        std::string strNum = ss.str();
        float offset = strNum.length() * 3.5f;
        desenharTexto(pt.x - offset, pt.y + 14.0f, strNum, GLUT_BITMAP_HELVETICA_10);
    }

    // 3. Graduações (Ticks) e números no Eixo Y
    for (float y = std::ceil(jm.yMin / passo) * passo; y <= jm.yMax; y += passo) {
        if (std::abs(y) < 0.001f) continue; // Pula o 0
        glm::vec2 pt = vp.mundoParaViewport(glm::vec2(0.0f, y));

        glColor3f(0.5f, 0.5f, 0.55f);
        glBegin(GL_LINES);
        glVertex2f(pt.x - 4.0f, pt.y);
        glVertex2f(pt.x + 4.0f, pt.y);
        glEnd();

        std::ostringstream ss;
        if (passo < 1.0f) ss << std::fixed << std::setprecision(1) << y;
        else ss << static_cast<int>(y);

        std::string strNum = ss.str();
        desenharTexto(pt.x + 7.0f, pt.y + 4.0f, strNum, GLUT_BITMAP_HELVETICA_10);
    }
}

void Aplicacao::desenharObjeto(const Objeto2D& obj, const Viewport& vp, bool aramado) {
    const glm::mat3& M = obj.getMatrizAcumulada();

    for (const auto& poly : obj.getPoligonos()) {
        const glm::vec3& cor = poly.getCor();

        if (aramado) {
            glColor3f(cor.r * 0.8f, cor.g * 0.8f, cor.b * 0.8f);
            glLineWidth(1.5f);
            glBegin(GL_LINE_LOOP);
        } else {
            glColor3f(cor.r, cor.g, cor.b);
            glBegin(GL_POLYGON);
        }

        for (const auto& v : poly.getVertices()) {
            // Passo 1: Aplicar matriz acumulada de transformações homogêneas 3x3 no mundo
            glm::vec2 pMundoTransf = Transformacoes::aplicar(M, v.posicao);

            // Passo 2: Mapeamento explícito mundo -> viewport com inversão do eixo Y
            glm::vec2 pVp = vp.mundoParaViewport(pMundoTransf);

            // Passo 3: Envio direto das coordenadas finais da janela ao OpenGL
            glVertex2f(pVp.x, pVp.y);
        }
        glEnd();

        // Se for preenchido, desenha borda fina para melhor contraste visual
        if (!aramado) {
            glColor3f(cor.r * 0.35f, cor.g * 0.35f, cor.b * 0.35f);
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
            for (const auto& v : poly.getVertices()) {
                glm::vec2 pMundoTransf = Transformacoes::aplicar(M, v.posicao);
                glm::vec2 pVp = vp.mundoParaViewport(pMundoTransf);
                glVertex2f(pVp.x, pVp.y);
            }
            glEnd();
        }
    }
}

void Aplicacao::desenharPainelLateral() {
    float xMin = viewportPrincipal.getRegiao().xMax + 15.0f;
    float xMax = static_cast<float>(larguraJanela) - 15.0f;
    float yMin = 40.0f;
    float yMax = static_cast<float>(alturaJanela) - 30.0f;

    // Fundo do Painel
    desenharRetangulo(xMin, yMin, xMax, yMax, glm::vec3(0.12f, 0.13f, 0.16f), true);
    desenharRetangulo(xMin, yMin, xMax, yMax, glm::vec3(0.35f, 0.45f, 0.60f), false);

    float yAtual = yMin + 25.0f;

    // Título
    glColor3f(0.3f, 0.8f, 1.0f);
    desenharTexto(xMin + 15.0f, yAtual, "PAINEL DE CONTROLE / STATUS", GLUT_BITMAP_HELVETICA_12);
    yAtual += 25.0f;

    // Linha divisória
    glColor3f(0.25f, 0.30f, 0.38f);
    glBegin(GL_LINES);
    glVertex2f(xMin + 10.0f, yAtual);
    glVertex2f(xMax - 10.0f, yAtual);
    glEnd();
    yAtual += 20.0f;

    // Objeto Selecionado
    const Objeto2D* objAtivo = cena.getObjetoAtivo();
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(xMin + 15.0f, yAtual, "OBJETO ATIVO:", GLUT_BITMAP_HELVETICA_12);
    yAtual += 18.0f;

    for (size_t i = 0; i < cena.getObjetos().size(); ++i) {
        bool selecionado = (static_cast<int>(i) == cena.getIndiceObjetoAtivo());
        if (selecionado) {
            glColor3f(0.2f, 0.9f, 0.4f);
            desenharTexto(xMin + 25.0f, yAtual, "-> [" + std::to_string(i + 1) + "] " + cena.getObjetos()[i].getNome() + " (ATIVO)", GLUT_BITMAP_HELVETICA_12);
        } else {
            glColor3f(0.7f, 0.7f, 0.7f);
            desenharTexto(xMin + 25.0f, yAtual, "   [" + std::to_string(i + 1) + "] " + cena.getObjetos()[i].getNome(), GLUT_BITMAP_HELVETICA_12);
        }
        yAtual += 18.0f;
    }

    yAtual += 10.0f;

    // Matriz Acumulada M do objeto ativo
    if (objAtivo) {
        const glm::mat3& M = objAtivo->getMatrizAcumulada();
        glm::vec2 cAtual = objAtivo->calcularCentroAtual();

        glColor3f(1.0f, 0.85f, 0.3f);
        desenharTexto(xMin + 15.0f, yAtual, "CENTRO (Mundo): (" + std::to_string(static_cast<int>(cAtual.x)) + ", " + std::to_string(static_cast<int>(cAtual.y)) + ")", GLUT_BITMAP_HELVETICA_10);
        yAtual += 20.0f;

        glColor3f(0.8f, 0.85f, 0.95f);
        desenharTexto(xMin + 15.0f, yAtual, "MATRIZ ACUMULADA M (3x3):", GLUT_BITMAP_HELVETICA_12);
        yAtual += 16.0f;

        // Formatação 3x3
        for (int r = 0; r < 3; ++r) {
            std::ostringstream ss;
            ss << "[ ";
            for (int c = 0; c < 3; ++c) {
                ss << std::fixed << std::setprecision(2) << std::setw(6) << M[c][r] << " ";
            }
            ss << "]";
            glColor3f(0.6f, 0.9f, 0.7f);
            desenharTexto(xMin + 25.0f, yAtual, ss.str(), GLUT_BITMAP_9_BY_15);
            yAtual += 16.0f;
        }
    }

    yAtual += 15.0f;
    glColor3f(0.25f, 0.30f, 0.38f);
    glBegin(GL_LINES);
    glVertex2f(xMin + 10.0f, yAtual);
    glVertex2f(xMax - 10.0f, yAtual);
    glEnd();
    yAtual += 20.0f;

    // Atalhos do Teclado
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(xMin + 15.0f, yAtual, "ATALHOS DE TECLADO:", GLUT_BITMAP_HELVETICA_12);
    yAtual += 18.0f;

    glColor3f(0.85f, 0.85f, 0.85f);
    desenharTexto(xMin + 20.0f, yAtual, "Setas : Transladar (X, Y)", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "R / r : Rotacionar (Centro)", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "O / o : Rotacionar (Origem)", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "+ / - : Escala Uniforme", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "] / [ : Escala Nao-Uniforme", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "H / h : Cisalhamento (X / Y)", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "X / Y : Espelhar Eixo X / Y", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "0 (zero) : Resetar Matriz M", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "D : Alternar Modo Ordem (T*S)", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "Clique + Arraste : Mover Objeto", GLUT_BITMAP_HELVETICA_10); yAtual += 20.0f;

    // Nota sobre terminal
    glColor3f(0.4f, 0.8f, 1.0f);
    desenharTexto(xMin + 15.0f, yAtual, "COMANDOS DE TERMINAL:", GLUT_BITMAP_HELVETICA_12);
    yAtual += 16.0f;
    glColor3f(0.7f, 0.7f, 0.7f);
    desenharTexto(xMin + 20.0f, yAtual, "Digite comandos no console:", GLUT_BITMAP_HELVETICA_10);
    yAtual += 14.0f;
    desenharTexto(xMin + 20.0f, yAtual, "ex: transladar 10 20 | escala 1.2", GLUT_BITMAP_HELVETICA_10);
    yAtual += 14.0f;
    desenharTexto(xMin + 20.0f, yAtual, "Digite 'ajuda' no terminal.", GLUT_BITMAP_HELVETICA_10);
}

void Aplicacao::desenharTexto(float x, float y, const std::string& texto, void* fonte) {
    if (!fonte) fonte = GLUT_BITMAP_HELVETICA_12;
    glRasterPos2f(x, y);
    for (char c : texto) {
        glutBitmapCharacter(fonte, c);
    }
}

void Aplicacao::desenharRetangulo(float x1, float y1, float x2, float y2, const glm::vec3& cor, bool preenchido) {
    glColor3f(cor.r, cor.g, cor.b);
    if (preenchido) {
        glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
        glEnd();
    } else {
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
        glEnd();
    }
}

void Aplicacao::salvarCaptura(const std::string& caminhoArquivo) {
    glClear(GL_COLOR_BUFFER_BIT);
    if (modoDemonstracaoOrdem) {
        desenharDemonstracaoOrdem();
    } else {
        desenharViewportNormal();
    }
    desenharPainelLateral();
    glFlush();

    glReadBuffer(GL_BACK);
    std::vector<unsigned char> pixels(larguraJanela * alturaJanela * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, larguraJanela, alturaJanela, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    glutSwapBuffers();

    std::string ppmPath = caminhoArquivo;
    if (ppmPath.size() >= 4 && ppmPath.substr(ppmPath.size() - 4) == ".png") {
        ppmPath = ppmPath.substr(0, ppmPath.size() - 4) + ".ppm";
    } else if (ppmPath.find('.') == std::string::npos) {
        ppmPath += ".ppm";
    }

    FILE* fp = fopen(ppmPath.c_str(), "wb");
    if (!fp) {
        std::cerr << "Erro ao criar arquivo de captura: " << ppmPath << std::endl;
        return;
    }

    fprintf(fp, "P6\n%d %d\n255\n", larguraJanela, alturaJanela);
    // Inverte verticalmente pois glReadPixels lê de baixo para cima
    for (int y = alturaJanela - 1; y >= 0; --y) {
        fwrite(&pixels[y * larguraJanela * 3], 1, larguraJanela * 3, fp);
    }
    fclose(fp);

    if (caminhoArquivo != ppmPath) {
        std::string cmd = "magick \"" + ppmPath + "\" \"" + caminhoArquivo + "\" && rm -f \"" + ppmPath + "\"";
        int res = system(cmd.c_str());
        (void)res;
        std::cout << ">> Captura salva em: " << caminhoArquivo << std::endl;
    } else {
        std::cout << ">> Captura salva em: " << ppmPath << std::endl;
    }
}

void Aplicacao::verificarEntradaTerminal() {
    // Usamos poll não-bloqueante no descritor padrão de entrada (stdin)
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    pfd.revents = 0;

    int ret = poll(&pfd, 1, 0); // Timeout 0 ms (não bloqueante)
    if (ret > 0 && (pfd.revents & POLLIN)) {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == '\n' || c == '\r') {
                if (!bufferTerminal.empty()) {
                    processarComandoTerminal(bufferTerminal);
                    bufferTerminal.clear();
                    std::cout << "\n[CG-Terminal]> " << std::flush;
                } else {
                    std::cout << "\n[CG-Terminal]> " << std::flush;
                }
            } else {
                bufferTerminal += c;
            }
        }
    }
}

void Aplicacao::processarComandoTerminal(const std::string& comandoLinha) {
    std::istringstream iss(comandoLinha);
    std::string cmd;
    if (!(iss >> cmd)) return;

    Objeto2D* obj = cena.getObjetoAtivo();

    if (cmd == "ajuda" || cmd == "help" || cmd == "?") {
        exibirAjudaTerminal();
    } else if (cmd == "status") {
        exibirStatusTerminal();
    } else if (cmd == "selecionar" || cmd == "sel") {
        std::string param;
        if (iss >> param) {
            bool achou = false;
            try {
                int num = std::stoi(param);
                if (num >= 1 && num <= static_cast<int>(cena.getObjetos().size())) {
                    cena.setIndiceObjetoAtivo(num - 1);
                    std::cout << ">> Objeto ativo selecionado: [" << num << "] " << cena.getObjetoAtivo()->getNome() << "\n";
                    achou = true;
                }
            } catch (...) {}

            if (!achou) {
                for (size_t i = 0; i < cena.getObjetos().size(); ++i) {
                    std::string nObj = cena.getObjetos()[i].getNome();
                    std::string pLow = param, nLow = nObj;
                    for (char& c : pLow) c = std::tolower(c);
                    for (char& c : nLow) c = std::tolower(c);
                    if (pLow == nLow) {
                        cena.setIndiceObjetoAtivo(static_cast<int>(i));
                        std::cout << ">> Objeto ativo selecionado: [" << (i + 1) << "] " << nObj << "\n";
                        achou = true;
                        break;
                    }
                }
            }
            if (!achou) {
                std::cout << ">> Objeto desconhecido: " << param << ". Use o indice de 1 a " << cena.getObjetos().size() << " ou o nome do objeto.\n";
            }
        }
    } else if (cmd == "transladar" || cmd == "mover") {
        float dx = 0.0f, dy = 0.0f;
        if (iss >> dx >> dy) {
            if (obj) {
                obj->transladar(dx, dy);
                std::cout << ">> Transladado (" << dx << ", " << dy << ") em " << obj->getNome() << "\n";
            }
        } else {
            std::cout << ">> Uso: transladar <dx> <dy>\n";
        }
    } else if (cmd == "rotacionar" || cmd == "rot") {
        float graus = 0.0f;
        if (iss >> graus) {
            if (obj) {
                obj->rotacionarCentro(glm::radians(graus));
                std::cout << ">> Rotacionado " << graus << " graus em torno do centro em " << obj->getNome() << "\n";
            }
        } else {
            std::cout << ">> Uso: rotacionar <graus>\n";
        }
    } else if (cmd == "rotacionar_origem" || cmd == "roto") {
        float graus = 0.0f;
        if (iss >> graus) {
            if (obj) {
                obj->rotacionarOrigem(glm::radians(graus));
                std::cout << ">> Rotacionado " << graus << " graus em torno da origem em " << obj->getNome() << "\n";
            }
        } else {
            std::cout << ">> Uso: rotacionar_origem <graus>\n";
        }
    } else if (cmd == "escala") {
        float sx = 1.0f, sy = 1.0f;
        if (iss >> sx) {
            if (!(iss >> sy)) sy = sx; // uniforme se omitido
            if (obj) {
                obj->escalarCentro(sx, sy);
                std::cout << ">> Escala aplicada (" << sx << ", " << sy << ") em " << obj->getNome() << "\n";
            }
        } else {
            std::cout << ">> Uso: escala <sx> [sy]\n";
        }
    } else if (cmd == "cisalhar" || cmd == "shear") {
        float hx = 0.0f, hy = 0.0f;
        if (iss >> hx >> hy) {
            if (obj) {
                obj->cisalhar(hx, hy);
                std::cout << ">> Cisalhamento aplicado (hx=" << hx << ", hy=" << hy << ") em " << obj->getNome() << "\n";
            }
        } else {
            std::cout << ">> Uso: cisalhar <hx> <hy>\n";
        }
    } else if (cmd == "espelhar" || cmd == "reflexao") {
        std::string eixo;
        if (iss >> eixo) {
            if (eixo == "x" || eixo == "X") {
                if (obj) obj->espelharX();
                std::cout << ">> Reflexao no eixo X aplicada.\n";
            } else if (eixo == "y" || eixo == "Y") {
                if (obj) obj->espelharY();
                std::cout << ">> Reflexao no eixo Y aplicada.\n";
            } else {
                std::cout << ">> Uso: espelhar <x|y>\n";
            }
        }
    } else if (cmd == "reset" || cmd == "reiniciar") {
        if (obj) {
            obj->resetar();
            std::cout << ">> Matriz acumulada de " << obj->getNome() << " reiniciada para a Identidade.\n";
        }
    } else if (cmd == "reset_todos") {
        cena.resetarTodosObjetos();
        std::cout << ">> Todos os objetos foram reiniciados.\n";
    } else if (cmd == "demo" || cmd == "ordem") {
        std::string sub;
        if (iss >> sub) {
            if (sub == "on" || sub == "ativar" || sub == "1") modoDemonstracaoOrdem = true;
            else if (sub == "off" || sub == "desativar" || sub == "0") modoDemonstracaoOrdem = false;
            else toggleModoDemonstracao();
        } else {
            toggleModoDemonstracao();
        }
        std::cout << ">> Modo de demonstracao da ordem " << (modoDemonstracaoOrdem ? "ATIVADO" : "DESATIVADO") << "\n";
    } else if (cmd == "captura" || cmd == "screenshot") {
        std::string nome;
        if (!(iss >> nome)) nome = "captura.png";
        salvarCaptura(nome);
    } else if (cmd == "sair" || cmd == "exit" || cmd == "quit") {
        std::cout << ">> Encerrando aplicacao...\n";
        exit(0);
    } else {
        std::cout << ">> Comando desconhecido: " << cmd << ". Digite 'ajuda' para a lista de comandos.\n";
    }

    glutPostRedisplay();
}

void Aplicacao::exibirAjudaTerminal() {
    std::cout << "\n===========================================================\n"
              << "       TP1 - COMPUTAÇÃO GRÁFICA: TRANSFORMACÕES 2D        \n"
              << "===========================================================\n"
              << "COMANDOS DISPONÍVEIS NO TERMINAL:\n"
              << "  selecionar <1|2|3|casa|barco|moinho>  - Escolhe o objeto ativo\n"
              << "  transladar <dx> <dy>                  - Move no mundo cartesiano\n"
              << "  rotacionar <graus>                    - Gira em torno do centro geometrico\n"
              << "  rotacionar_origem <graus>             - Gira em torno da origem (0,0)\n"
              << "  escala <sx> [sy]                      - Escala uniforme ou nao uniforme\n"
              << "  cisalhar <hx> <hy>                    - Aplica cisalhamento em X e Y\n"
              << "  espelhar <x|y>                        - Reflexao em relacao aos eixos\n"
              << "  reset                                 - Restaura a matriz acumulada (Identidade)\n"
              << "  reset_todos                           - Restaura todos os objetos\n"
              << "  demo                                  - Alterna a demonstracao T*S vs S*T\n"
              << "  status                                - Exibe a matriz acumulada M\n"
              << "  ajuda                                 - Exibe este menu\n"
              << "  sair                                  - Fecha a aplicacao\n"
              << "-----------------------------------------------------------\n"
              << "ATALHOS DE TECLADO NA JANELA GRÁFICA:\n"
              << "  [1, 2, 3] : Selecionar objeto ativo\n"
              << "  Setas     : Transladar X e Y\n"
              << "  R / r     : Rotacionar pelo centro\n"
              << "  O / o     : Rotacionar pela origem\n"
              << "  + / -     : Escala uniforme\n"
              << "  ] / [     : Escala nao uniforme\n"
              << "  H / h     : Cisalhamento\n"
              << "  X / Y     : Reflexao nos eixos X e Y\n"
              << "  0         : Resetar matriz acumulada M\n"
              << "  D         : Alternar tela de demonstracao de ordem\n"
              << "  Q / Esc   : Sair\n"
              << "===========================================================\n";
}

void Aplicacao::exibirStatusTerminal() {
    const Objeto2D* obj = cena.getObjetoAtivo();
    if (!obj) return;

    std::cout << "\n--- STATUS: " << obj->getNome() << " ---\n";
    glm::vec2 c = obj->calcularCentroAtual();
    std::cout << "Centro no Mundo: (" << c.x << ", " << c.y << ")\n";
    std::cout << "Matriz Acumulada M:\n";
    const glm::mat3& M = obj->getMatrizAcumulada();
    for (int r = 0; r < 3; ++r) {
        std::cout << "  | ";
        for (int col = 0; col < 3; ++col) {
            std::cout << std::fixed << std::setprecision(3) << std::setw(8) << M[col][r] << " ";
        }
        std::cout << "|\n";
    }
}
