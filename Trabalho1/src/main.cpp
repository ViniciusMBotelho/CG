#include "Cena.hpp"
#include "Viewport.hpp"
#include <GL/freeglut.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

// Estado global da aplicação para a Etapa 2
static Cena cena;
static Viewport viewport;
static int larguraJanela = 1000;
static int alturaJanela = 700;

// Utilitário para desenhar texto com GLUT
void desenharTexto(float x, float y, const std::string& texto, void* fonte = GLUT_BITMAP_HELVETICA_12) {
    glRasterPos2f(x, y);
    for (char c : texto) {
        glutBitmapCharacter(fonte, c);
    }
}

// Utilitário para desenhar retângulos (bordas e fundos)
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

// Desenha os eixos cartesianos X e Y mapeados para a viewport
void desenharEixosCartesianos(const Viewport& vp, float passo = 25.0f) {
    const JanelaMundo& jm = vp.getMundo();

    // Mapeamento dos extremos dos eixos no mundo para pixels na tela
    glm::vec2 pXInicio = vp.mundoParaViewport(glm::vec2(jm.xMin, 0.0f));
    glm::vec2 pXFim    = vp.mundoParaViewport(glm::vec2(jm.xMax, 0.0f));
    glm::vec2 pYInicio = vp.mundoParaViewport(glm::vec2(0.0f, jm.yMin));
    glm::vec2 pYFim    = vp.mundoParaViewport(glm::vec2(0.0f, jm.yMax));

    // Linhas dos eixos principais
    glColor3f(0.45f, 0.45f, 0.50f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
    glVertex2f(pXInicio.x, pXInicio.y);
    glVertex2f(pXFim.x, pXFim.y);
    glVertex2f(pYInicio.x, pYInicio.y);
    glVertex2f(pYFim.x, pYFim.y);
    glEnd();

    // Setas dos eixos
    glBegin(GL_LINES);
    // Seta no eixo X
    glVertex2f(pXFim.x, pXFim.y);
    glVertex2f(pXFim.x - 6.0f, pXFim.y - 4.0f);
    glVertex2f(pXFim.x, pXFim.y);
    glVertex2f(pXFim.x - 6.0f, pXFim.y + 4.0f);
    // Seta no eixo Y (aponta para o topo do mundo, que é o topo visual da tela)
    glVertex2f(pYFim.x, pYFim.y);
    glVertex2f(pYFim.x - 4.0f, pYFim.y + 6.0f);
    glVertex2f(pYFim.x, pYFim.y);
    glVertex2f(pYFim.x + 4.0f, pYFim.y + 6.0f);
    glEnd();

    desenharTexto(pXFim.x - 15.0f, pXFim.y - 6.0f, "x", GLUT_BITMAP_HELVETICA_12);
    desenharTexto(pYFim.x + 8.0f, pYFim.y + 12.0f, "y", GLUT_BITMAP_HELVETICA_12);

    // Graduações numéricas no eixo X
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

    // Graduações numéricas no eixo Y
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

// Renderiza um objeto convertendo cada vértice explicitamente para a viewport
void desenharObjeto(const Objeto2D& obj, const Viewport& vp) {
    const glm::mat3& M = obj.getMatrizAcumulada();

    for (const auto& poly : obj.getPoligonos()) {
        const glm::vec3& cor = poly.getCor();
        glColor3f(cor.r, cor.g, cor.b);

        // Preenchimento do polígono
        glBegin(GL_POLYGON);
        for (const auto& v : poly.getVertices()) {
            // 1. Aplica matriz M (Identidade na Etapa 2) no mundo
            glm::vec3 pHomogeneo(v.posicao.x, v.posicao.y, 1.0f);
            glm::vec3 pMundoTransf = M * pHomogeneo;

            // 2. Mapeamento explícito mundo -> viewport com inversão do eixo Y
            glm::vec2 pVp = vp.mundoParaViewport(glm::vec2(pMundoTransf.x, pMundoTransf.y));

            // 3. Envia coordenadas em pixels ao OpenGL
            glVertex2f(pVp.x, pVp.y);
        }
        glEnd();

        // Borda do polígono para contraste
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

// Callback de renderização GLUT
void callbackDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    const RegiaoViewport& regiao = viewport.getRegiao();

    // 1. Fundo da Viewport Gráfica
    desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.02f, 0.02f, 0.04f), true);

    // 2. Borda externa delimitadora da Viewport
    desenharRetangulo(regiao.xMin, regiao.yMin, regiao.xMax, regiao.yMax, glm::vec3(0.4f, 0.6f, 0.8f), false);

    // 3. Título explicativo
    glColor3f(0.8f, 0.8f, 0.9f);
    desenharTexto(regiao.xMin + 10.0f, regiao.yMin - 10.0f, "ETAPA 2: VIEWPORT GRAFICA (MAPEAMENTO EXPLICITO COM INVERSAO DE Y)", GLUT_BITMAP_HELVETICA_12);

    // Scissor test para garantir que nada desenhado escape da região da viewport
    glEnable(GL_SCISSOR_TEST);
    glScissor(static_cast<GLint>(regiao.xMin),
              static_cast<GLint>(alturaJanela - regiao.yMax),
              static_cast<GLsizei>(regiao.getLargura()),
              static_cast<GLsizei>(regiao.getAltura()));

    // 4. Eixos cartesianos
    desenharEixosCartesianos(viewport, 25.0f);

    // 5. Desenhar todos os objetos da cena
    for (const auto& obj : cena.getObjetos()) {
        desenharObjeto(obj, viewport);
    }

    glDisable(GL_SCISSOR_TEST);

    glutSwapBuffers();
}

// Callback de redimensionamento da janela GLUT
void callbackReshape(int w, int h) {
    larguraJanela = w;
    alturaJanela = h;

    // Configura projeção 2D ortogonal em pixels da janela
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, static_cast<double>(w), static_cast<double>(h), 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Atualiza a região da viewport proporcionalmente
    RegiaoViewport novaRegiao(30.0f, static_cast<float>(w) - 30.0f, 40.0f, static_cast<float>(h) - 30.0f);
    viewport.setRegiao(novaRegiao);

    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

// Tecla ESC ou Q para sair
void callbackKeyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27 || key == 'q' || key == 'Q') {
        std::cout << "\nEncerrando visualizacao da Etapa 2.\n";
        exit(0);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(larguraJanela, alturaJanela);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("TP1 - Etapa 2: Mapeamento Mundo -> Viewport");

    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);

    // Configuração inicial da viewport (Mundo [-120, 120])
    JanelaMundo jmMundo(-120.0f, 120.0f, -120.0f, 120.0f);
    RegiaoViewport rvTela(30.0f, 970.0f, 40.0f, 670.0f);
    viewport = Viewport(jmMundo, rvTela);

    glutDisplayFunc(callbackDisplay);
    glutReshapeFunc(callbackReshape);
    glutKeyboardFunc(callbackKeyboard);

    std::cout << "==========================================================\n";
    std::cout << "  ETAPA 2: MAPEAMENTO MUNDO -> VIEWPORT (OPENGL / GLUT)   \n";
    std::cout << "==========================================================\n";
    std::cout << "Janela do Mundo: X[-120, 120], Y[-120, 120]\n";
    std::cout << "Inversao do eixo Y aplicada explicitamente.\n";
    std::cout << "Pressione 'Q' ou 'ESC' na janela para fechar.\n";
    std::cout << "==========================================================\n";

    // Se estiver em modo teste não-interativo ou com timeout
    glutMainLoop();
    return 0;
}
