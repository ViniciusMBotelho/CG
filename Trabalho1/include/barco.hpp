#pragma once

#include "scene_types.hpp"

inline ObjetoBase criarBarco() {
    return {
        "Barco",
        {
            // Casco ciano
            {{0.04f, 0.75f, 0.78f}, {{ 25.0f, 16.0f}, { 55.0f, 16.0f}, { 49.0f,  7.0f}, { 31.0f,  7.0f}}},
            // Mastro
            {{0.96f, 0.70f, 0.08f}, {{ 39.0f, 16.0f}, { 42.0f, 16.0f}, { 42.0f, 38.0f}, { 39.0f, 38.0f}}},
            // Vela verde
            {{0.15f, 0.86f, 0.30f}, {{ 42.0f, 36.0f}, { 42.0f, 22.0f}, { 53.0f, 22.0f}}},
            // Vela azul-escura
            {{0.08f, 0.30f, 0.85f}, {{ 39.0f, 34.0f}, { 39.0f, 20.0f}, { 30.0f, 20.0f}}}
        }
    };
}
