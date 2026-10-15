#ifndef FOTO_H
#define FOTO_H

#include "MonitorRecursos.h"

class Foto {
private:
    int idFoto;
    char* rutaArchivo;      // Ruta absoluta en notacion Linux (memoria dinamica propia)
    size_t tamanoBytes;
    long fechaCaptura;      // AAAAMMDD
    int contadorLikes;
    bool likeActivo;        // Solo de la sesion (no se guarda): permite alternar dar/quitar like

public:
    Foto(int id, const char* ruta, size_t tam, long fecha, int likes);
    Foto(const Foto& otra);                 // Copia profunda
    Foto& operator=(const Foto& otra);      // Asignacion con copia profunda
    ~Foto();

    void darQuitarLike();
    void reiniciarLike();                   // Al cerrar sesion: la marca de la sesion vuelve a false (no cambia el conteo)

    int obtenerId() const;
    const char* obtenerRuta() const;
    size_t obtenerTamano() const;
    long obtenerFecha() const;
    int obtenerLikes() const;

    void establecerLikes(int likes);
};

#endif // FOTO_H
