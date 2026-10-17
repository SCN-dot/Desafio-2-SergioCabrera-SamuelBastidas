#include "Album.h"
#include "Cadenas.h"

using namespace std;

static void copiarFotos(const Lista<Foto*>& origen, Lista<Foto*>& destino) {
    for (Nodo<Foto*>* n = origen.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        Foto* original = n->obtenerDato();
        if (original != nullptr) {
            Foto* copia = new Foto(*original);
            monitorGlobal.registrarAsignacion(sizeof(Foto));
            destino.insertar(copia);
        }
    }
}



Album::Album(int id, int idProp, const char* nom, int priv, const char* rutaAlm) {
    idAlbum = id;
    idPropietario = idProp;

    if (priv >= 1 && priv <= 3) {
        privacidad = priv;
    } else {
        privacidad = 3;
    }

    if (nom != nullptr) {
        nombre = Cadenas::duplicar(nom);
    } else {
        nombre = Cadenas::duplicar("");
    }

    if (rutaAlm != nullptr) {
        rutaAlmacenamiento = Cadenas::duplicar(rutaAlm);
    } else {
        rutaAlmacenamiento = Cadenas::duplicar("");
    }
}

Album::Album(const Album& otro) {
    idAlbum = otro.idAlbum;
    idPropietario = otro.idPropietario;
    privacidad = otro.privacidad;
    nombre = Cadenas::duplicar(otro.nombre);
    rutaAlmacenamiento = Cadenas::duplicar(otro.rutaAlmacenamiento);
    copiarFotos(otro.fotos, fotos);
}

Album& Album::operator=(const Album& otro) {
    if (this != &otro) {
        char* nuevoNombre = Cadenas::duplicar(otro.nombre);
        char* nuevaRuta = Cadenas::duplicar(otro.rutaAlmacenamiento);
        Cadenas::liberar(nombre);
        Cadenas::liberar(rutaAlmacenamiento);
        nombre = nuevoNombre;
        rutaAlmacenamiento = nuevaRuta;

        idAlbum = otro.idAlbum;
        idPropietario = otro.idPropietario;
        privacidad = otro.privacidad;

        liberarFotos();
        copiarFotos(otro.fotos, fotos);
    }
    return *this;
}

Album::~Album() {
    liberarFotos();
    Cadenas::liberar(nombre);
    Cadenas::liberar(rutaAlmacenamiento);
}

void Album::liberarFotos() {
    for (Nodo<Foto*>* n = fotos.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        Foto* f = n->obtenerDato();
        if (f != nullptr) {
            delete f;
            monitorGlobal.registrarLiberacion(sizeof(Foto));
        }
    }
    fotos.vaciar();
}

void Album::agregarFoto(Foto* f) {
    if (f != nullptr) {
        fotos.insertar(f);
    }
}

Foto* Album::obtenerFoto(int indice) const {
    if (indice < 0 || indice >= fotos.obtenerTamano()) {
        return nullptr;
    }
    return fotos[indice];
}

const Lista<Foto*>& Album::obtenerFotos() const {
    return fotos;
}

int Album::obtenerCantidadFotos() const {
    return fotos.obtenerTamano();
}

int Album::obtenerId() const {
    return idAlbum;
}

int Album::obtenerIdPropietario() const {
    return idPropietario;
}

const char* Album::obtenerNombre() const {
    return nombre;
}

const char* Album::obtenerRutaAlmacenamiento() const {
    return rutaAlmacenamiento;
}

int Album::obtenerPrivacidad() const {
    return privacidad;
}

