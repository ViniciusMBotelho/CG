#pragma once

#include "scene_types.hpp"

inline ObjetoBase criarCasa() {
    return {
        "Casa",
        {
            // Corpo amarelo
            {{1.00f, 0.86f, 0.08f}, {{-72.0f,  8.0f}, {-48.0f,  8.0f}, {-48.0f, 30.0f}, {-72.0f, 30.0f}}},
            // Telhado vermelho
            {{0.93f, 0.10f, 0.08f}, {{-76.0f, 30.0f}, {-60.0f, 48.0f}, {-44.0f, 30.0f}}},
            // Porta marrom
            {{0.45f, 0.24f, 0.05f}, {{-68.0f,  8.0f}, {-62.0f,  8.0f}, {-62.0f, 20.0f}, {-68.0f, 20.0f}}},
            // Janela azul
            {{0.10f, 0.62f, 0.90f}, {{-56.0f, 18.0f}, {-50.0f, 18.0f}, {-50.0f, 25.0f}, {-56.0f, 25.0f}}},
            // Chamine
            {{0.50f, 0.28f, 0.08f}, {{-53.0f, 36.0f}, {-48.0f, 36.0f}, {-48.0f, 46.0f}, {-53.0f, 46.0f}}}
        }
    };
}
