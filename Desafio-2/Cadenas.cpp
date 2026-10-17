#include "Cadenas.h"
#include "MonitorRecursos.h"

using namespace std;

Cadenas::Cadenas() {
    // Clase utilitaria: no se instancia
}

int Cadenas::longitud(const char* texto) {
    if (texto == nullptr) {
        return 0;
    }
    int n = 0;
    while (texto[n] != '\0') {
        n = n + 1;
    }
    monitorGlobal.registrarIteraciones(n);
    return n;
}

char* Cadenas::duplicar(const char* origen) {
    if (origen == nullptr) {
        return nullptr;
    }
    int n = longitud(origen);
    char* copia = new char[n + 1];
    monitorGlobal.registrarAsignacion((size_t)(n + 1) * sizeof(char));
    for (int i = 0; i <= n; i++) {     // incluye el '\0'
        copia[i] = origen[i];
    }
    monitorGlobal.registrarIteraciones(n + 1);
    return copia;
}

void Cadenas::liberar(char*& texto) {
    if (texto != nullptr) {
        int n = longitud(texto);
        delete[] texto;
        texto = nullptr;
        monitorGlobal.registrarLiberacion((size_t)(n + 1) * sizeof(char));
    }
}

int Cadenas::comparar(const char* a, const char* b) {
    if (a == nullptr && b == nullptr) {
        return 0;
    }
    if (a == nullptr) {
        return -1;
    }
    if (b == nullptr) {
        return 1;
    }
    int i = 0;
    while (a[i] != '\0' && a[i] == b[i]) {
        i = i + 1;
    }
    monitorGlobal.registrarIteraciones(i + 1);
    return (int)((unsigned char)a[i]) - (int)((unsigned char)b[i]);
}

bool Cadenas::iguales(const char* a, const char* b) {
    return comparar(a, b) == 0;
}

bool Cadenas::estaVacia(const char* texto) {
    return texto == nullptr || texto[0] == '\0';
}

int Cadenas::indiceDe(const char* texto, char caracter, int desde) {
    if (texto == nullptr || desde < 0) {
        return -1;
    }
    for (int i = 0; i < desde; i++) {
        monitorGlobal.registrarIteraciones(1);
        if (texto[i] == '\0') {
            return -1;
        }
    }
    int pos = desde;
    while (texto[pos] != '\0') {
        monitorGlobal.registrarIteraciones(1);
        if (texto[pos] == caracter) {
            return pos;
        }
        pos = pos + 1;
    }
    return -1;
}

char* Cadenas::subcadena(const char* texto, int inicio, int fin) {
    if (texto == nullptr || inicio < 0 || fin < inicio) {
        return duplicar("");
    }
    for (int i = 0; i < inicio; i++) {
        if (texto[i] == '\0') {
            monitorGlobal.registrarIteraciones(i + 1);
            return duplicar("");
        }
    }
    monitorGlobal.registrarIteraciones(inicio);

    int maximo = fin - inicio;
    int n = 0;
    while (n < maximo && texto[inicio + n] != '\0') {
        n = n + 1;
    }
    char* resultado = new char[n + 1];
    monitorGlobal.registrarAsignacion((size_t)(n + 1) * sizeof(char));
    for (int i = 0; i < n; i++) {
        resultado[i] = texto[inicio + i];
    }
    resultado[n] = '\0';
    monitorGlobal.registrarIteraciones(2 * n);
    return resultado;
}

char* Cadenas::extraerCampo(const char* linea, int& posicion, char separador, bool hastaElFinal) {
    if (linea == nullptr || posicion < 0) {
        return duplicar("");
    }
    int fin = posicion;
    while (linea[fin] != '\0' && linea[fin] != '\n' && linea[fin] != '\r'
           && (hastaElFinal || linea[fin] != separador)) {
        fin = fin + 1;
    }
    monitorGlobal.registrarIteraciones(fin - posicion + 1);

    int n = fin - posicion;
    char* campo = new char[n + 1];
    monitorGlobal.registrarAsignacion((size_t)(n + 1) * sizeof(char));
    for (int i = 0; i < n; i++) {
        campo[i] = linea[posicion + i];
    }
    campo[n] = '\0';
    monitorGlobal.registrarIteraciones(n);

    if (!hastaElFinal && linea[fin] == separador) {
        posicion = fin + 1;
    } else {
        posicion = fin;
    }
    return campo;
}

bool Cadenas::aEntero(const char* texto, long& resultado) {
    if (estaVacia(texto)) {
        return false;
    }
    int i = 0;
    bool negativo = false;
    if (texto[0] == '-') {
        negativo = true;
        i = 1;
    }
    if (texto[i] == '\0') {
        return false;
    }
    long valor = 0;
    while (texto[i] != '\0') {
        monitorGlobal.registrarIteraciones(1);
        if (texto[i] < '0' || texto[i] > '9') {
            return false;
        }

        long digito = (long)(texto[i] - '0');

        if (valor > (2147483647L - digito) / 10L) {
            return false;
        }
        valor = valor * 10L + digito;
        i = i + 1;
    }

    if (negativo) {
        resultado = -valor;
    } else {
        resultado = valor;
    }
    return true;
}
