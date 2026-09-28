#!/usr/bin/env bash
# Ejecutar desde MSYS2 UCRT64: bash compilar.sh
set -euo pipefail
cd -- "$(dirname -- "$0")"
mkdir -p build
flex -o build/lexer.c src/lexer.l
gcc -Isrc src/main.c build/lexer.c -o build/AnalizadorLexico.exe -mwindows -lcomctl32 -lgdi32
echo "Compilación terminada: build/AnalizadorLexico.exe"
