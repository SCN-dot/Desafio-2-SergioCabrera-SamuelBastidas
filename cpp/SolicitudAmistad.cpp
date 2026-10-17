#include "SolicitudAmistad.h"

SolicitudAmistad::SolicitudAmistad(int id, int emisorId, int receptorId, int estadoInicial) {
    idSolicitud = id;
    idEmisor = emisorId;
    idReceptor = receptorId;
    if (estadoInicial >= PENDIENTE && estadoInicial <= RECHAZADA) {
        estado = estadoInicial;
    } else {
        estado = PENDIENTE;
    }
}

SolicitudAmistad::SolicitudAmistad(const SolicitudAmistad& otra) {
    idSolicitud = otra.idSolicitud;
    idEmisor = otra.idEmisor;
    idReceptor = otra.idReceptor;
    estado = otra.estado;
}

SolicitudAmistad& SolicitudAmistad::operator=(const SolicitudAmistad& otra) {
    if (this != &otra) {
        idSolicitud = otra.idSolicitud;
        idEmisor = otra.idEmisor;
        idReceptor = otra.idReceptor;
        estado = otra.estado;
    }
    return *this;
}

SolicitudAmistad::~SolicitudAmistad() {
}

void SolicitudAmistad::aceptar() {
    if (estado == PENDIENTE) {
        estado = ACEPTADA;
    }
}

void SolicitudAmistad::rechazar() {
    if (estado == PENDIENTE) {
        estado = RECHAZADA;
    }
}

int SolicitudAmistad::obtenerId() const {
    return idSolicitud;
}

int SolicitudAmistad::obtenerIdEmisor() const {
    return idEmisor;
}

int SolicitudAmistad::obtenerIdReceptor() const {
    return idReceptor;
}

int SolicitudAmistad::obtenerEstado() const {
    return estado;
}
