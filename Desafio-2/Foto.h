#ifndef FOTO_H
#define FOTO_H

#include "MonitorRecursos.h"

class Foto {
private:
    int idFoto;
    char* rutaArchivo;
    size_t tamanoBytes;
    long fechaCaptura;
    int contadorLikes;
    bool likeActivo;

public:
    Foto(const char* ruta, size_t tam, long fecha);
    Foto(int id, const char* ruta, size_t tam, long fecha, int likes = 0);
    Foto(const Foto& otra);
    Foto& operator=(const Foto& otra);
    ~Foto();

    void darQuitarLike();
    void reiniciarLike();

    int obtenerId() const;
    const char* obtenerRuta() const;
    char* obtenerRuta();
    size_t obtenerTamano() const;
    long obtenerFecha() const;
    int obtenerLikes() const;

    friend std::ostream& operator<<(std::ostream& salida, const Foto& f);
};

#endif
