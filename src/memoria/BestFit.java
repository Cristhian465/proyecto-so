package memoria;

import java.util.List;

public class BestFit implements EstrategiaMemoria {

    @Override
    public BloqueMemoria asignar(List<BloqueMemoria> listaBloques, int tamSolicitud, int pid) {
        int mejorIdx = -1;
        int mejorTam = Integer.MAX_VALUE;
        for (int i = 0; i < listaBloques.size(); i++) {
            BloqueMemoria b = listaBloques.get(i);
            if (b.libre && b.tam >= tamSolicitud) {
                if (b.tam < mejorTam) {
                    mejorTam = b.tam;
                    mejorIdx = i;
                }
            }
        }
        if (mejorIdx == -1) return null;
        BloqueMemoria b = listaBloques.get(mejorIdx);
        if (b.tam > tamSolicitud) {
            BloqueMemoria resto = new BloqueMemoria(b.inicio + tamSolicitud, b.tam - tamSolicitud);
            listaBloques.add(mejorIdx + 1, resto);
            b.tam = tamSolicitud;
        }
        b.libre = false;
        b.pidAsignado = pid;
        return b;
    }
}
