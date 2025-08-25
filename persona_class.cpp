#include "persona_class.h"
#include <iomanip> // Para std::setprecision

void PersonaClass::calcularCalendarioRenta() {
    
    int ultimosDigitos = std::stoi(id_cedula.substr(id_cedula.length() - 2)); 
    if( ultimosDigitos >= 0 && ultimosDigitos <= 39) {
        this-> calendarioRenta = 'A';
    } else if( ultimosDigitos >= 40 && ultimosDigitos <= 79) {
        this-> calendarioRenta = 'B';
    } else {
        this-> calendarioRenta = 'C';
    }
}

PersonaClass::PersonaClass(std::string nom, std::string ape, std::string id_ced, std::string ciudad, std::string fecha,
    double ingresos, double patri, double deud, bool declara)  :
     nombre(std::move(nom)), 
      apellido(std::move(ape)), 
      id_cedula(std::move(id_ced)), 
      ciudadNacimiento(std::move(ciudad)),
      fechaNacimiento(std::move(fecha)), 
      ingresosAnuales(ingresos), 
      patrimonio(patri),
      deudas(deud), 
      declaranteRenta(declara)
       {
        this->calcularCalendarioRenta();
      }

void PersonaClass::mostrar() const {
    std::cout << "-------------------------------------\n";
    std::cout << "[" << id_cedula<< "] Nombre: " << nombre << " " << apellido << "\n";
    std::cout << "   - Ciudad de nacimiento: " << ciudadNacimiento << "\n";
    std::cout << "   - Fecha de nacimiento: " << fechaNacimiento << "\n\n";
    std::cout << std::fixed << std::setprecision(2); // Formato de números
    std::cout << "   - Ingresos anuales: $" << ingresosAnuales << "\n";
    std::cout << "   - Patrimonio: $" << patrimonio << "\n";
    std::cout << "   - Deudas: $" << deudas << "\n";
    std::cout << "   - Declarante de renta: " << (declaranteRenta ? "Sí" : "No") << "\n";
    std::cout << "   - Grupo calendario renta: " << calendarioRenta << "\n";
}


void PersonaClass::mostrarResumen() const {
    std::cout << "[" << id_cedula << "] " << nombre << " " << apellido
              << " | " << ciudadNacimiento 
              << " | $" << std::fixed << std::setprecision(2) << ingresosAnuales;
}