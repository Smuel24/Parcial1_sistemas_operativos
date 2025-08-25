#ifndef MONITOR_H
#define MONITOR_H

#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstddef> // size_t

class Monitor {
public:
    // -------- Cronómetro --------
    void iniciar_tiempo();
    double detener_tiempo() const; // ms

    // -------- Memoria TEÓRICA (helpers) --------
    template <class T>
    static inline std::size_t bytes_vector_valores(const std::vector<T>& v,
                                                   bool usar_capacity=false) {
        const std::size_t n = usar_capacity ? v.capacity() : v.size();
        return n * sizeof(T);
    }

    template <class T>
    static inline std::size_t bytes_copia_por_valor(const std::vector<T>& v) {
        return v.size() * sizeof(T);
    }

    template <class T>
    static inline std::size_t bytes_vector_wrapper(const std::vector<T>& v) {
        (void)v;
        return sizeof(std::vector<T>);
    }

    static inline std::string formato_bytes(std::size_t bytes) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(2);
        const double KB = 1024.0;
        const double MB = 1024.0 * KB;
        const double GB = 1024.0 * MB;
        if (bytes >= static_cast<std::size_t>(GB))      os << (bytes / GB) << " GB";
        else if (bytes >= static_cast<std::size_t>(MB)) os << (bytes / MB) << " MB";
        else if (bytes >= static_cast<std::size_t>(KB)) os << (bytes / KB) << " KB";
        else                                            os << bytes << " B";
        return os.str();
    }

    // -------- Registro TEÓRICO (opcional) --------
    struct EntradaTeorica {
        std::string tag;
        double ms{0.0};
        std::size_t bytes_teoricos{0};
    };

    void registrar_teorico(const EntradaTeorica& e);
    void mostrar_estadistica_teorica(const EntradaTeorica& e) const;
    void mostrar_resumen_teorico() const;
    void exportar_csv_teorico(const std::string& nombre) const;

    // -------- Registro SIMPLE (lo que usa tu main) --------
    void registrar(const std::string& operacion, double tiempo_ms, long memoria_kb);
    void mostrar_estadistica(const std::string& operacion, double tiempo_ms, long memoria_kb);
    void mostrar_resumen();
    void exportar_csv(const std::string& nombre_archivo = "estadisticas.csv");

private:
    static std::string csv_escape_(const std::string& s);

    // Cronómetro
    std::chrono::high_resolution_clock::time_point t0_{};

    // Log TEÓRICO
    std::vector<EntradaTeorica> te_log_;
    double te_total_ms_{0.0};

    // Log SIMPLE
    struct Registro {
        std::string operacion;
        double tiempo{0.0};     // ms
        long   memoria{0};       // KB (teóricos que pasas desde main)
    };
    std::vector<Registro> registros_;
    double total_tiempo_{0.0};
    long   max_memoria_kb_{0};
};

#endif // MONITOR_H
