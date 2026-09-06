#pragma once

#include "scene_types.hpp"

inline ObjetoBase criarMoinho() {
    return {
        "Moinho",
        {
            // Base azul
            {{0.12f, 0.30f, 0.95f}, {{-22.0f, -52.0f}, {-2.0f, -52.0f}, {-6.0f, -18.0f}, {-18.0f, -18.0f}}},
            // Porta
            {{0.75f, 0.66f, 0.40f}, {{-15.0f, -52.0f}, {-9.0f, -52.0f}, {-9.0f, -41.0f}, {-15.0f, -41.0f}}},
            // Pa do rotor superior direito
            {{0.35f, 0.55f, 1.00f}, {{-12.0f, -25.0f}, {-4.0f, -12.0f}, { 1.0f, -17.0f}, {-8.0f, -29.0f}}},
            // Pa do rotor superior esquerdo
            {{0.35f, 0.55f, 1.00f}, {{-13.0f, -25.0f}, {-26.0f, -17.0f}, {-21.0f, -12.0f}, {-9.0f, -21.0f}}},
            // Pa do rotor inferior esquerdo
            {{0.35f, 0.55f, 1.00f}, {{-16.0f, -25.0f}, {-25.0f, -38.0f}, {-20.0f, -43.0f}, {-11.0f, -30.0f}}},
            // Pa do rotor inferior direito
            {{0.35f, 0.55f, 1.00f}, {{-13.0f, -28.0f}, { -5.0f, -41.0f}, { 0.0f, -36.0f}, { -9.0f, -24.0f}}},
            // Cubo do rotor
            {{0.94f, 0.70f, 0.10f}, {{-16.0f, -29.0f}, {-10.0f, -29.0f}, {-10.0f, -23.0f}, {-16.0f, -23.0f}}}
        }
    };
}
