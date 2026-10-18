#include <iostream>
#include "RedSocial.h"
#include "Usuario.h"
#include "Album.h"
#include "Foto.h"
#include "Mensaje.h"
#include "SolicitudAmistad.h"
#include "Cadenas.h"
#include "Fecha.h"
#include "MonitorRecursos.h"

using namespace std;

static const int LINEA_MAX = 1024;
static const long OPCION_INVALIDA = -9999;
static bool entradaCerrada = false;

static bool leerLinea(const char* prompt, char* buf, int n) {
    cout << prompt;
    cin.getline(buf, n);
    if (cin.eof() && cin.gcount() == 0) {
        entradaCerrada = true;
        buf[0] = '\0';
        return false;
    }
    if (cin.fail()) {
        if (cin.eof()) {
            entradaCerrada = true;
            return false;
        }
        cin.clear();
        cin.ignore(100000, '\n');
    }
    int len = Cadenas::longitud(buf);
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return true;
}

static long leerNumero(const char* prompt) {
    char buf[64];
    if (!leerLinea(prompt, buf, 64)) {
        return OPCION_INVALIDA;
    }
    long v = 0;
    if (!Cadenas::aEntero(buf, v)) {
        return OPCION_INVALIDA;
    }
    return v;
}

static void mostrarMetricas(size_t bytesStack) {
    monitorGlobal.auditarEstado(bytesStack + sizeof(RedSocial));
    monitorGlobal.reiniciarIteraciones();
}

static const char* nombrePrivacidad(int p) {
    if (p == 1) return "Publico";
    if (p == 2) return "Amigos";
    return "Privado";
}

static void imprimirMensaje(RedSocial& red, const Mensaje* m) {
    Usuario* emisor = red.obtenerUsuarioPorId(m->obtenerIdEmisor());
    cout << "  De: " << (emisor != nullptr ? emisor->obtenerUsername() : "(desconocido)")
         << "  " << *m << endl;
}

static void mostrarMejoresAmigos(RedSocial& red, Usuario* u, bool alternar) {
    monitorGlobal.reiniciarIteraciones();

    if (alternar && !u->puedeAlternarFotos()) {
        cout << "Las fotos de mejores amigos solo se pueden actualizar una vez por sesion." << endl;
        return;
    }

    const char* rutas[3] = {nullptr, nullptr, nullptr};
    int n = red.obtenerMejoresAmigos(u, rutas, alternar);

    if (n == 0) {
        if (alternar) {
            cout << "No hay otros amigos distintos para mostrar." << endl;
        } else {
            cout << "Aun no tienes amigos para mostrar." << endl;
        }
    } else {
        cout << "\nTus mejores amigos" << endl;
        for (int i = 0; i < n; i++) {
            cout << "  " << (i + 1) << ". " << rutas[i] << endl;
        }
    }
    size_t stack = sizeof(rutas) + sizeof(n) + sizeof(alternar) + sizeof(u) + sizeof(&red);
    mostrarMetricas(stack);
}

static void informarSolicitud(int codigo, const char* nombre) {
    if (codigo == RedSocial::SOLICITUD_OK) {
        cout << "Solicitud enviada a " << nombre << "." << endl;
    } else if (codigo == RedSocial::SOLICITUD_NO_EXISTE) {
        cout << "No existe un usuario con ese nombre." << endl;
    } else if (codigo == RedSocial::SOLICITUD_A_SI_MISMO) {
        cout << "No puedes enviarte una solicitud a ti mismo." << endl;
    } else if (codigo == RedSocial::SOLICITUD_YA_AMIGOS) {
        cout << "Ya son amigos." << endl;
    } else if (codigo == RedSocial::SOLICITUD_YA_ENVIADA) {
        cout << "Ya enviaste una solicitud pendiente a este usuario." << endl;
    } else if (codigo == RedSocial::SOLICITUD_YA_RECIBIDA) {
        cout << "Ese usuario ya te envio una solicitud: aceptala en 'Ver solicitudes recibidas'." << endl;
    } else {
        cout << "No se pudo enviar la solicitud." << endl;
    }
}

static void informarMensaje(int codigo, const char* nombre) {
    if (codigo == RedSocial::MENSAJE_OK) {
        cout << "Mensaje enviado a " << nombre << "." << endl;
    } else if (codigo == RedSocial::MENSAJE_VACIO) {
        cout << "No se pueden enviar mensajes vacios." << endl;
    } else if (codigo == RedSocial::MENSAJE_LARGO) {
        cout << "El mensaje supera los " << Mensaje::LONGITUD_MAXIMA << " caracteres." << endl;
    } else if (codigo == RedSocial::MENSAJE_CON_SALTO) {
        cout << "El mensaje no puede contener saltos de linea." << endl;
    } else if (codigo == RedSocial::MENSAJE_NO_AMIGO) {
        cout << "Solo puedes enviar mensajes a tus amigos." << endl;
    } else if (codigo == RedSocial::MENSAJE_DESTINO_INVALIDO) {
        cout << "Ese usuario no existe." << endl;
    } else {
        cout << "No se pudo enviar el mensaje." << endl;
    }
}

static void enviarSolicitud(RedSocial& red, Usuario* u) {
    char nombre[LINEA_MAX];
    if (!leerLinea("Nombre de usuario destino: ", nombre, LINEA_MAX)) return;

    monitorGlobal.reiniciarIteraciones();
    if (Cadenas::estaVacia(nombre)) {
        cout << "El nombre no puede estar vacio." << endl;
    } else {
        informarSolicitud(red.enviarSolicitud(u, nombre), nombre);
    }
    mostrarMetricas(sizeof(nombre) + sizeof(u) + sizeof(&red));
}

static void verSolicitudes(RedSocial& red, Usuario* u) {
    monitorGlobal.reiniciarIteraciones();
    int pendientes = 0;
    cout << "\nSolicitudes recibidas pendientes" << endl;
    for (Nodo<SolicitudAmistad*>* n = u->obtenerSolicitudes().obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        const SolicitudAmistad* s = n->obtenerDato();
        if (s->obtenerEstado() != SolicitudAmistad::PENDIENTE) {
            continue;
        }
        Usuario* emisor = red.obtenerUsuarioPorId(s->obtenerIdEmisor());
        cout << "  ID " << s->obtenerId() << " - de: "
             << (emisor != nullptr ? emisor->obtenerUsername() : "(desconocido)") << endl;
        pendientes = pendientes + 1;
    }
    if (pendientes == 0) {
        cout << "  No tienes solicitudes pendientes." << endl;
        mostrarMetricas(sizeof(pendientes) + sizeof(u) + sizeof(&red));
        return;
    }
    mostrarMetricas(sizeof(pendientes) + sizeof(u) + sizeof(&red));

    long id = leerNumero("ID de la solicitud a responder (-1 para cancelar): ");
    if (id == OPCION_INVALIDA || id < 0) {
        return;
    }
    long accion = leerNumero("1. Aceptar   2. Rechazar   0. Cancelar: ");
    if (accion != 1 && accion != 2) {
        return;
    }

    monitorGlobal.reiniciarIteraciones();
    SolicitudAmistad* s = u->obtenerSolicitudPorId((int)id);
    if (s == nullptr || s->obtenerEstado() != SolicitudAmistad::PENDIENTE) {
        cout << "Esa solicitud no existe o ya fue procesada." << endl;
    } else if (red.responderSolicitud(u, (int)id, accion == 1)) {
        if (accion == 1) {
            cout << "Solicitud aceptada: ahora son amigos." << endl;
        } else {
            cout << "Solicitud rechazada." << endl;
        }
    } else {
        cout << "No se pudo procesar la solicitud." << endl;
    }
    mostrarMetricas(sizeof(id) + sizeof(accion) + sizeof(s) + sizeof(u) + sizeof(&red));
}

static void menuAmistades(RedSocial& red, Usuario* u) {
    while (!entradaCerrada) {
        cout << "\nAdministrar amistades" << endl;
        cout << "1. Enviar solicitud de amistad" << endl;
        cout << "2. Ver solicitudes recibidas (aceptar/rechazar)" << endl;
        cout << "0. Volver" << endl;
        long op = leerNumero("Opcion: ");
        if (op == 1) enviarSolicitud(red, u);
        else if (op == 2) verSolicitudes(red, u);
        else if (op == 0) return;
        else cout << "Opcion invalida." << endl;
    }
}

static void carrusel(RedSocial& red, Album* album) {
    int total = album->obtenerCantidadFotos();
    if (total == 0) {
        cout << "El album no tiene fotos." << endl;
        return;
    }
    monitorGlobal.reiniciarIteraciones();
    int indice = 0;
    while (!entradaCerrada) {
        Foto* f = album->obtenerFoto(indice);
        cout << "\n[" << (indice + 1) << "/" << total << "] " << *f << endl;
        cout << "1. Siguiente  2. Previa  3. Dar/quitar like  0. Salir del album" << endl;
        long op = leerNumero("Opcion: ");
        if (op == 1) {
            indice = (indice + 1) % total;
        } else if (op == 2) {
            indice = (indice - 1 + total) % total;
        } else if (op == 3) {
            red.alternarLike(f);
            cout << "Likes actuales: " << f->obtenerLikes() << endl;
        } else if (op == 0) {
            break;
        } else {
            cout << "Opcion invalida." << endl;
        }
    }
    mostrarMetricas(sizeof(total) + sizeof(indice) + sizeof(album) + sizeof(&red));
}

static void visitarPerfil(RedSocial& red, Usuario* u) {
    char nombre[LINEA_MAX];
    if (!leerLinea("Nombre de usuario del amigo: ", nombre, LINEA_MAX)) return;

    monitorGlobal.reiniciarIteraciones();
    Usuario* amigo = red.obtenerUsuarioPorUsername(nombre);
    if (amigo == nullptr || amigo == u || !red.sonAmigos(u, amigo)) {
        cout << "Ese usuario no existe o no es tu amigo." << endl;
        mostrarMetricas(sizeof(nombre) + sizeof(amigo) + sizeof(u) + sizeof(&red));
        return;
    }

    int visibles = 0;
    cout << "\nAlbumes de " << amigo->obtenerUsername() << endl;
    for (Nodo<Album*>* n = amigo->obtenerAlbumes().obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        const Album* a = n->obtenerDato();
        if (a->obtenerPrivacidad() == 3) {
            continue;
        }
        visibles = visibles + 1;
        cout << "  " << visibles << ". " << a->obtenerNombre() << " [" << nombrePrivacidad(a->obtenerPrivacidad())
             << "] - " << a->obtenerCantidadFotos() << " fotos" << endl;
    }
    mostrarMetricas(sizeof(nombre) + sizeof(amigo) + sizeof(visibles) + sizeof(u) + sizeof(&red));

    if (visibles == 0) {
        cout << "No hay albumes visibles." << endl;
        return;
    }

    while (!entradaCerrada) {
        long sel = leerNumero("Numero de album a abrir (0 para volver): ");
        if (sel == 0) return;
        if (sel < 1 || sel > visibles) {
            cout << "Seleccion invalida." << endl;
            continue;
        }
        int contador = 0;
        Album* elegido = nullptr;
        for (Nodo<Album*>* n = amigo->obtenerAlbumes().obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
            if (n->obtenerDato()->obtenerPrivacidad() == 3) {
                continue;
            }
            contador = contador + 1;
            if (contador == sel) {
                elegido = n->obtenerDato();
                break;
            }
        }
        if (elegido != nullptr) {
            carrusel(red, elegido);
        }
    }
}

static void verSugerencias(RedSocial& red, Usuario* u) {
    long k = leerNumero("Cuantas sugerencias desea ver (1-20): ");
    if (k == OPCION_INVALIDA || k < 1 || k > 20) {
        cout << "K debe ser un entero entre 1 y 20." << endl;
        mostrarMetricas(sizeof(k) + sizeof(u) + sizeof(&red));
        return;
    }

    monitorGlobal.reiniciarIteraciones();
    Lista<Usuario*> sugeridos;
    red.obtenerSugerencias(u, (int)k, sugeridos);
    int cantidad = sugeridos.obtenerTamano();

    cout << "\nSugerencias de amistad" << endl;
    if (cantidad == 0) {
        cout << "  No hay sugerencias disponibles." << endl;
    }
    int i = 0;
    for (Nodo<Usuario*>* n = sugeridos.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        i = i + 1;
        cout << "  " << i << ". " << n->obtenerDato()->obtenerUsername() << " ("
             << red.obtenerAmigosEnComun(u, n->obtenerDato()) << " amigos en comun)" << endl;
    }
    mostrarMetricas(sizeof(k) + sizeof(cantidad) + sizeof(i) + sizeof(sugeridos) + sizeof(u) + sizeof(&red));

    if (cantidad == 0) {
        return;
    }
    long sel = leerNumero("Numero de usuario al que enviar solicitud (0 para omitir): ");
    if (sel < 1 || sel > cantidad) {
        return;
    }

    monitorGlobal.reiniciarIteraciones();
    Usuario* dest = sugeridos[(int)sel - 1];
    if (dest == nullptr) {
        return;
    }
    informarSolicitud(red.enviarSolicitud(u, dest->obtenerUsername()), dest->obtenerUsername());
    mostrarMetricas(sizeof(sel) + sizeof(dest) + sizeof(sugeridos) + sizeof(u) + sizeof(&red));
}

static void enviarMensaje(RedSocial& red, Usuario* u) {
    char nombre[LINEA_MAX];
    char texto[LINEA_MAX];
    if (!leerLinea("Nombre del amigo destinatario: ", nombre, LINEA_MAX)) return;

    monitorGlobal.reiniciarIteraciones();
    Usuario* dest = red.obtenerUsuarioPorUsername(nombre);
    if (dest == nullptr || dest == u) {
        cout << "Ese usuario no existe." << endl;
        mostrarMetricas(sizeof(nombre) + sizeof(texto) + sizeof(dest) + sizeof(u) + sizeof(&red));
        return;
    }
    if (!red.sonAmigos(u, dest)) {
        cout << "Solo puedes enviar mensajes a tus amigos." << endl;
        mostrarMetricas(sizeof(nombre) + sizeof(texto) + sizeof(dest) + sizeof(u) + sizeof(&red));
        return;
    }

    if (!leerLinea("Mensaje (max. 512 caracteres): ", texto, LINEA_MAX)) return;

    monitorGlobal.reiniciarIteraciones();
    informarMensaje(red.enviarMensaje(u, nombre, texto), nombre);
    mostrarMetricas(sizeof(nombre) + sizeof(texto) + sizeof(dest) + sizeof(u) + sizeof(&red));
}

static void leerNuevos(RedSocial& red, Usuario* u) {
    monitorGlobal.reiniciarIteraciones();
    Lista<Mensaje*> nuevos;
    red.leerMensajesNuevos(u, nuevos);
    cout << "\nMensajes nuevos" << endl;
    if (nuevos.estaVacia()) {
        cout << "  No tienes mensajes nuevos." << endl;
    }
    for (Nodo<Mensaje*>* n = nuevos.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        imprimirMensaje(red, n->obtenerDato());
    }
    mostrarMetricas(sizeof(nuevos) + sizeof(u) + sizeof(&red));
}

static void buscarPorRango(RedSocial& red, Usuario* u) {
    char t1[LINEA_MAX];
    char t2[LINEA_MAX];
    long inicio = 0;
    long fin = 0;
    if (!leerLinea("Fecha de inicio (AAAA-MM-DD): ", t1, LINEA_MAX)) return;
    if (!leerLinea("Fecha de fin    (AAAA-MM-DD): ", t2, LINEA_MAX)) return;

    monitorGlobal.reiniciarIteraciones();
    if (!Fecha::desdeTexto(t1, inicio) || !Fecha::desdeTexto(t2, fin)) {
        cout << "Fecha invalida. Use el formato AAAA-MM-DD." << endl;
        mostrarMetricas(sizeof(t1) + sizeof(t2) + sizeof(inicio) + sizeof(fin) + sizeof(u) + sizeof(&red));
        return;
    }
    if (inicio > fin) {
        cout << "La fecha de inicio no puede ser posterior a la de fin." << endl;
        mostrarMetricas(sizeof(t1) + sizeof(t2) + sizeof(inicio) + sizeof(fin) + sizeof(u) + sizeof(&red));
        return;
    }

    Lista<Mensaje*> encontrados;
    u->buscarMensajesPorRango(inicio, fin, encontrados);
    cout << "\nMensajes recibidos entre " << t1 << " y " << t2 << endl;
    if (encontrados.estaVacia()) {
        cout << "  No hay mensajes en ese rango." << endl;
    }
    for (Nodo<Mensaje*>* n = encontrados.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        imprimirMensaje(red, n->obtenerDato());
    }
    cout << "Total: " << encontrados.obtenerTamano() << " mensajes." << endl;
    mostrarMetricas(sizeof(t1) + sizeof(t2) + sizeof(inicio) + sizeof(fin) + sizeof(encontrados) + sizeof(u) + sizeof(&red));
}

static void menuMensajes(RedSocial& red, Usuario* u) {
    while (!entradaCerrada) {
        cout << "\nAdministrar mensajes" << endl;
        cout << "1. Enviar mensaje" << endl;
        cout << "2. Leer nuevos mensajes" << endl;
        cout << "3. Buscar mensajes por rango de fecha" << endl;
        cout << "0. Volver" << endl;
        long op = leerNumero("Opcion: ");
        if (op == 1) enviarMensaje(red, u);
        else if (op == 2) leerNuevos(red, u);
        else if (op == 3) buscarPorRango(red, u);
        else if (op == 0) return;
        else cout << "Opcion invalida." << endl;
    }
}

static void sesion(RedSocial& red, Usuario* u) {
    cout << "\nBienvenido, " << u->obtenerUsername() << "!" << endl;
    mostrarMejoresAmigos(red, u, false);

    while (!entradaCerrada) {
        cout << "\nUde@Book - Menu principal" << endl;
        cout << "1. Actualizar fotos de mejores amigos (una sola vez)" << endl;
        cout << "2. Administrar amistades" << endl;
        cout << "3. Visitar perfil de un amigo" << endl;
        cout << "4. Ver sugerencias de amistad" << endl;
        cout << "5. Administrar mensajes" << endl;
        cout << "0. Cerrar sesion" << endl;
        long op = leerNumero("Opcion: ");
        if (op == 1) mostrarMejoresAmigos(red, u, true);
        else if (op == 2) menuAmistades(red, u);
        else if (op == 3) visitarPerfil(red, u);
        else if (op == 4) verSugerencias(red, u);
        else if (op == 5) menuMensajes(red, u);
        else if (op == 0) break;
        else cout << "Opcion invalida." << endl;
    }

    monitorGlobal.reiniciarIteraciones();
    red.cerrarSesion(u);
    if (!red.guardarDatos()) {
        cout << "[AVISO] No se pudieron sincronizar algunos archivos." << endl;
    }
    cout << "Sesion cerrada." << endl;
}

int main() {
    RedSocial red;

    monitorGlobal.reiniciarIteraciones();
    if (!red.cargarDatos()) {
        cerr << "No se pudieron cargar los datos. Revise los archivos .txt." << endl;
        return 1;
    }
    cout << "Datos cargados: " << red.obtenerTotalUsuarios() << " usuarios." << endl;
    mostrarMetricas(0);

    while (!entradaCerrada) {
        cout << "\nUde@Book" << endl;
        cout << "1. Iniciar sesion" << endl;
        cout << "0. Salir" << endl;
        long op = leerNumero("Opcion: ");
        if (op == 0) {
            break;
        }
        if (op != 1) {
            cout << "Opcion invalida." << endl;
            continue;
        }

        char nombre[LINEA_MAX];
        char clave[LINEA_MAX];
        if (!leerLinea("Usuario: ", nombre, LINEA_MAX)) break;
        if (!leerLinea("Contrasena: ", clave, LINEA_MAX)) break;

        monitorGlobal.reiniciarIteraciones();
        Usuario* u = red.autenticar(nombre, clave);
        if (u == nullptr) {
            cout << "Credenciales incorrectas." << endl;
            mostrarMetricas(sizeof(nombre) + sizeof(clave) + sizeof(u) + sizeof(&red));
            continue;
        }
        mostrarMetricas(sizeof(nombre) + sizeof(clave) + sizeof(u) + sizeof(&red));
        sesion(red, u);
    }

    red.guardarDatos();
    cout << "\nHasta pronto." << endl;
    return 0;
}