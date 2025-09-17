import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import proceso.Proceso;
import planificador.*;
import memoria.*;

import java.io.FileReader;
import java.io.Reader;
import java.util.*;

public class SimuladorSO {

    public static void main(String[] args) {
        // Ruta por defecto relativa a la raíz del proyecto
        String configPath = "config/ejemplo.json";

        // Si el usuario pasa otra ruta por consola, la usamos
        if (args.length >= 1) {
            configPath = args[0];
        }

        try (Reader r = new FileReader(configPath)) {
            Gson gson = new GsonBuilder().create();
            Config cfg = gson.fromJson(r, Config.class);

            // Validaciones básicas
            if (cfg == null) {
                System.err.println("Error leyendo configuración (null).");
                return;
            }
            if (cfg.cpu == null || cfg.cpu.algoritmo == null) {
                System.err.println("Falta configuración CPU (algoritmo).");
                return;
            }
            if (cfg.procesos == null || cfg.procesos.isEmpty()) {
                System.err.println("No hay procesos en la configuración.");
                return;
            }

            // Crear lista de procesos (copias para evitar efectos laterales)
            List<Proceso> procesos = new ArrayList<>();
            for (Proceso p : cfg.procesos) {
                procesos.add(new Proceso(p.pid, p.llegada, p.servicio));
            }

            // Selección del planificador
            Planificador planificador;
            String alg = cfg.cpu.algoritmo.trim().toUpperCase();
            switch (alg) {
                case "FCFS":
                    planificador = new FCFS();
                    break;
                case "SPN":
                case "SJF":
                    planificador = new SPN();
                    break;
                case "RR":
                    int q = (cfg.cpu.quantum == null ? 4 : cfg.cpu.quantum);
                    planificador = new RoundRobin(q);
                    break;
                default:
                    System.err.println("Algoritmo desconocido: " + cfg.cpu.algoritmo + ". Usando FCFS por defecto.");
                    planificador = new FCFS();
            }

            // Ejecutar planificación (trabaja sobre la lista referenciada)
            planificador.ejecutar(procesos);

            // Inicializar memoria
            List<BloqueMemoria> bloques = new ArrayList<>();
            int tamMem = (cfg.memoria == null ? 1048576 : cfg.memoria.tam);
            // inicialmente un bloque libre completo
            bloques.add(new BloqueMemoria(0, tamMem));

            EstrategiaMemoria estrategia;
            String estr = (cfg.memoria == null ? "first-fit" : cfg.memoria.estrategia);
            if (estr == null) estr = "first-fit";
            estr = estr.toLowerCase();
            if (estr.equals("best-fit") || estr.equals("bestfit")) estrategia = new BestFit();
            else estrategia = new FirstFit();

            // atender solicitudes de memoria (se hace después de planificación en este ejemplo)
            if (cfg.solicitudes_mem != null) {
                System.out.println("Asignando memoria segun estrategia: " + estr);
                for (Config.MemSolicitud req : cfg.solicitudes_mem) {
                    BloqueMemoria asignado = estrategia.asignar(bloques, req.tam, req.pid);
                    if (asignado != null) {
                        System.out.printf("PID %d => asignado inicio=%d, tam=%d%n", req.pid, asignado.inicio, asignado.tam);
                    } else {
                        System.out.printf("PID %d => NO HAY ESPACIO para tam=%d%n", req.pid, req.tam);
                    }
                }
            }

            // Reporte
            System.out.println();
            Reporte.imprimirTablaProcesos(procesos);
            Reporte.imprimirResumen(procesos);
            Reporte.imprimirMemoria(bloques);

        } catch (Exception ex) {
            ex.printStackTrace();
            System.err.println("Error al ejecutar simulador: " + ex.getMessage());
        }
    }
}
