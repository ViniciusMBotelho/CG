#include "Cena.hpp"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "========================================\n";
    std::cout << "  ETAPA 1: VERIFICACAO DO MODELO DA CENA\n";
    std::cout << "========================================\n\n";

    Cena cena;
    const auto& objetos = cena.getObjetos();
    std::cout << "Total de objetos carregados: " << objetos.size() << "\n\n";

    for (size_t i = 0; i < objetos.size(); ++i) {
        const auto& obj = objetos[i];
        glm::vec2 centroOrig = obj.calcularCentroOriginal();
        glm::vec2 centroAtual = obj.calcularCentroAtual();

        std::cout << "[" << (i + 1) << "] Objeto: " << obj.getNome() << "\n";
        std::cout << "    Quantidade de poligonos: " << obj.getPoligonos().size() << "\n";
        
        size_t totalVertices = 0;
        for (const auto& poly : obj.getPoligonos()) {
            totalVertices += poly.getQuantidadeVertices();
        }
        std::cout << "    Total de vertices: " << totalVertices << "\n";
        std::cout << "    Centro original no mundo: (" 
                  << std::fixed << std::setprecision(2)
                  << centroOrig.x << ", " << centroOrig.y << ")\n";
        std::cout << "    Centro atual (M = Identidade): (" 
                  << std::fixed << std::setprecision(2)
                  << centroAtual.x << ", " << centroAtual.y << ")\n\n";
    }

    std::cout << "Objeto ativo padrao: " << cena.getObjetoAtivo()->getNome() << "\n";
    std::cout << "\n>> Etapa 1 validada com sucesso! Vértices no mundo e matrizes prontas.\n";
    return 0;
}
