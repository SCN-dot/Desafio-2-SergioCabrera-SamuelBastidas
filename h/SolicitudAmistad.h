#ifndef SOLICITUDAMISTAD_H
#define SOLICITUDAMISTAD_H

#include "MonitorRecursos.h"

class SolicitudAmistad {
private:
    int idSolicitud;
    int idEmisor;
    int idReceptor;
    int estado;             // 0: Pendiente, 1: Aceptada, 2: Rechazada

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

#endif // SOLICITUDAMISTAD_H
