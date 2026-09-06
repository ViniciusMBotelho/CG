#ifndef TRANSFORMACOES_HPP
#define TRANSFORMACOES_HPP

#include <glm/glm.hpp>
#include <cmath>

/**
 * @brief Fábrica de matrizes de transformações geométricas 2D homogêneas 3x3 usando GLM.
 */
class Transformacoes {
public:
    /**
     * @brief Cria uma matriz de identidade 3x3.
     */
    static glm::mat3 identidade();

    /**
     * @brief Matriz de translação homogênea 3x3.
     * [ 1   0  dx ]
     * [ 0   1  dy ]
     * [ 0   0   1 ]
     */
    static glm::mat3 translacao(float dx, float dy);

    /**
     * @brief Matriz de escala homogênea 3x3 (em relação à origem).
     * [ sx  0   0 ]
     * [ 0   sy  0 ]
     * [ 0   0   1 ]
     */
    static glm::mat3 escala(float sx, float sy);

    /**
     * @brief Matriz de rotação homogênea 3x3 em torno da origem (em radianos).
     * [ cos(th)  -sin(th)  0 ]
     * [ sin(th)   cos(th)  0 ]
     * [    0         0     1 ]
     */
    static glm::mat3 rotacao(float anguloRadianos);

    /**
     * @brief Matriz de rotação composta em torno de um ponto de pivô arbitrário (Cx, Cy).
     * M = T(Cx, Cy) * R(theta) * T(-Cx, -Cy)
     */
    static glm::mat3 rotacaoPonto(float anguloRadianos, const glm::vec2& centro);

    /**
     * @brief Matriz de escala composta em torno de um ponto de pivô arbitrário (Cx, Cy).
     * M = T(Cx, Cy) * S(sx, sy) * T(-Cx, -Cy)
     */
    static glm::mat3 escalaPonto(float sx, float sy, const glm::vec2& centro);

    /**
     * @brief Reflexão em relação ao eixo X (inverte Y: y' = -y).
     */
    static glm::mat3 reflexaoX();

    /**
     * @brief Reflexão em relação ao eixo Y (inverte X: x' = -x).
     */
    static glm::mat3 reflexaoY();

    /**
     * @brief Reflexão em relação ao centro geométrico.
     */
    static glm::mat3 reflexaoPonto(bool emX, bool emY, const glm::vec2& centro);

    /**
     * @brief Cisalhamento no eixo X (x' = x + hx * y).
     */
    static glm::mat3 cisalhamentoX(float hx);

    /**
     * @brief Cisalhamento no eixo Y (y' = y + hy * x).
     */
    static glm::mat3 cisalhamentoY(float hy);

    /**
     * @brief Multiplica a matriz 3x3 pelo ponto homogêneo (x, y, 1) e retorna (x', y').
     */
    static glm::vec2 aplicar(const glm::mat3& M, const glm::vec2& ponto);
};

#endif // TRANSFORMACOES_HPP
