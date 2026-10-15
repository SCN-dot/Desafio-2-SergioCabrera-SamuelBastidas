#include "SolicitudAmistad.h"

using namespace std;

SolicitudAmistad::SolicitudAmistad(int id, int emisor, int receptor, int estadoInicial) {
    idSolicitud = id;
    idEmisor = emisor;
    idReceptor = receptor;
    // Defensa minima: un estado fuera de 0..2 se trata como pendiente
    estado = (estadoInicial >= 0 && estadoInicial <= 2) ? estadoInicial : 0;
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
    // No posee memoria dinamica propia
}

// Solo una solicitud pendiente puede cambiar de estado; una ya resuelta no se altera
void SolicitudAmistad::aceptar() {
    if (estado == 0) {
        estado = 1;
    }
}

void SolicitudAmistad::rechazar() {
    if (estado == 0) {
        estado = 2;
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
