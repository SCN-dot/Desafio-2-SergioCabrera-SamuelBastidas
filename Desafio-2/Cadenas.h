#ifndef CADENAS_H
#define CADENAS_H

#include "MonitorRecursos.h"

class Cadenas {
private:
    Cadenas();

public:
    static int longitud(const char* texto);

    static char* duplicar(const char* origen);

    static void liberar(char*& texto);

    static int comparar(const char* a, const char* b);
    static bool iguales(const char* a, const char* b);
    static bool estaVacia(const char* texto);

    static int indiceDe(const char* texto, char caracter, int desde);

    static char* subcadena(const char* texto, int inicio, int fin);

    static char* extraerCampo(const char* linea, int& posicion, char separador, bool hastaElFinal = false);

    static bool aEntero(const char* texto, long& resultado);
};

#endif // CADENAS_H

