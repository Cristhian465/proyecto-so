package proceso;

public class Proceso {
    public int pid;
    public int llegada;
    public int servicio;

    // Tiempos calculados
    public Integer inicio = null;
    public Integer fin = null;

    // Para RR
    public int servicioRestante;

    public Proceso() {}

    public Proceso(int pid, int llegada, int servicio) {
        this.pid = pid;
        this.llegada = llegada;
        this.servicio = servicio;
        this.servicioRestante = servicio;
    }

    public int getRespuesta() {
        if (inicio == null) return -1;
        return inicio - llegada;
    }

    public int getEspera() {
        if (fin == null) return -1;
        return fin - llegada - servicio;
    }

    public int getRetorno() {
        if (fin == null) return -1;
        return fin - llegada;
    }

    public String toStringRow() {
        return String.format("%3d | %7d | %7d | %6s | %3s | %8s | %6s | %7s",
                pid, llegada, servicio,
                inicio == null ? "-" : inicio.toString(),
                fin == null ? "-" : fin.toString(),
                (inicio == null ? "-" : String.valueOf(getRespuesta())),
                (fin == null ? "-" : String.valueOf(getEspera())),
                (fin == null ? "-" : String.valueOf(getRetorno()))
        );
    }
}
