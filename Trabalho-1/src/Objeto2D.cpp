#include "Objeto2D.hpp"
#include "Transformacoes.hpp"

Objeto2D::Objeto2D() : nome("Objeto"), matrizAcumulada(1.0f), matrizOrdemInversa(1.0f) {}

Objeto2D::Objeto2D(const std::string& nome) : nome(nome), matrizAcumulada(1.0f), matrizOrdemInversa(1.0f) {}

Objeto2D::Objeto2D(const ObjetoBase& base)
    : nome(base.nome), matrizAcumulada(1.0f), matrizOrdemInversa(1.0f) {
    poligonos.reserve(base.poligonos.size());
    for (const auto& p : base.poligonos) {
        poligonos.emplace_back(p);
    }
}

void Objeto2D::adicionarPoligono(const Poligono& poligono) {
    poligonos.push_back(poligono);
}

void Objeto2D::comporTransformacao(const glm::mat3& novaTransformacao) {
    matrizAcumulada = novaTransformacao * matrizAcumulada;
    matrizOrdemInversa = matrizOrdemInversa * novaTransformacao;
}

void Objeto2D::resetar() {
    matrizAcumulada = glm::mat3(1.0f);
    matrizOrdemInversa = glm::mat3(1.0f);
}

glm::vec2 Objeto2D::calcularCentroOriginal() const {
    glm::vec2 soma(0.0f, 0.0f);
    size_t totalVertices = 0;
    for (const auto& poly : poligonos) {
        for (const auto& v : poly.getVertices()) {
            soma += v.posicao;
            totalVertices++;
        }
    }
    if (totalVertices == 0) return glm::vec2(0.0f, 0.0f);
    return soma / static_cast<float>(totalVertices);
}

glm::vec2 Objeto2D::calcularCentroAtual(bool ordemInversa) const {
    glm::vec2 centroOrig = calcularCentroOriginal();
    glm::vec3 pHomogeneo(centroOrig.x, centroOrig.y, 1.0f);
    const glm::mat3& M = ordemInversa ? matrizOrdemInversa : matrizAcumulada;
    glm::vec3 pTransformado = M * pHomogeneo;
    return glm::vec2(pTransformado.x, pTransformado.y);
}

ObjetoBase Objeto2D::toObjetoBase() const {
    ObjetoBase base;
    base.nome = nome;
    base.poligonos.reserve(poligonos.size());
    for (const auto& p : poligonos) {
        base.poligonos.push_back(p.toPoligonoBase());
    }
    return base;
}

void Objeto2D::transladar(float dx, float dy) {
    glm::mat3 T = Transformacoes::translacao(dx, dy);
    comporTransformacao(T);
}

void Objeto2D::rotacionarCentro(float anguloRadianos) {
    glm::vec2 centro = calcularCentroAtual();
    glm::mat3 R = Transformacoes::rotacaoPonto(anguloRadianos, centro);
    comporTransformacao(R);
}

void Objeto2D::rotacionarOrigem(float anguloRadianos) {
    glm::mat3 R = Transformacoes::rotacao(anguloRadianos);
    comporTransformacao(R);
}

void Objeto2D::escalar(float sx, float sy) {
    glm::mat3 S = Transformacoes::escala(sx, sy);
    comporTransformacao(S);
}

void Objeto2D::escalarCentro(float sx, float sy) {
    glm::vec2 centro = calcularCentroAtual();
    glm::mat3 S = Transformacoes::escalaPonto(sx, sy, centro);
    comporTransformacao(S);
}

void Objeto2D::cisalhar(float hx, float hy) {
    if (hx != 0.0f) comporTransformacao(Transformacoes::cisalhamentoX(hx));
    if (hy != 0.0f) comporTransformacao(Transformacoes::cisalhamentoY(hy));
}

void Objeto2D::espelharX() {
    comporTransformacao(Transformacoes::reflexaoX());
}

void Objeto2D::espelharY() {
    comporTransformacao(Transformacoes::reflexaoY());
}
