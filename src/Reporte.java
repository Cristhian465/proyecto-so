import proceso.Proceso;
import memoria.BloqueMemoria;

import java.util.List;
import java.util.DoubleSummaryStatistics;

public class Reporte {

    public static void imprimirTablaProcesos(List<Proceso> procesos) {
        System.out.println("PID | Llegada | Servicio | Inicio | Fin | Respuesta | Espera | Retorno");
        System.out.println("----+---------+----------+--------+-----+-----------+--------+--------");
        // ordenar por PID para salida legible
        procesos.stream()
                .sorted((a,b) -> Integer.compare(a.pid, b.pid))
                .forEach(p -> {
                    System.out.printf("%3d | %7d | %7d | %6s | %3s | %8s | %6s | %7s%n",
                            p.pid, p.llegada, p.servicio,
                            p.inicio == null ? "-" : p.inicio.toString(),
                            p.fin == null ? "-" : p.fin.toString(),
                            p.inicio == null ? "-" : String.valueOf(p.getRespuesta()),
                            p.fin == null ? "-" : String.valueOf(p.getEspera()),
                            p.fin == null ? "-" : String.valueOf(p.getRetorno())
                    );
                });
    }

    public static void imprimirResumen(List<Proceso> procesos) {
        double totalTiempo = 0;
        int n = procesos.size();
        int maxFin = 0;
        double sumaResp = 0;
        double sumaEspera = 0;
        double sumaRetorno = 0;

        for (Proceso p : procesos) {
            if (p.fin != null) {
                maxFin = Math.max(maxFin, p.fin);
            }
            sumaResp += (p.inicio == null ? 0 : p.getRespuesta());
            sumaEspera += (p.fin == null ? 0 : p.getEspera());
            sumaRetorno += (p.fin == null ? 0 : p.getRetorno());
        }

        totalTiempo = maxFin; // asumimos tiempo total hasta último fin
        System.out.println();
        System.out.println("Resumen global:");
        System.out.printf(" - Promedio respuesta: %.3f%n", sumaResp / n);
        System.out.printf(" - Promedio espera:    %.3f%n", sumaEspera / n);
        System.out.printf(" - Promedio retorno:   %.3f%n", sumaRetorno / n);
        System.out.printf(" - Throughput:         %.6f procesos / unidad_tiempo (n=%d, tiempo_total=%.0f)%n", (n / (totalTiempo > 0 ? totalTiempo : 1.0)), n, totalTiempo);
    }

    public static void imprimirMemoria(List<BloqueMemoria> bloques) {
        System.out.println();
        System.out.println("Estado bloques de memoria (ordenados por inicio):");
        System.out.println("Inicio | Tam       | Libre | PID");
        System.out.println("-------+-----------+-------+-----");
        bloques.stream().sorted((a,b)->Integer.compare(a.inicio,b.inicio)).forEach(b -> {
            System.out.printf("%6d | %9d | %5s | %3d%n", b.inicio, b.tam, b.libre ? "SI" : "NO", b.pidAsignado);
        });
    }
}
