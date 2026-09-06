#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <glm/glm.hpp>

/**
 * @brief Define os limites de visualização no sistema de coordenadas do mundo.
 * No mundo cartesiano, o eixo Y cresce para CIMA.
 */
struct JanelaMundo {
    float xMin, xMax;
    float yMin, yMax;

    JanelaMundo(float xmin = -120.0f, float xmax = 120.0f, float ymin = -120.0f, float ymax = 120.0f)
        : xMin(xmin), xMax(xmax), yMin(ymin), yMax(ymax) {}

    float getLargura() const { return xMax - xMin; }
    float getAltura() const { return yMax - yMin; }
};

/**
 * @brief Define os limites da região da janela (em pixels) onde o mundo será desenhado.
 * Na tela da janela, a origem (0,0) fica no canto superior esquerdo e Y cresce para BAIXO.
 */
struct RegiaoViewport {
    float xMin, xMax;
    float yMin, yMax;

    RegiaoViewport(float xmin = 25.0f, float xmax = 650.0f, float ymin = 40.0f, float ymax = 700.0f)
        : xMin(xmin), xMax(xmax), yMin(ymin), yMax(ymax) {}

    float getLargura() const { return xMax - xMin; }
    float getAltura() const { return yMax - yMin; }
};

/**
 * @brief Realiza o cálculo explícito de mapeamento entre o Mundo e a Viewport,
 * incluindo a inversão vertical obrigatória do eixo Y.
 */
class Viewport {
private:
    JanelaMundo mundo;
    RegiaoViewport regiao;

public:
    Viewport();
    Viewport(const JanelaMundo& jm, const RegiaoViewport& rv);

    /**
     * @brief Mapeamento explícito Mundo -> Viewport.
     * Fórmula:
     *   xv = xvMin + ((xw - xwMin) / (xwMax - xwMin)) * (xvMax - xvMin)
     *   yv = yvMin + ((ywMax - yw) / (ywMax - ywMin)) * (yvMax - yvMin)  <-- Inversão do eixo Y
     */
    glm::vec2 mundoParaViewport(const glm::vec2& pMundo) const;

    /**
     * @brief Mapeamento inverso Viewport -> Mundo (usado para cliques de mouse).
     */
    glm::vec2 viewportParaMundo(const glm::vec2& pViewport) const;

    bool contemPonto(float xPixel, float yPixel) const;

    const JanelaMundo& getMundo() const { return mundo; }
    void setMundo(const JanelaMundo& jm) { mundo = jm; }

    const RegiaoViewport& getRegiao() const { return regiao; }
    void setRegiao(const RegiaoViewport& rv) { regiao = rv; }
};

#endif // VIEWPORT_HPP
