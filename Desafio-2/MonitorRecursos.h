#ifndef MONITORRECURSOS_H
#define MONITORRECURSOS_H

#include <iostream>

class MonitorRecursos {
private:
    size_t bytesAsignados;
    int asignacionesActivas;
    size_t totalIteraciones;
    int llamadasExternas;

    MonitorRecursos(const MonitorRecursos& otro);
    MonitorRecursos& operator=(const MonitorRecursos& otro);

public:
    MonitorRecursos();
    ~MonitorRecursos();

    void registrarAsignacion(size_t bytes);
    void registrarLiberacion(size_t bytes);
    void registrarIteraciones(int n);
    void registrarLlamadaExterna();

    void reiniciarIteraciones();

    size_t obtenerIteraciones() const;
    int obtenerLlamadasExternas() const;

    size_t obtenerMemoriaTotal(size_t bytesStack) const;

    void auditarEstado() const;
    void auditarEstado(size_t bytesStack) const;
};

extern MonitorRecursos monitorGlobal;

#endif // MONITORRECURSOS_H

