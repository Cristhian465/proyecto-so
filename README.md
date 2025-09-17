# SimuladorSO (Proyecto Unidad 01)

## Requisitos
- Java 11+ (javac/java)
- Gson (biblioteca para JSON). Puedes descargar `gson-2.10.1.jar` u otra versión desde Maven Central:
  https://repo1.maven.org/maven2/com/google/code/gson/gson/2.10.1/gson-2.10.1.jar

## Estructura
- `src/` -> código fuente Java
- `config/ejemplo.json` -> ejemplo de configuración

## Compilar (línea de comandos)
1. Coloca `gson-2.10.1.jar` en la carpeta raíz del proyecto o en `lib/`.
2. Compilar:
```bash
javac -cp gson-2.10.1.jar -d out $(find src -name "*.java")
