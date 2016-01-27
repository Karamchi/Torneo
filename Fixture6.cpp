#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

typedef int equipo;

struct Torneo {
public:
    Torneo();
    Torneo(int equipos, int rondas);
    void CargarNombres(int equipos);
    void CargarResultados(int equipos, int rondas);
    void imprimirChances(char* out);

private:
    int Puntaje(equipo a);
    float Pganar(equipo a, equipo b);
    float Pempatar(equipo a, equipo b);

    float PSumarXaux(equipo a, int x, int faltan, int puntos, int prox);
    float PSumarHastaX(equipo a, int x);
    float PSerCampeon(int a);
    float PDesempatar(int a);
    float PSumarX(equipo a, int x);
    void AgregarResultado(int a, int b, int golesa, int golesb);
    void AgregarGrondona(int a, int b, int golesa, int golesb);
    void mostrarTabla();
    void mostrarFixture();

    vector<int> _PG;
    vector<int> _PE;
    vector<int> _PP;
    vector<int> _PJ;
    vector<vector<int> > _Res; 
    vector<vector<float> > _px;

	vector<string> nombres;
	
    int fechas;
};

Torneo::Torneo(){};

Torneo::Torneo(int equipos, int rondas){
	fechas=equipos*rondas;
	_PG.resize(equipos);
	_PE.resize(equipos);
	_PP.resize(equipos);
	_PJ.resize(equipos);
	_px.resize(equipos);
	_Res.resize(equipos);
	nombres.resize(equipos);
	for (int i=0;i<equipos;i++){
		_Res[i].resize(equipos+1); for (int j=0;j<equipos+1;j++) _Res[i][j]=-1;
		_px[i].resize(fechas*3+1); for (int j=0;j<fechas*3+1;j++) _px[i][j]=-1;
	}
};

int Torneo::Puntaje(equipo a) {
    return _PG[a]*3+_PE[a];
}

float Torneo::Pganar(equipo a, equipo b) {
    return (_PG[a]/(float)_PJ[a]+_PP[b]/(float)_PJ[b])/2;
}

float Torneo::Pempatar(equipo a, equipo b) {
    return (_PE[a]/(float)_PJ[a]+_PE[b]/(float)_PJ[b])/2;
}

float Torneo::PSumarX(equipo a, int x) {
	float res;
	if (_px[a][x]==-1) {
	    int faltan=_PJ.size()-_PJ[a];
		res=PSumarXaux(a,x,faltan,Puntaje(a),0);
		_px[a][x]=res;
    } else 
		res=_px[a][x];
	return res;
}

float Torneo::PSumarXaux(equipo a, int x, int faltan, int puntos, int prox) {
    if (faltan==0 || faltan*3<(x-puntos) || x<puntos ) {
        if (x==puntos) return 1;
        return 0;
    } if (_Res[a][prox]>-1 || a==prox) {
		return PSumarXaux(a,x,faltan,puntos,prox+1);
	} 
    float PEprox=Pempatar(a,prox);
    float PGprox=Pganar(a,prox);
    float PPprox=1-PEprox-PGprox;
    float h1=PGprox*PSumarXaux(a,x,faltan-1,puntos+3,prox+1);
    float h2=PEprox*PSumarXaux(a,x,faltan-1,puntos+1,prox+1);
    float h3=PPprox*PSumarXaux(a,x,faltan-1,puntos,prox+1);
    return h1+h2+h3;
}

float Torneo::PSumarHastaX(equipo a, int x) {
    float acum=0;
    for (int i=0;i<=x;i++)
        acum+=PSumarX(a,i);
    return acum;
}

float Torneo::PSerCampeon(int a) {
	int maxpuntos=_px[0].size();
	int equipos=_PJ.size();
    float acum=0;
    for (int i=0;i<=maxpuntos;i++) {
        float subacum=1;
        for (int j=0;j<equipos;j++) {
            if (j==a) subacum*=PSumarX(a,i);
            else subacum*=PSumarHastaX(j,i-1);
        } acum+=subacum;
    } return acum;
}

float Torneo::PDesempatar(int a) {
	int maxpuntos=_px[0].size();
	int equipos=_PJ.size();
    float acum=0;
    for (int i=0;i<=maxpuntos;i++) {
        float subacum=1;
        for (int j=0;j<equipos;j++) {
            if (j==a) subacum*=PSumarX(a,i);
            else subacum*=PSumarHastaX(j,i);
        } acum+=subacum;
    } return acum;
}

void Torneo::CargarNombres(int equipos) {
    ifstream fixture;
    fixture.open("fixture2.txt");
    string basura;
	string nombre1,nombre2;
    int goles;
    getline(fixture,basura);
    for (int j=0;j<equipos/2;j++) {
        fixture >> nombres[2*j];
        fixture >> goles;
        fixture >> goles;
        fixture >> nombres[2*j+1];
	}
}

void Torneo::AgregarResultado(int a, int b, int golesa, int golesb) {
    _PJ[a]++;
    _PJ[b]++;
    if (golesa>golesb) {
        _PG[a]++;
        _PP[b]++;
		_Res[a][b]=3;
		_Res[b][a]=0;
    } else if (golesa==golesb) {
        _PE[a]++;
        _PE[b]++;
		_Res[a][b]=1;
		_Res[b][a]=1;
    } else {
        _PP[a]++;
        _PG[b]++;
		_Res[a][b]=0;
		_Res[b][a]=3;
    }
}

void Torneo::AgregarGrondona(int a, int b, int golesa, int golesb) {
    _PJ[a]++;
    _PJ[b]++;
    if (golesa>golesb) {
        _PG[a]++;
        _PP[b]++;
		_Res[a][fechas]=3;
		_Res[b][fechas]=0;
    } else if (golesa==golesb) {
        _PE[a]++;
        _PE[b]++;
		_Res[a][fechas]=1;
		_Res[b][fechas]=1;
    } else {
        _PP[a]++;
        _PG[b]++;
		_Res[a][fechas]=0;
		_Res[b][fechas]=3;
    }
}

int def(const vector<string>& nombres, const string nombre) {
	int j;
	for (int i=0;i<nombres.size();i++) {
		for (j=0;nombres[i][j]>0;j++) {
			if (nombre[j]!=nombres[i][j]) break;
		} if (nombre[j]==0) return i;
	}
}

void Torneo::CargarResultados(int equipos, int rondas) {
    ifstream fixture;
    fixture.open("fixture2.txt");
    string basura;
	string nombre1,nombre2;
    int equipo1, goles1, equipo2, goles2;
    for (int i=0;i<fechas;i++) {
        getline(fixture,basura);
        for (int j=0;j<equipos/2;j++) {
            fixture >> nombre1;
            fixture >> goles1;
            fixture >> goles2;
            fixture >> nombre2;
			if (goles1>-1) {
				equipo1=def(nombres,nombre1);
				equipo2=def(nombres,nombre2);
	            if (i!=23) AgregarResultado(equipo1,equipo2,goles1,goles2);
				else AgregarGrondona(equipo1,equipo2,goles1,goles2);
			}
        } getline(fixture,basura);
    }
}

void Torneo::imprimirChances(char* out) {
    ofstream salida;
    salida.open(out);
	salida.setf(ios::fixed,ios::floatfield);
	salida.precision(5);
    for (int i=0;i<_PJ.size();i++) 
        salida << nombres[i] << '\t' << PSerCampeon(i) << '\t' << PDesempatar(i) << endl;
}

void Torneo::mostrarFixture() {
    for (int i=0;i<_PJ.size();i++) {
	    for (int j=0;j<_Res[0].size();j++)
        	cout << _Res[i][j] << ' ';
	    cout << endl;
    }
}

void Torneo::mostrarTabla() {
    for (int i=0;i<_PJ.size();i++) {
		cout << i << ' ';
		cout << "PJ" << _PJ[i] << ' ';
		cout << "PG" << _PG[i] << ' ';
		cout << "PE" << _PE[i] << ' ';
		cout << "PP" << _PP[i] << ' ';
		cout << "PTS" << Puntaje(i) << endl;
    }
}

int main(){
	int equipos, rondas;
	cout << "Equipos: ";
	cin >> equipos;
	cout << "Rondas: ";
	cin >> rondas;
    Torneo t(equipos, rondas);
    t.CargarNombres(equipos);
    t.CargarResultados(equipos, rondas);
    t.imprimirChances("Chances.txt");
};
