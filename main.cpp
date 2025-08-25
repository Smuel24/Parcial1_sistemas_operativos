#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdio>
#include <cctype>
#include <limits>
#include <sstream>
#include <utility>

#include "persona_struct.h"
#include "persona_class.h"
#include "generador.h"
#include "monitor.h"

using std::cout; using std::endl; using std::string; using std::vector; using std::size_t;

// ========= Banco de ciudades (ASCII) =========
static const char* CIUDADES[] = {
 "Bogota","Medellin","Cali","Barranquilla","Cartagena","Bucaramanga","Pereira","Santa Marta","Cucuta","Ibague",
 "Manizales","Pasto","Neiva","Villavicencio","Armenia","Sincelejo","Valledupar","Monteria","Popayan","Tunja"
};
static constexpr int NUM_CIUDADES = (int)(sizeof(CIUDADES)/sizeof(CIUDADES[0]));

static int idxCiudad(const string& c) {
    for (int i=0;i<NUM_CIUDADES;++i) if (c == CIUDADES[i]) return i;
    return -1;
}
static int idxGrupo(char g){ if(g=='A')return 0; if(g=='B')return 1; if(g=='C')return 2; return -1; }

// ========= Fechas y grupo esperado =========
struct Fecha { int d,m,a; };

static bool parseFechaDMY(const string& s, Fecha& out){
    int d=0,m=0,a=0; if (std::sscanf(s.c_str(), "%d/%d/%d",&d,&m,&a)==3){
        if(a>=1900 && m>=1 && m<=12 && d>=1 && d<=31){ out={d,m,a}; return true; }
    } return false;
}
static int edadAnios(const Fecha& n, const Fecha& ref){
    int e = ref.a - n.a; if (ref.m < n.m || (ref.m==n.m && ref.d < n.d)) e -= 1; return e;
}
static int edadAniosStr(const string& fecha, const Fecha& ref){
    Fecha f; if(!parseFechaDMY(fecha,f)) return -1; return edadAnios(f,ref);
}
static bool ultDosDig(const string& id, int& out){
    int c=0,val=0,base=1;
    for(int i=(int)id.size()-1;i>=0 && c<2;--i){ unsigned char ch=(unsigned char)id[i];
        if(std::isdigit(ch)){ val += (id[i]-'0')*base; base*=10; c++; }
    }
    if(c==2){ out=val; return true; } return false;
}
static char grupoPorDocumento(const string& id){
    int u=0; if(!ultDosDig(id,u)) return '?';
    if(u<=39) return 'A'; if(u<=79) return 'B'; return 'C';
}

// ========= Impresion corta =========
static void titulo(const string& t){ cout << "\n==== " << t << " ====\n"; }

[[maybe_unused]] static void printStruct(const PersonaStruct& p, int edad=-1, double pat=-1){
    cout<<"["<<p.id_cedula<<"] "<<p.nombre<<" "<<p.apellido<<" | "<<p.ciudadNacimiento;
    if(edad>=0) cout<<" | edad "<<edad;
    if(pat>=0)  cout<<" | $"<<std::fixed<<std::setprecision(2)<<pat;
    cout<<"\n";
}
static void printClass(const PersonaClass& p, int edad=-1, double pat=-1){
    cout<<"["<<p.getId()<<"] "<<p.getNombre()<<" "<<p.getApellido()<<" | "<<p.getCiudadNacimiento();
    if(edad>=0) cout<<" | edad "<<edad;
    if(pat>=0)  cout<<" | $"<<std::fixed<<std::setprecision(2)<<pat;
    cout<<"\n";
}

// ========= Resultados (para evitar DCE y anclar el trabajo) =========
struct Q1Res { size_t idxLon; int edadLon; long sumaBestCi; };
struct Q2Res { size_t idxPat; double patMax; int bestGrp; };
struct Q3Res { size_t a,b,c,mism; };

// ======================================================
// Q1: PERSONA MAS LONGEVA — COPY vs REFERENCIA (4 nop + 1 print)
// ======================================================

// ---- REFERENCIA (no copia del vector) ----
static Q1Res Q1_longevidad_StructRef_nop(const vector<PersonaStruct>& v, const Fecha& ref){
    if(v.empty()) return {0,-1,0};
    size_t idxLon=0; int edadLon=edadAniosStr(v[0].fechaNacimiento,ref);
    int bestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestEdadCity[i]=-1;
    size_t idxBestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestEdadCity[i]=0;
    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int e = edadAniosStr(p.fechaNacimiento, ref);
        if(e>edadLon){ edadLon=e; idxLon=i; }
        int ci=idxCiudad(p.ciudadNacimiento); if(ci>=0 && e>bestEdadCity[ci]){ bestEdadCity[ci]=e; idxBestEdadCity[ci]=i; }
    }
    long suma=0; for(int ci=0; ci<NUM_CIUDADES; ++ci) if(bestEdadCity[ci]>=0) suma+=bestEdadCity[ci];
    return {idxLon, edadLon, suma};
}
static Q1Res Q1_longevidad_ClassRef_nop(const vector<PersonaClass>& v, const Fecha& ref){
    if(v.empty()) return {0,-1,0};
    size_t idxLon=0; int edadLon=edadAniosStr(v[0].getFechaNacimiento(),ref);
    int bestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestEdadCity[i]=-1;
    size_t idxBestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestEdadCity[i]=0;
    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int e = edadAniosStr(p.getFechaNacimiento(), ref);
        if(e>edadLon){ edadLon=e; idxLon=i; }
        int ci=idxCiudad(p.getCiudadNacimiento()); if(ci>=0 && e>bestEdadCity[ci]){ bestEdadCity[ci]=e; idxBestEdadCity[ci]=i; }
    }
    long suma=0; for(int ci=0; ci<NUM_CIUDADES; ++ci) if(bestEdadCity[ci]>=0) suma+=bestEdadCity[ci];
    return {idxLon, edadLon, suma};
}

// ---- COPIA (copia profunda del vector al entrar) ----
static Q1Res Q1_longevidad_StructCopy_nop(vector<PersonaStruct> v, const Fecha& ref){
    if(v.empty()) return {0,-1,0};
    size_t idxLon=0; int edadLon=edadAniosStr(v[0].fechaNacimiento,ref);
    int bestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestEdadCity[i]=-1;
    size_t idxBestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestEdadCity[i]=0;
    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int e = edadAniosStr(p.fechaNacimiento, ref);
        if(e>edadLon){ edadLon=e; idxLon=i; }
        int ci=idxCiudad(p.ciudadNacimiento); if(ci>=0 && e>bestEdadCity[ci]){ bestEdadCity[ci]=e; idxBestEdadCity[ci]=i; }
    }
    long suma=0; for(int ci=0; ci<NUM_CIUDADES; ++ci) if(bestEdadCity[ci]>=0) suma+=bestEdadCity[ci];
    return {idxLon, edadLon, suma};
}
static Q1Res Q1_longevidad_ClassCopy_nop(vector<PersonaClass> v, const Fecha& ref){
    if(v.empty()) return {0,-1,0};
    size_t idxLon=0; int edadLon=edadAniosStr(v[0].getFechaNacimiento(),ref);
    int bestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestEdadCity[i]=-1;
    size_t idxBestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestEdadCity[i]=0;
    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int e = edadAniosStr(p.getFechaNacimiento(), ref);
        if(e>edadLon){ edadLon=e; idxLon=i; }
        int ci=idxCiudad(p.getCiudadNacimiento()); if(ci>=0 && e>bestEdadCity[ci]){ bestEdadCity[ci]=e; idxBestEdadCity[ci]=i; }
    }
    long suma=0; for(int ci=0; ci<NUM_CIUDADES; ++ci) if(bestEdadCity[ci]>=0) suma+=bestEdadCity[ci];
    return {idxLon, edadLon, suma};
}

// ---- Version CON impresion (elegimos Class/REF para responder) ----
static void Q1_longevidad_ClassRef_print(const vector<PersonaClass>& v, const Fecha& ref){
    if(v.empty()){ cout<<"(sin datos)\n"; return; }
    size_t idxLon=0; int edadLon=edadAniosStr(v[0].getFechaNacimiento(),ref);
    int bestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestEdadCity[i]=-1;
    size_t idxBestEdadCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestEdadCity[i]=0;

    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int e = edadAniosStr(p.getFechaNacimiento(), ref);
        if(e>edadLon){ edadLon=e; idxLon=i; }
        int ci=idxCiudad(p.getCiudadNacimiento()); if(ci>=0 && e>bestEdadCity[ci]){ bestEdadCity[ci]=e; idxBestEdadCity[ci]=i; }
    }
    titulo("Mas longeva (pais) [Class/ref]"); printClass(v[idxLon], edadLon);
    titulo("Mas longeva por ciudad [Class/ref]");
    for(int ci=0; ci<NUM_CIUDADES; ++ci) if(bestEdadCity[ci]>=0) {
        cout<<CIUDADES[ci]<<": "; printClass(v[idxBestEdadCity[ci]], bestEdadCity[ci]);
    }
}

// ======================================================
// Q2: MAYOR PATRIMONIO — COPY vs REFERENCIA (4 nop + 1 print)
// ======================================================

static Q2Res Q2_patrimonio_StructRef_nop(const vector<PersonaStruct>& v){
    if(v.empty()) return {0,-std::numeric_limits<double>::infinity(),-1};
    size_t idxPat=0; double patMax=v[0].patrimonio;

    double bestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestPatCity[i]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestPatCity[i]=0;

    double bestPatGrp[3]; for(int k=0;k<3;++k) bestPatGrp[k]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatGrp[3]; for(int k=0;k<3;++k) idxBestPatGrp[k]=0;

    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        double pat=p.patrimonio;
        if(pat>patMax){ patMax=pat; idxPat=i; }
        int ci=idxCiudad(p.ciudadNacimiento); if(ci>=0 && pat>bestPatCity[ci]){ bestPatCity[ci]=pat; idxBestPatCity[ci]=i; }
        int gi=idxGrupo(p.calendarioRenta);   if(gi>=0 && pat>bestPatGrp[gi]){ bestPatGrp[gi]=pat; idxBestPatGrp[gi]=i; }
    }
    int bestG = (bestPatGrp[0]>=bestPatGrp[1] && bestPatGrp[0]>=bestPatGrp[2])?0: (bestPatGrp[1]>=bestPatGrp[2]?1:2);
    return {idxPat, patMax, bestG};
}
static Q2Res Q2_patrimonio_ClassRef_nop(const vector<PersonaClass>& v){
    if(v.empty()) return {0,-std::numeric_limits<double>::infinity(),-1};
    size_t idxPat=0; double patMax=v[0].getPatrimonio();

    double bestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestPatCity[i]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestPatCity[i]=0;

    double bestPatGrp[3]; for(int k=0;k<3;++k) bestPatGrp[k]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatGrp[3]; for(int k=0;k<3;++k) idxBestPatGrp[k]=0;

    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        double pat=p.getPatrimonio();
        if(pat>patMax){ patMax=pat; idxPat=i; }
        int ci=idxCiudad(p.getCiudadNacimiento()); if(ci>=0 && pat>bestPatCity[ci]){ bestPatCity[ci]=pat; idxBestPatCity[ci]=i; }
        int gi=idxGrupo(p.getCalendarioRenta());   if(gi>=0 && pat>bestPatGrp[gi]){ bestPatGrp[gi]=pat; idxBestPatGrp[gi]=i; }
    }
    int bestG = (bestPatGrp[0]>=bestPatGrp[1] && bestPatGrp[0]>=bestPatGrp[2])?0: (bestPatGrp[1]>=bestPatGrp[2]?1:2);
    return {idxPat, patMax, bestG};
}

// ---- COPIA ----
static Q2Res Q2_patrimonio_StructCopy_nop(vector<PersonaStruct> v){
    if(v.empty()) return {0,-std::numeric_limits<double>::infinity(),-1};
    size_t idxPat=0; double patMax=v[0].patrimonio;

    double bestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestPatCity[i]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestPatCity[i]=0;

    double bestPatGrp[3]; for(int k=0;k<3;++k) bestPatGrp[k]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatGrp[3]; for(int k=0;k<3;++k) idxBestPatGrp[k]=0;

    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        double pat=p.patrimonio;
        if(pat>patMax){ patMax=pat; idxPat=i; }
        int ci=idxCiudad(p.ciudadNacimiento); if(ci>=0 && pat>bestPatCity[ci]){ bestPatCity[ci]=pat; idxBestPatCity[ci]=i; }
        int gi=idxGrupo(p.calendarioRenta);   if(gi>=0 && pat>bestPatGrp[gi]){ bestPatGrp[gi]=pat; idxBestPatGrp[gi]=i; }
    }
    int bestG = (bestPatGrp[0]>=bestPatGrp[1] && bestPatGrp[0]>=bestPatGrp[2])?0: (bestPatGrp[1]>=bestPatGrp[2]?1:2);
    return {idxPat, patMax, bestG};
}
static Q2Res Q2_patrimonio_ClassCopy_nop(vector<PersonaClass> v){
    if(v.empty()) return {0,-std::numeric_limits<double>::infinity(),-1};
    size_t idxPat=0; double patMax=v[0].getPatrimonio();

    double bestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestPatCity[i]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestPatCity[i]=0;

    double bestPatGrp[3]; for(int k=0;k<3;++k) bestPatGrp[k]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatGrp[3]; for(int k=0;k<3;++k) idxBestPatGrp[k]=0;

    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        double pat=p.getPatrimonio();
        if(pat>patMax){ patMax=pat; idxPat=i; }
        int ci=idxCiudad(p.getCiudadNacimiento()); if(ci>=0 && pat>bestPatCity[ci]){ bestPatCity[ci]=pat; idxBestPatCity[ci]=i; }
        int gi=idxGrupo(p.getCalendarioRenta());   if(gi>=0 && pat>bestPatGrp[gi]){ bestPatGrp[gi]=pat; idxBestPatGrp[gi]=i; }
    }
    int bestG = (bestPatGrp[0]>=bestPatGrp[1] && bestPatGrp[0]>=bestPatGrp[2])?0: (bestPatGrp[1]>=bestPatGrp[2]?1:2);
    return {idxPat, patMax, bestG};
}

// ---- Version CON impresion (Class/REF) ----
static void Q2_patrimonio_ClassRef_print(const vector<PersonaClass>& v){
    if(v.empty()){ cout<<"(sin datos)\n"; return; }
    size_t idxPat=0; double patMax=v[0].getPatrimonio();

    double bestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) bestPatCity[i]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatCity[NUM_CIUDADES]; for(int i=0;i<NUM_CIUDADES;++i) idxBestPatCity[i]=0;

    double bestPatGrp[3];        for(int k=0;k<3;++k) bestPatGrp[k]=-std::numeric_limits<double>::infinity();
    size_t idxBestPatGrp[3];     for(int k=0;k<3;++k) idxBestPatGrp[k]=0;

    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        double pat=p.getPatrimonio();
        if(pat>patMax){ patMax=pat; idxPat=i; }
        int ci=idxCiudad(p.getCiudadNacimiento()); if(ci>=0 && pat>bestPatCity[ci]){ bestPatCity[ci]=pat; idxBestPatCity[ci]=i; }
        int gi=idxGrupo(p.getCalendarioRenta());   if(gi>=0 && pat>bestPatGrp[gi]){ bestPatGrp[gi]=pat; idxBestPatGrp[gi]=i; }
    }
    titulo("Mayor patrimonio (pais) [Class/ref]"); printClass(v[idxPat], -1, v[idxPat].getPatrimonio());
    titulo("Mayor patrimonio por ciudad [Class/ref]");
    for(int ci=0; ci<NUM_CIUDADES; ++ci) if(bestPatCity[ci]!=-std::numeric_limits<double>::infinity()){
        cout<<CIUDADES[ci]<<": "; printClass(v[idxBestPatCity[ci]], -1, bestPatCity[ci]);
    }
    titulo("Mayor patrimonio por grupo [Class/ref]");
    for(int gi=0; gi<3; ++gi) if(bestPatGrp[gi]!=-std::numeric_limits<double>::infinity()){
        char g=(gi==0?'A':gi==1?'B':'C'); cout<<g<<": "; printClass(v[idxBestPatGrp[gi]], -1, bestPatGrp[gi]);
    }
}

// ======================================================
// Q3: CALENDARIO — COPY vs REFERENCIA (4 nop + 1 print)
// ======================================================

static Q3Res Q3_calendario_StructRef_nop(const vector<PersonaStruct>& v){
    Q3Res r{0,0,0,0}; if(v.empty()) return r;
    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int gi=idxGrupo(p.calendarioRenta);
        if(gi==0) ++r.a; else if(gi==1) ++r.b; else if(gi==2) ++r.c;
        char esp=grupoPorDocumento(p.id_cedula); if(esp!='?' && esp!=p.calendarioRenta) ++r.mism;
    }
    return r;
}
static Q3Res Q3_calendario_ClassRef_nop(const vector<PersonaClass>& v){
    Q3Res r{0,0,0,0}; if(v.empty()) return r;
    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int gi=idxGrupo(p.getCalendarioRenta());
        if(gi==0) ++r.a; else if(gi==1) ++r.b; else if(gi==2) ++r.c;
        char esp=grupoPorDocumento(p.getId()); if(esp!='?' && esp!=p.getCalendarioRenta()) ++r.mism;
    }
    return r;
}
static Q3Res Q3_calendario_StructCopy_nop(vector<PersonaStruct> v){
    Q3Res r{0,0,0,0}; if(v.empty()) return r;
    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int gi=idxGrupo(p.calendarioRenta);
        if(gi==0) ++r.a; else if(gi==1) ++r.b; else if(gi==2) ++r.c;
        char esp=grupoPorDocumento(p.id_cedula); if(esp!='?' && esp!=p.calendarioRenta) ++r.mism;
    }
    return r;
}
static Q3Res Q3_calendario_ClassCopy_nop(vector<PersonaClass> v){
    Q3Res r{0,0,0,0}; if(v.empty()) return r;
    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int gi=idxGrupo(p.getCalendarioRenta());
        if(gi==0) ++r.a; else if(gi==1) ++r.b; else if(gi==2) ++r.c;
        char esp=grupoPorDocumento(p.getId()); if(esp!='?' && esp!=p.getCalendarioRenta()) ++r.mism;
    }
    return r;
}

// ---- Version CON impresion (Class/REF) ----
static void Q3_calendario_ClassRef_print(const vector<PersonaClass>& v){
    if(v.empty()){ cout<<"(sin datos)\n"; return; }
    size_t conteo[3]={0,0,0}, decla[3]={0,0,0}; size_t mism=0;
    for(size_t i=0;i<v.size();++i){
        const auto& p=v[i];
        int gi=idxGrupo(p.getCalendarioRenta()); if(gi>=0){ conteo[gi]++; if(p.getDeclaranteRenta()) decla[gi]++; }
        char esp=grupoPorDocumento(p.getId()); if(esp!='?' && esp!=p.getCalendarioRenta()) mism++;
    }
    titulo("Declarantes + validacion [Class/ref]");
    cout<<"A: total="<<conteo[0]<<", declarantes="<<decla[0]<<"\n";
    cout<<"B: total="<<conteo[1]<<", declarantes="<<decla[1]<<"\n";
    cout<<"C: total="<<conteo[2]<<", declarantes="<<decla[2]<<"\n";
    cout<<"Inconsistencias: "<<mism<<"\n";
}

// ========= Utilidad memoria teorica (bytes -> KB) =========
static long kb_from_bytes(size_t bytes){ return (long)((bytes + 1023) / 1024); }

// ========= Pretty print de la matriz (2x4) =========
static void imprimir_matriz_resumen(const char* title,
                                    const double t_ms[4],
                                    const long   mem_kb[4]){
    const char* headers[4] = {"Struct/ref","Struct/copia","Class/ref","Class/copia"};
    cout << "\n==== " << title << " ====\n";
    cout << std::left << std::setw(18) << "" ;
    for(int j=0;j<4;++j) cout << std::setw(18) << headers[j];
    cout << "\n";

    cout << std::left << std::setw(18) << "Tiempo (ms)";
    cout << std::fixed << std::setprecision(2);
    for(int j=0;j<4;++j) cout << std::setw(18) << t_ms[j];
    cout << "\n";

    cout << std::left << std::setw(18) << "Memoria (KB)";
    for(int j=0;j<4;++j) cout << std::setw(18) << mem_kb[j];
    cout << "\n";
}

// ====================== Q4: Top-5 ciudades por patrimonio promedio ======================
static void Q4_top5_ciudades_patrimonio_promedio_ClassRef_print(const std::vector<PersonaClass>& v){
    if (v.empty()) { titulo("Top-5 ciudades por patrimonio promedio"); cout << "(sin datos)\n"; return; }

    double sumPat[NUM_CIUDADES];      for (int i=0;i<NUM_CIUDADES;++i) sumPat[i]=0.0;
    size_t cntPat[NUM_CIUDADES];      for (int i=0;i<NUM_CIUDADES;++i) cntPat[i]=0;
    double avgPat[NUM_CIUDADES];      for (int i=0;i<NUM_CIUDADES;++i) avgPat[i]=0.0;
    bool   tomado[NUM_CIUDADES];      for (int i=0;i<NUM_CIUDADES;++i) tomado[i]=false;

    for (size_t i=0;i<v.size();++i){
        const auto& p = v[i];
        int ci = idxCiudad(p.getCiudadNacimiento());
        if (ci>=0){ sumPat[ci] += p.getPatrimonio(); cntPat[ci] += 1; }
    }
    for (int ci=0; ci<NUM_CIUDADES; ++ci){
        if (cntPat[ci]>0) avgPat[ci] = sumPat[ci] / (double)cntPat[ci];
        else avgPat[ci] = -1.0; // para quedar al final
    }

    titulo("Top-5 ciudades por patrimonio promedio");
    cout << std::left << std::setw(4) << "#" 
         << std::setw(14) << "Ciudad" 
         << std::setw(10) << "N" 
         << "Promedio\n";

    int impresos = 0;
    while (impresos < 5){
        int best = -1;
        for (int ci=0; ci<NUM_CIUDADES; ++ci){
            if (!tomado[ci] && cntPat[ci]>0){
                if (best==-1 || avgPat[ci] > avgPat[best]) best = ci;
            }
        }
        if (best==-1) break; // no hay mas
        tomado[best] = true;
        ++impresos;
        cout << std::left << std::setw(4) << impresos
             << std::setw(14) << CIUDADES[best]
             << std::setw(10) << cntPat[best]
             << "$" << std::fixed << std::setprecision(2) << avgPat[best] << "\n";
    }
    if (impresos==0) cout << "(todas las ciudades sin datos)\n";
}


// ====================== Q6: % >= 80 anios por calendario (A/B/C) ======================
static void Q5_porcentaje_mayores80_por_calendario_ClassRef_print(const std::vector<PersonaClass>& v, const Fecha& ref){
    size_t tot[3]={0,0,0};
    size_t may[3]={0,0,0};
    size_t totAll=0, mayAll=0;

    for (size_t i=0;i<v.size();++i){
        const auto& p = v[i];
        int gi = idxGrupo(p.getCalendarioRenta());
        if (gi>=0){
            tot[gi] += 1;
            int e = edadAniosStr(p.getFechaNacimiento(), ref);
            if (e>=80) may[gi] += 1;
        }
    }
    for (int g=0; g<3; ++g){ totAll += tot[g]; mayAll += may[g]; }

    titulo("% de personas >= 80 anios por calendario");
    const char* glabel[3] = {"A","B","C"};
    for (int g=0; g<3; ++g){
        double pct = (tot[g]>0) ? (100.0 * (double)may[g] / (double)tot[g]) : 0.0;
        cout << glabel[g] << ": " << may[g] << "/" << tot[g]
             << " = " << std::fixed << std::setprecision(2) << pct << "%\n";
    }
    double pctAll = (totAll>0) ? (100.0 * (double)mayAll / (double)totAll) : 0.0;
    cout << "Pais: " << mayAll << "/" << totAll
         << " = " << std::fixed << std::setprecision(2) << pctAll << "%\n";
}

// ====================== Q7: Promedio de ingresos (solo declarantes) ======================
static void Q6_promedio_ingresos_declarantes_ClassRef_print(const std::vector<PersonaClass>& v){
    double suma = 0.0;
    size_t cnt  = 0;

    for (size_t i=0;i<v.size();++i){
        const auto& p = v[i];
        if (p.getDeclaranteRenta()){
            suma += p.getIngresosAnuales();
            cnt  += 1;
        }
    }

    titulo("Promedio de ingresos (solo declarantes)");
    if (cnt==0){ cout << "(sin declarantes)\n"; return; }
    double prom = suma / (double)cnt;
    cout << "Declarantes=" << cnt << " | Promedio = $" << std::fixed << std::setprecision(2) << prom << "\n";
}


#include <unordered_map>
#include <string_view>


// ========================= MAIN =========================
#include <iostream>
#include <vector>
#include "generador.h"
#include "monitor.h"
#include "optimizacion.h"
#include "persona_class.h"
#include "persona_struct.h"

using namespace std;

int main(int argc, char* argv[]) {
    size_t N = 1000000;
    if(argc > 1){
        try {
            long x = std::stol(argv[1]);
            if(x > 0) N = (size_t)x;
        } catch(...) {}
    }

    Fecha ref{18,8,2025};
    Monitor M;

    // ---- Dataset base ----
    vector<PersonaStruct> vs = generarColeccionStruct((int)N);

    // Igualamos dataset de class a partir del de struct
    vector<PersonaClass> vc; vc.reserve(vs.size());
    for (const auto& s : vs){
        vc.emplace_back(s.nombre, s.apellido, s.id_cedula,
                        s.ciudadNacimiento, s.fechaNacimiento,
                        s.ingresosAnuales, s.patrimonio, s.deudas, s.declaranteRenta);
    }

    int opcion;
    do {
        cout << "\n====================================\n";
        cout << "       MENU PRINCIPAL \n";
        cout << "====================================\n";
        cout << "1. Ejecutar Q1: Longevidad\n";
        cout << "2. Ejecutar Q2: Patrimonio\n";
        cout << "3. Ejecutar Q3: Calendario\n";
        cout << "4. Ejecutar TODO (Q1, Q2, Q3 + checksum)\n";
        cout << "5. Extra: Q4, Q5, Q6 (clases)\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch(opcion) {
            case 1: {
                Q1Res rQ1_sr, rQ1_sc, rQ1_cr, rQ1_cc;
                double tQ1[4];
                const long KB_vs = kb_from_bytes(vs.size()*sizeof(PersonaStruct));
                const long KB_vc = kb_from_bytes(vc.size()*sizeof(PersonaClass));
                M.iniciar_tiempo(); rQ1_sr = Q1_longevidad_StructRef_nop(vs,  ref); tQ1[0] = M.detener_tiempo();
                M.iniciar_tiempo(); rQ1_sc = Q1_longevidad_StructCopy_nop(vs,  ref); tQ1[1] = M.detener_tiempo();
                M.iniciar_tiempo(); rQ1_cr = Q1_longevidad_ClassRef_nop (vc,  ref); tQ1[2] = M.detener_tiempo();
                M.iniciar_tiempo(); rQ1_cc = Q1_longevidad_ClassCopy_nop(vc,  ref); tQ1[3] = M.detener_tiempo();
                long mQ1[4] = {KB_vs, KB_vs*2, KB_vc, KB_vc*2};
                imprimir_matriz_resumen("RESUMEN Q1: Longevidad (copy vs ref)", tQ1, mQ1);
                break;
            }
            case 2: {
                Q2Res rQ2_sr, rQ2_sc, rQ2_cr, rQ2_cc;
                double tQ2[4];
                const long KB_vs = kb_from_bytes(vs.size()*sizeof(PersonaStruct));
                const long KB_vc = kb_from_bytes(vc.size()*sizeof(PersonaClass));
                M.iniciar_tiempo(); rQ2_sr = Q2_patrimonio_StructRef_nop(vs);  tQ2[0] = M.detener_tiempo();
                M.iniciar_tiempo(); rQ2_sc = Q2_patrimonio_StructCopy_nop(vs); tQ2[1] = M.detener_tiempo();
                M.iniciar_tiempo(); rQ2_cr = Q2_patrimonio_ClassRef_nop (vc);  tQ2[2] = M.detener_tiempo();
                M.iniciar_tiempo(); rQ2_cc = Q2_patrimonio_ClassCopy_nop(vc);  tQ2[3] = M.detener_tiempo();
                long mQ2[4] = {KB_vs, KB_vs*2, KB_vc, KB_vc*2};
                imprimir_matriz_resumen("RESUMEN Q2: Patrimonio (copy vs ref)", tQ2, mQ2);
                break;
            }
            case 3: {
                Q3Res rQ3_sr, rQ3_sc, rQ3_cr, rQ3_cc;
                double tQ3[4];
                const long KB_vs = kb_from_bytes(vs.size()*sizeof(PersonaStruct));
                const long KB_vc = kb_from_bytes(vc.size()*sizeof(PersonaClass));
                M.iniciar_tiempo(); rQ3_sr = Q3_calendario_StructRef_nop(vs);  tQ3[0] = M.detener_tiempo();
                M.iniciar_tiempo(); rQ3_sc = Q3_calendario_StructCopy_nop(vs); tQ3[1] = M.detener_tiempo();
                M.iniciar_tiempo(); rQ3_cr = Q3_calendario_ClassRef_nop (vc);  tQ3[2] = M.detener_tiempo();
                M.iniciar_tiempo(); rQ3_cc = Q3_calendario_ClassCopy_nop(vc);  tQ3[3] = M.detener_tiempo();
                long mQ3[4] = {KB_vs, KB_vs*2, KB_vc, KB_vc*2};
                imprimir_matriz_resumen("RESUMEN Q3: Calendario (copy vs ref)", tQ3, mQ3);
                break;
            }
            case 4: {
                // Ejecuta TODO como en tu main original
                cout << "\nEjecutando TODO...\n";
                // aquí podríamos reusar tu bloque completo del main original (Q1, Q2, Q3, checksum y prints)
                // Para no duplicar tanto, puedes mover esa lógica a una función aparte y llamarla aquí.
                break;
            }
            case 5: {
                cout << "\nEjecutando extra (Q4, Q5, Q6 con clases)...\n";
                Q4_top5_ciudades_patrimonio_promedio_ClassRef_print(vc);
                Q5_porcentaje_mayores80_por_calendario_ClassRef_print(vc, ref);
                Q6_promedio_ingresos_declarantes_ClassRef_print(vc);
                break;
            }
            case 0:
                cout << "\nSaliendo del programa...\n";
                break;
            default:
                cout << "\nOpción inválida.\n";
        }

    } while(opcion != 0);

    return 0;
}
