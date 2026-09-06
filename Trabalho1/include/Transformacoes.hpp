#ifndef TRANSFORMACOES_HPP
#define TRANSFORMACOES_HPP

#include <glm/glm.hpp>
#include <cmath>

/**
 * @brief Fábrica de matrizes homogêneas 3x3 para transformações geométricas 2D.
 *
 * NOTA SOBRE A GLM:
 * A GLM adota a convenção de VETORES-COLUNA e MATRIZES COLUMN-MAJOR.
 * Em glm::mat3 M, M[coluna][linha]:
 * M[0] é a coluna 0, M[1] é a coluna 1, M[2] é a coluna 2.
 * A multiplicação é aplicada à esquerda do vetor homogêneo: P' = M * P.
 */
class Transformacoes {
public:
    /**
     * @brief Matriz de Translação 2D:
     * | 1  0  dx |
     * | 0  1  dy |
     * | 0  0  1  |
     */
    static glm::mat3 translacao(float dx, float dy) {
        glm::mat3 T(1.0f);
        T[2] = glm::vec3(dx, dy, 1.0f); // 3ª coluna
        return T;
    }

    /**
     * @brief Matriz de Escala 2D em relação à origem:
     * | sx  0   0 |
     * | 0   sy  0 |
     * | 0   0   1 |
     */
    static glm::mat3 escala(float sx, float sy) {
        glm::mat3 S(1.0f);
        S[0][0] = sx;
        S[1][1] = sy;
        return S;
    }

    /**
     * @brief Matriz de Rotação 2D em torno da origem (ângulo em radianos):
     * | cos(θ)  -sin(θ)  0 |
     * | sin(θ)   cos(θ)  0 |
     * |   0        0     1 |
     */
    static glm::mat3 rotacao(float anguloRadianos) {
        glm::mat3 R(1.0f);
        float c = std::cos(anguloRadianos);
        float s = std::sin(anguloRadianos);

        R[0][0] = c;  R[1][0] = -s;
        R[0][1] = s;  R[1][1] = c;
        return R;
    }

    /**
     * @brief Rotação em torno de um ponto arbitrário (ex: Centro Geométrico C):
     * Composição: T(C) * R(θ) * T(-C)
     */
    static glm::mat3 rotacaoPonto(float anguloRadianos, const glm::vec2& ponto) {
        glm::mat3 T_ida   = translacao(-ponto.x, -ponto.y);
        glm::mat3 R       = rotacao(anguloRadianos);
        glm::mat3 T_volta = translacao(ponto.x, ponto.y);
        return T_volta * R * T_ida;
    }

    /**
     * @brief Escala em torno de um ponto arbitrário (ex: Centro Geométrico C):
     * Composição: T(C) * S(sx, sy) * T(-C)
     */
    static glm::mat3 escalaPonto(float sx, float sy, const glm::vec2& ponto) {
        glm::mat3 T_ida   = translacao(-ponto.x, -ponto.y);
        glm::mat3 S       = escala(sx, sy);
        glm::mat3 T_volta = translacao(ponto.x, ponto.y);
        return T_volta * S * T_ida;
    }

    /**
     * @brief Cisalhamento no eixo X (Shear X):
     * | 1  hx  0 |
     * | 0  1   0 |
     * | 0  0   1 |
     */
    static glm::mat3 cisalhamentoX(float hx) {
        glm::mat3 H(1.0f);
        H[1][0] = hx; // coluna 1, linha 0
        return H;
    }

    /**
     * @brief Cisalhamento no eixo Y (Shear Y):
     * | 1   0  0 |
     * | hy  1  0 |
     * | 0   0  1 |
     */
    static glm::mat3 cisalhamentoY(float hy) {
        glm::mat3 H(1.0f);
        H[0][1] = hy; // coluna 0, linha 1
        return H;
    }

    /**
     * @brief Reflexão em relação ao eixo X (inverte coordenadas Y):
     * | 1   0  0 |
     * | 0  -1  0 |
     * | 0   0  1 |
     */
    static glm::mat3 reflexaoX() {
        glm::mat3 R(1.0f);
        R[1][1] = -1.0f;
        return R;
    }

    /**
     * @brief Reflexão em relação ao eixo Y (inverte coordenadas X):
     * |-1   0  0 |
     * | 0   1  0 |
     * | 0   0  1 |
     */
    static glm::mat3 reflexaoY() {
        glm::mat3 R(1.0f);
        R[0][0] = -1.0f;
        return R;
    }

    /**
     * @brief Aplica a matriz homogênea 3x3 em um ponto 2D.
     */
    static glm::vec2 aplicar(const glm::mat3& M, const glm::vec2& ponto) {
        glm::vec3 pHomogeneo(ponto.x, ponto.y, 1.0f);
        glm::vec3 pResultado = M * pHomogeneo;
        return glm::vec2(pResultado.x, pResultado.y);
    }
};

#endif // TRANSFORMACOES_HPP
