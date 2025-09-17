package memoria;

public class BloqueMemoria {
    public int inicio;
    public int tam;
    public boolean libre;
    public int pidAsignado; // -1 si libre

    public BloqueMemoria(int inicio, int tam) {
        this.inicio = inicio;
        this.tam = tam;
        this.libre = true;
        this.pidAsignado = -1;
    }

    @Override
    public String toString() {
        return String.format("Bloque[inicio=%d, tam=%d, libre=%s, pid=%d]", inicio, tam, libre, pidAsignado);
    }
}
