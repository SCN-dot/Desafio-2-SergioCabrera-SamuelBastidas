#ifndef MENSAJE_H
#define MENSAJE_H

#include "MonitorRecursos.h"

class Mensaje {
public:
    static const int LONGITUD_MAXIMA = 512;     // Caracteres maximos del contenido

private:
    int idMensaje;
    int idEmisor;           // Se guardan ids (no nombres) para no duplicar cadenas por mensaje
    int idReceptor;
    char* contenido;
    long timestamp;         // Fecha de envio AAAAMMDD
    bool leido;

public:
    Mensaje(int id, int emisor, int receptor, long fecha, bool yaLeido, const char* cont);
    Mensaje(const Mensaje& otro);               // Copia profunda
    Mensaje& operator=(const Mensaje& otro);
    ~Mensaje();

    void marcarLeido();
    bool estaLeido() const;

    int obtenerId() const;
    int obtenerIdEmisor() const;
    int obtenerIdReceptor() const;
    long obtenerTimestamp() const;
    const char* obtenerContenido() const;
};

#endif // MENSAJE_H
