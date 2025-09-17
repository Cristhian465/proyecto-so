package planificador;

import proceso.Proceso;
import java.util.*;

public class FCFS implements Planificador {

    @Override
    public void ejecutar(List<Proceso> procesos) {
        // Orden por llegada ascendente, en empates por PID ascendente
        procesos.sort(Comparator.comparingInt((Proceso p) -> p.llegada).thenComparingInt(p -> p.pid));
        int tiempo = 0;
        for (Proceso p : procesos) {
            if (tiempo < p.llegada) tiempo = p.llegada;
            p.inicio = tiempo;
            p.fin = tiempo + p.servicio;
            tiempo = p.fin;
        }
    }
}
