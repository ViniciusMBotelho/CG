#ifndef CENA_HPP
#define CENA_HPP

#include "Objeto2D.hpp"
#include "scene_types.hpp"
#include <vector>
#include <string>

/**
 * @brief Gerencia a coleção de objetos 2D da cena e o objeto ativo.
 */
class Cena {
private:
    std::vector<Objeto2D> objetos;
    int indiceObjetoAtivo;

public:
    Cena();

    void inicializarObjetosPadrao();
    void carregarDeCenaBase(const std::vector<ObjetoBase>& cenaBase);
    bool carregarDeArquivo(const std::string& caminhoArquivo);

    const std::vector<Objeto2D>& getObjetos() const { return objetos; }
    std::vector<Objeto2D>& getObjetos() { return objetos; }

    int getIndiceObjetoAtivo() const { return indiceObjetoAtivo; }
    void setIndiceObjetoAtivo(int indice);

    Objeto2D* getObjetoAtivo();
    const Objeto2D* getObjetoAtivo() const;

    void adicionarObjeto(const Objeto2D& obj);
    void resetarObjetoAtivo();
    void resetarTodosObjetos();
};

#endif // CENA_HPP
