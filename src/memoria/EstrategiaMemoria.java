package memoria;

import java.util.List;

public interface EstrategiaMemoria {
    /**
     * Intenta asignar un bloque para pid con tamaño tamSolicitud.
     * Retorna el BloqueMemoria asignado (referencia dentro de la lista) o null si no hay espacio.
     */
    BloqueMemoria asignar(List<BloqueMemoria> listaBloques, int tamSolicitud, int pid);
}
