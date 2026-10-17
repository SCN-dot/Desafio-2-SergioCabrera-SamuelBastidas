#ifndef SOLICITUDAMISTAD_H
#define SOLICITUDAMISTAD_H

#include "MonitorRecursos.h"

class SolicitudAmistad {
public:
    static const int PENDIENTE = 0;
    static const int ACEPTADA = 1;
    static const int RECHAZADA = 2;

private:
    int idSolicitud;
    int idEmisor;
    int idReceptor;
    int estado;

public:
    SolicitudAmistad(int id, int emisor, int receptor, int estadoInicial);
    SolicitudAmistad(const SolicitudAmistad& otra);
    SolicitudAmistad& operator=(const SolicitudAmistad& otra);
    ~SolicitudAmistad();

    void aceptar();
    void rechazar();

    int obtenerId() const;
    int obtenerIdEmisor() const;
    int obtenerIdReceptor() const;
    int obtenerEstado() const;
};

#endif
