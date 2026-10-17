#ifndef MENSAJE_H
#define MENSAJE_H

#include "MonitorRecursos.h"

class Mensaje {
public:
    static const int LONGITUD_MAXIMA = 512;

private:
    int idMensaje;
    int idEmisor;
    int idReceptor;
    char* contenido;
    long timestamp;
    bool leido;

public:
    Mensaje(int id, int emisor, int receptor, long fecha, bool yaLeido, const char* cont);
    Mensaje(const Mensaje& otro);
    Mensaje& operator=(const Mensaje& otro);
    ~Mensaje();

    void marcarLeido();
    bool estaLeido() const;

    int obtenerId() const;
    int obtenerIdEmisor() const;
    int obtenerIdReceptor() const;
    long obtenerTimestamp() const;
    const char* obtenerContenido() const;

    bool operator<(const Mensaje& otro) const;
    friend std::ostream& operator<<(std::ostream& salida, const Mensaje& m);
};

#endif
