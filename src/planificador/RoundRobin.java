package planificador;

import proceso.Proceso;
import java.util.*;

public class RoundRobin implements Planificador {
    private final int quantum;

    public RoundRobin(int quantum) {
        if (quantum < 1) throw new IllegalArgumentException("Quantum debe ser >= 1");
        this.quantum = quantum;
    }

    @Override
    public void ejecutar(List<Proceso> procesos) {
        // orden inicial por llegada
        List<Proceso> pendientes = new ArrayList<>(procesos);
        pendientes.sort(Comparator.comparingInt((Proceso p) -> p.llegada).thenComparingInt(p -> p.pid));

        Queue<Proceso> cola = new ArrayDeque<>();
        int tiempo = 0;
        int idxPendientes = 0;
        int n = pendientes.size();

        while (idxPendientes < n || !cola.isEmpty()) {
            // encolar todos los que llegaron hasta ahora
            while (idxPendientes < n && pendientes.get(idxPendientes).llegada <= tiempo) {
                cola.add(pendientes.get(idxPendientes));
                idxPendientes++;
            }

            if (cola.isEmpty()) {
                // si no hay listos, avanzar al siguiente evento de llegada
                tiempo = pendientes.get(idxPendientes).llegada;
                continue;
            }

            Proceso p = cola.poll();
            if (p.inicio == null) p.inicio = tiempo; // primer despacho
            int ejec = Math.min(quantum, p.servicioRestante);
            p.servicioRestante -= ejec;
            tiempo += ejec;

            // al avanzar tiempo, encolar los que llegaron durante este quantum
            while (idxPendientes < n && pendientes.get(idxPendientes).llegada <= tiempo) {
                cola.add(pendientes.get(idxPendientes));
                idxPendientes++;
            }

            if (p.servicioRestante == 0) {
                p.fin = tiempo;
            } else {
                // reinsertar al final de la cola
                cola.add(p);
            }
        }
    }
}
