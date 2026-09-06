#include "Cena.hpp"
#include "Viewport.hpp"
#include "Transformacoes.hpp"
#include <GL/freeglut.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

static Cena cena;
static Viewport viewport;
static int larguraJanela = 1000;
static int alturaJanela = 700;
static bool modoDemonstracaoOrdem = false;

void desenharTexto(float x, float y, const std::string& texto, void* fonte = GLUT_BITMAP_HELVETICA_12) {
    glRasterPos2f(x, y);
    for (char c : texto) {
        glutBitmapCharacter(fonte, c);
    }
}

void desenharRetangulo(float x1, float y1, float x2, float y2, const glm::vec3& cor, bool preenchido) {
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

void desenharEixosCartesianos(const Viewport& vp, float passo = 25.0f) {
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

    for (float x = std::ceil(jm.xMin / passo) * passo; x <= jm.xMax; x += passo) {
        if (std::abs(x) < 0.001f) continue;
        glm::vec2 pt = vp.mundoParaViewport(glm::vec2(x, 0.0f));

        glColor3f(0.5f, 0.5f, 0.55f);
        glBegin(GL_LINES);
        glVertex2f(pt.x, pt.y - 4.0f);
        glVertex2f(pt.x, pt.y + 4.0f);
        glEnd();

        std::string strNum = std::to_string(static_cast<int>(x));
        desenharTexto(pt.x - strNum.length() * 3.5f, pt.y + 14.0f, strNum, GLUT_BITMAP_HELVETICA_10);
    }

    for (float y = std::ceil(jm.yMin / passo) * passo; y <= jm.yMax; y += passo) {
        if (std::abs(y) < 0.001f) continue;
        glm::vec2 pt = vp.mundoParaViewport(glm::vec2(0.0f, y));

        glColor3f(0.5f, 0.5f, 0.55f);
        glBegin(GL_LINES);
        glVertex2f(pt.x - 4.0f, pt.y);
        glVertex2f(pt.x + 4.0f, pt.y);
        glEnd();

        std::string strNum = std::to_string(static_cast<int>(y));
        desenharTexto(pt.x + 7.0f, pt.y + 4.0f, strNum, GLUT_BITMAP_HELVETICA_10);
    }
}

void desenharObjeto(const Objeto2D& obj, const Viewport& vp, bool aramado = false, bool ordemInversa = false) {
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

void callbackDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    const RegiaoViewport& regiao = viewport.getRegiao();

    if (modoDemonstracaoOrdem) {
        desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.05f, 0.01f, 0.02f), true);
        desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.95f, 0.35f, 0.35f), false);

        glColor3f(1.0f, 0.4f, 0.4f);
        desenharTexto(regiao.xMin + 10.0f, regiao.yMin - 10.0f, "ORDEM CONTRARIA (M * Nova) - [Pressione 'D' para voltar ao Normal]", GLUT_BITMAP_HELVETICA_12);

        glEnable(GL_SCISSOR_TEST);
        glScissor(static_cast<GLint>(regiao.xMin),
                  static_cast<GLint>(alturaJanela - regiao.yMax),
                  static_cast<GLsizei>(regiao.getLargura()),
                  static_cast<GLsizei>(regiao.getAltura()));

        desenharEixosCartesianos(viewport, 25.0f);

        for (const auto& obj : cena.getObjetos()) {
            desenharObjeto(obj, viewport, true, false);
            desenharObjeto(obj, viewport, false, true);
        }

        glDisable(GL_SCISSOR_TEST);
    } else {
        desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.02f, 0.02f, 0.04f), true);
        desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.4f, 0.6f, 0.8f), false);

        glColor3f(0.8f, 0.8f, 0.9f);
        desenharTexto(regiao.xMin + 10.0f, regiao.yMin - 10.0f, "VIEWPORT GRAFICA [NORMAL: Nova * M] - (Pressione 'D' para ver Ordem Contraria)", GLUT_BITMAP_HELVETICA_12);

        glEnable(GL_SCISSOR_TEST);
        glScissor(static_cast<GLint>(regiao.xMin),
                  static_cast<GLint>(alturaJanela - regiao.yMax),
                  static_cast<GLsizei>(regiao.getLargura()),
                  static_cast<GLsizei>(regiao.getAltura()));

        desenharEixosCartesianos(viewport, 25.0f);

        for (const auto& obj : cena.getObjetos()) {
            desenharObjeto(obj, viewport, false, false);
        }

        glDisable(GL_SCISSOR_TEST);
    }

    glutSwapBuffers();
}

void callbackReshape(int w, int h) {
    larguraJanela = w;
    alturaJanela = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, static_cast<double>(w), static_cast<double>(h), 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    RegiaoViewport novaRegiao(30.0f, static_cast<float>(w) - 30.0f, 40.0f, static_cast<float>(h) - 30.0f);
    viewport.setRegiao(novaRegiao);

    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

void callbackKeyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    Objeto2D* obj = cena.getObjetoAtivo();

    switch (key) {
        case '1': case '2': case '3': {
            int idx = key - '1';
            if (idx < static_cast<int>(cena.getObjetos().size())) {
                cena.setIndiceObjetoAtivo(idx);
                std::cout << ">> Objeto ativo: [" << (idx + 1) << "] " << cena.getObjetoAtivo()->getNome() << "\n";
            }
            break;
        }
        case 'r':
            if (obj) { obj->rotacionarCentro(glm::radians(5.0f)); }
            break;
        case 'R':
            if (obj) { obj->rotacionarCentro(glm::radians(-5.0f)); }
            break;
        case 'o':
            if (obj) { obj->rotacionarOrigem(glm::radians(5.0f)); }
            break;
        case 'O':
            if (obj) { obj->rotacionarOrigem(glm::radians(-5.0f)); }
            break;
        case '+': case '=':
            if (obj) { obj->escalarCentro(1.1f, 1.1f); }
            break;
        case '-': case '_':
            if (obj) { obj->escalarCentro(0.9f, 0.9f); }
            break;
        case ']':
            if (obj) { obj->escalarCentro(1.1f, 1.0f); }
            break;
        case '[':
            if (obj) { obj->escalarCentro(0.9f, 1.0f); }
            break;
        case 'h':
            if (obj) { obj->cisalhar(0.1f, 0.0f); }
            break;
        case 'H':
            if (obj) { obj->cisalhar(0.0f, 0.1f); }
            break;
        case 'x': case 'X':
            if (obj) { obj->espelharX(); }
            break;
        case 'y': case 'Y':
            if (obj) { obj->espelharY(); }
            break;
        case 'd': case 'D':
            modoDemonstracaoOrdem = !modoDemonstracaoOrdem;
            std::cout << ">> Modo Demonstracao da Ordem: " << (modoDemonstracaoOrdem ? "ATIVADO" : "DESATIVADO") << "\n";
            break;
        case '0':
            if (obj) { obj->resetar(); }
            break;
        case 27: case 'q': case 'Q':
            std::cout << "\nEncerrando visualizacao.\n";
            exit(0);
    }
    glutPostRedisplay();
}

void callbackSpecial(int key, int x, int y) {
    (void)x; (void)y;
    Objeto2D* obj = cena.getObjetoAtivo();
    if (!obj) return;

    float passo = 5.0f;
    switch (key) {
        case GLUT_KEY_LEFT:  obj->transladar(-passo, 0.0f); break;
        case GLUT_KEY_RIGHT: obj->transladar(passo, 0.0f);  break;
        case GLUT_KEY_UP:    obj->transladar(0.0f, passo);  break;
        case GLUT_KEY_DOWN:  obj->transladar(0.0f, -passo); break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(larguraJanela, alturaJanela);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("TP1 - Etapa 4: Nao-Comutatividade das Transformacoes");

    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);

    JanelaMundo jmMundo(-120.0f, 120.0f, -120.0f, 120.0f);
    RegiaoViewport rvTela(30.0f, 970.0f, 40.0f, 670.0f);
    viewport = Viewport(jmMundo, rvTela);

    glutDisplayFunc(callbackDisplay);
    glutReshapeFunc(callbackReshape);
    glutKeyboardFunc(callbackKeyboard);
    glutSpecialFunc(callbackSpecial);

    std::cout << "==========================================================\n";
    std::cout << "  ETAPA 4: NAO-COMUTATIVIDADE DAS TRANSFORMACOES (TECLA D)\n";
    std::cout << "==========================================================\n";

    glutMainLoop();
    return 0;
}
