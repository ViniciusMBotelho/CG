#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

// Todas as coordenadas desta estrutura pertencem ao sistema do mundo.
// A conversao para a viewport deve ser implementada pelos estudantes.
struct PoligonoBase {
    glm::vec3 cor;                 // RGB, componentes entre 0.0f e 1.0f
    std::vector<glm::vec2> vertices;
};

struct ObjetoBase {
    std::string nome;
    std::vector<PoligonoBase> poligonos;
};
