#ifndef GENERADOR_H
#define GENERADOR_H

#include <string>
#include <vector>

struct PersonaStruct;
class  PersonaClass;

std::string generarFechaNacimiento();        // "DD/MM/AAAA", años 1960–2009
std::string generarID();                     // secuencial tipo cédula
double      randomDouble(double min, double max);

 
// PersonaStruct
PersonaStruct generarPersonaStruct();
std::vector<PersonaStruct> generarColeccionStruct(int n);
const PersonaStruct* buscarPorID(const std::vector<PersonaStruct>& v, const std::string& id);

// PersonaClass
PersonaClass generarPersonaClass();
std::vector<PersonaClass> generarColeccionClass(int n);
const PersonaClass* buscarPorID(const std::vector<PersonaClass>& v, const std::string& id);

#endif // GENERADOR_H
