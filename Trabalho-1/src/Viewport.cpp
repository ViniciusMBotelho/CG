#include "Viewport.hpp"

Viewport::Viewport() : mundo(), regiao() {}

Viewport::Viewport(const JanelaMundo& jm, const RegiaoViewport& rv)
    : mundo(jm), regiao(rv) {}

glm::vec2 Viewport::mundoParaViewport(const glm::vec2& pMundo) const {
    float xw = pMundo.x;
    float yw = pMundo.y;

    // Escala e translação no eixo X
    float xv = regiao.xMin + ((xw - mundo.xMin) / mundo.getLargura()) * regiao.getLargura();

    // Escala, translação e INVERSÃO no eixo Y:
    // No mundo: ywMin é a base e ywMax é o topo.
    // Na tela:  regiao.yMin é o topo e regiao.yMax é a base.
    float yv = regiao.yMin + ((mundo.yMax - yw) / mundo.getAltura()) * regiao.getAltura();

    return glm::vec2(xv, yv);
}

glm::vec2 Viewport::viewportParaMundo(const glm::vec2& pViewport) const {
    float xv = pViewport.x;
    float yv = pViewport.y;

    float xw = mundo.xMin + ((xv - regiao.xMin) / regiao.getLargura()) * mundo.getLargura();
    float yw = mundo.yMax - ((yv - regiao.yMin) / regiao.getAltura()) * mundo.getAltura();

    return glm::vec2(xw, yw);
}

bool Viewport::contemPonto(float xPixel, float yPixel) const {
    return (xPixel >= regiao.xMin && xPixel <= regiao.xMax &&
            yPixel >= regiao.yMin && yPixel <= regiao.yMax);
}
