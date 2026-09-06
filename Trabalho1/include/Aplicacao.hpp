#ifndef APLICACAO_HPP
#define APLICACAO_HPP

#include "Cena.hpp"
#include "Viewport.hpp"
#include <string>
#include <vector>

/**
 * @brief Classe principal que encapsula o loop da aplicação gráfica, callbacks do GLUT,
 * estado da cena, viewport, renderização, painel lateral, mouse e teclado.
 */
class Aplicacao {
private:
    int larguraJanela;
    int alturaJanela;
    std::string tituloJanela;

    Cena cena;
    Viewport viewportPrincipal;

    // Modo de demonstração da não-comutatividade: ordem contrária
    bool modoDemonstracaoOrdem;

    // Estado do mouse para arraste/seleção
    bool mousePressionado;
    int mouseUltimoX;
    int mouseUltimoY;

    // Instância estática para callbacks do GLUT
    static Aplicacao* instancia;

public:
    Aplicacao(int largura = 1100, int altura = 750, const std::string& titulo = "TP1 - Transformacoes Geometricas 2D e Viewport");
    ~Aplicacao();

    static Aplicacao* getInstancia() { return instancia; }

    void inicializar(int argc, char** argv);
    void executar();

    // Callbacks do GLUT
    static void callbackDisplay();
    static void callbackReshape(int w, int h);
    static void callbackKeyboard(unsigned char key, int x, int y);
    static void callbackSpecial(int key, int x, int y);
    static void callbackMouse(int button, int state, int x, int y);
    static void callbackMotion(int x, int y);

    // Funções de desenho
    void desenharCena();
    void desenharViewportNormal();
    void desenharDemonstracaoOrdem();
    void desenharEixosCartesianos(const Viewport& vp, float passo = 25.0f);
    void desenharObjeto(const Objeto2D& obj, const Viewport& vp, bool aramado = false, bool ordemInversa = false);
    void desenharPainelLateral();

    // Utilitários de desenho
    void desenharTexto(float x, float y, const std::string& texto, void* fonte = nullptr);
    void desenharRetangulo(float x1, float y1, float x2, float y2, const glm::vec3& cor, bool preenchido);
    void salvarCaptura(const std::string& caminhoArquivo);

    // Getters e Setters
    Cena& getCena() { return cena; }
    Viewport& getViewport() { return viewportPrincipal; }
    bool isModoDemonstracao() const { return modoDemonstracaoOrdem; }
    void toggleModoDemonstracao() { modoDemonstracaoOrdem = !modoDemonstracaoOrdem; }
};

#endif // APLICACAO_HPP
