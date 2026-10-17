#ifndef ALBUM_H
#define ALBUM_H

#include "Lista.h"
#include "Foto.h"
#include "MonitorRecursos.h"

class Album {
private:
    int idAlbum;
    int idPropietario;
    char* nombre;
    char* rutaAlmacenamiento;
    int privacidad;
    Lista<Foto*> fotos

    void liberarFotos();


public:
    Album(int id, int idProp, const char* nom, int priv, const char* rutaAlm);
    Album(const Album& otro);
    Album& operator=(const Album& otro);
    ~Album();

    void agregarFoto(Foto* f);
    Foto* obtenerFoto(int indice) const;
    const Lista<Foto*>& obtenerFotos() const;
    int obtenerCantidadFotos() const;

    int obtenerId() const;
    int obtenerIdPropietario() const;
    const char* obtenerNombre() const;
    const char* obtenerRutaAlmacenamiento() const;
    int obtenerPrivacidad() const;
};

#endif // ALBUM_H
