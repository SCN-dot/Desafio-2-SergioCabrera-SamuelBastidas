#include "Foto.h"
#include "Cadenas.h"

using namespace std;

Foto::Foto(const char* ruta, size_t tam, long fecha) {
    idFoto = 0;
    tamanoBytes = tam;
    fechaCaptura = fecha;
    contadorLikes = 0;
    likeActivo = false;

    if (ruta != nullptr) {
        rutaArchivo = Cadenas::duplicar(ruta);
    } else {
        rutaArchivo = Cadenas::duplicar("");
    }
}

Foto::Foto(int id, const char* ruta, size_t tam, long fecha, int likes) {
    idFoto = id;
    tamanoBytes = tam;
    fechaCaptura = fecha;

    if (likes < 0) {
        contadorLikes = 0;
    } else {
        contadorLikes = likes;
    }

    likeActivo = false;

    if (ruta != nullptr) {
        rutaArchivo = Cadenas::duplicar(ruta);
    } else {
        rutaArchivo = Cadenas::duplicar("");
    }
}

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

void Foto::reiniciarLike() {
    likeActivo = false;
}

int Foto::obtenerId() const {
    return idFoto;
}

const char* Foto::obtenerRuta() const {
    return rutaArchivo;
}

char* Foto::obtenerRuta() {
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

ostream& operator<<(ostream& salida, const Foto& f) {
    salida << f.rutaArchivo << "  (likes: " << f.contadorLikes << ")";
    return salida;
}
