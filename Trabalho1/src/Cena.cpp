#include "Cena.hpp"
#include "cena_inicial.hpp"
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
