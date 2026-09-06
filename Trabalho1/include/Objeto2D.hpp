#ifndef OBJETO2D_HPP
#define OBJETO2D_HPP

#include "Poligono.hpp"
#include "scene_types.hpp"
#include <string>
#include <vector>
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

/**
 * @brief Representa um objeto bidimensional composto por polígonos.
 * Armazena os vértices originais e mantém uma matriz homogênea acumulada M (3x3).
 */
class Objeto2D {
private:
    std::string nome;
    std::vector<Poligono> poligonos; // Vértices originais imutáveis no mundo
    glm::mat3 matrizAcumulada;        // Matriz M acumulada das transformações

public:
    Objeto2D();
    Objeto2D(const std::string& nome);
    Objeto2D(const ObjetoBase& base);

    const std::string& getNome() const { return nome; }
    void setNome(const std::string& novoNome) { nome = novoNome; }

    void adicionarPoligono(const Poligono& poligono);
    const std::vector<Poligono>& getPoligonos() const { return poligonos; }
    std::vector<Poligono>& getPoligonos() { return poligonos; }

    const glm::mat3& getMatrizAcumulada() const { return matrizAcumulada; }
    void setMatrizAcumulada(const glm::mat3& M) { matrizAcumulada = M; }

    void comporTransformacao(const glm::mat3& novaTransformacao);
    void resetar();

    glm::vec2 calcularCentroOriginal() const;
    glm::vec2 calcularCentroAtual() const;

    // Métodos diretos de transformação exigidos no edital
    void transladar(float dx, float dy);
    void rotacionarCentro(float anguloRadianos);
    void rotacionarOrigem(float anguloRadianos);
    void escalar(float sx, float sy);
    void escalarCentro(float sx, float sy);
    void cisalhar(float hx, float hy);
    void espelharX();
    void espelharY();

    ObjetoBase toObjetoBase() const;
};

#endif // OBJETO2D_HPP
