package planificador;

import proceso.Proceso;
import java.util.List;

public interface Planificador {
    /**
     * Ejecuta la planificación. Debe poblar inicio y fin de cada Proceso en la lista entregada.
     * @param procesos lista de procesos (se puede asumir que son copias o que el caller maneja referencias)
     */
    void ejecutar(List<Proceso> procesos);
}
