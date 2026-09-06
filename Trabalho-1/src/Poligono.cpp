#include "Poligono.hpp"

Poligono::Poligono() : cor(1.0f, 1.0f, 1.0f) {}

Poligono::Poligono(const glm::vec3& cor) : cor(cor) {}

Poligono::Poligono(const glm::vec3& cor, const std::vector<Vertice>& verts)
    : cor(cor), vertices(verts) {}

Poligono::Poligono(const PoligonoBase& base) : cor(base.cor) {
    vertices.reserve(base.vertices.size());
    for (const auto& v : base.vertices) {
        vertices.emplace_back(v);
    }
}

void Poligono::adicionarVertice(const Vertice& v) {
    vertices.push_back(v);
}

void Poligono::adicionarVertice(float x, float y) {
    vertices.emplace_back(x, y);
}

void Poligono::adicionarVertice(const glm::vec2& v) {
    vertices.emplace_back(v);
}

PoligonoBase Poligono::toPoligonoBase() const {
    PoligonoBase base;
    base.cor = cor;
    base.vertices.reserve(vertices.size());
    for (const auto& v : vertices) {
        base.vertices.push_back(v.posicao);
    }
    return base;
}
