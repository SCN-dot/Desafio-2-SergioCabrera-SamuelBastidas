#ifndef FECHA_H
#define FECHA_H

class Fecha {
private:
    Fecha();

    static bool esBisiesto(int anio);
    static int diasDelMes(int anio, int mes);
    static long desdeSegundos(long long segundos);

public:
    static long hoy();

    static char* aTexto(long fecha);

    static bool desdeTexto(const char* texto, long& fecha);

    static bool esValida(long fecha);
};

#endif // FECHA_H
