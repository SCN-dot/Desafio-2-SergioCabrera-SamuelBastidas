#include "MonitorRecursos.h"

using namespace std;

MonitorRecursos monitorGlobal;

MonitorRecursos::MonitorRecursos() {
    bytesAsignados = 0;
    asignacionesActivas = 0;
    totalIteraciones = 0;
    llamadasExternas = 0;
}

MonitorRecursos::~MonitorRecursos() {
}

void MonitorRecursos::registrarAsignacion(size_t bytes) {
    bytesAsignados = bytesAsignados + bytes;
    asignacionesActivas = asignacionesActivas + 1;
}

void MonitorRecursos::registrarLiberacion(size_t bytes) {
    if (bytesAsignados >= bytes) {
        bytesAsignados = bytesAsignados - bytes;
    } else {
        bytesAsignados = 0;
    }
    if (asignacionesActivas > 0) {
        asignacionesActivas = asignacionesActivas - 1;
    }
}

void MonitorRecursos::registrarIteraciones(int n) {
    if (n > 0) {
        totalIteraciones = totalIteraciones + (size_t)n;
    }
}

void MonitorRecursos::registrarLlamadaExterna() {
    llamadasExternas = llamadasExternas + 1;
}

void MonitorRecursos::reiniciarIteraciones() {
    totalIteraciones = 0;
    llamadasExternas = 0;
}

size_t MonitorRecursos::obtenerIteraciones() const {
    return totalIteraciones;
}

int MonitorRecursos::obtenerLlamadasExternas() const {
    return llamadasExternas;
}

size_t MonitorRecursos::obtenerMemoriaTotal(size_t bytesStack) const {
    return bytesAsignados + bytesStack;
}

void MonitorRecursos::auditarEstado(size_t bytesStack) const {
    cout << "\n    CONSUMO DE RECURSOS    " << endl;
    cout << "Iteraciones requeridas        : " << totalIteraciones << endl;
    cout << "Usos de componentes externos  : " << llamadasExternas << endl;
    cout << "Memoria dinamica (heap)       : " << bytesAsignados << " bytes" << endl;
    cout << "Variables locales/parametros  : " << bytesStack << " bytes" << endl;
    cout << "Memoria total en este momento : " << obtenerMemoriaTotal(bytesStack) << " bytes" << endl;
    cout << "Bloques dinamicos activos     : " << asignacionesActivas << endl;
    cout << "\n" << endl;
}

