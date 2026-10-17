#include "GrafoAmistad.h"

using namespace std;

GrafoAmistad::GrafoAmistad(int cap) {
    // Si la capacidad es negativa, la dejamos en 0 por proteccion
    if (cap < 0) {
        capacidad = 0;
    } else {
        capacidad = cap;
    }
    numUsuarios = 0;

    matrizAdyacencia = new bool*[capacidad];
    monitorGlobal.registrarAsignacion((size_t)capacidad * sizeof(bool*));

    for (int i = 0; i < capacidad; i++) {
        matrizAdyacencia[i] = new bool[capacidad];
        monitorGlobal.registrarAsignacion((size_t)capacidad * sizeof(bool));
        for (int j = 0; j < capacidad; j++) {
            matrizAdyacencia[i][j] = false;
        }
        monitorGlobal.registrarIteraciones(capacidad + 1);   // por fila, para no desbordar el int
    }
}

GrafoAmistad::~GrafoAmistad() {
    if (matrizAdyacencia != nullptr) {
        for (int i = 0; i < capacidad; i++) {
            delete[] matrizAdyacencia[i];
            monitorGlobal.registrarLiberacion((size_t)capacidad * sizeof(bool));
        }
        monitorGlobal.registrarIteraciones(capacidad);
        delete[] matrizAdyacencia;
        matrizAdyacencia = nullptr;
        monitorGlobal.registrarLiberacion((size_t)capacidad * sizeof(bool*));
    }
}

void GrafoAmistad::incrementarUsuarios() {
    if (numUsuarios < capacidad) {
        numUsuarios = numUsuarios + 1;
    }
}

void GrafoAmistad::agregarAmistad(int u, int v) {
    monitorGlobal.registrarIteraciones(1);
    if (u < 0 || u >= numUsuarios || v < 0 || v >= numUsuarios || u == v) {
        return;
    }
    matrizAdyacencia[u][v] = true;
    matrizAdyacencia[v][u] = true;
}

bool GrafoAmistad::sonAmigos(int u, int v) const {
    monitorGlobal.registrarIteraciones(1);
    if (u < 0 || u >= numUsuarios || v < 0 || v >= numUsuarios) {
        return false;
    }
    return matrizAdyacencia[u][v];
}

int GrafoAmistad::obtenerAmigosEnComun(int u, int v) const {
    if (u < 0 || u >= numUsuarios || v < 0 || v >= numUsuarios || u == v) {
        return 0;
    }
    int enComun = 0;
    const bool* filaU = matrizAdyacencia[u];
    const bool* filaV = matrizAdyacencia[v];
    for (int k = 0; k < numUsuarios; k++) {
        if (filaU[k] && filaV[k]) {
            enComun = enComun + 1;
        }
    }
    monitorGlobal.registrarIteraciones(numUsuarios);
    return enComun;
}

void GrafoAmistad::contarAmigosEnComunConTodos(int u, int* conteos) const {
    if (conteos == nullptr || u < 0 || u >= numUsuarios) {
        return;
    }
    for (int k = 0; k < numUsuarios; k++) {
        conteos[k] = 0;
    }
    monitorGlobal.registrarIteraciones(numUsuarios);

    const bool* filaU = matrizAdyacencia[u];
    for (int f = 0; f < numUsuarios; f++) {
        if (filaU[f]) {
            const bool* filaF = matrizAdyacencia[f];
            for (int k = 0; k < numUsuarios; k++) {
                if (filaF[k]) {
                    conteos[k] = conteos[k] + 1;
                }
            }
            monitorGlobal.registrarIteraciones(numUsuarios);
        }
    }
    monitorGlobal.registrarIteraciones(numUsuarios);

    conteos[u] = 0;
}

int GrafoAmistad::obtenerCapacidad() const {
    return capacidad;
}

int GrafoAmistad::obtenerNumUsuarios() const {
    return numUsuarios;
}
