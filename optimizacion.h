#ifndef OPTIMIZACION_H
#define OPTIMIZACION_H

#include <vector>
#include <string>
#include <limits>
#include <iomanip>
#include <iostream>
#include "persona_class.h"

// Máximo de ciudades soportadas (suficiente para tu lista de 20)
constexpr int MAX_CIUDADES_OPT = 64;

class Optimizacion {
public:
    // Resultados agregados (sin “nombre más común”)
    struct RQ1 {
        size_t idxLonPais; int edadLonPais;
        int    bestEdadCity[MAX_CIUDADES_OPT];
        size_t idxBestEdadCity[MAX_CIUDADES_OPT];
    };
    struct RQ2 {
        size_t idxPatPais; double patMaxPais;
        double bestPatCity[MAX_CIUDADES_OPT]; size_t idxBestPatCity[MAX_CIUDADES_OPT];
        double bestPatGrp[3]; size_t idxBestPatGrp[3];
    };
    struct RQ3 {
        size_t totGrp[3]; size_t declaGrp[3]; size_t mismatches;
    };
    struct RQ4 { // Top-5 por patrimonio promedio
        double sumPatCity[MAX_CIUDADES_OPT]; size_t cntCity[MAX_CIUDADES_OPT];
        int topIdx[5]; // índices de ciudad del top-5
    };
    struct RQ6 { size_t tot[3]; size_t may80[3]; double pct[3]; double pctPais; };
    struct RQ7 { size_t declarantes; double promedioIngresos; };

    // Constructor: pasa dataset, fecha ref y catálogo de ciudades
    Optimizacion(const std::vector<PersonaClass>& data,
                 int ref_d, int ref_m, int ref_a,
                 const char* const* ciudades, int num_ciudades);

    // Impresiones finales (todas calculadas en una sola pasada)
    void print_Q1() const;
    void print_Q2() const;
    void print_Q3() const;
    void print_Q4_top5_promedio_patrimonio() const;
    void print_Q6_pct_mayores80() const;
    void print_Q7_prom_ingresos_declarantes() const;

    // Accesores (por si quieres usarlos)
    const RQ1& getRQ1() const { return rq1_; }
    const RQ2& getRQ2() const { return rq2_; }
    const RQ3& getRQ3() const { return rq3_; }
    const RQ4& getRQ4() const { return rq4_; }
    const RQ6& getRQ6() const { return rq6_; }
    const RQ7& getRQ7() const { return rq7_; }

private:
    // --------- Tipos/estado interno ---------
    struct Fecha { int d,m,a; };

    const std::vector<PersonaClass>& v_;
    Fecha ref_;
    const char* const* ciudades_; // puntero al arreglo de nombres de ciudad
    int numCiudades_;             // cuántas ciudades hay realmente (<= MAX_CIUDADES_OPT)

    RQ1 rq1_{}; RQ2 rq2_{}; RQ3 rq3_{}; RQ4 rq4_{}; RQ6 rq6_{}; RQ7 rq7_{};

    // --------- Helpers internos (no tocan tu main) ---------
    static bool parseFechaDMY_(const std::string& s, Fecha& out);
    static int  edadAnios_(const Fecha& n, const Fecha& ref);
    static int  edadAniosStr_(const std::string& fecha, const Fecha& ref);
    static bool ultDosDig_(const std::string& id, int& out);
    static char grupoPorDocumento_(const std::string& id);
    int  idxCiudad_(const std::string& c) const;
    static void printClass_(const PersonaClass& p, int edad=-1, double pat=-1);

    void initArrays_();
    void build_();        // una sola pasada O(N)
    void postprocess_();  // top-5 y porcentajes
};

#endif // OPTIMIZACION_H
