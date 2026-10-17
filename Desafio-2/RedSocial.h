#ifndef REDSOCIAL_H
#define REDSOCIAL_H

#include "Lista.h"
#include "Usuario.h"
#include "GrafoAmistad.h"
#include "MonitorRecursos.h"
#include <random>

class RedSocial {
public:
    static const int SOLICITUD_OK = 0;
    static const int SOLICITUD_NO_EXISTE = 1;
    static const int SOLICITUD_A_SI_MISMO = 2;
    static const int SOLICITUD_YA_AMIGOS = 3;
    static const int SOLICITUD_YA_ENVIADA = 4;
    static const int SOLICITUD_YA_RECIBIDA = 5;
    static const int SOLICITUD_ERROR = 6;

    static const int MENSAJE_OK = 0;
    static const int MENSAJE_VACIO = 1;
    static const int MENSAJE_LARGO = 2;
    static const int MENSAJE_CON_SALTO = 3;
    static const int MENSAJE_DESTINO_INVALIDO = 4;
    static const int MENSAJE_NO_AMIGO = 5;
    static const int MENSAJE_ERROR = 6;

private:
    Lista<Usuario*> listaUsuarios;
    Usuario** usuariosPorId;
    int totalUsuarios;
    int capacidad;
    GrafoAmistad* relaciones;

    int siguienteIdMensaje;
    int siguienteIdSolicitud;

    int grupoPrevio[3];
    int cantidadGrupoPrevio;

    std::mt19937 generador;

    Lista<Foto*> fotosTocadas;

    bool amistadesPendientes;
    bool albumesPendientes;
    bool solicitudesPendientes;
    bool mensajesPendientes;

    RedSocial(const RedSocial& otra);
    RedSocial& operator=(const RedSocial& otra);

    bool cargarUsuarios();
    bool cargarAmistades();
    bool cargarAlbumes();
    bool cargarSolicitudes();
    bool cargarMensajes();

    bool guardarAmistades();
    bool guardarAlbumes();
    bool guardarSolicitudes();
    bool guardarMensajes();

public:
    RedSocial();
    ~RedSocial();

    bool cargarDatos();
    bool guardarDatos();

    bool registrarUsuario(Usuario* u);
    Usuario* autenticar(const char* nom, const char* clave);
    Usuario* obtenerUsuarioPorUsername(const char* nom) const;
    Usuario* obtenerUsuarioPorId(int idUsuario) const;
    int obtenerTotalUsuarios() const;

    bool sonAmigos(const Usuario* a, const Usuario* b) const;
    int obtenerAmigosEnComun(const Usuario* a, const Usuario* b) const;
    int enviarSolicitud(Usuario* emisor, const char* nombreReceptor);
    bool responderSolicitud(Usuario* receptor, int idSolicitud, bool aceptar);
    void obtenerSugerencias(Usuario* u, int K, Lista<Usuario*>& resultado);

    int obtenerMejoresAmigos(Usuario* u, const char** rutas, bool alternar);

    int enviarMensaje(Usuario* emisor, const char* nombreDestinatario, const char* texto);
    void leerMensajesNuevos(Usuario* u, Lista<Mensaje*>& nuevos);
    void alternarLike(Foto* f);

    void cerrarSesion(Usuario* u);
};

#endif // REDSOCIAL_H
