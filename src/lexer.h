/* Contrato compartido por la interfaz y el analizador.
   La guarda evita declarar estos tipos varias veces al incluir el archivo. */
#ifndef LEXER_H
#define LEXER_H

/* Límites de almacenamiento. Los textos reservan un espacio para '\0'. */
#define MAX_TOKENS 1000
#define MAX_LEXEMA 256
#define MAX_TIPO 80

/* Un token es la categoría de un fragmento reconocido.
   Ejemplo: para edad en la línea 1, lexema="edad",
   tipo="IDENTIFICADOR" y línea=1. */
typedef struct
{
    char lexema[MAX_LEXEMA];
    char tipo[MAX_TIPO];
    int linea;
} Token;

/* Analiza un texto completo */
/* Cada llamada sustituye el resultado anterior. */
void analizar_texto(const char *texto);

/* Devuelve la cantidad de tokens encontrados */
int obtener_cantidad_tokens(void);

/* Devuelve un token según su posición */
/* El índice empieza en 0. Devuelve NULL si está fuera de rango.
   El puntero pertenece al analizador: no debe liberarse ni modificarse. */
const Token *obtener_token(int indice);

#endif
