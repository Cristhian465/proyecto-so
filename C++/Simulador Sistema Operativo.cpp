#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <sstream>
#include <iomanip>
#include <map>
#include <locale.h>
#include <climits>

using namespace std;

// ==================== ESTRUCTURA PCB ====================
struct PCB {
    int pid;
    int llegada;
    int servicio;
    int inicio;
    int fin;
    int restante;  // Para Round Robin
    
    // Métricas calculadas
    int respuesta() const { return inicio - llegada; }
    int espera() const { return fin - llegada - servicio; }
    int retorno() const { return fin - llegada; }
};

// ==================== BLOQUE DE MEMORIA ====================
struct BloqueMemoria {
    int id;
    int inicio;
    int tamano;
    bool ocupado;
    int pid_asignado;
    
    BloqueMemoria(int _id, int _inicio, int _tamano) 
        : id(_id), inicio(_inicio), tamano(_tamano), ocupado(false), pid_asignado(-1) {}
};

// ==================== SOLICITUD DE MEMORIA ====================
struct SolicitudMemoria {
    int pid;
    int tamano;
    int bloque_asignado;
    bool asignada;
    
    SolicitudMemoria(int _pid, int _tam) 
        : pid(_pid), tamano(_tam), bloque_asignado(-1), asignada(false) {}
};

// ==================== PARSER JSON SIMPLE ====================
class JSONParser {
private:
    string contenido;
    size_t pos;
    
    void saltarEspacios() {
        while (pos < contenido.length() && isspace(contenido[pos])) pos++;
    }
    
    string leerString() {
        saltarEspacios();
        if (contenido[pos] != '"') return "";
        pos++; // saltar "
        
        string resultado = "";
        while (pos < contenido.length() && contenido[pos] != '"') {
            resultado += contenido[pos++];
        }
        pos++; // saltar "
        return resultado;
    }
    
    int leerEntero() {
        saltarEspacios();
        string num = "";
        while (pos < contenido.length() && (isdigit(contenido[pos]) || contenido[pos] == '-')) {
            num += contenido[pos++];
        }
        return num.empty() ? 0 : stoi(num);
    }
    
    bool buscarClave(const string& clave) {
        string buscar = "\"" + clave + "\"";
        size_t encontrado = contenido.find(buscar, pos);
        if (encontrado != string::npos) {
            pos = encontrado + buscar.length();
            saltarEspacios();
            if (pos < contenido.length() && contenido[pos] == ':') {
                pos++;
                return true;
            }
        }
        return false;
    }
    
public:
    JSONParser(const string& json) : contenido(json), pos(0) {}
    
    string obtenerString(const string& clave) {
        pos = 0;
        if (buscarClave(clave)) {
            return leerString();
        }
        return "";
    }
    
    int obtenerEntero(const string& clave) {
        pos = 0;
        if (buscarClave(clave)) {
            return leerEntero();
        }
        return 0;
    }
    
    vector<PCB> obtenerProcesos() {
        vector<PCB> procesos;
        size_t inicio_array = contenido.find("\"procesos\"");
        if (inicio_array == string::npos) return procesos;
        
        inicio_array = contenido.find("[", inicio_array);
        size_t fin_array = contenido.find("]", inicio_array);
        
        pos = inicio_array + 1;
        
        while (pos < fin_array) {
            if (contenido[pos] == '{') {
                PCB proceso;
                pos++;
                
                // Leer pid
                if (buscarClave("pid")) proceso.pid = leerEntero();
                // Leer llegada
                if (buscarClave("llegada")) proceso.llegada = leerEntero();
                // Leer servicio
                if (buscarClave("servicio")) proceso.servicio = leerEntero();
                
                proceso.inicio = -1;
                proceso.fin = -1;
                proceso.restante = proceso.servicio;
                
                procesos.push_back(proceso);
            }
            pos++;
        }
        
        return procesos;
    }
    
    vector<SolicitudMemoria> obtenerSolicitudesMemoria() {
        vector<SolicitudMemoria> solicitudes;
        size_t inicio_array = contenido.find("\"solicitudes_mem\"");
        if (inicio_array == string::npos) return solicitudes;
        
        inicio_array = contenido.find("[", inicio_array);
        size_t fin_array = contenido.find("]", inicio_array);
        
        pos = inicio_array + 1;
        
        while (pos < fin_array) {
            if (contenido[pos] == '{') {
                pos++;
                int pid = 0, tam = 0;
                
                // Leer pid
                if (buscarClave("pid")) pid = leerEntero();
                // Leer tam
                if (buscarClave("tam")) tam = leerEntero();
                
                solicitudes.push_back(SolicitudMemoria(pid, tam));
            }
            pos++;
        }
        
        return solicitudes;
    }
};

// ==================== PLANIFICADOR DE CPU ====================
class PlanificadorCPU {
private:
    vector<PCB> procesos;
    string algoritmo;
    int quantum;
    
public:
    PlanificadorCPU(vector<PCB> _procesos, string _algoritmo, int _quantum = 4) 
        : procesos(_procesos), algoritmo(_algoritmo), quantum(_quantum) {}
    
    void ejecutarFCFS() {
        // Ordenar por tiempo de llegada
        sort(procesos.begin(), procesos.end(), [](const PCB& a, const PCB& b) {
            return a.llegada < b.llegada;
        });
        
        int tiempo_actual = 0;
        
        for (auto& proceso : procesos) {
            if (tiempo_actual < proceso.llegada) {
                tiempo_actual = proceso.llegada;
            }
            
            proceso.inicio = tiempo_actual;
            tiempo_actual += proceso.servicio;
            proceso.fin = tiempo_actual;
        }
    }
    
    void ejecutarSPN() {
        vector<PCB> procesos_copia = procesos;
        vector<PCB> completados;
        int tiempo_actual = 0;
        int procesos_completados = 0;
        int n = procesos_copia.size();
        
        // Ordenar por tiempo de llegada
        sort(procesos_copia.begin(), procesos_copia.end(), [](const PCB& a, const PCB& b) {
            return a.llegada < b.llegada;
        });
        
        vector<bool> completado(n, false);
        
        while (procesos_completados < n) {
            int idx = -1;
            int menor_servicio = INT_MAX;
            
            // Encontrar el proceso con el menor tiempo de servicio que haya llegado
            for (int i = 0; i < n; i++) {
                if (!completado[i] && procesos_copia[i].llegada <= tiempo_actual && 
                    procesos_copia[i].servicio < menor_servicio) {
                    menor_servicio = procesos_copia[i].servicio;
                    idx = i;
                }
            }
            
            if (idx == -1) {
                // Si no hay procesos listos, avanzar al tiempo de llegada del siguiente proceso
                int siguiente_llegada = INT_MAX;
                for (int i = 0; i < n; i++) {
                    if (!completado[i] && procesos_copia[i].llegada < siguiente_llegada) {
                        siguiente_llegada = procesos_copia[i].llegada;
                    }
                }
                tiempo_actual = siguiente_llegada;
                continue;
            }
            
            // Ejecutar el proceso seleccionado
            procesos_copia[idx].inicio = tiempo_actual;
            tiempo_actual += procesos_copia[idx].servicio;
            procesos_copia[idx].fin = tiempo_actual;
            completado[idx] = true;
            procesos_completados++;
            
            completados.push_back(procesos_copia[idx]);
        }
        
        // Actualizar procesos originales
        for (size_t i = 0; i < procesos.size(); i++) {
            for (const auto& completado : completados) {
                if (procesos[i].pid == completado.pid) {
                    procesos[i] = completado;
                    break;
                }
            }
        }
    }
    
    void ejecutarRoundRobin() {
        queue<int> cola_listos;
        vector<bool> en_cola(procesos.size(), false);
        vector<bool> completado(procesos.size(), false);
        int tiempo_actual = 0;
        int completados = 0;
        size_t indice_llegada = 0;
        
        // Ordenar por tiempo de llegada
        sort(procesos.begin(), procesos.end(), [](const PCB& a, const PCB& b) {
            return a.llegada < b.llegada;
        });
        
        // Inicializar tiempo restante
        for (auto& proceso : procesos) {
            proceso.restante = proceso.servicio;
        }
        
        while (completados < procesos.size()) {
            // Agregar procesos que han llegado
            while (indice_llegada < procesos.size() && procesos[indice_llegada].llegada <= tiempo_actual) {
                if (!en_cola[indice_llegada] && !completado[indice_llegada]) {
                    cola_listos.push(indice_llegada);
                    en_cola[indice_llegada] = true;
                }
                indice_llegada++;
            }
            
            if (cola_listos.empty()) {
                // No hay procesos listos, avanzar al siguiente proceso
                if (indice_llegada < procesos.size()) {
                    tiempo_actual = procesos[indice_llegada].llegada;
                }
                continue;
            }
            
            int idx_actual = cola_listos.front();
            cola_listos.pop();
            en_cola[idx_actual] = false;
            
            PCB& proceso_actual = procesos[idx_actual];
            
            // Si es la primera vez que se ejecuta
            if (proceso_actual.inicio == -1) {
                proceso_actual.inicio = tiempo_actual;
            }
            
            // Ejecutar por quantum o hasta completar
            int tiempo_ejecucion = min(quantum, proceso_actual.restante);
            tiempo_actual += tiempo_ejecucion;
            proceso_actual.restante -= tiempo_ejecucion;
            
            // Agregar procesos que llegaron durante la ejecución
            while (indice_llegada < procesos.size() && procesos[indice_llegada].llegada <= tiempo_actual) {
                if (!en_cola[indice_llegada] && !completado[indice_llegada]) {
                    cola_listos.push(indice_llegada);
                    en_cola[indice_llegada] = true;
                }
                indice_llegada++;
            }
            
            if (proceso_actual.restante == 0) {
                // Proceso completado
                proceso_actual.fin = tiempo_actual;
                completado[idx_actual] = true;
                completados++;
            } else {
                // Proceso no completado, volver a la cola
                cola_listos.push(idx_actual);
                en_cola[idx_actual] = true;
            }
        }
    }
    
    void ejecutar() {
        if (algoritmo == "FCFS") {
            ejecutarFCFS();
        } else if (algoritmo == "SPN") {
            ejecutarSPN();
        } else if (algoritmo == "RR") {
            ejecutarRoundRobin();
        }
    }
    
    void mostrarResultados() {
        cout << "\n==================== RESULTADOS CPU ====================" << endl;
        cout << "Algoritmo: " << algoritmo;
        if (algoritmo == "RR") cout << " (Quantum: " << quantum << ")";
        cout << endl << endl;
        
        cout << left << setw(5) << "PID" << "| " 
             << setw(8) << "Llegada" << "| "
             << setw(9) << "Servicio" << "| "
             << setw(7) << "Inicio" << "| "
             << setw(4) << "Fin" << "| "
             << setw(10) << "Respuesta" << "| "
             << setw(7) << "Espera" << "| "
             << setw(8) << "Retorno" << endl;
        
        cout << "----|---------|---------|---------|---------|-----------|---------|---------" << endl;
        
        double suma_respuesta = 0, suma_espera = 0, suma_retorno = 0;
        int tiempo_total = 0;
        
        for (const auto& proceso : procesos) {
            cout << left << setw(5) << proceso.pid << "| "
                 << setw(8) << proceso.llegada << "| "
                 << setw(9) << proceso.servicio << "| "
                 << setw(7) << proceso.inicio << "| "
                 << setw(4) << proceso.fin << "| "
                 << setw(10) << proceso.respuesta() << "| "
                 << setw(7) << proceso.espera() << "| "
                 << setw(8) << proceso.retorno() << endl;
            
            suma_respuesta += proceso.respuesta();
            suma_espera += proceso.espera();
            suma_retorno += proceso.retorno();
            tiempo_total = max(tiempo_total, proceso.fin);
        }
        
        cout << endl << "==================== MÉTRICAS GLOBALES ====================" << endl;
        cout << fixed << setprecision(2);
        cout << "Promedio de Respuesta: " << suma_respuesta / procesos.size() << endl;
        cout << "Promedio de Espera: " << suma_espera / procesos.size() << endl;
        cout << "Promedio de Retorno: " << suma_retorno / procesos.size() << endl;
        if (tiempo_total > 0) {
            cout << "Throughput: " << (double)procesos.size() / tiempo_total << " procesos/unidad de tiempo" << endl;
        } else {
            cout << "Throughput: 0 procesos/unidad de tiempo" << endl;
        }
    }
};

// ==================== GESTOR DE MEMORIA ====================
class GestorMemoria {
private:
    vector<BloqueMemoria> bloques;
    vector<SolicitudMemoria> solicitudes;
    int tamano_total;
    string estrategia;
    
public:
    GestorMemoria(int tamano, string _estrategia) 
        : tamano_total(tamano), estrategia(_estrategia) {
        // Inicializar con un solo bloque libre
        bloques.push_back(BloqueMemoria(0, 0, tamano));
    }
    
    void agregarSolicitud(const SolicitudMemoria& solicitud) {
        solicitudes.push_back(solicitud);
    }
    
    void procesarSolicitudes() {
        for (auto& solicitud : solicitudes) {
            if (estrategia == "first-fit") {
                asignarFirstFit(solicitud);
            } else if (estrategia == "best-fit") {
                asignarBestFit(solicitud);
            }
        }
    }
    
private:
    void asignarFirstFit(SolicitudMemoria& solicitud) {
        for (size_t i = 0; i < bloques.size(); i++) {
            if (!bloques[i].ocupado && bloques[i].tamano >= solicitud.tamano) {
                // Asignar el bloque
                if (bloques[i].tamano > solicitud.tamano) {
                    // Dividir el bloque
                    BloqueMemoria nuevo_bloque(bloques.size(), 
                                               bloques[i].inicio + solicitud.tamano,
                                               bloques[i].tamano - solicitud.tamano);
                    bloques.push_back(nuevo_bloque);
                    bloques[i].tamano = solicitud.tamano;
                }
                
                bloques[i].ocupado = true;
                bloques[i].pid_asignado = solicitud.pid;
                solicitud.bloque_asignado = bloques[i].id;
                solicitud.asignada = true;
                return;
            }
        }
    }
    
    void asignarBestFit(SolicitudMemoria& solicitud) {
        int mejor_indice = -1;
        int menor_desperdicio = INT_MAX;
        
        for (size_t i = 0; i < bloques.size(); i++) {
            if (!bloques[i].ocupado && bloques[i].tamano >= solicitud.tamano) {
                int desperdicio = bloques[i].tamano - solicitud.tamano;
                if (desperdicio < menor_desperdicio) {
                    menor_desperdicio = desperdicio;
                    mejor_indice = i;
                }
            }
        }
        
        if (mejor_indice != -1) {
            if (bloques[mejor_indice].tamano > solicitud.tamano) {
                // Dividir el bloque
                BloqueMemoria nuevo_bloque(bloques.size(),
                                           bloques[mejor_indice].inicio + solicitud.tamano,
                                           bloques[mejor_indice].tamano - solicitud.tamano);
                bloques.push_back(nuevo_bloque);
                bloques[mejor_indice].tamano = solicitud.tamano;
            }
            
            bloques[mejor_indice].ocupado = true;
            bloques[mejor_indice].pid_asignado = solicitud.pid;
            solicitud.bloque_asignado = bloques[mejor_indice].id;
            solicitud.asignada = true;
        }
    }
    
    void compactarMemoria() {
        // Implementar compactación de memoria si es necesario
        // Esto uniría bloques libres adyacentes
        for (size_t i = 0; i < bloques.size(); i++) {
            if (!bloques[i].ocupado) {
                for (size_t j = i + 1; j < bloques.size(); j++) {
                    if (!bloques[j].ocupado && 
                        bloques[i].inicio + bloques[i].tamano == bloques[j].inicio) {
                        bloques[i].tamano += bloques[j].tamano;
                        bloques.erase(bloques.begin() + j);
                        j--;
                    }
                }
            }
        }
    }
    
public:
    void liberarMemoria(int pid) {
        for (auto& bloque : bloques) {
            if (bloque.pid_asignado == pid) {
                bloque.ocupado = false;
                bloque.pid_asignado = -1;
            }
        }
        compactarMemoria();
    }
    
    void mostrarResultados() {
        cout << "\n==================== RESULTADOS MEMORIA ====================" << endl;
        cout << "Tamano total: " << tamano_total << " bytes" << endl;
        cout << "Estrategia: " << estrategia << endl << endl;
        
        cout << left << setw(5) << "PID" << "| "
             << setw(12) << "Solicitud" << "| "
             << setw(16) << "Bloque asignado" << "| "
             << setw(8) << "Tamano" << "| "
             << setw(12) << "Estado" << endl;
        
        cout << "-----|--------------|--------------------|----------|-------" << endl;
        
        for (const auto& solicitud : solicitudes) {
            cout << left << setw(5) << solicitud.pid << "| "
                 << setw(12) << solicitud.tamano << "| ";
            
            if (solicitud.asignada) {
                cout << setw(16) << solicitud.bloque_asignado << "| "
                     << setw(8) << solicitud.tamano << "| "
                     << setw(12) << "Asignado";
            } else {
                cout << setw(16) << "N/A" << "| "
                     << setw(8) << "N/A" << "| "
                     << setw(12) << "No encontrado";
            }
            cout << endl;
        }
        
        cout << "\n==================== ESTADO DE LA MEMORIA ====================" << endl;
        cout << left << setw(10) << "Bloque ID" << "| "
             << setw(8) << "Inicio" << "| "
             << setw(8) << "Tamano" << "| "
             << setw(8) << "Estado" << "| "
             << setw(12) << "PID asignado" << endl;
        
        cout << "-----------|--------|--------|--------|--------" << endl;
        
        for (const auto& bloque : bloques) {
            cout << left << setw(10) << bloque.id << "| "
                 << setw(8) << bloque.inicio << "| "
                 << setw(8) << bloque.tamano << "| "
                 << setw(8) << (bloque.ocupado ? "Ocupado" : "Libre") << "| "
                 << setw(12) << (bloque.ocupado ? to_string(bloque.pid_asignado) : "N/A") << endl;
        }
    }
};

// ==================== FUNCIONES PARA ENTRADA MANUAL ====================
vector<PCB> ingresarProcesosManual() {
    vector<PCB> procesos;
    int cantidad;
    
    cout << "\nIngrese la cantidad de procesos: ";
    cin >> cantidad;
    
    for (int i = 0; i < cantidad; i++) {
        PCB proceso;
        proceso.pid = i + 1;
        
        cout << "\nProceso " << proceso.pid << ":\n";
        cout << "Tiempo de llegada: ";
        cin >> proceso.llegada;
        cout << "Tiempo de servicio: ";
        cin >> proceso.servicio;
        
        proceso.inicio = -1;
        proceso.fin = -1;
        proceso.restante = proceso.servicio;
        
        procesos.push_back(proceso);
    }
    
    return procesos;
}

vector<SolicitudMemoria> ingresarSolicitudesManual(const vector<PCB>& procesos) {
    vector<SolicitudMemoria> solicitudes;
    int cantidad;
    
    cout << "\nIngrese la cantidad de solicitudes de memoria: ";
    cin >> cantidad;
    
    for (int i = 0; i < cantidad; i++) {
        int pid, tam;
        
        cout << "\nSolicitud " << (i + 1) << ":\n";
        cout << "PID del proceso: ";
        cin >> pid;
        cout << "Tamano de memoria requerido: ";
        cin >> tam;
        
        // Verificar si el PID existe
        bool pid_valido = false;
        for (const auto& proceso : procesos) {
            if (proceso.pid == pid) {
                pid_valido = true;
                break;
            }
        }
        
        if (pid_valido) {
            solicitudes.push_back(SolicitudMemoria(pid, tam));
        } else {
            cout << "Error: El PID " << pid << " no existe. Esta solicitud sera ignorada.\n";
        }
    }
    
    return solicitudes;
}

// ==================== FUNCIÓN PRINCIPAL ====================
int main() {
    setlocale(LC_ALL, "Spanish");
    cout << "==================== SIMULADOR DE SISTEMA OPERATIVO ====================" << endl;
    cout << "Universidad Privada de Tacna - Ingeniería de Sistemas" << endl;
    cout << "Sistemas Operativos I - Proyecto Unidad 01" << endl << endl;
    
    int opcion;
    cout << "Seleccione el modo de entrada:\n";
    cout << "1. Desde archivo JSON\n";
    cout << "2. Ingreso manual de datos\n";
    cout << "Opcion: ";
    cin >> opcion;
    
    vector<PCB> procesos;
    vector<SolicitudMemoria> solicitudes;
    string algoritmo_cpu = "FCFS";
    int quantum = 4;
    int tamano_memoria = 1024;
    string estrategia_memoria = "first-fit";
    
    if (opcion == 1) {
        // Modo archivo JSON
        string nombre_archivo;
        cout << "Ingrese el nombre del archivo JSON (ej: ejemplo.json): ";
        cin >> nombre_archivo;
        
        // Leer archivo JSON
        ifstream archivo(nombre_archivo);
        if (!archivo.is_open()) {
            cout << "Error: No se pudo abrir el archivo " << nombre_archivo << endl;
            return 1;
        }
        
        string contenido_json;
        string linea;
        while (getline(archivo, linea)) {
            contenido_json += linea + "\n";
        }
        archivo.close();
        
        // Parsear JSON
        JSONParser parser(contenido_json);
        
        // Obtener configuración de CPU
        algoritmo_cpu = parser.obtenerString("algoritmo");
        quantum = parser.obtenerEntero("quantum");
        if (quantum < 2) quantum = 4; // Valor por defecto
        
        // Obtener procesos
        procesos = parser.obtenerProcesos();
        
        // Obtener configuración de memoria
        tamano_memoria = parser.obtenerEntero("tam");
        estrategia_memoria = parser.obtenerString("estrategia");
        
        // Obtener solicitudes de memoria
        solicitudes = parser.obtenerSolicitudesMemoria();
        
        cout << "Datos cargados desde " << nombre_archivo << endl;
        
    } else if (opcion == 2) {
        // Modo manual
        cout << "\n=== CONFIGURACIÓN DEL SISTEMA ===\n";
        
        // Configurar algoritmo de CPU
        cout << "\nSeleccione el algoritmo de planificación:\n";
        cout << "1. FCFS (First-Come, First-Served)\n";
        cout << "2. SPN (Shortest Process Next)\n";
        cout << "3. RR (Round Robin)\n";
        cout << "Opcion: ";
        int algo_opcion;
        cin >> algo_opcion;
        
        if (algo_opcion == 1) algoritmo_cpu = "FCFS";
        else if (algo_opcion == 2) algoritmo_cpu = "SPN";
        else if (algo_opcion == 3) {
            algoritmo_cpu = "RR";
            cout << "Ingrese el quantum para Round Robin: ";
            cin >> quantum;
        }
        
        // Configurar memoria
        cout << "\nIngrese el tamano total de memoria: ";
        cin >> tamano_memoria;
        
        cout << "Seleccione la estrategia de asignación de memoria:\n";
        cout << "1. First-Fit\n";
        cout << "2. Best-Fit\n";
        cout << "Opcion: ";
        int mem_opcion;
        cin >> mem_opcion;
        
        if (mem_opcion == 1) estrategia_memoria = "first-fit";
        else if (mem_opcion == 2) estrategia_memoria = "best-fit";
        
        // Ingresar procesos
        procesos = ingresarProcesosManual();
        
        // Ingresar solicitudes de memoria
        solicitudes = ingresarSolicitudesManual(procesos);
        
    } else {
        cout << "Opción no válida. Saliendo...\n";
        return 1;
    }
    
    cout << "Procesos encontrados: " << procesos.size() << endl;
    cout << "Algoritmo CPU: " << algoritmo_cpu << endl;
    if (algoritmo_cpu == "RR") cout << "Quantum: " << quantum << endl;
    cout << "Estrategia Memoria: " << estrategia_memoria << endl;
    
    // Ejecutar planificación de CPU
    PlanificadorCPU planificador(procesos, algoritmo_cpu, quantum);
    planificador.ejecutar();
    planificador.mostrarResultados();
    
    // Ejecutar gestión de memoria
    GestorMemoria gestor(tamano_memoria, estrategia_memoria);
    for (const auto& solicitud : solicitudes) {
        gestor.agregarSolicitud(solicitud);
    }
    gestor.procesarSolicitudes();
    gestor.mostrarResultados();
    
    // Liberar memoria de los procesos que han terminado
    for (const auto& proceso : procesos) {
        gestor.liberarMemoria(proceso.pid);
    }
    
    cout << "\n==================== ESTADO FINAL DE MEMORIA ====================" << endl;
    gestor.mostrarResultados();
    
    cout << "\nSimulación completada." << endl;
    system("pause");
    return 0;
}