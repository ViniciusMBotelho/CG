#ifndef POLIGONO_HPP
#define POLIGONO_HPP

#include "Vertice.hpp"
#include "scene_types.hpp"
#include <glm/glm.hpp>
#include <vector>

/**
 * @brief Representa um polígono com cor RGB e lista ordenada de vértices no mundo.
 */
class Poligono {
private:
    glm::vec3 cor; // R, G, B no intervalo [0, 1]
    std::vector<Vertice> vertices;

public:
    Poligono();
    Poligono(const glm::vec3& cor);
    Poligono(const glm::vec3& cor, const std::vector<Vertice>& verts);
    Poligono(const PoligonoBase& base);

    void adicionarVertice(const Vertice& v);
    void adicionarVertice(float x, float y);
    void adicionarVertice(const glm::vec2& v);

    const glm::vec3& getCor() const { return cor; }
    void setCor(const glm::vec3& novaCor) { cor = novaCor; }

    const std::vector<Vertice>& getVertices() const { return vertices; }
    std::vector<Vertice>& getVertices() { return vertices; }
    size_t getQuantidadeVertices() const { return vertices.size(); }

    PoligonoBase toPoligonoBase() const;
};

#endif // POLIGONO_HPP
