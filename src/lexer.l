/* FLEX genera código C a partir de este archivo.
   Secciones separadas por %%: definiciones, reglas y funciones de apoyo.
   noyywrap termina al agotar la entrada sin pedir otro archivo.
   yylineno mantiene el número de línea al consumir saltos de línea. */
%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include <string.h>
#include "lexer.h"

/* El bloque entre %{ y %} se copia al código C generado.
   static mantiene estos datos internos al módulo del analizador. */
static Token tokens[MAX_TOKENS];
static int cantidad_tokens = 0;

void agregar_token(const char *tipo, const char *lexema)
{
    /* Evita escribir fuera del arreglo. Al llegar al límite, los tokens
       adicionales se descartan sin mostrar un aviso en esta versión. */
    if (cantidad_tokens >= MAX_TOKENS)
        return;

    /* Copiar el tipo con un límite y agregar '\0' manualmente, porque
       strncpy no garantiza ese terminador si el texto es muy largo. */
    strncpy(
        tokens[cantidad_tokens].tipo,
        tipo,
        MAX_TIPO - 1
    );

    tokens[cantidad_tokens].tipo[MAX_TIPO - 1] = '\0';

    strncpy(
        tokens[cantidad_tokens].lexema,
        lexema,
        MAX_LEXEMA - 1
    );

    /* Los lexemas mayores que la capacidad quedan recortados. */
    tokens[cantidad_tokens].lexema[MAX_LEXEMA - 1] = '\0';

    tokens[cantidad_tokens].linea = yylineno;

    cantidad_tokens++;
}
%}

    /* ---------- DEFINICIONES ---------- */

    /* [] define un conjunto de caracteres; | significa alternativa;
       * permite cero o más repeticiones y + exige al menos una.
       Un identificador empieza con letra o guion bajo y puede continuar
       con letras, guiones bajos o dígitos. Un decimal exige dígitos
       antes y después del punto; el signo se reconoce por separado. */
LETRA       [a-zA-Z_]
DIGITO      [0-9]

IDENTIFICADOR   {LETRA}({LETRA}|{DIGITO})*
ENTERO          {DIGITO}+
DECIMAL         {DIGITO}+"."{DIGITO}+
    /* Patrón de cadenas entre comillas dobles con secuencias escapadas.
       Limitación actual: la clase interior permite comillas sin escapar,
       por lo que puede abarcar varias cadenas de una misma línea. */
CADENA          \"([^\\\n]|\\.)*\"

%%

    /* ---------- PALABRAS RESERVADAS ---------- */
    /* Cada regla tiene un patrón y una acción en C entre llaves.
       yytext contiene el lexema reconocido. FLEX elige la coincidencia
       más larga; en un empate gana la regla escrita primero.
       Por eso las palabras reservadas preceden al identificador:
       int es reservada, mientras que entero es un identificador.
       El analizador clasifica texto; no ejecuta ni valida su sintaxis. */

"int"       { agregar_token("PALABRA RESERVADA", yytext); }
"float"     { agregar_token("PALABRA RESERVADA", yytext); }
"double"    { agregar_token("PALABRA RESERVADA", yytext); }
"char"      { agregar_token("PALABRA RESERVADA", yytext); }
"void"      { agregar_token("PALABRA RESERVADA", yytext); }

"if"        { agregar_token("PALABRA RESERVADA", yytext); }
"else"      { agregar_token("PALABRA RESERVADA", yytext); }
"while"     { agregar_token("PALABRA RESERVADA", yytext); }
"for"       { agregar_token("PALABRA RESERVADA", yytext); }
"do"        { agregar_token("PALABRA RESERVADA", yytext); }

"switch"    { agregar_token("PALABRA RESERVADA", yytext); }
"case"      { agregar_token("PALABRA RESERVADA", yytext); }
"default"   { agregar_token("PALABRA RESERVADA", yytext); }

"return"    { agregar_token("PALABRA RESERVADA", yytext); }
"break"     { agregar_token("PALABRA RESERVADA", yytext); }
"continue"  { agregar_token("PALABRA RESERVADA", yytext); }

"struct"    { agregar_token("PALABRA RESERVADA", yytext); }
"const"     { agregar_token("PALABRA RESERVADA", yytext); }


    /* ---------- COMENTARIOS ---------- */
    /* Desde // hasta antes del salto de línea. Se guarda como token
       para mostrarlo en la tabla; no hay regla de comentario de bloque. */

"//"[^\n]* {
    agregar_token("COMENTARIO", yytext);
}


    /* ---------- CADENAS ---------- */

{CADENA} {
    agregar_token("CADENA", yytext);
}


    /* ---------- NÚMEROS ---------- */

{DECIMAL} {
    agregar_token("NUMERO DECIMAL", yytext);
}

{ENTERO} {
    agregar_token("NUMERO ENTERO", yytext);
}


    /* ---------- OPERADORES RELACIONALES ---------- */

"==" {
    agregar_token("OPERADOR RELACIONAL", yytext);
}

"!=" {
    agregar_token("OPERADOR RELACIONAL", yytext);
}

">=" {
    agregar_token("OPERADOR RELACIONAL", yytext);
}

"<=" {
    agregar_token("OPERADOR RELACIONAL", yytext);
}

">" {
    agregar_token("OPERADOR RELACIONAL", yytext);
}

"<" {
    agregar_token("OPERADOR RELACIONAL", yytext);
}


    /* ---------- OPERADORES LÓGICOS ---------- */

"&&" {
    agregar_token("OPERADOR LOGICO", yytext);
}

"||" {
    agregar_token("OPERADOR LOGICO", yytext);
}

"!" {
    agregar_token("OPERADOR LOGICO", yytext);
}


    /* ---------- INCREMENTO / DECREMENTO ---------- */

"++" {
    agregar_token("INCREMENTO", yytext);
}

"--" {
    agregar_token("DECREMENTO", yytext);
}


    /* ---------- ASIGNACIÓN ---------- */

"+=" {
    agregar_token("ASIGNACION COMPUESTA", yytext);
}

"-=" {
    agregar_token("ASIGNACION COMPUESTA", yytext);
}

"*=" {
    agregar_token("ASIGNACION COMPUESTA", yytext);
}

"/=" {
    agregar_token("ASIGNACION COMPUESTA", yytext);
}

"=" {
    agregar_token("ASIGNACION", yytext);
}


    /* ---------- OPERADORES ARITMÉTICOS ---------- */

"+" {
    agregar_token("OPERADOR ARITMETICO", yytext);
}

"-" {
    agregar_token("OPERADOR ARITMETICO", yytext);
}

"*" {
    agregar_token("OPERADOR ARITMETICO", yytext);
}

"/" {
    agregar_token("OPERADOR ARITMETICO", yytext);
}

"%" {
    agregar_token("OPERADOR ARITMETICO", yytext);
}


    /* ---------- SÍMBOLOS ---------- */

"(" {
    agregar_token("PARENTESIS ABRE", yytext);
}

")" {
    agregar_token("PARENTESIS CIERRA", yytext);
}

"{" {
    agregar_token("LLAVE ABRE", yytext);
}

"}" {
    agregar_token("LLAVE CIERRA", yytext);
}

"[" {
    agregar_token("CORCHETE ABRE", yytext);
}

"]" {
    agregar_token("CORCHETE CIERRA", yytext);
}

";" {
    agregar_token("PUNTO Y COMA", yytext);
}

"," {
    agregar_token("COMA", yytext);
}

"." {
    agregar_token("PUNTO", yytext);
}


    /* ---------- IDENTIFICADORES ---------- */

{IDENTIFICADOR} {
    agregar_token("IDENTIFICADOR", yytext);
}


    /* ---------- ESPACIOS ---------- */

[ \t\r]+ {
    /* Ignorar */
}

\n {
    /* FLEX actualiza yylineno */
}


    /* ---------- CUALQUIER CARÁCTER NO VÁLIDO ---------- */
    /* El punto sin comillas reconoce un carácter salvo salto de línea.
       Esta regla final registra lo que ninguna regla anterior reconoce
       y permite continuar buscando los siguientes tokens. */

. {
    agregar_token("ERROR LEXICO", yytext);
}

%%


void analizar_texto(const char *texto)
{
    /* Procesar texto en memoria permite usar FLEX desde la ventana
       sin pedir entrada por consola ni abrir un archivo. */
    YY_BUFFER_STATE buffer;

    /* Cada análisis comienza de nuevo, desde la primera línea. */
    cantidad_tokens = 0;
    yylineno = 1;

    /* FLEX crea una copia del texto y la selecciona como entrada. */
    buffer = yy_scan_string(texto);

    /* Ejecuta las reglas hasta agotar la entrada. Sus acciones van
       llenando el arreglo mediante agregar_token. */
    yylex();

    /* Libera la entrada temporal; los tokens conservan sus propias copias. */
    yy_delete_buffer(buffer);
}


int obtener_cantidad_tokens(void)
{
    return cantidad_tokens;
}


const Token *obtener_token(int indice)
{
    /* Validar el índice antes de devolver la dirección del elemento. */
    if (indice < 0 || indice >= cantidad_tokens)
        return NULL;

    return &tokens[indice];
}
