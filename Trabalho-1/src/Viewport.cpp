#include "Viewport.hpp"

Viewport::Viewport() : mundo(), regiao() {}

Viewport::Viewport(const JanelaMundo& mundo, const RegiaoViewport& regiao)
    : mundo(mundo), regiao(regiao) {}

glm::vec2 Viewport::mundoParaViewport(const glm::vec2& pMundo) const {
    // 1. Normalização das coordenadas do mundo para o intervalo [0, 1]
    float normX = (pMundo.x - mundo.xMin) / (mundo.xMax - mundo.xMin);
    float normY = (pMundo.y - mundo.yMin) / (mundo.yMax - mundo.yMin);

    // 2. Mapeamento explícito para a viewport com inversão do eixo Y
    // No mundo: Y mínimo está na base, Y máximo no topo.
    // Na tela (viewport): Y mínimo (regiao.yMin) está no topo e cresce para baixo até regiao.yMax.
    // Logo, (1.0f - normY) inverte a orientação vertical.
    float xVp = regiao.xMin + normX * (regiao.xMax - regiao.xMin);
    float yVp = regiao.yMin + (1.0f - normY) * (regiao.yMax - regiao.yMin);

    return glm::vec2(xVp, yVp);
}

glm::vec2 Viewport::viewportParaMundo(const glm::vec2& pViewport) const {
    float normX = (pViewport.x - regiao.xMin) / (regiao.xMax - regiao.xMin);
    // Inversão do eixo Y na volta
    float normY = 1.0f - ((pViewport.y - regiao.yMin) / (regiao.yMax - regiao.yMin));

    float xMundo = mundo.xMin + normX * (mundo.xMax - mundo.xMin);
    float yMundo = mundo.yMin + normY * (mundo.yMax - mundo.yMin);

    return glm::vec2(xMundo, yMundo);
}

bool Viewport::contemPonto(float x, float y) const {
    return (x >= regiao.xMin && x <= regiao.xMax &&
            y >= regiao.yMin && y <= regiao.yMax);
}
