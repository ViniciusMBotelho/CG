#include "Cena.hpp"
#include "cena_inicial.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Cena::Cena() : indiceObjetoAtivo(0) {
    inicializarObjetosPadrao();
}

void Cena::setIndiceObjetoAtivo(int indice) {
    if (indice >= 0 && indice < static_cast<int>(objetos.size())) {
        indiceObjetoAtivo = indice;
    }
}

Objeto2D* Cena::getObjetoAtivo() {
    if (indiceObjetoAtivo >= 0 && indiceObjetoAtivo < static_cast<int>(objetos.size())) {
        return &objetos[indiceObjetoAtivo];
    }
    return nullptr;
}

const Objeto2D* Cena::getObjetoAtivo() const {
    if (indiceObjetoAtivo >= 0 && indiceObjetoAtivo < static_cast<int>(objetos.size())) {
        return &objetos[indiceObjetoAtivo];
    }
    return nullptr;
}

void Cena::adicionarObjeto(const Objeto2D& obj) {
    objetos.push_back(obj);
}

void Cena::resetarObjetoAtivo() {
    Objeto2D* obj = getObjetoAtivo();
    if (obj) {
        obj->resetar();
    }
}

void Cena::resetarTodosObjetos() {
    for (auto& obj : objetos) {
        obj.resetar();
    }
}

void Cena::carregarDeCenaBase(const std::vector<ObjetoBase>& cenaBase) {
    objetos.clear();
    objetos.reserve(cenaBase.size());
    for (const auto& objBase : cenaBase) {
        objetos.emplace_back(objBase);
    }
    indiceObjetoAtivo = 0;
}

void Cena::inicializarObjetosPadrao() {
    std::vector<ObjetoBase> cenaBase = criarCenaInicial();
    carregarDeCenaBase(cenaBase);
}

bool Cena::carregarDeArquivo(const std::string& caminhoArquivo) {
    std::ifstream file(caminhoArquivo);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo: " << caminhoArquivo << std::endl;
        return false;
    }

    // Leitor de arquivo genérico se fornecido pelo professor
    std::string line;
    Objeto2D* objAtual = nullptr;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        if (!(iss >> token)) continue;

        if (token == "OBJETO") {
            std::string nomeObj;
            iss >> nomeObj;
            objetos.emplace_back(nomeObj);
            objAtual = &objetos.back();
        } else if (token == "POLIGONO") {
            float r = 1.0f, g = 1.0f, b = 1.0f;
            iss >> r >> g >> b;
            if (objAtual) {
                objAtual->adicionarPoligono(Poligono(glm::vec3(r, g, b)));
                // Recupera ponteiro para o polígono recém inserido
                // (nota: evitar ponteiro se vetor realocar, então pegamos referência ao término)
            }
        } else if (token == "VERTICE") {
            float x, y;
            iss >> x >> y;
            if (objAtual && !objAtual->getPoligonos().empty()) {
                objAtual->getPoligonos().back().adicionarVertice(x, y);
            }
        }
    }
    return true;
}
