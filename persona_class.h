#ifndef PERSONA_CLASS_H
#define PERSONA_CLASS_H

#include <string>
#include <iostream>
#include <iomanip>


class PersonaClass {
private:
    std::string nombre;           // Nombre de pila
    std::string apellido;         // Apellidos
    std::string id_cedula;               // Identificador único (cédula)
    std::string ciudadNacimiento; // Ciudad de nacimiento
    std::string fechaNacimiento;  // Fecha de nacimiento en formato DD/MM/AAAA
    double ingresosAnuales;       // Ingresos anuales en pesos colombianos
    double patrimonio;            // Patrimonio total (activos)
    double deudas;                // Deudas totales (pasivos)
    bool declaranteRenta;         // Si es declarante de renta
    char calendarioRenta;         // Calendario de renta (A/B/C)
    void calcularCalendarioRenta();


public:
   
    PersonaClass(std::string nom, std::string ape, std::string id_cedula, 
            std::string ciudad, std::string fecha, double ingresos, 
            double patri, double deud, bool declara);
    // Getters SIN copia de strings (devuelven referencias const)
  const std::string& getNombre()           const   { return nombre; }
  const std::string& getApellido()         const   { return apellido; }
  const std::string& getId()               const   { return id_cedula; }
  const std::string& getCiudadNacimiento() const   { return ciudadNacimiento; }
  const std::string& getFechaNacimiento()  const   { return fechaNacimiento; }

  


// Escalares: por valor (está bien)
  double getIngresosAnuales() const   { return ingresosAnuales; }
  double getPatrimonio()      const   { return patrimonio; }
  double getDeudas()          const   { return deudas; }
bool   getDeclaranteRenta() const   { return declaranteRenta; }
 char   getCalendarioRenta() const   { return calendarioRenta; }


  
    void mostrar() const;
    void mostrarResumen() const;
};

#endif // PERSONA_H





