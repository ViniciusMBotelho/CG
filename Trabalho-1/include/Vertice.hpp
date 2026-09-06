#ifndef VERTICE_HPP
#define VERTICE_HPP

#include <glm/glm.hpp>

/**
 * @brief Representa um vértice 2D no sistema de coordenadas do mundo.
 */
class Vertice {
public:
    glm::vec2 posicao;

    Vertice();
    Vertice(float x, float y);
    Vertice(const glm::vec2& p);

    float getX() const { return posicao.x; }
    float getY() const { return posicao.y; }
    void setX(float x) { posicao.x = x; }
    void setY(float y) { posicao.y = y; }
};

#endif // VERTICE_HPP
