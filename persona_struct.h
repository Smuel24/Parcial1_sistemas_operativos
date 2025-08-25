#ifndef PERSONA_STRUCT_H
#define PERSONA_STRUCT_H

#include <string>
#include <iostream>
#include <iomanip>


struct PersonaStruct {
    // Datos básicos de identificación
    std::string nombre;           // Nombre de pila
    std::string apellido;         // Apellidos
    std::string id_cedula;               // Identificador único
    std::string ciudadNacimiento; // Ciudad de nacimiento en Colombia
    std::string fechaNacimiento;  // Fecha en formato DD/MM/AAAA
    
    // Datos fiscales y económicos
    double ingresosAnuales;       // Ingresos anuales en pesos colombianos
    double patrimonio;            // Valor total de bienes y activos
    double deudas;                // Deudas pendientes
    bool declaranteRenta;         // Si está obligado a declarar renta
    char calendarioRenta;

    // --- Métodos de visualización ---
    void mostrar() const;         // Muestra todos los detalles completos
    void mostrarResumen() const;  // Muestra versión compacta para listados

    void fillCalendario(); 

 
};


inline void PersonaStruct::fillCalendario(){

    int ultimosDigitos = std::stoi(id_cedula.substr(id_cedula.length() - 2)); 
    if( ultimosDigitos >= 0 && ultimosDigitos <= 39) {
        calendarioRenta = 'A';
    } else if( ultimosDigitos >= 40 && ultimosDigitos <= 79) {
        calendarioRenta = 'B';
    } else {
        calendarioRenta = 'C';
    }

}

// Implementación de métodos inline para mantener la estructura simple
inline void PersonaStruct::mostrar() const {
    std::cout << "-------------------------------------\n";
    std::cout << "[" << id_cedula << "] Nombre: " << nombre << " " << apellido << "\n";
    std::cout << "   - Ciudad de nacimiento: " << ciudadNacimiento << "\n";
    std::cout << "   - Fecha de nacimiento: " << fechaNacimiento << "\n\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "   - Ingresos anuales: $" << ingresosAnuales << "\n";
    std::cout << "   - Patrimonio: $" << patrimonio << "\n";
    std::cout << "   - Deudas: $" << deudas << "\n";
    std::cout << "   - Declarante de renta: " << (declaranteRenta ? "Sí" : "No") << "\n";
    std::cout << "   - Calendario de dian: " << calendarioRenta << "\n";
}

inline void PersonaStruct::mostrarResumen() const {
    std::cout << "[" << id_cedula << "] " << nombre << " " << apellido
              << " | " << ciudadNacimiento 
              << " | $" << std::fixed << std::setprecision(2) << ingresosAnuales;
}

#endif // PERSONA_H

  