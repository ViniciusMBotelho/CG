#include "Aplicacao.hpp"
#include "Transformacoes.hpp"
#include <GL/freeglut.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

Aplicacao* Aplicacao::instancia = nullptr;

Aplicacao::Aplicacao(int largura, int altura, const std::string& titulo)
    : larguraJanela(largura), alturaJanela(altura), tituloJanela(titulo),
      cena(), modoDemonstracaoOrdem(false),
      mousePressionado(false), mouseUltimoX(0), mouseUltimoY(0) {
    instancia = this;

    // Viewport gráfica ocupa a esquerda e o centro; painel ocupa a direita
    float vpXMin = 30.0f;
    float vpXMax = 720.0f;
    float vpYMin = 40.0f;
    float vpYMax = alturaJanela - 30.0f;

    // Janela do mundo de [-120, 120] em X e Y
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

    // Registra callbacks do GLUT
    glutDisplayFunc(callbackDisplay);
    glutReshapeFunc(callbackReshape);
    glutKeyboardFunc(callbackKeyboard);
    glutSpecialFunc(callbackSpecial);
    glutMouseFunc(callbackMouse);
    glutMotionFunc(callbackMotion);

    std::cout << "===========================================================\n"
              << "       TP1 - COMPUTACAO GRAFICA: TRANSFORMACOES 2D        \n"
              << "===========================================================\n"
              << "ATALHOS DE TECLADO NA JANELA GRAFICA:\n"
              << "  [1, 2, 3] : Selecionar objeto ativo (Casa, Barco, Moinho)\n"
              << "  Setas     : Transladar nos eixos X e Y\n"
              << "  R / r     : Rotacionar pelo centro geometrico (+/- 5 deg)\n"
              << "  O / o     : Rotacionar pela origem (0,0) (+/- 5 deg)\n"
              << "  + / -     : Escala uniforme (+10% / -10%)\n"
              << "  ] / [     : Escala nao-uniforme no eixo X (+10% / -10%)\n"
              << "  H / h     : Cisalhamento (Shear Y / X)\n"
              << "  X / Y     : Reflexao nos eixos X e Y\n"
              << "  0 (zero)  : Resetar matriz acumulada M para Identidade\n"
              << "  D         : Alternar ordem normal vs ordem contraria\n"
              << "  Q / Esc   : Sair da aplicacao\n"
              << "===========================================================\n";
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

        // Projeção 2D ortogonal em pixels da tela: X de 0 a w, Y de 0 a h (0 no topo, h na base)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, static_cast<double>(w), static_cast<double>(h), 0.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Atualiza a região da viewport gráfica mantendo o painel lateral à direita
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
    (void)x; (void)y;
    if (!instancia) return;

    Objeto2D* obj = instancia->cena.getObjetoAtivo();

    switch (key) {
        // Seleção de Objetos
        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': {
            int idx = key - '1';
            if (idx < static_cast<int>(instancia->cena.getObjetos().size())) {
                instancia->cena.setIndiceObjetoAtivo(idx);
                std::cout << ">> Objeto ativo: [" << (idx + 1) << "] "
                          << instancia->cena.getObjetoAtivo()->getNome() << "\n";
            }
            break;
        }

        // Rotação no Centro Geométrico
        case 'r':
            if (obj) obj->rotacionarCentro(glm::radians(5.0f));
            break;
        case 'R':
            if (obj) obj->rotacionarCentro(glm::radians(-5.0f));
            break;

        // Rotação na Origem (0, 0)
        case 'o':
            if (obj) obj->rotacionarOrigem(glm::radians(5.0f));
            break;
        case 'O':
            if (obj) obj->rotacionarOrigem(glm::radians(-5.0f));
            break;

        // Escala Uniforme
        case '+': case '=':
            if (obj) obj->escalarCentro(1.1f, 1.1f);
            break;
        case '-': case '_':
            if (obj) obj->escalarCentro(0.9f, 0.9f);
            break;

        // Escala Não-Uniforme no eixo X
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

        // Reflexão / Espelhamento
        case 'x': case 'X':
            if (obj) obj->espelharX();
            break;
        case 'y': case 'Y':
            if (obj) obj->espelharY();
            break;

        // Reset da Matriz Acumulada M para a Identidade
        case '0':
            if (obj) {
                obj->resetar();
                std::cout << ">> Reset: Matriz M de " << obj->getNome() << " voltou a Identidade.\n";
            }
            break;

        // Alternar modo de ordem contrária (Etapa 4)
        case 'd': case 'D':
            instancia->toggleModoDemonstracao();
            std::cout << ">> Modo Demonstracao da Ordem: "
                      << (instancia->isModoDemonstracao() ? "ORDEM CONTRARIA (M * Nova)" : "NORMAL (Nova * M)")
                      << "\n";
            break;

        case 27: case 'q': case 'Q':
            std::cout << "Encerrando aplicacao.\n";
            exit(0);

        default:
            break;
    }

    glutPostRedisplay();
}

void Aplicacao::callbackSpecial(int key, int x, int y) {
    (void)x; (void)y;
    if (!instancia) return;
    Objeto2D* obj = instancia->cena.getObjetoAtivo();
    if (!obj) return;

    float passo = 5.0f;
    switch (key) {
        case GLUT_KEY_LEFT:  obj->transladar(-passo, 0.0f); break;
        case GLUT_KEY_RIGHT: obj->transladar(passo, 0.0f);  break;
        case GLUT_KEY_UP:    obj->transladar(0.0f, passo);  break;
        case GLUT_KEY_DOWN:  obj->transladar(0.0f, -passo); break;
        default: break;
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

            // Se clicou dentro da viewport, seleciona o objeto mais próximo
            if (instancia->viewportPrincipal.contemPonto(x, y)) {
                glm::vec2 pMundo = instancia->viewportPrincipal.viewportParaMundo(glm::vec2(x, y));
                float menorDist = 1e9f;
                int melhorIdx = -1;
                for (size_t i = 0; i < instancia->cena.getObjetos().size(); ++i) {
                    glm::vec2 c = instancia->cena.getObjetos()[i].calcularCentroAtual(instancia->isModoDemonstracao());
                    float dist = glm::distance(pMundo, c);
                    if (dist < menorDist) {
                        menorDist = dist;
                        melhorIdx = static_cast<int>(i);
                    }
                }
                if (melhorIdx != -1 && menorDist < 50.0f) {
                    instancia->cena.setIndiceObjetoAtivo(melhorIdx);
                    std::cout << ">> Objeto selecionado por clique: "
                              << instancia->cena.getObjetoAtivo()->getNome() << "\n";
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
                            std::cout << ">> Objeto selecionado por clique no painel: "
                                      << instancia->cena.getObjetoAtivo()->getNome() << "\n";
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

    // 1. Fundo da Viewport Gráfica
    desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.02f, 0.02f, 0.04f), true);

    // 2. Borda externa da Viewport
    desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.4f, 0.6f, 0.8f), false);

    // 3. Título da Viewport
    glColor3f(0.8f, 0.8f, 0.9f);
    desenharTexto(regiao.xMin + 10.0f, regiao.yMin - 10.0f, "VIEWPORT GRAFICA [NORMAL: Nova * M] - (Pressione 'D' para Ordem Contraria)", GLUT_BITMAP_HELVETICA_12);

    glEnable(GL_SCISSOR_TEST);
    glScissor(static_cast<GLint>(regiao.xMin),
              static_cast<GLint>(alturaJanela - regiao.yMax),
              static_cast<GLsizei>(regiao.getLargura()),
              static_cast<GLsizei>(regiao.getAltura()));

    // 4. Eixos Cartesianos do Mundo com graduações
    desenharEixosCartesianos(viewportPrincipal, 25.0f);

    // 5. Desenhar Objetos da Cena
    const auto& objs = cena.getObjetos();
    for (size_t i = 0; i < objs.size(); ++i) {
        desenharObjeto(objs[i], viewportPrincipal, false, false);

        // Se estiver ativo, desenha uma cruz discreta no centro geométrico atual
        if (static_cast<int>(i) == cena.getIndiceObjetoAtivo()) {
            glm::vec2 centroMundo = objs[i].calcularCentroAtual(false);
            glm::vec2 centroVp = viewportPrincipal.mundoParaViewport(centroMundo);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
            glVertex2f(centroVp.x - 6.0f, centroVp.y);
            glVertex2f(centroVp.x + 6.0f, centroVp.y);
            glVertex2f(centroVp.x, centroVp.y - 6.0f);
            glVertex2f(centroVp.x, centroVp.y + 6.0f);
            glEnd();
        }
    }

    glDisable(GL_SCISSOR_TEST);
}

void Aplicacao::desenharDemonstracaoOrdem() {
    const RegiaoViewport& regiao = viewportPrincipal.getRegiao();

    // 1. Fundo com matiz avermelhado/alerta
    desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.05f, 0.01f, 0.02f), true);

    // 2. Borda de destaque avermelhada
    desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.95f, 0.35f, 0.35f), false);

    // 3. Título explicativo
    glColor3f(1.0f, 0.4f, 0.4f);
    desenharTexto(regiao.xMin + 10.0f, regiao.yMin - 10.0f, "ORDEM CONTRARIA (M * Nova) - [Pressione 'D' para voltar ao Normal]", GLUT_BITMAP_HELVETICA_12);

    glEnable(GL_SCISSOR_TEST);
    glScissor(static_cast<GLint>(regiao.xMin),
              static_cast<GLint>(alturaJanela - regiao.yMax),
              static_cast<GLsizei>(regiao.getLargura()),
              static_cast<GLsizei>(regiao.getAltura()));

    // 4. Eixos cartesianos
    desenharEixosCartesianos(viewportPrincipal, 25.0f);

    // 5. Desenhar objetos: fantasma da ordem normal em aramado e resultado da ordem inversa preenchido
    const auto& objs = cena.getObjetos();
    for (size_t i = 0; i < objs.size(); ++i) {
        desenharObjeto(objs[i], viewportPrincipal, true, false);  // Fantasma normal aramado
        desenharObjeto(objs[i], viewportPrincipal, false, true); // Ordem contrária preenchido

        if (static_cast<int>(i) == cena.getIndiceObjetoAtivo()) {
            glm::vec2 centroMundo = objs[i].calcularCentroAtual(true);
            glm::vec2 centroVp = viewportPrincipal.mundoParaViewport(centroMundo);
            glColor3f(1.0f, 0.3f, 0.3f);
            glBegin(GL_LINES);
            glVertex2f(centroVp.x - 6.0f, centroVp.y);
            glVertex2f(centroVp.x + 6.0f, centroVp.y);
            glVertex2f(centroVp.x, centroVp.y - 6.0f);
            glVertex2f(centroVp.x, centroVp.y + 6.0f);
            glEnd();
        }
    }

    glDisable(GL_SCISSOR_TEST);
}

void Aplicacao::desenharEixosCartesianos(const Viewport& vp, float passo) {
    const JanelaMundo& jm = vp.getMundo();

    glm::vec2 pXInicio = vp.mundoParaViewport(glm::vec2(jm.xMin, 0.0f));
    glm::vec2 pXFim    = vp.mundoParaViewport(glm::vec2(jm.xMax, 0.0f));
    glm::vec2 pYInicio = vp.mundoParaViewport(glm::vec2(0.0f, jm.yMin));
    glm::vec2 pYFim    = vp.mundoParaViewport(glm::vec2(0.0f, jm.yMax));

    glColor3f(0.45f, 0.45f, 0.50f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
    glVertex2f(pXInicio.x, pXInicio.y);
    glVertex2f(pXFim.x, pXFim.y);
    glVertex2f(pYInicio.x, pYInicio.y);
    glVertex2f(pYFim.x, pYFim.y);
    glEnd();

    // Setas nos eixos
    glBegin(GL_LINES);
    glVertex2f(pXFim.x, pXFim.y);
    glVertex2f(pXFim.x - 6.0f, pXFim.y - 4.0f);
    glVertex2f(pXFim.x, pXFim.y);
    glVertex2f(pXFim.x - 6.0f, pXFim.y + 4.0f);

    glVertex2f(pYFim.x, pYFim.y);
    glVertex2f(pYFim.x - 4.0f, pYFim.y + 6.0f);
    glVertex2f(pYFim.x, pYFim.y);
    glVertex2f(pYFim.x + 4.0f, pYFim.y + 6.0f);
    glEnd();

    desenharTexto(pXFim.x - 15.0f, pXFim.y - 6.0f, "x", GLUT_BITMAP_HELVETICA_12);
    desenharTexto(pYFim.x + 8.0f, pYFim.y + 12.0f, "y", GLUT_BITMAP_HELVETICA_12);

    // Graduações no Eixo X
    for (float x = std::ceil(jm.xMin / passo) * passo; x <= jm.xMax; x += passo) {
        if (std::abs(x) < 0.001f) continue;
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

    // Graduações no Eixo Y
    for (float y = std::ceil(jm.yMin / passo) * passo; y <= jm.yMax; y += passo) {
        if (std::abs(y) < 0.001f) continue;
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

void Aplicacao::desenharObjeto(const Objeto2D& obj, const Viewport& vp, bool aramado, bool ordemInversa) {
    const glm::mat3& M = ordemInversa ? obj.getMatrizOrdemInversa() : obj.getMatrizAcumulada();

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
            glm::vec3 pHomogeneo(v.posicao.x, v.posicao.y, 1.0f);
            glm::vec3 pMundoTransf = M * pHomogeneo;
            glm::vec2 pVp = vp.mundoParaViewport(glm::vec2(pMundoTransf.x, pMundoTransf.y));
            glVertex2f(pVp.x, pVp.y);
        }
        glEnd();

        if (!aramado) {
            glColor3f(cor.r * 0.35f, cor.g * 0.35f, cor.b * 0.35f);
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
            for (const auto& v : poly.getVertices()) {
                glm::vec3 pHomogeneo(v.posicao.x, v.posicao.y, 1.0f);
                glm::vec3 pMundoTransf = M * pHomogeneo;
                glm::vec2 pVp = vp.mundoParaViewport(glm::vec2(pMundoTransf.x, pMundoTransf.y));
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
    desenharTexto(xMin + 15.0f, yAtual, "OBJETO ATIVO (Clique para selecionar):", GLUT_BITMAP_HELVETICA_12);
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

    // Matrizes do objeto ativo (Normal e Ordem Inversa para comparação)
    if (objAtivo) {
        const glm::mat3& M_norm = objAtivo->getMatrizAcumulada();
        const glm::mat3& M_inv  = objAtivo->getMatrizOrdemInversa();
        glm::vec2 cNorm = objAtivo->calcularCentroAtual(false);
        glm::vec2 cInv  = objAtivo->calcularCentroAtual(true);

        glColor3f(1.0f, 0.85f, 0.3f);
        desenharTexto(xMin + 15.0f, yAtual, "CENTRO (Normal): (" + std::to_string(static_cast<int>(cNorm.x)) + ", " + std::to_string(static_cast<int>(cNorm.y)) + ")", GLUT_BITMAP_HELVETICA_10);
        yAtual += 14.0f;
        desenharTexto(xMin + 15.0f, yAtual, "CENTRO (Inverso): (" + std::to_string(static_cast<int>(cInv.x)) + ", " + std::to_string(static_cast<int>(cInv.y)) + ")", GLUT_BITMAP_HELVETICA_10);
        yAtual += 18.0f;

        // 1. Matriz Acumulada Normal (Nova * M)
        if (!modoDemonstracaoOrdem) {
            glColor3f(0.3f, 0.9f, 0.5f);
            desenharTexto(xMin + 15.0f, yAtual, "MATRIZ NORMAL [Nova * M] (ATIVA):", GLUT_BITMAP_HELVETICA_12);
        } else {
            glColor3f(0.7f, 0.75f, 0.8f);
            desenharTexto(xMin + 15.0f, yAtual, "MATRIZ NORMAL [Nova * M]:", GLUT_BITMAP_HELVETICA_12);
        }
        yAtual += 15.0f;

        for (int r = 0; r < 3; ++r) {
            std::ostringstream ss;
            ss << "[ ";
            for (int c = 0; c < 3; ++c) {
                ss << std::fixed << std::setprecision(2) << std::setw(6) << M_norm[c][r] << " ";
            }
            ss << "]";
            glColor3f(0.6f, 0.9f, 0.7f);
            desenharTexto(xMin + 20.0f, yAtual, ss.str(), GLUT_BITMAP_9_BY_15);
            yAtual += 15.0f;
        }

        yAtual += 6.0f;

        // 2. Matriz Ordem Inversa (M * Nova)
        if (modoDemonstracaoOrdem) {
            glColor3f(1.0f, 0.4f, 0.4f);
            desenharTexto(xMin + 15.0f, yAtual, "MATRIZ ORDEM INVERSA [M * Nova] (ATIVA):", GLUT_BITMAP_HELVETICA_12);
        } else {
            glColor3f(0.9f, 0.6f, 0.6f);
            desenharTexto(xMin + 15.0f, yAtual, "MATRIZ ORDEM INVERSA [M * Nova]:", GLUT_BITMAP_HELVETICA_12);
        }
        yAtual += 15.0f;

        for (int r = 0; r < 3; ++r) {
            std::ostringstream ss;
            ss << "[ ";
            for (int c = 0; c < 3; ++c) {
                ss << std::fixed << std::setprecision(2) << std::setw(6) << M_inv[c][r] << " ";
            }
            ss << "]";
            glColor3f(1.0f, 0.65f, 0.65f);
            desenharTexto(xMin + 20.0f, yAtual, ss.str(), GLUT_BITMAP_9_BY_15);
            yAtual += 15.0f;
        }
    }

    yAtual += 10.0f;
    glColor3f(0.25f, 0.30f, 0.38f);
    glBegin(GL_LINES);
    glVertex2f(xMin + 10.0f, yAtual);
    glVertex2f(xMax - 10.0f, yAtual);
    glEnd();
    yAtual += 16.0f;

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
    desenharTexto(xMin + 20.0f, yAtual, "D : Alternar Ordem Inversa", GLUT_BITMAP_HELVETICA_10); yAtual += 15.0f;
    desenharTexto(xMin + 20.0f, yAtual, "Clique + Arraste : Mover Objeto", GLUT_BITMAP_HELVETICA_10); yAtual += 20.0f;
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
