import proceso.Proceso;
import java.util.List;

public class Config {
    public CpuConfig cpu;
    public List<Proceso> procesos;
    public MemConfig memoria;
    public List<MemSolicitud> solicitudes_mem;

    public static class CpuConfig {
        public String algoritmo; // "FCFS","SPN","RR"
        public Integer quantum;  // opcional para RR
    }

    public static class MemConfig {
        public int tam;
        public String estrategia; // "first-fit" o "best-fit"
    }

    public static class MemSolicitud {
        public int pid;
        public int tam;
    }
}
