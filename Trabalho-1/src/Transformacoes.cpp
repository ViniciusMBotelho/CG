#include "Transformacoes.hpp"

glm::mat3 Transformacoes::identidade() {
    return glm::mat3(1.0f);
}

glm::mat3 Transformacoes::translacao(float dx, float dy) {
    glm::mat3 T(1.0f);
    T[2] = glm::vec3(dx, dy, 1.0f);
    return T;
}

glm::mat3 Transformacoes::escala(float sx, float sy) {
    glm::mat3 S(1.0f);
    S[0][0] = sx;
    S[1][1] = sy;
    return S;
}

glm::mat3 Transformacoes::rotacao(float anguloRadianos) {
    glm::mat3 R(1.0f);
    float c = std::cos(anguloRadianos);
    float s = std::sin(anguloRadianos);
    R[0][0] = c;
    R[0][1] = s;
    R[1][0] = -s;
    R[1][1] = c;
    return R;
}

glm::mat3 Transformacoes::rotacaoPonto(float anguloRadianos, const glm::vec2& centro) {
    return translacao(centro.x, centro.y) * rotacao(anguloRadianos) * translacao(-centro.x, -centro.y);
}

glm::mat3 Transformacoes::escalaPonto(float sx, float sy, const glm::vec2& centro) {
    return translacao(centro.x, centro.y) * escala(sx, sy) * translacao(-centro.x, -centro.y);
}

glm::mat3 Transformacoes::reflexaoX() {
    glm::mat3 Ref(1.0f);
    Ref[1][1] = -1.0f;
    return Ref;
}

glm::mat3 Transformacoes::reflexaoY() {
    glm::mat3 Ref(1.0f);
    Ref[0][0] = -1.0f;
    return Ref;
}

glm::mat3 Transformacoes::reflexaoPonto(bool emX, bool emY, const glm::vec2& centro) {
    glm::mat3 Ref(1.0f);
    if (emX) Ref[1][1] = -1.0f;
    if (emY) Ref[0][0] = -1.0f;
    return translacao(centro.x, centro.y) * Ref * translacao(-centro.x, -centro.y);
}

glm::mat3 Transformacoes::cisalhamentoX(float hx) {
    glm::mat3 Sh(1.0f);
    Sh[1][0] = hx; // x' = 1*x + hx*y
    return Sh;
}

glm::mat3 Transformacoes::cisalhamentoY(float hy) {
    glm::mat3 Sh(1.0f);
    Sh[0][1] = hy; // y' = hy*x + 1*y
    return Sh;
}

glm::vec2 Transformacoes::aplicar(const glm::mat3& M, const glm::vec2& ponto) {
    glm::vec3 pHomogeneo(ponto.x, ponto.y, 1.0f);
    glm::vec3 res = M * pHomogeneo;
    return glm::vec2(res.x, res.y);
}
