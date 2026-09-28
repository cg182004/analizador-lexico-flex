# Documentación del lenguaje y guía de pruebas

## 1. Objetivo

El proyecto es una aplicación gráfica que recibe texto escrito por el usuario y lo divide en componentes léxicos llamados **tokens**. Para cada componente muestra su lexema, tipo y línea de aparición.

El **lexema** es el fragmento concreto encontrado, por ejemplo `edad`. El **tipo de token** es su clasificación, por ejemplo `IDENTIFICADOR`.

## 2. Lenguaje de implementación y lenguaje reconocido

La aplicación está escrita en **C**. La interfaz utiliza la **API Win32 de Windows** y el analizador se genera con **FLEX**, a partir de las expresiones regulares y acciones del archivo `src/lexer.l`.

El lenguaje de entrada es un **subconjunto léxico de C** definido por las reglas de este proyecto. No se admite todo el lenguaje C. El analizador clasifica fragmentos de texto; no ejecuta instrucciones, comprueba tipos de variables ni verifica la estructura sintáctica del programa.

Por ejemplo, `int = ;` produce tokens reconocidos, aunque no es una declaración válida en C. La ausencia de errores léxicos no garantiza que un programa sea correcto.

## 3. Reglas del lenguaje de entrada

### 3.1. Palabras reservadas

Se reconocen exactamente estas 18 palabras, escritas en minúsculas:

```text
int float double char void
if else while for do
switch case default
return break continue
struct const
```

Todas reciben el tipo `PALABRA RESERVADA`. Se distinguen mayúsculas y minúsculas: `int` es reservada, pero `Int` e `INT` son identificadores.

### 3.2. Identificadores

Representan nombres de variables, funciones u otros elementos. Deben comenzar con una letra de `a` a `z`, de `A` a `Z` o un guion bajo `_`. Después pueden contener esas mismas letras, guiones bajos y dígitos.

Expresión regular equivalente:

```text
[a-zA-Z_][a-zA-Z_0-9]*
```

Ejemplos: `edad`, `notaFinal`, `_contador`, `valor2`.

No se incluyen letras acentuadas ni `ñ` en los identificadores. `2edad` no se reconoce como un solo identificador: se divide en el entero `2` y el identificador `edad`, sin generar un error específico por esa combinación.

### 3.3. Números

| Tipo mostrado | Regla | Ejemplos |
|---|---|---|
| `NUMERO ENTERO` | Uno o más dígitos: `[0-9]+` | `0`, `20`, `150` |
| `NUMERO DECIMAL` | Dígitos, punto y dígitos: `[0-9]+[.][0-9]+` | `0.5`, `95.5`, `10.25` |

El signo se reconoce por separado: `-20` produce `-` como operador aritmético y `20` como entero. `.5` produce un punto y un entero; `5.` produce un entero y un punto. No hay reglas específicas para notación científica ni números hexadecimales.

### 3.4. Cadenas

Se reconoce texto entre comillas dobles como `CADENA`. Por ejemplo:

```text
"Hola mundo"
```

Las comillas forman parte del lexema mostrado. La regla admite secuencias precedidas por una barra inversa, como `\"` y `\n`, sin interpretar ni validar su significado. No admite un salto de línea real dentro de una cadena.

**Limitación de la versión actual:** el patrón también permite comillas dobles sin escapar dentro de la coincidencia. Por eso `"hola" + "mundo"` se reconoce como una sola `CADENA`. Para las pruebas básicas, utilizar una cadena por línea. Este comportamiento es una limitación del patrón, no una regla correcta del lenguaje C.

Si se escribe `"hola` sin cerrar las comillas, la comilla inicial se marca como `ERROR LEXICO` y `hola` se reconoce como `IDENTIFICADOR`; no existe un mensaje específico de cadena sin cerrar.

### 3.5. Comentarios

Los comentarios comienzan con `//` y continúan hasta el final de la línea. Se muestran como un token `COMENTARIO`, incluyendo el texto `//`.

```text
// Esto es un comentario
```

No hay una regla para comentarios de bloque. Una entrada como `/* hola */` se divide en operadores e identificadores, en lugar de reconocerse como comentario.

### 3.6. Operadores

| Tipo mostrado | Lexemas reconocidos |
|---|---|
| `OPERADOR ARITMETICO` | `+`, `-`, `*`, `/`, `%` |
| `OPERADOR RELACIONAL` | `==`, `!=`, `>=`, `<=`, `>`, `<` |
| `OPERADOR LOGICO` | `&&`, `\|\|`, `!` |
| `ASIGNACION` | `=` |
| `ASIGNACION COMPUESTA` | `+=`, `-=`, `*=`, `/=` |
| `INCREMENTO` | `++` |
| `DECREMENTO` | `--` |

`&` y `|` aislados no están definidos y producen errores léxicos. `%=` se divide en `%` y `=`, porque no existe una regla para ese operador compuesto.

### 3.7. Símbolos

| Lexema | Tipo mostrado |
|---|---|
| `(` | `PARENTESIS ABRE` |
| `)` | `PARENTESIS CIERRA` |
| `{` | `LLAVE ABRE` |
| `}` | `LLAVE CIERRA` |
| `[` | `CORCHETE ABRE` |
| `]` | `CORCHETE CIERRA` |
| `;` | `PUNTO Y COMA` |
| `,` | `COMA` |
| `.` | `PUNTO` |

No están definidos `:`, `#`, `?` ni las comillas simples. Aunque `case` y `char` son palabras reservadas, el programa no admite todos los elementos necesarios para analizar sus construcciones completas en C.

### 3.8. Espacios, líneas y errores

Los espacios, tabulaciones y retornos de carro no generan tokens. Los saltos de línea tampoco generan tokens, pero actualizan el número de línea, que comienza en 1 en cada análisis.

Cualquier carácter que no coincida con las reglas anteriores se registra como `ERROR LEXICO`. El análisis continúa después del error. El contador de la interfaz incluye todos los registros: tokens reconocidos, comentarios y errores.

FLEX selecciona la coincidencia más larga. Por ejemplo, `>=` se reconoce como un solo operador. Cuando dos reglas coinciden con la misma longitud, se utiliza la que aparece primero: así `int` se clasifica como palabra reservada antes que como identificador.

## 4. Cómo realizar las pruebas en la aplicación

1. Abrir `build/AnalizadorLexico.exe` desde el Explorador de Windows.
2. Escribir o pegar uno de los ejemplos siguientes en el editor.
3. Pulsar **Analizar**.
4. Comparar las columnas **Lexema**, **Token** y **Linea**, y el contador, con el resultado esperado.
5. Pulsar **Limpiar** antes de introducir otro ejemplo.

Los bloques siguientes contienen únicamente el texto que debe introducirse. Los resultados son expectativas derivadas de las reglas actuales; esta guía no constituye un registro de pruebas visuales ya realizadas.

## 5. Casos de prueba y resultados esperados

### Prueba 1. Declaración sencilla

```c
int edad = 20;
```

| Lexema | Token | Línea |
|---|---|---|
| `int` | `PALABRA RESERVADA` | 1 |
| `edad` | `IDENTIFICADOR` | 1 |
| `=` | `ASIGNACION` | 1 |
| `20` | `NUMERO ENTERO` | 1 |
| `;` | `PUNTO Y COMA` | 1 |

**Total esperado: 5 tokens, sin errores léxicos.**

### Prueba 2. Decimales, condición y varias líneas

```c
float nota = 95.5;
if (nota >= 70) {
    nota++;
}
```

| Línea | Lexemas esperados, en orden | Cantidad |
|---|---|---|
| 1 | `float`, `nota`, `=`, `95.5`, `;` | 5 |
| 2 | `if`, `(`, `nota`, `>=`, `70`, `)`, `{` | 7 |
| 3 | `nota`, `++`, `;` | 3 |
| 4 | `}` | 1 |

Comprobar especialmente que `95.5` sea `NUMERO DECIMAL`, `>=` sea `OPERADOR RELACIONAL` y `++` sea `INCREMENTO`.

**Total esperado: 16 tokens, sin errores léxicos.**

### Prueba 3. Palabras reservadas e identificadores

```text
int Int int2 _dato dato_2
```

`int` debe ser `PALABRA RESERVADA`. Los otros cuatro lexemas deben ser `IDENTIFICADOR`. Todos están en la línea 1.

**Total esperado: 5 tokens, sin errores léxicos.**

### Prueba 4. Comentario y cadena

```c
// Saludo
mensaje = "Hola mundo";
```

| Lexema | Token | Línea |
|---|---|---|
| `// Saludo` | `COMENTARIO` | 1 |
| `mensaje` | `IDENTIFICADOR` | 2 |
| `=` | `ASIGNACION` | 2 |
| `"Hola mundo"` | `CADENA` | 2 |
| `;` | `PUNTO Y COMA` | 2 |

**Total esperado: 5 tokens, sin errores léxicos.** No se comprueba si `mensaje` fue declarado.

### Prueba 5. Operadores

```text
+ - * / %
== != >= <= > <
&& || !
+= -= *= /= =
++ --
```

| Línea | Resultado esperado | Cantidad |
|---|---|---|
| 1 | Cinco operadores aritméticos | 5 |
| 2 | Seis operadores relacionales | 6 |
| 3 | Tres operadores lógicos | 3 |
| 4 | Cuatro asignaciones compuestas y una asignación simple | 5 |
| 5 | Un incremento y un decremento | 2 |

**Total esperado: 21 tokens, sin errores léxicos.** Esta entrada prueba categorías, no un programa completo.

### Prueba 6. Símbolos

```text
( ) { } [ ] ; , .
```

Cada símbolo debe aparecer por separado con el tipo indicado en la sección 3.7.

**Total esperado: 9 tokens, todos en la línea 1.**

### Prueba 7. Caracteres desconocidos

```text
int edad = 20;
@ # $
```

La primera línea produce los cinco tokens de la prueba 1. En la segunda línea, `@`, `#` y `$` producen un `ERROR LEXICO` cada uno.

**Total esperado: 8 registros, de los cuales 3 son errores léxicos.**

### Prueba 8. Límites de las reglas numéricas

```text
-20 .5 5. 2edad
```

| Lexema | Token |
|---|---|
| `-` | `OPERADOR ARITMETICO` |
| `20` | `NUMERO ENTERO` |
| `.` | `PUNTO` |
| `5` | `NUMERO ENTERO` |
| `5` | `NUMERO ENTERO` |
| `.` | `PUNTO` |
| `2` | `NUMERO ENTERO` |
| `edad` | `IDENTIFICADOR` |

**Total esperado: 8 tokens, sin errores léxicos, todos en la línea 1.**

### Prueba 9. Limitación conocida de las cadenas

```text
"hola" + "mundo"
```

**Resultado actual esperado: 1 token `CADENA` con toda la línea como lexema.** Se documenta para mostrar una limitación pendiente de corrección.

### Prueba 10. Entrada vacía y reinicio

1. Pulsar **Limpiar**: el editor y la tabla deben quedar vacíos y el contador debe indicar 0.
2. Pulsar **Analizar** sin escribir: debe aparecer el aviso `Escribe algun codigo antes de analizar.`
3. Escribir `int x;` y analizar: deben aparecer 3 tokens en la línea 1.
4. Sustituir el texto por `20` y analizar: debe aparecer únicamente 1 token `NUMERO ENTERO` en la línea 1, sin acumular resultados anteriores.

## 6. Límites de almacenamiento

- Se almacenan como máximo **1.000 tokens** por análisis. Los siguientes se descartan sin aviso.
- Cada lexema dispone de **255 bytes de contenido** y un byte para el terminador de C. Los lexemas más largos se recortan en el resultado mostrado.
- La interfaz usa funciones de Windows para texto de tipo `char`; no ofrece soporte Unicode completo.
- No se verifica que los paréntesis o llaves estén equilibrados, que existan las variables ni que los operadores tengan operandos válidos.

## 7. Registro de la comprobación manual

Completar esta tabla después de ejecutar cada prueba en la ventana y adjuntar capturas si se requieren para la entrega.

| Prueba | Resultado observado | ¿Coincide con lo esperado? |
|---|---|---|
| 1. Declaración | Pendiente | Pendiente |
| 2. Varias líneas | Pendiente | Pendiente |
| 3. Identificadores | Pendiente | Pendiente |
| 4. Comentario y cadena | Pendiente | Pendiente |
| 5. Operadores | Pendiente | Pendiente |
| 6. Símbolos | Pendiente | Pendiente |
| 7. Errores | Pendiente | Pendiente |
| 8. Números | Pendiente | Pendiente |
| 9. Limitación de cadenas | Pendiente | Pendiente |
| 10. Interfaz y reinicio | Pendiente | Pendiente |

## 8. Archivos de referencia del proyecto

- `src/lexer.l`: reglas que definen el lenguaje reconocido.
- `src/lexer.h`: estructura de los tokens y límites de almacenamiento.
- `src/main.c`: entrada por pantalla y presentación de resultados.
- `build/AnalizadorLexico.exe`: aplicación gráfica para realizar las pruebas.
