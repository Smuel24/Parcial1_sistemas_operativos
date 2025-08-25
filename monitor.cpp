#include "monitor.h"

// ---- Cronómetro ----
void Monitor::iniciar_tiempo() {
    t0_ = std::chrono::high_resolution_clock::now();
}

double Monitor::detener_tiempo() const {
    const auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dur = t1 - t0_;
    return dur.count();
}

// =============== Registro TEÓRICO (opcional) ===============
void Monitor::registrar_teorico(const EntradaTeorica& e) {
    te_log_.push_back(e);
    te_total_ms_ += e.ms;
}

void Monitor::mostrar_estadistica_teorica (const EntradaTeorica& e) const {
    std::cout << "\n[TEÓRICO] " << e.tag
              << " | Tiempo: " << std::fixed << std::setprecision(2) << e.ms << " ms"
              << " | Bytes(teórico): " << e.bytes_teoricos
              << " (" << formato_bytes(e.bytes_teoricos) << ")\n";
}

void Monitor::mostrar_resumen_teorico() const {
    std::cout << "\n=== RESUMEN TEÓRICO ===\n";
    for (const auto& r : te_log_) {
        std::cout << r.tag << ": "
                  << std::fixed << std::setprecision(2) << r.ms << " ms, "
                  << r.bytes_teoricos << " B ("
                  << formato_bytes(r.bytes_teoricos) << ")\n";
    }
    std::cout << "Total tiempo medido: "
              << std::fixed << std::setprecision(2) << te_total_ms_ << " ms\n";
}

void Monitor::exportar_csv_teorico(const std::string& nombre) const {
    std::ofstream f(nombre, std::ios::out | std::ios::trunc);
    if (!f) {
        std::cerr << "Error al abrir archivo: " << nombre << "\n";
        return;
    }
    f << "Tag,Tiempo(ms),Bytes_teoricos\n";
    f << std::fixed << std::setprecision(2);
    for (const auto& r : te_log_) {
        f << csv_escape_(r.tag) << ","
          << r.ms << ","
          << r.bytes_teoricos << "\n";
    }
    f.close();
    std::cout << "Exportado TEÓRICO a " << nombre << "\n";
}

// --- util CSV (escapa comillas/comas) ---
std::string Monitor::csv_escape_(const std::string& s) {
    bool needs_quotes = false;
    for (char c : s) {
        if (c == ',' || c == '"' || c == '\n' || c == '\r') { needs_quotes = true; break; }
    }
    if (!needs_quotes) return s;
    std::string out; out.reserve(s.size()+2);
    out.push_back('"');
    for (char c : s) { if (c=='"') out.push_back('"'); out.push_back(c); }
    out.push_back('"');
    return out;
}

// =============== Registro SIMPLE ===============
void Monitor::registrar(const std::string& operacion, double tiempo_ms, long memoria_kb) {
    registros_.push_back(Registro{operacion, tiempo_ms, memoria_kb});
    total_tiempo_ += tiempo_ms;
    if (memoria_kb > max_memoria_kb_) max_memoria_kb_ = memoria_kb;
}

void Monitor::mostrar_estadistica(const std::string& operacion, double tiempo_ms, long memoria_kb) {
    std::cout << "\n[ESTADÍSTICAS] " << operacion
              << "  Tiempo: "  << std::fixed << std::setprecision(2) << tiempo_ms << " ms"
              << "  Memoria: " << memoria_kb << " KB\n";
}

void Monitor::mostrar_resumen() {
    std::cout << "\n=== RESUMEN DE ESTADÍSTICAS ===\n";
    for (const auto& r : registros_) {
        std::cout << r.operacion << ": "
                  << std::fixed << std::setprecision(2) << r.tiempo << " ms, "
                  << r.memoria << " KB\n";
    }
    std::cout << "Total tiempo: "  << std::fixed << std::setprecision(2)
              << total_tiempo_ << " ms\n";
    std::cout << "Memoria máxima: " << max_memoria_kb_ << " KB\n";
}

void Monitor::exportar_csv(const std::string& nombre_archivo) {
    std::ofstream archivo(nombre_archivo, std::ios::out | std::ios::trunc);
    if (!archivo) {
        std::cerr << "Error al abrir archivo: " << nombre_archivo << "\n";
        return;
    }
    archivo << "Operacion,Tiempo(ms),Memoria(KB)\n";
    archivo << std::fixed << std::setprecision(2);
    for (const auto& r : registros_) {
        archivo << csv_escape_(r.operacion) << ","
                << r.tiempo << ","
                << r.memoria << "\n";
    }
    archivo.close();
    std::cout << "Estadísticas exportadas a " << nombre_archivo << "\n";
}
