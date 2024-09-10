#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <stdexcept>
#include <glm/glm.hpp>
#include <util/pathfinding_AStar/NodoPathfinding.hpp>

std::vector<glm::vec2> CalculatePathFinding(glm::vec2 inicio, glm::vec2 final, std::vector<std::vector<int>> &mapa)
{
    std::vector<glm::vec2> camino;

    NodoPathfinding nodoFinal{nullptr, final};
    nodoFinal.f = nodoFinal.g = nodoFinal.h = 0;

    std::vector<std::unique_ptr<NodoPathfinding>> listaFrontera;
    std::vector<std::unique_ptr<NodoPathfinding>> listaInterior;

    NodoPathfinding *nodoInicio = listaFrontera.emplace_back(std::make_unique<NodoPathfinding>(nullptr, inicio)).get();
    nodoInicio->g = 0;
    nodoInicio->h = std::abs(inicio.x - final.x) + std::abs(inicio.y - final.y);
    nodoInicio->f = nodoInicio->g + nodoInicio->h + 1;
    NodoPathfinding *nodoActual{nodoInicio};
    bool existe{false};
    int aux_i{0};
    int contadorDeVeces{0};
    while (listaFrontera.size() > 0)
    {
        contadorDeVeces++;
        nodoActual = listaFrontera[0].get();
        aux_i = 0;

        for (unsigned int i = 0; i < listaFrontera.size(); i++)
        {
            if (listaFrontera.size() != 1 && listaFrontera[i]->f < nodoActual->f)
            {
                nodoActual = listaFrontera[i].get();
                aux_i = i;
            }
        }

        listaInterior.emplace_back(std::move(*(listaFrontera.begin() + aux_i)));
        listaFrontera.erase(listaFrontera.begin() + aux_i);

        if (nodoActual->posicion == nodoFinal.posicion)
        {
            NodoPathfinding *nodoCaminoActual = nodoActual;
            while (nodoCaminoActual->posicion != nodoInicio->posicion)
            {
                camino.emplace_back(nodoCaminoActual->posicion);
                nodoCaminoActual = nodoCaminoActual->padre;
            }

            return camino;
        }
        std::vector<NodoPathfinding> hijos;
        std::vector<int> gHijos;
        glm::vec2 nueva_posicion;
        float g2add{0};
        for (int i = 0; i < 4; i++)
        {
            switch (i)
            {
            case 0: // ARRIBA
                g2add = nodoActual->g + 1;
                nueva_posicion.x = 0;
                nueva_posicion.y = -1;
                break;
            case 1: // ABAJO
                g2add = nodoActual->g + 1;
                nueva_posicion.x = 0;
                nueva_posicion.y = 1;
                break;
            case 2: // IZQUIERDA
                g2add = nodoActual->g + 1;
                nueva_posicion.x = -1;
                nueva_posicion.y = 0;
                break;
            case 3: // DERECHA
                g2add = nodoActual->g + 1;
                nueva_posicion.x = 1;
                nueva_posicion.y = 0;
                break;

            }

            glm::vec2 posicion_nodo = glm::vec2(nodoActual->posicion.x + nueva_posicion.x, nodoActual->posicion.y + nueva_posicion.y);
            if (posicion_nodo.x > float(int(mapa.size()) - 1) || posicion_nodo.x < 0 || posicion_nodo.y > float(int(mapa[0].size()) - 1) || posicion_nodo.y < 0)
            {
                continue;
            }

            if (mapa[int(posicion_nodo.x)][int(posicion_nodo.y)] != 1)
            { // Si es pared, continue
                continue;
            }
            gHijos.push_back(int(g2add));
            hijos.emplace_back(nodoActual, posicion_nodo);
        }

        for (std::size_t i = 0; i < hijos.size(); i++)
        {

            existe = false;
            for (std::size_t j = 0; j < listaFrontera.size(); j++)
            {                
                if (listaFrontera[j]->posicion == hijos[i].posicion)
                {
                    existe = true;
                }
            }
            for (std::size_t j = 0; j < listaInterior.size(); j++)
            {
                if (listaInterior[j]->posicion == hijos[i].posicion)
                {
                    existe = true;
                }
            }

            if (!existe)
            {
                hijos[i].g = float(gHijos[i]);
                hijos[i].h = std::abs(hijos[i].posicion.x - nodoFinal.posicion.x) + std::abs(hijos[i].posicion.y - nodoFinal.posicion.y); // distancia manhattan en caso de 4 direcciones
                hijos[i].f = hijos[i].g + hijos[i].h;

                listaFrontera.emplace_back(std::make_unique<NodoPathfinding>(std::move(hijos[i])));
            }
        }
    }
    return camino;
}
