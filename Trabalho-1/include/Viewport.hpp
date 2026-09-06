#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <glm/glm.hpp>

/**
 * @brief Define os limites de uma janela no sistema de coordenadas do mundo (cartesiano).
 */
struct JanelaMundo {
    float xMin;
    float xMax;
    float yMin;
    float yMax;

    JanelaMundo(float xmin = -120.0f, float xmax = 120.0f, float ymin = -120.0f, float ymax = 120.0f)
        : xMin(xmin), xMax(xmax), yMin(ymin), yMax(ymax) {}
};

/**
 * @brief Define a região de visualização (viewport) na janela em coordenadas de pixel.
 * Origem no canto superior esquerdo (xMin, yMin) e Y crescendo para baixo.
 */
struct RegiaoViewport {
    float xMin;
    float xMax;
    float yMin;
    float yMax;

    RegiaoViewport(float xmin = 20.0f, float xmax = 700.0f, float ymin = 40.0f, float ymax = 680.0f)
        : xMin(xmin), xMax(xmax), yMin(ymin), yMax(ymax) {}

    float getLargura() const { return xMax - xMin; }
    float getAltura() const { return yMax - yMin; }
};

/**
 * @brief Responsável pelo mapeamento explícito mundo -> viewport com inversão do eixo Y.
 */
class Viewport {
private:
    JanelaMundo mundo;
    RegiaoViewport regiao;

public:
    Viewport();
    Viewport(const JanelaMundo& mundo, const RegiaoViewport& regiao);

    const JanelaMundo& getMundo() const { return mundo; }
    void setMundo(const JanelaMundo& novoMundo) { mundo = novoMundo; }

    const RegiaoViewport& getRegiao() const { return regiao; }
    void setRegiao(const RegiaoViewport& novaRegiao) { regiao = novaRegiao; }

    /**
     * @brief Converte um ponto do mundo (cartesiano: Y cresce p/ cima)
     * para coordenadas da viewport (pixels da janela: Y cresce p/ baixo).
     */
    glm::vec2 mundoParaViewport(const glm::vec2& pMundo) const;

    /**
     * @brief Converte um ponto da viewport (pixels) de volta para o sistema do mundo.
     */
    glm::vec2 viewportParaMundo(const glm::vec2& pViewport) const;

    /**
     * @brief Verifica se um ponto em coordenadas de pixel está contido na viewport.
     */
    bool contemPonto(float x, float y) const;
};

#endif // VIEWPORT_HPP
