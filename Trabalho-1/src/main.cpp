#include "Aplicacao.hpp"

int main(int argc, char** argv) {
    Aplicacao app(1100, 750, "TP1 - Transformacoes Geometricas 2D e Viewport");
    app.inicializar(argc, argv);
    app.executar();
    return 0;
}
