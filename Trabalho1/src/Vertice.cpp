#include "Vertice.hpp"

Vertice::Vertice() : posicao(0.0f, 0.0f) {}

Vertice::Vertice(float x, float y) : posicao(x, y) {}

Vertice::Vertice(const glm::vec2& p) : posicao(p) {}
