#include "Usuario.h"
#include "Cadenas.h"

using namespace std;

static const char* textoSeguro(const char* texto) {
    if (texto != nullptr) {
        return texto;
    }
    return "";
}

static void insertarOrdenado(Lista<Mensaje*>& lista, Mensaje* m) {
    int indice = 0;
    for (Nodo<Mensaje*>* n = lista.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        if (*m < *(n->obtenerDato())) {
            break;
        }
        indice = indice + 1;
    }
    lista.insertar(m, indice);
}

void Usuario::copiarDatosPersonales(const Usuario& otro) {
    id = otro.id;
    username = Cadenas::duplicar(otro.username);
    clave = Cadenas::duplicar(otro.clave);
    email = Cadenas::duplicar(otro.email);
    fechaNacimiento = Cadenas::duplicar(otro.fechaNacimiento);
    ciudad = Cadenas::duplicar(otro.ciudad);
    pais = Cadenas::duplicar(otro.pais);
    fechaInscripcion = Cadenas::duplicar(otro.fechaInscripcion);
    rutaFotoPerfil = Cadenas::duplicar(otro.rutaFotoPerfil);
}

void Usuario::liberarDatosPersonales() {
    Cadenas::liberar(username);
    Cadenas::liberar(clave);
    Cadenas::liberar(email);
    Cadenas::liberar(fechaNacimiento);
    Cadenas::liberar(ciudad);
    Cadenas::liberar(pais);
    Cadenas::liberar(fechaInscripcion);
    Cadenas::liberar(rutaFotoPerfil);
}

Usuario::Usuario(int idUsuario, const char* user, const char* pass, const char* correo, const char* nacimiento, const char* ciu, const char* pa, const char* inscripcion, const char* rutaFoto) {
    id = idUsuario;
    username = Cadenas::duplicar(textoSeguro(user));
    clave = Cadenas::duplicar(textoSeguro(pass));
    email = Cadenas::duplicar(textoSeguro(correo));
    fechaNacimiento = Cadenas::duplicar(textoSeguro(nacimiento));
    ciudad = Cadenas::duplicar(textoSeguro(ciu));
    pais = Cadenas::duplicar(textoSeguro(pa));
    fechaInscripcion = Cadenas::duplicar(textoSeguro(inscripcion));
    rutaFotoPerfil = Cadenas::duplicar(textoSeguro(rutaFoto));
    fotosAlternadas = false;
}

Usuario::Usuario(const Usuario& otro) {
    copiarDatosPersonales(otro);
    fotosAlternadas = false;
}

Usuario& Usuario::operator=(const Usuario& otro) {
    if (this != &otro) {
        liberarDatosPersonales();
        copiarDatosPersonales(otro);
    }
    return *this;
}

Usuario::~Usuario() {
    liberarDatosPersonales();

    for (Nodo<Album*>* n = albumes.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        delete n->obtenerDato();
        monitorGlobal.registrarLiberacion(sizeof(Album));
    }

    for (Nodo<Mensaje*>* n = mensajesEnviados.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        delete n->obtenerDato();
        monitorGlobal.registrarLiberacion(sizeof(Mensaje));
    }

    for (Nodo<SolicitudAmistad*>* n = solicitudes.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        delete n->obtenerDato();
        monitorGlobal.registrarLiberacion(sizeof(SolicitudAmistad));
    }
}

bool Usuario::operator==(const Usuario& otro) const {
    monitorGlobal.registrarIteraciones(1);
    if (id != otro.id) {
        return false;
    }
    return Cadenas::iguales(username, otro.username);
}

int Usuario::obtenerId() const {
    return id;
}

const char* Usuario::obtenerUsername() const {
    return username;
}

const char* Usuario::obtenerEmail() const {
    return email;
}

const char* Usuario::obtenerFechaNacimiento() const {
    return fechaNacimiento;
}

const char* Usuario::obtenerCiudad() const {
    return ciudad;
}

const char* Usuario::obtenerPais() const {
    return pais;
}

const char* Usuario::obtenerFechaInscripcion() const {
    return fechaInscripcion;
}

const char* Usuario::obtenerRutaFotoPerfil() const {
    return rutaFotoPerfil;
}

bool Usuario::validarClave(const char* pass) const {
    if (pass == nullptr) {
        return false;
    }
    return Cadenas::iguales(clave, pass);
}

void Usuario::agregarAmigo(Usuario* amigo) {
    if (amigo == nullptr || *amigo == *this) {
        return;
    }

    for (Nodo<Usuario*>* n = amigos.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        if (*(n->obtenerDato()) == *amigo) {
            return;
        }
    }
    amigos.insertar(amigo);
}

int Usuario::obtenerCantidadAmigos() const {
    return amigos.obtenerTamano();
}

const Lista<Usuario*>& Usuario::obtenerAmigos() const {
    return amigos;
}

void Usuario::agregarAlbum(Album* a) {
    if (a != nullptr) {
        albumes.insertar(a);
    }
}

const Lista<Album*>& Usuario::obtenerAlbumes() const {
    return albumes;
}

Mensaje* Usuario::enviarMensaje(Usuario* dest, int idMensaje, const char* txt, long fecha) {
    if (dest == nullptr || dest == this || Cadenas::estaVacia(txt)) {
        return nullptr;
    }
    if (Cadenas::longitud(txt) > Mensaje::LONGITUD_MAXIMA) {
        return nullptr;
    }
    Mensaje* nuevo = new Mensaje(idMensaje, id, dest->id, fecha, false, txt);
    monitorGlobal.registrarAsignacion(sizeof(Mensaje));
    insertarOrdenado(mensajesEnviados, nuevo);
    dest->recibirMensaje(nuevo);
    return nuevo;
}

void Usuario::agregarMensajeEnviado(Mensaje* m) {
    if (m != nullptr) {
        insertarOrdenado(mensajesEnviados, m);
    }
}

void Usuario::recibirMensaje(Mensaje* m) {
    if (m != nullptr) {
        insertarOrdenado(mensajesRecibidos, m);
    }
}

void Usuario::obtenerMensajesNoLeidos(Lista<Mensaje*>& resultado) const {
    for (Nodo<Mensaje*>* n = mensajesRecibidos.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        if (!n->obtenerDato()->estaLeido()) {
            resultado.insertar(n->obtenerDato());
        }
    }
}

const Lista<Mensaje*>& Usuario::obtenerMensajesRecibidos() const {
    return mensajesRecibidos;
}

void Usuario::buscarMensajesPorRango(long inicio, long fin, Lista<Mensaje*>& resultado) const {
    if (inicio > fin) {
        return;
    }
    for (Nodo<Mensaje*>* n = mensajesRecibidos.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        long fecha = n->obtenerDato()->obtenerTimestamp();

        if (fecha > fin) {
            break;
        }
        if (fecha >= inicio) {
            resultado.insertar(n->obtenerDato());
        }
    }
}

void Usuario::recibirSolicitud(SolicitudAmistad* s) {
    if (s != nullptr) {
        solicitudes.insertar(s);
    }
}

SolicitudAmistad* Usuario::obtenerSolicitudPorId(int idSol) const {
    for (Nodo<SolicitudAmistad*>* n = solicitudes.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        if (n->obtenerDato()->obtenerId() == idSol) {
            return n->obtenerDato();
        }
    }
    return nullptr;
}

bool Usuario::tieneSolicitudPendienteDe(int idEmisor) const {
    for (Nodo<SolicitudAmistad*>* n = solicitudes.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        SolicitudAmistad* s = n->obtenerDato();
        if (s->obtenerIdEmisor() == idEmisor && s->obtenerEstado() == SolicitudAmistad::PENDIENTE) {
            return true;
        }
    }
    return false;
}

bool Usuario::procesarSolicitud(int idSol, bool aceptar) {
    SolicitudAmistad* s = obtenerSolicitudPorId(idSol);
    if (s == nullptr || s->obtenerEstado() != SolicitudAmistad::PENDIENTE) {
        return false;
    }
    if (aceptar) {
        s->aceptar();
    } else {
        s->rechazar();
    }
    return true;
}

const Lista<SolicitudAmistad*>& Usuario::obtenerSolicitudes() const {
    return solicitudes;
}

bool Usuario::puedeAlternarFotos() const {
    return !fotosAlternadas;
}

void Usuario::marcarFotosAlternadas() {
    fotosAlternadas = true;
}

void Usuario::reiniciarFotosAlternadas() {
    fotosAlternadas = false;
}