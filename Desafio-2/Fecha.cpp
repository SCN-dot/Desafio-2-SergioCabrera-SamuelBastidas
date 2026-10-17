#include "Fecha.h"
#include "Cadenas.h"
#include "MonitorRecursos.h"
#include <chrono>

using namespace std;

Fecha::Fecha() {
}

bool Fecha::esBisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

int Fecha::diasDelMes(int anio, int mes) {
    switch (mes) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        return 31;
    case 4: case 6: case 9: case 11:
        return 30;
    case 2:
        if (esBisiesto(anio)) {
            return 29;
        } else {
            return 28;
        }
    default:
        return 0;
    }
}

bool Fecha::esValida(long fecha) {
    if (fecha <= 0) {
        return false;
    }
    int anio = (int)(fecha / 10000);
    int mes = (int)((fecha / 100) % 100);
    int dia = (int)(fecha % 100);

    if (anio < 1 || anio > 9999 || mes < 1 || mes > 12) {
        return false;
    }
    return (dia >= 1 && dia <= diasDelMes(anio, mes));
}

long Fecha::desdeSegundos(long long segundos) {
    long long dias = segundos / 86400LL;
    if (segundos % 86400LL < 0) {
        dias = dias - 1;
    }
    if (dias < 0) {
        dias = 0;
    }

    int anio = 1970;
    int vueltas = 0;
    while (true) {
        int diasEnEsteAnio = 365;
        if (esBisiesto(anio)) {
            diasEnEsteAnio = 366;
        }

        if (dias < diasEnEsteAnio) {
            break;
        }
        dias = dias - diasEnEsteAnio;
        anio = anio + 1;
        vueltas = vueltas + 1;
    }

    int mes = 1;
    while (mes < 12) {
        int diasEnEsteMes = diasDelMes(anio, mes);
        if (dias < diasEnEsteMes) {
            break;
        }
        dias = dias - diasEnEsteMes;
        mes = mes + 1;
        vueltas = vueltas + 1;
    }
    monitorGlobal.registrarIteraciones(vueltas);

    int dia = (int)dias + 1;

    return (long)anio * 10000L + (long)mes * 100L + (long)dia;
}

long Fecha::hoy() {
    auto ahora = std::chrono::system_clock::now();
    auto epoch = ahora.time_since_epoch();
    long long segundos = std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
    monitorGlobal.registrarLlamadaExterna();
    // Hora de Colombia: UTC-5
    return desdeSegundos(segundos - 5LL * 3600LL);
}

char* Fecha::aTexto(long fecha) {
    if (fecha < 0) {
        fecha = 0;
    }
    int anio = (int)((fecha / 10000) % 10000);
    int mes = (int)((fecha / 100) % 100);
    int dia = (int)(fecha % 100);

    char* texto = new char[11];     // "AAAA-MM-DD" + '\0'
    monitorGlobal.registrarAsignacion(11 * sizeof(char));

    texto[0] = (char)('0' + (anio / 1000) % 10);
    texto[1] = (char)('0' + (anio / 100) % 10);
    texto[2] = (char)('0' + (anio / 10) % 10);
    texto[3] = (char)('0' + anio % 10);
    texto[4] = '-';
    texto[5] = (char)('0' + (mes / 10) % 10);
    texto[6] = (char)('0' + mes % 10);
    texto[7] = '-';
    texto[8] = (char)('0' + (dia / 10) % 10);
    texto[9] = (char)('0' + dia % 10);
    texto[10] = '\0';
    return texto;
}

bool Fecha::desdeTexto(const char* texto, long& fecha) {
    if (texto == nullptr || Cadenas::longitud(texto) != 10) {
        return false;
    }
    if (texto[4] != '-' || texto[7] != '-') {
        return false;
    }
    long valor = 0;
    for (int i = 0; i < 10; i++) {
        monitorGlobal.registrarIteraciones(1);
        if (i == 4 || i == 7) {
            continue;
        }
        if (texto[i] < '0' || texto[i] > '9') {
            return false;
        }
        valor = valor * 10 + (long)(texto[i] - '0');
    }
    if (!esValida(valor)) {
        return false;
    }
    fecha = valor;
    return true;
}
