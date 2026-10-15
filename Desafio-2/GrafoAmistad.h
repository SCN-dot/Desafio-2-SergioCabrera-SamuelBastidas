#ifndef GRAFOAMISTAD_H
#define GRAFOAMISTAD_H

#include "MonitorRecursos.h"

class GrafoAmistad {
private:
    bool** matrizAdyacencia;
    int capacidad;
    int numUsuarios;

    GrafoAmistad(const GrafoAmistad& otro);
    GrafoAmistad& operator=(const GrafoAmistad& otro);

public:
    GrafoAmistad(int cap);
    ~GrafoAmistad();

    void incrementarUsuarios();
    void agregarAmistad(int u, int v);
    bool sonAmigos(int u, int v) const;
    int obtenerAmigosEnComun(int u, int v) const;

    void contarAmigosEnComunConTodos(int u, int* conteos) const;

    int obtenerCapacidad() const;
    int obtenerNumUsuarios() const;
};

#endif // GRAFOAMISTAD_H

