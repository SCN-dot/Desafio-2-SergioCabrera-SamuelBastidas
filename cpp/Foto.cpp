#include "Foto.h"
#include "Cadenas.h"

using namespace std;

Foto::Foto(int id, const char* ruta, size_t tam, long fecha, int likes) {
    idFoto = id;
    tamanoBytes = tam;
    fechaCaptura = fecha;
    contadorLikes = (likes < 0) ? 0 : likes;    // Defensa minima: nunca negativos
    likeActivo = false;
    rutaArchivo = Cadenas::duplicar(ruta != nullptr ? ruta : "");
}

// Copia profunda: la ruta se duplica en memoria propia
Foto::Foto(const Foto& otra) {
    idFoto = otra.idFoto;
    tamanoBytes = otra.tamanoBytes;
    fechaCaptura = otra.fechaCaptura;
    contadorLikes = otra.contadorLikes;
    likeActivo = otra.likeActivo;
    rutaArchivo = Cadenas::duplicar(otra.rutaArchivo);
}

Foto& Foto::operator=(const Foto& otra) {
    if (this != &otra) {
        // Primero se duplica la nueva ruta y luego se libera la anterior
        char* nuevaRuta = Cadenas::duplicar(otra.rutaArchivo);
        Cadenas::liberar(rutaArchivo);
        rutaArchivo = nuevaRuta;

        idFoto = otra.idFoto;
        tamanoBytes = otra.tamanoBytes;
        fechaCaptura = otra.fechaCaptura;
        contadorLikes = otra.contadorLikes;
        likeActivo = otra.likeActivo;
    }
    return *this;
}

Foto::~Foto() {
    Cadenas::liberar(rutaArchivo);
}

// Alterna: si el usuario de la sesion no habia dado like lo suma; si ya lo habia dado lo quita
void Foto::darQuitarLike() {
    if (!likeActivo) {
        contadorLikes = contadorLikes + 1;
        likeActivo = true;
    } else {
        if (contadorLikes > 0) {
            contadorLikes = contadorLikes - 1;
        }
        likeActivo = false;
    }
}

// Al cerrar sesion el siguiente usuario empieza sin like activo (el conteo no cambia)
void Foto::reiniciarLike() {
    likeActivo = false;
}

int Foto::obtenerId() const {
    return idFoto;
}

const char* Foto::obtenerRuta() const {
    return rutaArchivo;
}

size_t Foto::obtenerTamano() const {
    return tamanoBytes;
}

long Foto::obtenerFecha() const {
    return fechaCaptura;
}

int Foto::obtenerLikes() const {
    return contadorLikes;
}

void Foto::establecerLikes(int likes) {
    contadorLikes = (likes < 0) ? 0 : likes;
}
