#ifndef LISTA_H
#define LISTA_H

#include "MonitorRecursos.h"

template <typename T>
class Nodo {
private:
    T dato;
    Nodo<T>* siguiente;

    template <typename U> friend class Lista;

public:
    Nodo(const T& val) : dato(val), siguiente(nullptr) {}

    ~Nodo() {
        siguiente = nullptr;
    }

    const T& obtenerDato() const {
        return dato;
    }

    void establecerDato(const T& val) {
        dato = val;
    }

    Nodo<T>* obtenerSiguiente() const {
        return siguiente;
    }

    void establecerSiguiente(Nodo<T>* sig) {
        siguiente = sig;
    }
};

template <typename T>
class Lista {
private:
    Nodo<T>* cabeza;
    Nodo<T>* cola;
    int tamano;
    mutable T valorNulo;

    void copiarDesde(const Lista<T>& otra) {
        for (Nodo<T>* actual = otra.cabeza; actual != nullptr; actual = actual->siguiente) {
            monitorGlobal.registrarIteraciones(1);
            insertar(actual->dato);
        }
    }

public:
    Lista() : cabeza(nullptr), cola(nullptr), tamano(0), valorNulo() {}

    Lista(const Lista<T>& otra) : cabeza(nullptr), cola(nullptr), tamano(0), valorNulo() {
        copiarDesde(otra);
    }

    Lista<T>& operator=(const Lista<T>& otra) {
        if (this != &otra) {
            vaciar();
            copiarDesde(otra);
        }
        return *this;
    }

    ~Lista() {
        vaciar();
    }

    void vaciar() {
        Nodo<T>* actual = cabeza;
        while (actual != nullptr) {
            monitorGlobal.registrarIteraciones(1);
            Nodo<T>* nodoABorrar = actual;
            actual = actual->siguiente;
            delete nodoABorrar;
            monitorGlobal.registrarLiberacion(sizeof(Nodo<T>));
        }
        cabeza = nullptr;
        cola = nullptr;
        tamano = 0;
    }

    void insertar(const T& val) {
        Nodo<T>* nuevo = new Nodo<T>(val);
        monitorGlobal.registrarAsignacion(sizeof(Nodo<T>));

        if (cabeza == nullptr) {
            cabeza = nuevo;
            cola = nuevo;
        } else {
            cola->siguiente = nuevo;
            cola = nuevo;
        }
        tamano = tamano + 1;
    }

    bool insertar(const T& val, int indice) {
        if (indice < 0 || indice > tamano) {
            return false;
        }
        if (indice == tamano) {
            insertar(val);
            return true;
        }

        Nodo<T>* nuevo = new Nodo<T>(val);
        monitorGlobal.registrarAsignacion(sizeof(Nodo<T>));

        if (indice == 0) {
            nuevo->siguiente = cabeza;
            cabeza = nuevo;
        } else {
            Nodo<T>* anterior = cabeza;
            for (int i = 0; i < indice - 1; i++) {
                monitorGlobal.registrarIteraciones(1);
                anterior = anterior->siguiente;
            }
            nuevo->siguiente = anterior->siguiente;
            anterior->siguiente = nuevo;
        }
        tamano = tamano + 1;
        return true;
    }

    bool eliminar(int indice) {
        if (indice < 0 || indice >= tamano) {
            return false;
        }

        Nodo<T>* nodoABorrar = nullptr;

        if (indice == 0) {
            nodoABorrar = cabeza;
            cabeza = cabeza->siguiente;
            if (cabeza == nullptr) {
                cola = nullptr;
            }
        } else {
            Nodo<T>* anterior = cabeza;
            for (int i = 0; i < indice - 1; i++) {
                monitorGlobal.registrarIteraciones(1);
                anterior = anterior->siguiente;
            }
            nodoABorrar = anterior->siguiente;
            anterior->siguiente = nodoABorrar->siguiente;
            if (nodoABorrar == cola) {
                cola = anterior;
            }
        }

        delete nodoABorrar;
        monitorGlobal.registrarLiberacion(sizeof(Nodo<T>));
        tamano = tamano - 1;
        return true;
    }

    int obtenerTamano() const {
        return tamano;
    }

    bool estaVacia() const {
        return tamano == 0;
    }

    Nodo<T>* obtenerCabeza() const {
        return cabeza;
    }

    T& operator[](int indice) {
        if (indice < 0 || indice >= tamano) {
            valorNulo = T();
            return valorNulo;
        }
        Nodo<T>* actual = cabeza;
        for (int i = 0; i < indice; i++) {
            monitorGlobal.registrarIteraciones(1);
            actual = actual->siguiente;
        }
        return actual->dato;
    }

    const T& operator[](int indice) const {
        if (indice < 0 || indice >= tamano) {
            valorNulo = T();
            return valorNulo;
        }
        Nodo<T>* actual = cabeza;
        for (int i = 0; i < indice; i++) {
            monitorGlobal.registrarIteraciones(1);
            actual = actual->siguiente;
        }
        return actual->dato;
    }
};

#endif // LISTA_H
