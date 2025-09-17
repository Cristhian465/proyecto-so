package planificador;

import proceso.Proceso;
import java.util.*;

public class SPN implements Planificador {

    @Override
    public void ejecutar(List<Proceso> procesos) {
        // Trabajar sobre copia para manipular listas
        List<Proceso> pendientes = new ArrayList<>(procesos);
        pendientes.sort(Comparator.comparingInt(p -> p.llegada)); // orden base por llegada
        List<Proceso> terminados = new ArrayList<>();
        int tiempo = 0;

        while (!pendientes.isEmpty()) {
            // obtener lista de listos
            List<Proceso> listos = new ArrayList<>();
            for (Proceso p : pendientes) {
                if (p.llegada <= tiempo) listos.add(p);
                else break; // pendientes está ordenada por llegada
            }

            if (listos.isEmpty()) {
                // avanzar al próximo tiempo de llegada
                tiempo = pendientes.get(0).llegada;
                continue;
            }

            // elegir el de menor servicio (en empate, menor llegada, luego PID)
            listos.sort(Comparator.comparingInt((Proceso p) -> p.servicio)
                    .thenComparingInt(p -> p.llegada)
                    .thenComparingInt(p -> p.pid));

            Proceso elegido = listos.get(0);
            elegido.inicio = tiempo;
            elegido.fin = tiempo + elegido.servicio;
            tiempo = elegido.fin;
            terminados.add(elegido);
            pendientes.remove(elegido);
        }

        // reemplazar lista original con los tiempos calculados (ya que los objetos son referenciados, no hace falta copiar)
    }
}
