#include "generador.h"

#include <random>
#include <string>
#include <vector>
#include <cstdio> 


#include "persona_struct.h"   
#include "persona_class.h"    

// ---------------- RNG y helpers ----------------
static std::mt19937& rng() {
    static std::mt19937 gen{ std::random_device{}() };
    return gen;
}
static int uniformInt(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng());
}

double randomDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng());
}
// ---------------- Bancos de datos (ASCII puro) ----------------
static const std::vector<std::string> NOMBRES_F = {
    "Maria","Luisa","Carmen","Ana","Sofia","Isabel","Laura","Andrea","Paula","Valentina",
    "Camila","Daniela","Carolina","Fernanda","Gabriela","Patricia","Claudia","Diana","Lucia","Ximena"
};
static const std::vector<std::string> NOMBRES_M = {
    "Juan","Carlos","Jose","James","Andres","Miguel","Luis","Pedro","Alejandro","Ricardo",
    "Felipe","David","Jorge","Santiago","Daniel","Fernando","Diego","Rafael","Martin","Oscar",
    "Edison","Nestor","Gertridis"
};
static const std::vector<std::string> APELLIDOS = {
    "Gomez","Rodriguez","Martinez","Lopez","Garcia","Perez","Gonzalez","Sanchez","Ramirez","Torres",
    "Diaz","Vargas","Castro","Ruiz","Alvarez","Romero","Suarez","Rojas","Moreno","Munoz","Valencia"
};
static const std::vector<std::string> CIUDADES = {
    "Bogota","Medellin","Cali","Barranquilla","Cartagena","Bucaramanga","Pereira","Santa Marta","Cucuta","Ibague",
    "Manizales","Pasto","Neiva","Villavicencio","Armenia","Sincelejo","Valledupar","Monteria","Popayan","Tunja"
};


// ---------------- Primitivas públicas ----------------
std::string generarFechaNacimiento() {
    int anio = uniformInt(1960, 2009);
    int mes  = uniformInt(1, 12);
    int dia  = uniformInt(1, 28); // evita problemas de fin de mes
    char buf[11];
    std::snprintf(buf, sizeof(buf), "%02d/%02d/%04d", dia, mes, anio);
    return std::string(buf);
}

std::string generarID() {
    static long long contador = 1'000'000'000LL;  
    return std::to_string(contador++);
}

// ---------------- Núcleo: generar campos básicos ----------------
struct Campos {
    std::string nombre, apellido, id, ciudad, fecha;
    double ingresos, patrimonio, deudas;
    bool declarante;
};

static Campos generarCampos() {
    Campos c;
    const bool esHombre = static_cast<bool>(uniformInt(0, 1));
    const auto& N = esHombre ? NOMBRES_M : NOMBRES_F;

    c.nombre  = N[uniformInt(0, (int)N.size() - 1)];
    c.apellido = APELLIDOS[uniformInt(0, (int)APELLIDOS.size() - 1)]
               + std::string(" ")
               + APELLIDOS[uniformInt(0, (int)APELLIDOS.size() - 1)];
    c.id      = generarID();
    c.ciudad  = CIUDADES[uniformInt(0, (int)CIUDADES.size() - 1)];
    c.fecha   = generarFechaNacimiento();

    c.ingresos   = randomDouble(10'000'000.0, 500'000'000.0);
    c.patrimonio = randomDouble(0.0,         2'000'000'000.0);
    c.deudas     = randomDouble(0.0,         c.patrimonio * 0.7);

    const bool umbral = (c.ingresos > 50'000'000.0);
    const bool prob70 = (uniformInt(1, 100) > 30);  
    c.declarante = (umbral && prob70);

    return c;
}

// ---------------- Construcción PersonaStruct ----------------
PersonaStruct generarPersonaStruct() {
    const Campos c = generarCampos();
    PersonaStruct p{
        c.nombre, c.apellido, c.id, c.ciudad, c.fecha,
        c.ingresos, c.patrimonio, c.deudas, c.declarante,
        '\0'  
    };
    p.fillCalendario();
    return p;
}

std::vector<PersonaStruct> generarColeccionStruct(int n) {
    std::vector<PersonaStruct> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i) v.push_back(generarPersonaStruct());
    return v;
}

const PersonaStruct* buscarPorID(const std::vector<PersonaStruct>& v, const std::string& id) {
    for (const auto& p : v) if (p.id_cedula == id) return &p;
    return nullptr;
}

// ---------------- Construcción PersonaClass ----------------
PersonaClass generarPersonaClass() {
    const Campos c = generarCampos();
    // (nom, ape, id, ciudad, fecha, ingresos, patri, deud, declara)
    return PersonaClass(
        c.nombre, c.apellido, c.id, c.ciudad, c.fecha,
        c.ingresos, c.patrimonio, c.deudas, c.declarante
    ); // la clase calcula su calendario internamente
}

std::vector<PersonaClass> generarColeccionClass(int n) {
    std::vector<PersonaClass> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i) v.push_back(generarPersonaClass());
    return v;
}

const PersonaClass* buscarPorID(const std::vector<PersonaClass>& v, const std::string& id) {
    for (const auto& p : v) if (p.getId() == id) return &p;
    return nullptr;
}
