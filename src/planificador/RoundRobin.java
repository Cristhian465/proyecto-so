package planificador;

import proceso.Proceso;
import java.util.*;

public class RoundRobin implements Planificador {
    private final int quantum;

    public RoundRobin(int quantum) {
        this.quantum = quantum;
    }

    @Override
    public void ejecutar(List<Proceso> procesos) {
        procesos.sort(Comparator.comparingInt(p -> p.llegada));

        Queue<Proceso> cola = new LinkedList<>();
        int tiempo = 0;
        int index = 0;

        while (!cola.isEmpty() || index < procesos.size()) {
            // encolar los que llegaron
            while (index < procesos.size() && procesos.get(index).llegada <= tiempo) {
                cola.add(procesos.get(index));
                index++;
            }

            if (cola.isEmpty()) {
                tiempo++;
                continue;
            }

            Proceso actual = cola.poll();
            if (actual.inicio == null) actual.inicio = tiempo;

            int usado = 0;
            while (usado < quantum && actual.servicioRestante > 0) {
                // ejecutamos 1 unidad
                actual.servicioRestante--;
                tiempo++;
                usado++;

                // pueden llegar procesos en este tiempo
                while (index < procesos.size() && procesos.get(index).llegada <= tiempo) {
                    cola.add(procesos.get(index));
                    index++;
                }
            }

            if (actual.servicioRestante > 0) {
                cola.add(actual); // aún queda, vuelve a la cola
            } else {
                actual.fin = tiempo; // terminó
            }
        }
    }
}
