#include "Mensaje.h"
#include "Cadenas.h"

using namespace std;

Mensaje::Mensaje(int id, int emisor, int receptor, long fecha, bool yaLeido, const char* cont) {
    idMensaje = id;
    idEmisor = emisor;
    idReceptor = receptor;
    timestamp = fecha;
    leido = yaLeido;

    if (cont == nullptr) {
        contenido = Cadenas::duplicar("");
    } else if (Cadenas::longitud(cont) > LONGITUD_MAXIMA) {
        // Defensa minima: el contenido nunca supera el limite de 512 caracteres
        contenido = Cadenas::subcadena(cont, 0, LONGITUD_MAXIMA);
    } else {
        contenido = Cadenas::duplicar(cont);
    }
}

// Copia profunda: el contenido se duplica en memoria propia
Mensaje::Mensaje(const Mensaje& otro) {
    idMensaje = otro.idMensaje;
    idEmisor = otro.idEmisor;
    idReceptor = otro.idReceptor;
    timestamp = otro.timestamp;
    leido = otro.leido;
    contenido = Cadenas::duplicar(otro.contenido);
}

Mensaje& Mensaje::operator=(const Mensaje& otro) {
    if (this != &otro) {
        char* nuevoContenido = Cadenas::duplicar(otro.contenido);
        Cadenas::liberar(contenido);
        contenido = nuevoContenido;

        idMensaje = otro.idMensaje;
        idEmisor = otro.idEmisor;
        idReceptor = otro.idReceptor;
        timestamp = otro.timestamp;
        leido = otro.leido;
    }
    return *this;
}

Mensaje::~Mensaje() {
    Cadenas::liberar(contenido);
}

void Mensaje::marcarLeido() {
    leido = true;
}

bool Mensaje::estaLeido() const {
    return leido;
}

int Mensaje::obtenerId() const {
    return idMensaje;
}

int Mensaje::obtenerIdEmisor() const {
    return idEmisor;
}

int Mensaje::obtenerIdReceptor() const {
    return idReceptor;
}

long Mensaje::obtenerTimestamp() const {
    return timestamp;
}

const char* Mensaje::obtenerContenido() const {
    return contenido;
}
