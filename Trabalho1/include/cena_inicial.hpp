#pragma once

#include "barco.hpp"
#include "casa.hpp"
#include "moinho.hpp"

inline std::vector<ObjetoBase> criarCenaInicial() {
    return {criarCasa(), criarBarco(), criarMoinho()};
}
