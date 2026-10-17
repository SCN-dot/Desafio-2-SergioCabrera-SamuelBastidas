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

Usuario::Usuario(int idUsuario, const char* user, const char* pass, const char* correo,
                 const char* nacimiento, const char* ciu, const char* pa,
                 const char* inscripcion, const char* rutaFoto) {
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

                 const char* Usuario::obt
