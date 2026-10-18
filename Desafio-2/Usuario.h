#ifndef USUARIO_H
#define USUARIO_H

#include "Lista.h"
#include "Album.h"
#include "Mensaje.h"
#include "SolicitudAmistad.h"
#include "MonitorRecursos.h"

class Usuario {
private:
    int id;
    char* username;
    char* clave;
    char* email;
    char* fechaNacimiento;
    char* ciudad;
    char* pais;
    char* fechaInscripcion;
    char* rutaFotoPerfil;
    bool fotosAlternadas;

    Lista<Usuario*> amigos;
    Lista<Album*> albumes;
    Lista<Mensaje*> mensajesEnviados;
    Lista<Mensaje*> mensajesRecibidos;
    Lista<SolicitudAmistad*> solicitudes;

    void copiarDatosPersonales(const Usuario& otro);
    void liberarDatosPersonales();

public:
    Usuario(int idUsuario, const char* user, const char* pass, const char* correo,
            const char* nacimiento, const char* ciu, const char* pa,
            const char* inscripcion, const char* rutaFoto);

    Usuario(const Usuario& otro);
    Usuario& operator=(const Usuario& otro);
    ~Usuario();

    bool operator==(const Usuario& otro) const;

    int obtenerId() const;
    const char* obtenerUsername() const;
    const char* obtenerEmail() const;
    const char* obtenerFechaNacimiento() const;
    const char* obtenerCiudad() const;
    const char* obtenerPais() const;
    const char* obtenerFechaInscripcion() const;
    const char* obtenerRutaFotoPerfil() const;
    bool validarClave(const char* pass) const;

    void agregarAmigo(Usuario* amigo);
    int obtenerCantidadAmigos() const;
    const Lista<Usuario*>& obtenerAmigos() const;

    void agregarAlbum(Album* a);
    const Lista<Album*>& obtenerAlbumes() const;

    Mensaje* enviarMensaje(Usuario* dest, int idMensaje, const char* txt, long fecha);
    void agregarMensajeEnviado(Mensaje* m);
    void recibirMensaje(Mensaje* m);
    void obtenerMensajesNoLeidos(Lista<Mensaje*>& resultado) const;
    void buscarMensajesPorRango(long inicio, long fin, Lista<Mensaje*>& resultado) const;
    const Lista<Mensaje*>& obtenerMensajesRecibidos() const;

    void recibirSolicitud(SolicitudAmistad* s);
    SolicitudAmistad* obtenerSolicitudPorId(int idSol) const;
    bool tieneSolicitudPendienteDe(int idEmisor) const;
    bool procesarSolicitud(int idSol, bool aceptar);
    const Lista<SolicitudAmistad*>& obtenerSolicitudes() const;

    bool puedeAlternarFotos() const;
    void marcarFotosAlternadas();
    void reiniciarFotosAlternadas();
};

#endif