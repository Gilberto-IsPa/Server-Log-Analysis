/*
    Programa: Análisis de bitácora de servidor
    Autor: Gilberto Islas - A01255666
    Fecha de creación: 26/10/2025
    Descripción: Este programa lee un archivo de bitácora, ordena los registros por las cantidad de accesos de la ip y permite
                 realizar búsquedas de registros dentro de un rango de IP especificado por el usuario.
                 Si ya existe un archivo ordenado, lo utiliza directamente para agilizar el proceso.
*/


#include <bits/stdc++.h>
using namespace std;

// Estructura que representa cada registro de la bitácora
struct Registro {
    string mes;
    int dia;
    string hora;
    string ip;
    string razon;
    string lineaCompleta;
    unsigned long ipNumerica;
    int cantidadAccesos = 0; // Nuevo campo para contar accesos por IP
};

// Convierte IP a número para poder comparar rangos
unsigned long convertirIpNumerica(const string &ipPuerto) {
    string ip = ipPuerto.substr(0, ipPuerto.find(':'));
    stringstream ss(ip);
    string parte;
    unsigned long resultado = 0;
    int multiplicador = 16777216;
    while (getline(ss, parte, '.')) {
        int num = stoi(parte);
        resultado += num * multiplicador;
        multiplicador /= 256;
    }
    return resultado;
}

// Leer archivo de bitácora
void leerArchivo(const string &nombreArchivo, vector<Registro> &bitacora) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo " << nombreArchivo << endl;
        exit(1);
    }

    string mes, hora, ip, resto;
    int dia;
    while (archivo >> mes >> dia >> hora >> ip) {
        getline(archivo, resto);
        Registro r;
        r.mes = mes;
        r.dia = dia;
        r.hora = hora;
        r.ip = ip;
        r.razon = resto;
        r.lineaCompleta = mes + " " + to_string(dia) + " " + hora + " " + ip + resto;
        r.ipNumerica = convertirIpNumerica(ip);
        bitacora.push_back(r);
    }
    archivo.close();
}

// Verifica si el archivo existe
bool archivoExiste(const string &nombreArchivo) {
    ifstream f(nombreArchivo.c_str());
    return f.good();
}

// Búsqueda de registros por rango de IP
void buscarPorRango(const vector<Registro> &bitacora) {
    string ipInicio, ipFin;
    cout << "\nIngrese la IP de inicio sin puerto(ejemplo 66.249.73.135): ";
    cin >> ipInicio;
    cout << "Ingrese la IP de fin: ";
    cin >> ipFin;

    unsigned long inicio = convertirIpNumerica(ipInicio + ":0");
    unsigned long fin = convertirIpNumerica(ipFin + ":0");

    cout << "\n--- Registros dentro del rango ---\n";
    for (size_t i = 0; i < bitacora.size(); i++) {
        if (bitacora[i].ipNumerica >= inicio && bitacora[i].ipNumerica <= fin)
            cout << bitacora[i].lineaCompleta << "\n";
    }
}

// Contar accesos por IP
void contarAccesos(vector<Registro> &bitacora) {
    for (size_t i = 0; i < bitacora.size(); i++) {
        if (bitacora[i].cantidadAccesos == 0) {
            int contador = 0;
            for (size_t j = 0; j < bitacora.size(); j++) {
                if (bitacora[j].ip == bitacora[i].ip)
                    contador++;
            }
            for (size_t j = 0; j < bitacora.size(); j++) {
                if (bitacora[j].ip == bitacora[i].ip)
                    bitacora[j].cantidadAccesos = contador;
            }
        }
    }
}

// Guardar bitácora ordenada por cantidad de accesos en orden descendente
void guardarBitacoraOrdenada(vector<Registro> &bitacora) {
    sort(bitacora.begin(), bitacora.end(),
         [](const Registro &a, const Registro &b) {
             return a.cantidadAccesos > b.cantidadAccesos;
         });

    ofstream salida("bitacora_ordenada.txt");
    for (size_t i = 0; i < bitacora.size(); i++)
        salida << bitacora[i].lineaCompleta << "\n";
    salida.close();
}

int main() {
    vector<Registro> bitacora;

    if (archivoExiste("bitacora_ordenada.txt")) {
        cout << "Archivo ordenado ya existe, leyendo bitacora_ordenada.txt...\n";
        leerArchivo("bitacora_ordenada.txt", bitacora);
    } else {
        cout << "Leyendo bitacora.txt...\n";
        leerArchivo("bitacora.txt", bitacora);

        cout << "Contando accesos por IP...\n";
        contarAccesos(bitacora);

        cout << "Guardando bitacora_ordenada.txt ordenada por cantidad de accesos...\n";
        guardarBitacoraOrdenada(bitacora);

        cout << "Archivo generado exitosamente.\n";
    }

    buscarPorRango(bitacora);

    return 0;
}