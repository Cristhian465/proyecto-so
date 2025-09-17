package memoria;

import java.util.List;

public class FirstFit implements EstrategiaMemoria {

    @Override
    public BloqueMemoria asignar(List<BloqueMemoria> listaBloques, int tamSolicitud, int pid) {
        for (int i = 0; i < listaBloques.size(); i++) {
            BloqueMemoria b = listaBloques.get(i);
            if (b.libre && b.tam >= tamSolicitud) {
                // si sobra espacio, fragmentar
                if (b.tam > tamSolicitud) {
                    BloqueMemoria resto = new BloqueMemoria(b.inicio + tamSolicitud, b.tam - tamSolicitud);
                    listaBloques.add(i + 1, resto);
                    b.tam = tamSolicitud;
                }
                b.libre = false;
                b.pidAsignado = pid;
                return b;
            }
        }
        return null;
    }
}
