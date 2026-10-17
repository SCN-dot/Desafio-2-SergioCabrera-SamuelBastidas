#include "RedSocial.h"
#include "Cadenas.h"
#include "Fecha.h"
#include <fstream>
#include <random>

using namespace std;

static const char* ARCH_USUARIOS    = "usuarios.txt";
static const char* ARCH_AMISTADES   = "amistades.txt";
static const char* ARCH_ALBUMES     = "albumesfotos.txt";
static const char* ARCH_SOLICITUDES = "solicitudes.txt";
static const char* ARCH_MENSAJES    = "mensajes.txt";

static const char* ENC_AMISTADES   = "ID_Usuario_A|ID_Usuario_B";
static const char* ENC_ALBUMES     = "ID_Album|ID_Propietario|Nombre_Album|Privacidad|Ruta_Almacenamiento|ID_Foto|Ruta_Linux|Tamano_Bytes|Fecha_Captura|Likes";
static const char* ENC_SOLICITUDES = "ID_Solicitud|ID_Emisor|ID_Receptor|Estado";
static const char* ENC_MENSAJES    = "ID_Mensaje|ID_Emisor|ID_Receptor|Fecha_Envio|Leido|Contenido";

static const int  MAX_LINEA = 2048;
static const char SEP = '|';

static const int PESO_BASE = 2;
static const int PESO_MEDIO = 5;
static const int PESO_ALTO = 7;
static const int UMBRAL_MEDIO = 10;
static const int UMBRAL_ALTO = 20;

static const int MAX_SUGERENCIAS = 20;


static bool leerLinea(ifstream& archivo, char* buffer, bool& desbordada) {
    if (archivo.getline(buffer, MAX_LINEA)) {
        return true;
    }
    desbordada = !archivo.eof();
    return false;
}

static bool lineaEnBlanco(const char* linea) {
    return linea[0] == '\0' || (linea[0] == '\r' && linea[1] == '\0');
}

static bool esEncabezado(const char* linea) {
    return !(linea[0] >= '0' && linea[0] <= '9');
}

static int contarSeparadores(const char* linea) {
    int cuenta = 0;
    int i = 0;
    while (linea[i] != '\0') {
        if (linea[i] == SEP) {
            cuenta = cuenta + 1;
        }
        i = i + 1;
    }
    monitorGlobal.registrarIteraciones(i);
    return cuenta;
}

static void errorArchivo(const char* archivo, const char* motivo) {
    cerr << "ERROR " << archivo << ": " << motivo << endl;
}

static void errorLinea(const char* archivo, int numero, const char* motivo) {
    cerr << "ERROR " << archivo << ", linea " << numero << ": " << motivo << endl;
}

static void extraerCampos(const char* linea, char** campos, int cantidad) {
    int pos = 0;
    for (int i = 0; i < cantidad; i++) {
        campos[i] = Cadenas::extraerCampo(linea, pos, SEP, i == cantidad - 1);
    }
}

static void liberarCampos(char** campos, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        Cadenas::liberar(campos[i]);
    }
}

static bool aIndice(const char* texto, int limite, int& valor) {
    long v = 0;
    if (!Cadenas::aEntero(texto, v) || v < 0 || v >= (long)limite) {
        return false;
    }
    valor = (int)v;
    return true;
}

static bool aNoNegativo(const char* texto, long& valor) {
    long v = 0;
    if (!Cadenas::aEntero(texto, v) || v < 0) {
        return false;
    }
    valor = v;
    return true;
}

static bool leerFecha(const char* texto, long& fecha) {
    if (Cadenas::indiceDe(texto, '-', 0) >= 0) {
        return Fecha::desdeTexto(texto, fecha);
    }
    long v = 0;
    if (!Cadenas::aEntero(texto, v) || !Fecha::esValida(v)) {
        return false;
    }
    fecha = v;
    return true;
}

static bool contiene(const int* ids, int cantidad, int id) {
    for (int i = 0; i < cantidad; i++) {
        monitorGlobal.registrarIteraciones(1);
        if (ids[i] == id) {
            return true;
        }
    }
    return false;
}

static void escribirFecha(ostream& salida, long fecha) {
    if (fecha < 0) {
        fecha = 0;
    }
    int anio = (int)((fecha / 10000) % 10000);
    int mes = (int)((fecha / 100) % 100);
    int dia = (int)(fecha % 100);
    char t[11];
    t[0] = (char)('0' + (anio / 1000) % 10);
    t[1] = (char)('0' + (anio / 100) % 10);
    t[2] = (char)('0' + (anio / 10) % 10);
    t[3] = (char)('0' + anio % 10);
    t[4] = '-';
    t[5] = (char)('0' + (mes / 10) % 10);
    t[6] = (char)('0' + mes % 10);
    t[7] = '-';
    t[8] = (char)('0' + (dia / 10) % 10);
    t[9] = (char)('0' + dia % 10);
    t[10] = '\0';
    salida << t;
}

static void escribirPrefijoAlbum(ostream& salida, const Album* a) {
    salida << a->obtenerId() << SEP << a->obtenerIdPropietario() << SEP << a->obtenerNombre() << SEP
           << a->obtenerPrivacidad() << SEP << a->obtenerRutaAlmacenamiento() << SEP;
}

static void escribirLineaMensaje(ostream& salida, const Mensaje* m) {
    salida << m->obtenerId() << SEP << m->obtenerIdEmisor() << SEP << m->obtenerIdReceptor() << SEP;
    escribirFecha(salida, m->obtenerTimestamp());
    salida << SEP << (m->estaLeido() ? 1 : 0) << SEP << m->obtenerContenido() << '\n';
}

static bool abrirAnexo(ofstream& salida, const char* archivo, const char* encabezado) {
    salida.open(archivo, ios::app);
    if (!salida.is_open()) {
        return false;
    }
    monitorGlobal.registrarLlamadaExterna();
    salida.seekp(0, ios::end);
    if (salida.tellp() == streampos(0)) {
        salida << encabezado << '\n';
    }
    return true;
}

static bool anexarAmistad(int a, int b) {
    ofstream salida;
    if (!abrirAnexo(salida, ARCH_AMISTADES, ENC_AMISTADES)) {
        return false;
    }
    salida << a << SEP << b << '\n';
    salida.close();
    return !salida.fail();
}

static bool anexarSolicitud(const SolicitudAmistad* s) {
    ofstream salida;
    if (!abrirAnexo(salida, ARCH_SOLICITUDES, ENC_SOLICITUDES)) {
        return false;
    }
    salida << s->obtenerId() << SEP << s->obtenerIdEmisor() << SEP << s->obtenerIdReceptor() << SEP
           << s->obtenerEstado() << '\n';
    salida.close();
    return !salida.fail();
}

static bool anexarMensaje(const Mensaje* m) {
    ofstream salida;
    if (!abrirAnexo(salida, ARCH_MENSAJES, ENC_MENSAJES)) {
        return false;
    }
    escribirLineaMensaje(salida, m);
    salida.close();
    return !salida.fail();
}

RedSocial::RedSocial() {
    usuariosPorId = nullptr;
    totalUsuarios = 0;
    capacidad = 0;
    relaciones = nullptr;
    siguienteIdMensaje = 0;
    siguienteIdSolicitud = 0;
    for (int i = 0; i < 3; i++) {
        grupoPrevio[i] = -1;
    }
    cantidadGrupoPrevio = 0;
    amistadesPendientes = false;
    albumesPendientes = false;
    solicitudesPendientes = false;
    mensajesPendientes = false;

    std::random_device rd;
    generador.seed(rd());
}

RedSocial::~RedSocial() {
    for (Nodo<Usuario*>* n = listaUsuarios.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        delete n->obtenerDato();
        monitorGlobal.registrarLiberacion(sizeof(Usuario));
    }
    if (usuariosPorId != nullptr) {
        delete[] usuariosPorId;
        usuariosPorId = nullptr;
        monitorGlobal.registrarLiberacion((size_t)capacidad * sizeof(Usuario*));
    }
    if (relaciones != nullptr) {
        delete relaciones;
        relaciones = nullptr;
        monitorGlobal.registrarLiberacion(sizeof(GrafoAmistad));
    }
}

bool RedSocial::cargarDatos() {
    if (relaciones != nullptr) {
        return false;
    }
    return cargarUsuarios() && cargarAmistades() && cargarAlbumes()
           && cargarSolicitudes() && cargarMensajes();
}

bool RedSocial::cargarUsuarios() {
    ifstream archivo(ARCH_USUARIOS);
    if (!archivo.is_open()) {
        errorArchivo(ARCH_USUARIOS, "no se pudo abrir (es obligatorio)");
        return false;
    }
    monitorGlobal.registrarLlamadaExterna();

    char linea[MAX_LINEA];
    bool desbordada = false;

    int cuenta = 0;
    bool primera = true;
    while (leerLinea(archivo, linea, desbordada)) {
        monitorGlobal.registrarIteraciones(1);
        if (lineaEnBlanco(linea)) {
            continue;
        }
        if (primera) {
            primera = false;
            if (esEncabezado(linea)) {
                continue;
            }
        }
        cuenta = cuenta + 1;
    }
    if (desbordada) {
        errorArchivo(ARCH_USUARIOS, "hay una linea con mas de 2047 caracteres");
        return false;
    }

    capacidad = cuenta;
    usuariosPorId = new Usuario*[capacidad];
    monitorGlobal.registrarAsignacion((size_t)capacidad * sizeof(Usuario*));
    for (int i = 0; i < capacidad; i++) {
        usuariosPorId[i] = nullptr;
    }
    monitorGlobal.registrarIteraciones(capacidad);

    relaciones = new GrafoAmistad(capacidad);
    monitorGlobal.registrarAsignacion(sizeof(GrafoAmistad));

    archivo.clear();
    archivo.seekg(0, ios::beg);
    primera = true;
    int numLinea = 0;
    while (leerLinea(archivo, linea, desbordada)) {
        numLinea = numLinea + 1;
        monitorGlobal.registrarIteraciones(1);
        if (lineaEnBlanco(linea)) {
            continue;
        }
        if (primera) {
            primera = false;
            if (esEncabezado(linea)) {
                continue;
            }
        }
        if (contarSeparadores(linea) < 8) {
            errorLinea(ARCH_USUARIOS, numLinea, "se esperaban 9 campos");
            return false;
        }

        char* campos[9];
        extraerCampos(linea, campos, 9);

        int id = 0;
        long fechaNac = 0;
        long fechaIns = 0;

        bool idValido       = aIndice(campos[0], capacidad, id);
        bool usernameValido = !Cadenas::estaVacia(campos[1]);
        bool claveValida    = !Cadenas::estaVacia(campos[2]);
        bool fechaNacValida = leerFecha(campos[4], fechaNac);
        bool fechaInsValida = leerFecha(campos[7], fechaIns);
        bool fotoValida     = !Cadenas::estaVacia(campos[8]);

        bool datosValidos = idValido && usernameValido && claveValida && fechaNacValida && fechaInsValida && fotoValida;

        if (!datosValidos) {
            liberarCampos(campos, 9);
            errorLinea(ARCH_USUARIOS, numLinea, "ID fuera del rango, datos vacios, ruta de foto vacia o fecha invalida");
            return false;
        }

        char* nacimiento = Fecha::aTexto(fechaNac);
        char* inscripcion = Fecha::aTexto(fechaIns);
        Usuario* u = new Usuario(id, campos[1], campos[2], campos[3], nacimiento, campos[5], campos[6], inscripcion, campos[8]);
        monitorGlobal.registrarAsignacion(sizeof(Usuario));
        Cadenas::liberar(nacimiento);
        Cadenas::liberar(inscripcion);
        liberarCampos(campos, 9);

        if (!registrarUsuario(u)) {
            delete u;
            monitorGlobal.registrarLiberacion(sizeof(Usuario));
            errorLinea(ARCH_USUARIOS, numLinea, "ID repetido o username duplicado");
            return false;
        }
    }
    if (desbordada) {
        errorArchivo(ARCH_USUARIOS, "hay una linea con mas de 2047 caracteres");
        return false;
    }
    return true;
}

bool RedSocial::cargarAmistades() {
    ifstream archivo(ARCH_AMISTADES);
    if (!archivo.is_open()) {
        return true;
    }
    monitorGlobal.registrarLlamadaExterna();

    char linea[MAX_LINEA];
    bool desbordada = false;
    bool primera = true;
    int numLinea = 0;
    while (leerLinea(archivo, linea, desbordada)) {
        numLinea = numLinea + 1;
        monitorGlobal.registrarIteraciones(1);
        if (lineaEnBlanco(linea)) {
            continue;
        }
        if (primera) {
            primera = false;
            if (esEncabezado(linea)) {
                continue;
            }
        }
        if (contarSeparadores(linea) < 1) {
            errorLinea(ARCH_AMISTADES, numLinea, "se esperaban 2 campos");
            return false;
        }

        char* campos[2];
        extraerCampos(linea, campos, 2);
        int a = 0;
        int b = 0;

        bool idAValido     = aIndice(campos[0], capacidad, a);
        bool idBValido     = aIndice(campos[1], capacidad, b);
        bool sonDistintos  = (a != b);
        bool datosValidos  = idAValido && idBValido && sonDistintos;

        liberarCampos(campos, 2);
        if (!datosValidos) {
            errorLinea(ARCH_AMISTADES, numLinea, "IDs invalidos, fuera de rango o iguales");
            return false;
        }

        if (!relaciones->sonAmigos(a, b)) {
            relaciones->agregarAmistad(a, b);
            usuariosPorId[a]->agregarAmigo(usuariosPorId[b]);
            usuariosPorId[b]->agregarAmigo(usuariosPorId[a]);
        }
    }
    if (desbordada) {
        errorArchivo(ARCH_AMISTADES, "hay una linea con mas de 2047 caracteres");
        return false;
    }
    return true;
}

bool RedSocial::cargarAlbumes() {
    ifstream archivo(ARCH_ALBUMES);
    if (!archivo.is_open()) {
        return true;
    }
    monitorGlobal.registrarLlamadaExterna();

    char linea[MAX_LINEA];
    bool desbordada = false;
    bool primera = true;
    int numLinea = 0;
    Album* actual = nullptr;

    while (leerLinea(archivo, linea, desbordada)) {
        numLinea = numLinea + 1;
        monitorGlobal.registrarIteraciones(1);
        if (lineaEnBlanco(linea)) {
            continue;
        }
        if (primera) {
            primera = false;
            if (esEncabezado(linea)) {
                continue;
            }
        }
        if (contarSeparadores(linea) < 9) {
            errorLinea(ARCH_ALBUMES, numLinea, "se esperaban 10 campos (9 separadores)");
            return false;
        }

        char* campos[10];
        extraerCampos(linea, campos, 10);

        long idAlbum = 0;
        int idProp = 0;
        long privacidad = 0;

        bool idAlbumValido    = aNoNegativo(campos[0], idAlbum);
        bool idPropValido     = aIndice(campos[1], capacidad, idProp);
        bool nombreValido     = !Cadenas::estaVacia(campos[2]);
        bool privacidadLeida  = Cadenas::aEntero(campos[3], privacidad);
        bool privacidadValida = privacidadLeida && (privacidad >= 1 && privacidad <= 3);

        bool datosAlbumValidos = idAlbumValido && idPropValido && nombreValido && privacidadValida;

        bool tieneFoto = !Cadenas::estaVacia(campos[5]);
        long idFoto = 0;
        long tamano = 0;
        long fecha = 0;
        long likes = 0;
        bool datosFotoValidos = true;

        if (tieneFoto) {
            bool idFotoValido    = aNoNegativo(campos[5], idFoto);
            bool rutaFotoValida  = !Cadenas::estaVacia(campos[6]);
            bool tamanoValido    = aNoNegativo(campos[7], tamano);
            bool fechaFotoValida = leerFecha(campos[8], fecha);
            bool likesValidos    = aNoNegativo(campos[9], likes);

            datosFotoValidos = idFotoValido && rutaFotoValida && tamanoValido &&
                               fechaFotoValida && likesValidos;
        }

        bool lineaValida = datosAlbumValidos && datosFotoValidos;
        if (!lineaValida) {
            liberarCampos(campos, 10);
            errorLinea(ARCH_ALBUMES, numLinea, "datos del album o de la foto invalidos");
            return false;
        }

        Usuario* dueno = usuariosPorId[idProp];
        if (actual == nullptr || actual->obtenerId() != (int)idAlbum || actual->obtenerIdPropietario() != idProp) {
            actual = nullptr;
            for (Nodo<Album*>* n = dueno->obtenerAlbumes().obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
                monitorGlobal.registrarIteraciones(1);
                if (n->obtenerDato()->obtenerId() == (int)idAlbum) {
                    actual = n->obtenerDato();
                    break;
                }
            }
            if (actual == nullptr) {
                actual = new Album((int)idAlbum, idProp, campos[2], (int)privacidad, campos[4]);
                monitorGlobal.registrarAsignacion(sizeof(Album));
                dueno->agregarAlbum(actual);
            }
        }

        if (tieneFoto) {
            Foto* f = new Foto((int)idFoto, campos[6], (size_t)tamano, fecha, (int)likes);
            monitorGlobal.registrarAsignacion(sizeof(Foto));
            actual->agregarFoto(f);
        }
        liberarCampos(campos, 10);
    }
    if (desbordada) {
        errorArchivo(ARCH_ALBUMES, "hay una linea con mas de 2047 caracteres");
        return false;
    }
    return true;
}

bool RedSocial::cargarSolicitudes() {
    ifstream archivo(ARCH_SOLICITUDES);
    if (!archivo.is_open()) {
        return true;
    }
    monitorGlobal.registrarLlamadaExterna();

    char linea[MAX_LINEA];
    bool desbordada = false;
    bool primera = true;
    int numLinea = 0;
    while (leerLinea(archivo, linea, desbordada)) {
        numLinea = numLinea + 1;
        monitorGlobal.registrarIteraciones(1);
        if (lineaEnBlanco(linea)) {
            continue;
        }
        if (primera) {
            primera = false;
            if (esEncabezado(linea)) {
                continue;
            }
        }
        if (contarSeparadores(linea) < 3) {
            errorLinea(ARCH_SOLICITUDES, numLinea, "se esperaban 4 campos");
            return false;
        }

        char* campos[4];
        extraerCampos(linea, campos, 4);
        long idSol = 0;
        int emisor = 0;
        int receptor = 0;
        int estado = 0;

        bool idSolValido       = aNoNegativo(campos[0], idSol);
        bool emisorValido      = aIndice(campos[1], capacidad, emisor);
        bool receptorValido    = aIndice(campos[2], capacidad, receptor);
        bool estadoValido      = aIndice(campos[3], 3, estado);
        bool usuariosDistintos = (emisor != receptor);

        bool datosValidos = idSolValido && emisorValido && receptorValido && estadoValido && usuariosDistintos;

        liberarCampos(campos, 4);
        if (!datosValidos) {
            errorLinea(ARCH_SOLICITUDES, numLinea, "IDs o estado invalidos");
            return false;
        }

        SolicitudAmistad* s = new SolicitudAmistad((int)idSol, emisor, receptor, estado);
        monitorGlobal.registrarAsignacion(sizeof(SolicitudAmistad));
        usuariosPorId[receptor]->recibirSolicitud(s);
        if ((int)idSol >= siguienteIdSolicitud) {
            siguienteIdSolicitud = (int)idSol + 1;
        }
    }
    if (desbordada) {
        errorArchivo(ARCH_SOLICITUDES, "hay una linea con mas de 2047 caracteres");
        return false;
    }
    return true;
}

bool RedSocial::cargarMensajes() {
    ifstream archivo(ARCH_MENSAJES);
    if (!archivo.is_open()) {
        return true;
    }
    monitorGlobal.registrarLlamadaExterna();

    char linea[MAX_LINEA];
    bool desbordada = false;
    bool primera = true;
    int numLinea = 0;
    while (leerLinea(archivo, linea, desbordada)) {
        numLinea = numLinea + 1;
        monitorGlobal.registrarIteraciones(1);
        if (lineaEnBlanco(linea)) {
            continue;
        }
        if (primera) {
            primera = false;
            if (esEncabezado(linea)) {
                continue;
            }
        }
        if (contarSeparadores(linea) < 5) {
            errorLinea(ARCH_MENSAJES, numLinea, "se esperaban 6 campos");
            return false;
        }

        char* campos[6];
        extraerCampos(linea, campos, 6);
        long idMsg = 0;
        int emisor = 0;
        int receptor = 0;
        long fecha = 0;
        int leido = 0;

        bool idMsgValido       = aNoNegativo(campos[0], idMsg);
        bool emisorValido      = aIndice(campos[1], capacidad, emisor);
        bool receptorValido    = aIndice(campos[2], capacidad, receptor);
        bool usuariosDistintos = (emisor != receptor);
        bool fechaValida       = leerFecha(campos[3], fecha);
        bool leidoValido       = aIndice(campos[4], 2, leido);
        bool contenidoNoVacio  = !Cadenas::estaVacia(campos[5]);
        bool longitudValida    = Cadenas::longitud(campos[5]) <= Mensaje::LONGITUD_MAXIMA;

        bool datosValidos = idMsgValido && emisorValido && receptorValido && usuariosDistintos && fechaValida && leidoValido && contenidoNoVacio && longitudValida;

        if (!datosValidos) {
            liberarCampos(campos, 6);
            errorLinea(ARCH_MENSAJES, numLinea, "datos del mensaje invalidos, contenido vacio o de mas de 512 caracteres");
            return false;
        }

        Mensaje* m = new Mensaje((int)idMsg, emisor, receptor, fecha, leido == 1, campos[5]);
        monitorGlobal.registrarAsignacion(sizeof(Mensaje));
        liberarCampos(campos, 6);

        usuariosPorId[emisor]->agregarMensajeEnviado(m);
        usuariosPorId[receptor]->recibirMensaje(m);
        if ((int)idMsg >= siguienteIdMensaje) {
            siguienteIdMensaje = (int)idMsg + 1;
        }
    }
    if (desbordada) {
        errorArchivo(ARCH_MENSAJES, "hay una linea con mas de 2047 caracteres");
        return false;
    }
    return true;
}

bool RedSocial::guardarAmistades() {
    ofstream archivo(ARCH_AMISTADES);
    if (!archivo.is_open()) {
        return false;
    }
    monitorGlobal.registrarLlamadaExterna();
    archivo << ENC_AMISTADES << '\n';
    for (Nodo<Usuario*>* nu = listaUsuarios.obtenerCabeza(); nu != nullptr; nu = nu->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        const Usuario* u = nu->obtenerDato();
        for (Nodo<Usuario*>* na = u->obtenerAmigos().obtenerCabeza(); na != nullptr; na = na->obtenerSiguiente()) {
            monitorGlobal.registrarIteraciones(1);
            if (u->obtenerId() < na->obtenerDato()->obtenerId()) {
                archivo << u->obtenerId() << SEP << na->obtenerDato()->obtenerId() << '\n';
            }
        }
    }
    archivo.close();
    return !archivo.fail();
}

bool RedSocial::guardarAlbumes() {
    ofstream archivo(ARCH_ALBUMES);
    if (!archivo.is_open()) {
        return false;
    }
    monitorGlobal.registrarLlamadaExterna();
    archivo << ENC_ALBUMES << '\n';
    for (Nodo<Usuario*>* nu = listaUsuarios.obtenerCabeza(); nu != nullptr; nu = nu->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        for (Nodo<Album*>* nal = nu->obtenerDato()->obtenerAlbumes().obtenerCabeza(); nal != nullptr; nal = nal->obtenerSiguiente()) {
            monitorGlobal.registrarIteraciones(1);
            const Album* a = nal->obtenerDato();
            if (a->obtenerCantidadFotos() == 0) {
                escribirPrefijoAlbum(archivo, a);
                archivo << "||||" << '\n';
            } else {
                for (Nodo<Foto*>* nf = a->obtenerFotos().obtenerCabeza(); nf != nullptr; nf = nf->obtenerSiguiente()) {
                    monitorGlobal.registrarIteraciones(1);
                    const Foto* f = nf->obtenerDato();
                    escribirPrefijoAlbum(archivo, a);
                    archivo << f->obtenerId() << SEP << f->obtenerRuta() << SEP << f->obtenerTamano() << SEP;
                    escribirFecha(archivo, f->obtenerFecha());
                    archivo << SEP << f->obtenerLikes() << '\n';
                }
            }
        }
    }
    archivo.close();
    return !archivo.fail();
}

bool RedSocial::guardarSolicitudes() {
    ofstream archivo(ARCH_SOLICITUDES);
    if (!archivo.is_open()) {
        return false;
    }
    monitorGlobal.registrarLlamadaExterna();
    archivo << ENC_SOLICITUDES << '\n';
    for (Nodo<Usuario*>* nu = listaUsuarios.obtenerCabeza(); nu != nullptr; nu = nu->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        for (Nodo<SolicitudAmistad*>* ns = nu->obtenerDato()->obtenerSolicitudes().obtenerCabeza(); ns != nullptr; ns = ns->obtenerSiguiente()) {
            monitorGlobal.registrarIteraciones(1);
            const SolicitudAmistad* s = ns->obtenerDato();
            archivo << s->obtenerId() << SEP << s->obtenerIdEmisor() << SEP << s->obtenerIdReceptor() << SEP
                    << s->obtenerEstado() << '\n';
        }
    }
    archivo.close();
    return !archivo.fail();
}

bool RedSocial::guardarMensajes() {
    ofstream archivo(ARCH_MENSAJES);
    if (!archivo.is_open()) {
        return false;
    }
    monitorGlobal.registrarLlamadaExterna();
    archivo << ENC_MENSAJES << '\n';
    for (Nodo<Usuario*>* nu = listaUsuarios.obtenerCabeza(); nu != nullptr; nu = nu->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        for (Nodo<Mensaje*>* nm = nu->obtenerDato()->obtenerMensajesRecibidos().obtenerCabeza(); nm != nullptr; nm = nm->obtenerSiguiente()) {
            monitorGlobal.registrarIteraciones(1);
            escribirLineaMensaje(archivo, nm->obtenerDato());
        }
    }
    archivo.close();
    return !archivo.fail();
}

bool RedSocial::guardarDatos() {
    bool todoBien = true;
    if (amistadesPendientes) {
        if (guardarAmistades()) {
            amistadesPendientes = false;
        } else {
            todoBien = false;
        }
    }
    if (albumesPendientes) {
        if (guardarAlbumes()) {
            albumesPendientes = false;
        } else {
            todoBien = false;
        }
    }
    if (solicitudesPendientes) {
        if (guardarSolicitudes()) {
            solicitudesPendientes = false;
        } else {
            todoBien = false;
        }
    }
    if (mensajesPendientes) {
        if (guardarMensajes()) {
            mensajesPendientes = false;
        } else {
            todoBien = false;
        }
    }
    return todoBien;
}

bool RedSocial::registrarUsuario(Usuario* u) {
    if (u == nullptr) {
        return false;
    }
    if (Cadenas::estaVacia(u->obtenerUsername()) || obtenerUsuarioPorUsername(u->obtenerUsername()) != nullptr) {
        return false;
    }
    int id = u->obtenerId();
    if (usuariosPorId == nullptr || id >= capacidad) {
        int nuevaCap = 10;
        if (id >= capacidad) {
            nuevaCap = id + 10;
        }

        Usuario** nuevoArr = new Usuario*[nuevaCap];
        monitorGlobal.registrarAsignacion((size_t)nuevaCap * sizeof(Usuario*));

        for (int i = 0; i < nuevaCap; i++) {
            if (i < capacidad && usuariosPorId != nullptr) {
                nuevoArr[i] = usuariosPorId[i];
            } else {
                nuevoArr[i] = nullptr;
            }
        }
        if (usuariosPorId != nullptr) {
            delete[] usuariosPorId;
            monitorGlobal.registrarLiberacion((size_t)capacidad * sizeof(Usuario*));
        }
        usuariosPorId = nuevoArr;

        if (relaciones == nullptr) {
            relaciones = new GrafoAmistad(nuevaCap);
            monitorGlobal.registrarAsignacion(sizeof(GrafoAmistad));
        }
        capacidad = nuevaCap;
    }
    if (id < 0 || id >= capacidad || usuariosPorId[id] != nullptr) {
        return false;
    }

    listaUsuarios.insertar(u);
    usuariosPorId[id] = u;
    if (relaciones != nullptr) {
        relaciones->incrementarUsuarios();
    }
    totalUsuarios = totalUsuarios + 1;
    return true;
}

Usuario* RedSocial::autenticar(const char* nom, const char* clave) {
    Usuario* u = obtenerUsuarioPorUsername(nom);
    if (u != nullptr && u->validarClave(clave)) {
        return u;
    }
    return nullptr;
}

Usuario* RedSocial::obtenerUsuarioPorUsername(const char* nom) const {
    if (nom == nullptr) {
        return nullptr;
    }
    for (Nodo<Usuario*>* n = listaUsuarios.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        if (Cadenas::iguales(n->obtenerDato()->obtenerUsername(), nom)) {
            return n->obtenerDato();
        }
    }
    return nullptr;
}

Usuario* RedSocial::obtenerUsuarioPorId(int idUsuario) const {
    monitorGlobal.registrarIteraciones(1);
    if (usuariosPorId == nullptr || idUsuario < 0 || idUsuario >= capacidad) {
        return nullptr;
    }
    return usuariosPorId[idUsuario];
}

int RedSocial::obtenerTotalUsuarios() const {
    return totalUsuarios;
}


bool RedSocial::sonAmigos(const Usuario* a, const Usuario* b) const {
    if (a == nullptr || b == nullptr || relaciones == nullptr) {
        return false;
    }
    return relaciones->sonAmigos(a->obtenerId(), b->obtenerId());
}

int RedSocial::obtenerAmigosEnComun(const Usuario* a, const Usuario* b) const {
    if (a == nullptr || b == nullptr || relaciones == nullptr) {
        return 0;
    }
    return relaciones->obtenerAmigosEnComun(a->obtenerId(), b->obtenerId());
}

int RedSocial::enviarSolicitud(Usuario* emisor, const char* nombreReceptor) {
    if (emisor == nullptr || relaciones == nullptr) {
        return SOLICITUD_ERROR;
    }
    Usuario* receptor = obtenerUsuarioPorUsername(nombreReceptor);
    if (receptor == nullptr) {
        return SOLICITUD_NO_EXISTE;
    }
    if (receptor == emisor) {
        return SOLICITUD_A_SI_MISMO;
    }
    if (relaciones->sonAmigos(emisor->obtenerId(), receptor->obtenerId())) {
        return SOLICITUD_YA_AMIGOS;
    }
    if (receptor->tieneSolicitudPendienteDe(emisor->obtenerId())) {
        return SOLICITUD_YA_ENVIADA;
    }
    if (emisor->tieneSolicitudPendienteDe(receptor->obtenerId())) {
        return SOLICITUD_YA_RECIBIDA;
    }

    SolicitudAmistad* nueva = new SolicitudAmistad(siguienteIdSolicitud, emisor->obtenerId(), receptor->obtenerId(), SolicitudAmistad::PENDIENTE);
    monitorGlobal.registrarAsignacion(sizeof(SolicitudAmistad));
    siguienteIdSolicitud = siguienteIdSolicitud + 1;
    receptor->recibirSolicitud(nueva);

    if (solicitudesPendientes) {
        if (guardarSolicitudes()) {
            solicitudesPendientes = false;
        }
    } else if (!anexarSolicitud(nueva)) {
        solicitudesPendientes = true;
    }
    return SOLICITUD_OK;
}

bool RedSocial::responderSolicitud(Usuario* receptor, int idSolicitud, bool aceptar) {
    if (receptor == nullptr || relaciones == nullptr) {
        return false;
    }
    SolicitudAmistad* sol = receptor->obtenerSolicitudPorId(idSolicitud);
    if (sol == nullptr || sol->obtenerEstado() != SolicitudAmistad::PENDIENTE) {
        return false;
    }
    Usuario* emisor = obtenerUsuarioPorId(sol->obtenerIdEmisor());
    if (emisor == nullptr) {
        return false;
    }

    if (!receptor->procesarSolicitud(idSolicitud, aceptar)) {
        return false;
    }

    if (aceptar) {
        int idE = emisor->obtenerId();
        int idR = receptor->obtenerId();
        if (!relaciones->sonAmigos(idE, idR)) {
            relaciones->agregarAmistad(idE, idR);
            emisor->agregarAmigo(receptor);
            receptor->agregarAmigo(emisor);

            if (amistadesPendientes) {
                if (guardarAmistades()) {
                    amistadesPendientes = false;
                }
            } else if (!anexarAmistad(idE, idR)) {
                amistadesPendientes = true;
            }
        }
    }

    solicitudesPendientes = true;
    if (guardarSolicitudes()) {
        solicitudesPendientes = false;
    }
    return true;
}

void RedSocial::obtenerSugerencias(Usuario* u, int K, Lista<Usuario*>& resultado) {
    if (u == nullptr || relaciones == nullptr || K < 1 || K > MAX_SUGERENCIAS) {
        return;
    }

    int n = relaciones->obtenerNumUsuarios();
    int* conteos = new int[n];
    monitorGlobal.registrarAsignacion((size_t)n * sizeof(int));

    int idU = u->obtenerId();
    relaciones->contarAmigosEnComunConTodos(idU, conteos);

    int idsTop[MAX_SUGERENCIAS];
    int cntTop[MAX_SUGERENCIAS];
    int cantidadTop = 0;

    for (int idCandidato = 0; idCandidato < n; idCandidato++) {
        int amigosEnComun = conteos[idCandidato];

        if (amigosEnComun <= 0 || idCandidato == idU || relaciones->sonAmigos(idU, idCandidato)) {
            continue;
        }

        int posicionInsercion = 0;
        if (cantidadTop < K) {
            posicionInsercion = cantidadTop;
            cantidadTop = cantidadTop + 1;
        } else if (amigosEnComun > cntTop[K - 1]) {
            posicionInsercion = K - 1;
        } else {
            continue;
        }

        while (posicionInsercion > 0 && cntTop[posicionInsercion - 1] < amigosEnComun) {
            monitorGlobal.registrarIteraciones(1);
            cntTop[posicionInsercion] = cntTop[posicionInsercion - 1];
            idsTop[posicionInsercion] = idsTop[posicionInsercion - 1];
            posicionInsercion = posicionInsercion - 1;
        }

        cntTop[posicionInsercion] = amigosEnComun;
        idsTop[posicionInsercion] = idCandidato;
    }
    monitorGlobal.registrarIteraciones(n);

    for (int i = 0; i < cantidadTop; i++) {
        monitorGlobal.registrarIteraciones(1);
        resultado.insertar(usuariosPorId[idsTop[i]]);
    }

    delete[] conteos;
    monitorGlobal.registrarLiberacion((size_t)n * sizeof(int));
}

int RedSocial::obtenerMejoresAmigos(Usuario* u, const char** rutas, bool alternar) {
    if (u == nullptr || rutas == nullptr || relaciones == nullptr) {
        return 0;
    }

    if (alternar) {
        if (!u->puedeAlternarFotos()) {
            return 0;
        }
    } else {
        cantidadGrupoPrevio = 0;
    }

    int totalAmigos = u->obtenerCantidadAmigos();
    if (totalAmigos == 0) {
        return 0;
    }

    int n = relaciones->obtenerNumUsuarios();
    Usuario** candidatos = new Usuario*[totalAmigos];
    int* pesos = new int[totalAmigos];
    int* conteos = new int[n];
    monitorGlobal.registrarAsignacion((size_t)totalAmigos * sizeof(Usuario*));
    monitorGlobal.registrarAsignacion((size_t)totalAmigos * sizeof(int));
    monitorGlobal.registrarAsignacion((size_t)n * sizeof(int));

    relaciones->contarAmigosEnComunConTodos(u->obtenerId(), conteos);

    int numCandidatos = 0;
    int sumaTotalPesos = 0;

    for (Nodo<Usuario*>* nodo = u->obtenerAmigos().obtenerCabeza(); nodo != nullptr; nodo = nodo->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        Usuario* amigo = nodo->obtenerDato();

        if (alternar && contiene(grupoPrevio, cantidadGrupoPrevio, amigo->obtenerId())) {
            continue;
        }

        int enComun = conteos[amigo->obtenerId()];
        int peso = PESO_BASE;
        if (enComun > UMBRAL_ALTO) {
            peso = PESO_ALTO;
        } else if (enComun > UMBRAL_MEDIO) {
            peso = PESO_MEDIO;
        }

        candidatos[numCandidatos] = amigo;
        pesos[numCandidatos] = peso;
        sumaTotalPesos = sumaTotalPesos + peso;
        numCandidatos = numCandidatos + 1;
    }

    if (alternar && numCandidatos > 0) {
        u->marcarFotosAlternadas();
    }

    int cantidadAMostrar = 3;
    if (numCandidatos < 3) {
        cantidadAMostrar = numCandidatos;
    }

    int nuevoGrupo[3];

    for (int turno = 0; turno < cantidadAMostrar; turno++) {
        std::uniform_int_distribution<int> distribucion(0, sumaTotalPesos - 1);
        int tiroAleatorio = distribucion(generador);
        monitorGlobal.registrarLlamadaExterna();

        int acumulado = 0;
        int indiceGanador = 0;
        while (indiceGanador < numCandidatos - 1) {
            monitorGlobal.registrarIteraciones(1);
            acumulado = acumulado + pesos[indiceGanador];
            if (tiroAleatorio < acumulado) {
                break;
            }
            indiceGanador = indiceGanador + 1;
        }

        rutas[turno] = candidatos[indiceGanador]->obtenerRutaFotoPerfil();
        nuevoGrupo[turno] = candidatos[indiceGanador]->obtenerId();

        sumaTotalPesos = sumaTotalPesos - pesos[indiceGanador];
        int ultimoIndice = numCandidatos - 1;
        candidatos[indiceGanador] = candidatos[ultimoIndice];
        pesos[indiceGanador] = pesos[ultimoIndice];
        numCandidatos = numCandidatos - 1;
    }

    if (cantidadAMostrar > 0) {
        for (int i = 0; i < cantidadAMostrar; i++) {
            grupoPrevio[i] = nuevoGrupo[i];
        }
        cantidadGrupoPrevio = cantidadAMostrar;
    }

    delete[] candidatos;
    delete[] pesos;
    delete[] conteos;
    monitorGlobal.registrarLiberacion((size_t)totalAmigos * sizeof(Usuario*));
    monitorGlobal.registrarLiberacion((size_t)totalAmigos * sizeof(int));
    monitorGlobal.registrarLiberacion((size_t)n * sizeof(int));

    return cantidadAMostrar;
}

int RedSocial::enviarMensaje(Usuario* emisor, const char* nombreDestinatario, const char* texto) {
    if (emisor == nullptr) {
        return MENSAJE_ERROR;
    }
    if (Cadenas::estaVacia(texto)) {
        return MENSAJE_VACIO;
    }
    if (Cadenas::longitud(texto) > Mensaje::LONGITUD_MAXIMA) {
        return MENSAJE_LARGO;
    }
    if (Cadenas::indiceDe(texto, '\n', 0) >= 0 || Cadenas::indiceDe(texto, '\r', 0) >= 0) {
        return MENSAJE_CON_SALTO;
    }
    Usuario* destino = obtenerUsuarioPorUsername(nombreDestinatario);
    if (destino == nullptr || destino == emisor) {
        return MENSAJE_DESTINO_INVALIDO;
    }
    if (!sonAmigos(emisor, destino)) {
        return MENSAJE_NO_AMIGO;
    }

    long hoy = Fecha::hoy();
    Mensaje* m = emisor->enviarMensaje(destino, siguienteIdMensaje, texto, hoy);
    if (m == nullptr) {
        return MENSAJE_ERROR;
    }
    siguienteIdMensaje = siguienteIdMensaje + 1;

    if (mensajesPendientes) {
        if (guardarMensajes()) {
            mensajesPendientes = false;
        }
    } else if (!anexarMensaje(m)) {
        mensajesPendientes = true;
    }
    return MENSAJE_OK;
}

void RedSocial::leerMensajesNuevos(Usuario* u, Lista<Mensaje*>& nuevos) {
    if (u == nullptr) {
        return;
    }
    u->obtenerMensajesNoLeidos(nuevos);
    if (nuevos.estaVacia()) {
        return;
    }
    for (Nodo<Mensaje*>* n = nuevos.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        n->obtenerDato()->marcarLeido();
    }

    mensajesPendientes = true;
    if (guardarMensajes()) {
        mensajesPendientes = false;
    }
}

void RedSocial::alternarLike(Foto* f) {
    if (f == nullptr) {
        return;
    }
    f->darQuitarLike();

    bool registrada = false;
    for (Nodo<Foto*>* n = fotosTocadas.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        if (n->obtenerDato() == f) {
            registrada = true;
            break;
        }
    }
    if (!registrada) {
        fotosTocadas.insertar(f);
    }

    albumesPendientes = true;
    if (guardarAlbumes()) {
        albumesPendientes = false;
    }
}

void RedSocial::cerrarSesion(Usuario* u) {
    cantidadGrupoPrevio = 0;
    if (u != nullptr) {
        u->reiniciarFotosAlternadas();
    }
    for (Nodo<Foto*>* n = fotosTocadas.obtenerCabeza(); n != nullptr; n = n->obtenerSiguiente()) {
        monitorGlobal.registrarIteraciones(1);
        n->obtenerDato()->reiniciarLike();
    }
    fotosTocadas.vaciar();
}
