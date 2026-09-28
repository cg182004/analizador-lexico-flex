# Entrega del analizador léxico

## Archivos incluidos

- `src/main.c`: interfaz gráfica en C y Win32.
- `src/lexer.l`: reglas de FLEX, fuente original del analizador.
- `src/lexer.h`: estructura de los tokens y funciones compartidas.
- `build/lexer.c`: código C generado por FLEX.
- `build/AnalizadorLexico.exe`: ejecutable gráfico para Windows de 64 bits.
- `readme.md`: documentación del lenguaje y casos de prueba.
- `compilar.sh`: instrucciones automatizadas de compilación.

## Ejecutar

Extraer el ZIP completo, abrir la carpeta `build` y hacer doble clic en
`AnalizadorLexico.exe`. Escribir código en el editor y pulsar **Analizar**.
El botón **Limpiar** vacía el editor y los resultados.

Para usar el ejecutable no se necesita instalar FLEX ni GCC.

## Compilar desde el código fuente

Instalar MSYS2 y abrir la terminal **MSYS2 UCRT64**. Instalar las herramientas:

```bash
pacman -S --needed flex mingw-w64-ucrt-x86_64-gcc
```

Entrar en la carpeta del proyecto con `cd` y ejecutar:

```bash
bash compilar.sh
```

Cerrar la aplicación antes de recompilar. El proceso genera el analizador
desde `src/lexer.l` y actualiza `build/AnalizadorLexico.exe`.

## Entrega en GitHub

Subir las carpetas `src` y `build`, junto con `readme.md`, `compilar.sh`
y este documento, manteniendo sus nombres y estructura.
El archivo `AnalizadorLexico-entrega.zip` reúne esos mismos archivos para
descargar o adjuntar a la entrega académica.

Si el repositorio es privado, el profesor necesitará acceso como colaborador
para consultar el código mediante su enlace.
