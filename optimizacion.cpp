#include "optimizacion.h"
#include <cstdio>
#include <cctype>

// ======= Helpers estáticos =======
bool Optimizacion::parseFechaDMY_(const std::string& s, Fecha& out){
    int d=0,m=0,a=0;
    if (std::sscanf(s.c_str(), "%d/%d/%d",&d,&m,&a)==3){
        if(a>=1900 && m>=1 && m<=12 && d>=1 && d<=28){ out={d,m,a}; return true; }
    }
    return false;
}
int Optimizacion::edadAnios_(const Fecha& n, const Fecha& ref){
    int e = ref.a - n.a;
    if (ref.m < n.m || (ref.m==n.m && ref.d < n.d)) e -= 1;
    return e;
}
int Optimizacion::edadAniosStr_(const std::string& fecha, const Fecha& ref){
    Fecha f; if(!parseFechaDMY_(fecha,f)) return -1; return edadAnios_(f,ref);
}
bool Optimizacion::ultDosDig_(const std::string& id, int& out){
    int c=0,val=0,base=1;
    for(int i=(int)id.size()-1;i>=0 && c<2;--i){
        unsigned char ch=(unsigned char)id[i];
        if(std::isdigit(ch)){ val += (id[i]-'0')*base; base*=10; c++; }
    }
    if(c==2){ out=val; return true; }
    return false;
}
char Optimizacion::grupoPorDocumento_(const std::string& id){
    int u=0; if(!ultDosDig_(id,u)) return '?';
    if(u<=39) return 'A'; if(u<=79) return 'B'; return 'C';
}
int Optimizacion::idxCiudad_(const std::string& c) const {
    for (int i=0;i<numCiudades_;++i) if (c == ciudades_[i]) return i;
    return -1;
}
void Optimizacion::printClass_(const PersonaClass& p, int edad, double pat){
    std::cout<<"["<<p.getId()<<"] "<<p.getNombre()<<" "<<p.getApellido()
             <<" | "<<p.getCiudadNacimiento();
    if(edad>=0) std::cout<<" | edad "<<edad;
    if(pat>=0)  std::cout<<" | $"<<std::fixed<<std::setprecision(2)<<pat;
    std::cout<<"\n";
}

// ======= Constructor =======
Optimizacion::Optimizacion(const std::vector<PersonaClass>& data,
                           int ref_d, int ref_m, int ref_a,
                           const char* const* ciudades, int num_ciudades)
    : v_(data), ref_{ref_d,ref_m,ref_a}, ciudades_(ciudades) {
    numCiudades_ = (num_ciudades>MAX_CIUDADES_OPT? MAX_CIUDADES_OPT : num_ciudades);
    initArrays_();
    build_();
    postprocess_();
}

// ======= Inicialización de acumuladores =======
void Optimizacion::initArrays_(){
    rq1_.idxLonPais=-1; rq1_.edadLonPais=-1;
    for (int i=0;i<MAX_CIUDADES_OPT;++i){ rq1_.bestEdadCity[i]=-1; rq1_.idxBestEdadCity[i]=0; }

    rq2_.idxPatPais=0; rq2_.patMaxPais=-std::numeric_limits<double>::infinity();
    for (int i=0;i<MAX_CIUDADES_OPT;++i){ rq2_.bestPatCity[i]=-std::numeric_limits<double>::infinity(); rq2_.idxBestPatCity[i]=0; }
    for (int g=0; g<3; ++g){ rq2_.bestPatGrp[g]=-std::numeric_limits<double>::infinity(); rq2_.idxBestPatGrp[g]=0; }

    for (int g=0; g<3; ++g){ rq3_.totGrp[g]=0; rq3_.declaGrp[g]=0; } rq3_.mismatches=0;

    for (int i=0;i<MAX_CIUDADES_OPT;++i){ rq4_.sumPatCity[i]=0.0; rq4_.cntCity[i]=0; }
    for (int k=0;k<5;++k) rq4_.topIdx[k]=-1;

    for (int g=0; g<3; ++g){ rq6_.tot[g]=0; rq6_.may80[g]=0; rq6_.pct[g]=0.0; }
    rq6_.pctPais=0.0;

    rq7_.declarantes=0; rq7_.promedioIngresos=0.0;
}

// ======= Único barrido O(N) para todas las métricas =======
void Optimizacion::build_(){
    for (size_t i=0;i<v_.size();++i){
        const auto& p = v_[i];

        int ci = idxCiudad_(p.getCiudadNacimiento());
        int gi = (p.getCalendarioRenta()=='A'?0 : p.getCalendarioRenta()=='B'?1 :
                 (p.getCalendarioRenta()=='C'?2 : -1));
        int e  = edadAniosStr_(p.getFechaNacimiento(), ref_);

        // Q1: longevidad país / ciudad
        if (e > rq1_.edadLonPais){ rq1_.edadLonPais=e; rq1_.idxLonPais=i; }
        if (ci>=0){
            if (e > rq1_.bestEdadCity[ci]){ rq1_.bestEdadCity[ci]=e; rq1_.idxBestEdadCity[ci]=i; }
        }

        // Q2: patrimonio país / ciudad / grupo
        double pat = p.getPatrimonio();
        if (pat > rq2_.patMaxPais){ rq2_.patMaxPais=pat; rq2_.idxPatPais=i; }
        if (ci>=0){
            if (pat > rq2_.bestPatCity[ci]){ rq2_.bestPatCity[ci]=pat; rq2_.idxBestPatCity[ci]=i; }
            rq4_.sumPatCity[ci] += pat; rq4_.cntCity[ci] += 1; // para Q4
        }
        if (gi>=0){
            if (pat > rq2_.bestPatGrp[gi]){ rq2_.bestPatGrp[gi]=pat; rq2_.idxBestPatGrp[gi]=i; }
        }

        // Q3: declarantes + validación
        if (gi>=0){
            rq3_.totGrp[gi] += 1;
            if (p.getDeclaranteRenta()) rq3_.declaGrp[gi] += 1;
        }
        char esp = grupoPorDocumento_(p.getId());
        if (esp!='?' && gi>=0 && esp!=p.getCalendarioRenta()) rq3_.mismatches += 1;

        // Q6: % >= 80 por calendario
        if (gi>=0){
            rq6_.tot[gi] += 1;
            if (e>=80) rq6_.may80[gi] += 1;
        }

        // Q7: promedio ingresos declarantes
        if (p.getDeclaranteRenta()){
            ++rq7_.declarantes;
            rq7_.promedioIngresos += p.getIngresosAnuales();
        }
    }
}

// ======= Post-proceso: top-5 y porcentajes =======
void Optimizacion::postprocess_(){
    // Q4: top-5 por promedio de patrimonio (selección iterativa)
    bool tomado[MAX_CIUDADES_OPT]; for (int i=0;i<MAX_CIUDADES_OPT;++i) tomado[i]=false;
    for (int k=0; k<5; ++k){
        int best=-1; double bestAvg=-1.0;
        for (int ci=0; ci<numCiudades_; ++ci){
            if (tomado[ci] || rq4_.cntCity[ci]==0) continue;
            double avg = rq4_.sumPatCity[ci]/(double)rq4_.cntCity[ci];
            if (best==-1 || avg>bestAvg){ best=ci; bestAvg=avg; }
        }
        rq4_.topIdx[k] = best;
        if (best>=0) tomado[best]=true; else break;
    }

    // Q6: porcentajes
    size_t totPais = rq6_.tot[0]+rq6_.tot[1]+rq6_.tot[2];
    size_t mayPais = rq6_.may80[0]+rq6_.may80[1]+rq6_.may80[2];
    for (int g=0; g<3; ++g){
        rq6_.pct[g] = (rq6_.tot[g]>0) ? (100.0*(double)rq6_.may80[g]/(double)rq6_.tot[g]) : 0.0;
    }
    rq6_.pctPais = (totPais>0) ? (100.0*(double)mayPais/(double)totPais) : 0.0;

    // Q7: promedio final
    if (rq7_.declarantes>0) rq7_.promedioIngresos /= (double)rq7_.declarantes;
}

// ======= Impresiones =======
void Optimizacion::print_Q1() const {
    std::cout << "\n==== Persona mas longeva (pais) ====\n";
    printClass_(v_[rq1_.idxLonPais], rq1_.edadLonPais);
    std::cout << "==== Persona mas longeva por ciudad ====\n";
    for (int ci=0; ci<numCiudades_; ++ci){
        if (rq1_.bestEdadCity[ci]>=0){
            std::cout << ciudades_[ci] << ": ";
            printClass_(v_[rq1_.idxBestEdadCity[ci]], rq1_.bestEdadCity[ci]);
        }
    }
}
void Optimizacion::print_Q2() const {
    std::cout << "\n==== Mayor patrimonio (pais) ====\n";
    printClass_(v_[rq2_.idxPatPais], -1, rq2_.patMaxPais);
    std::cout << "==== Mayor patrimonio por ciudad ====\n";
    for (int ci=0; ci<numCiudades_; ++ci){
        if (rq4_.cntCity[ci]>0){
            std::cout << ciudades_[ci] << ": ";
            printClass_(v_[rq2_.idxBestPatCity[ci]], -1, rq2_.bestPatCity[ci]);
        }
    }
    std::cout << "==== Mayor patrimonio por grupo ====\n";
    for (int g=0; g<3; ++g){
        char cg = (g==0?'A': g==1?'B':'C');
        if (rq2_.bestPatGrp[g]!=-std::numeric_limits<double>::infinity()){
            std::cout << cg << ": ";
            printClass_(v_[rq2_.idxBestPatGrp[g]], -1, rq2_.bestPatGrp[g]);
        }
    }
}
void Optimizacion::print_Q3() const {
    std::cout << "\n==== Declarantes + validacion ====\n";
    std::cout<<"A: total="<<rq3_.totGrp[0]<<", declarantes="<<rq3_.declaGrp[0]<<"\n";
    std::cout<<"B: total="<<rq3_.totGrp[1]<<", declarantes="<<rq3_.declaGrp[1]<<"\n";
    std::cout<<"C: total="<<rq3_.totGrp[2]<<", declarantes="<<rq3_.declaGrp[2]<<"\n";
    std::cout<<"Inconsistencias: "<<rq3_.mismatches<<"\n";
}
void Optimizacion::print_Q4_top5_promedio_patrimonio() const {
    std::cout << "\n==== Top-5 ciudades por patrimonio promedio ====\n";
    std::cout << std::left << std::setw(4) << "#"
              << std::setw(14) << "Ciudad"
              << std::setw(10) << "N" << "Promedio\n";
    for (int k=0;k<5;++k){
        int ci = rq4_.topIdx[k];
        if (ci<0) break;
        double avg = (rq4_.cntCity[ci]>0) ? (rq4_.sumPatCity[ci]/(double)rq4_.cntCity[ci]) : 0.0;
        std::cout << std::left << std::setw(4) << (k+1)
                  << std::setw(14) << ciudades_[ci]
                  << std::setw(10) << rq4_.cntCity[ci]
                  << "$" << std::fixed << std::setprecision(2) << avg << "\n";
    }
}
void Optimizacion::print_Q6_pct_mayores80() const {
    std::cout << "\n==== % de personas >= 80 anios por calendario ====\n";
    const char* glabel[3]={"A","B","C"};
    for (int g=0; g<3; ++g){
        std::cout << glabel[g] << ": " << rq6_.may80[g] << "/" << rq6_.tot[g]
                  << " = " << std::fixed << std::setprecision(2) << rq6_.pct[g] << "%\n";
    }
    std::cout << "Pais: " << (rq6_.may80[0]+rq6_.may80[1]+rq6_.may80[2]) << "/"
              << (rq6_.tot[0]+rq6_.tot[1]+rq6_.tot[2])
              << " = " << std::fixed << std::setprecision(2) << rq6_.pctPais << "%\n";
}
void Optimizacion::print_Q7_prom_ingresos_declarantes() const {
    std::cout << "\n==== Promedio de ingresos (solo declarantes) ====\n";
    if (rq7_.declarantes==0){ std::cout << "(sin declarantes)\n"; return; }
    std::cout << "Declarantes="<<rq7_.declarantes<<" | Promedio=$"
              << std::fixed << std::setprecision(2) << rq7_.promedioIngresos << "\n";
}
