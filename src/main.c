/*
   Interfaz gráfica del analizador, escrita en C con la API Win32.
   Recorrido: WinMain crea la ventana; Windows envia eventos a
   WindowProcedure; el boton Analizar llama a analizarCodigo;
   FLEX procesa el texto y mostrarTokens llena la tabla.
   Las reglas que reconocen los tokens están en lexer.l.
*/
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

/* ============================================
   IDENTIFICADORES DE LOS CONTROLES
   ============================================ */

#define ID_ANALIZAR  101
#define ID_LIMPIAR   102
#define ID_EDITOR    103
#define ID_LISTA     104

/* ============================================
   CONTROLES
   ============================================ */

/* HWND identifica un control de Windows: permite leerlo o modificarlo.
   Estas variables se comparten entre las funciones de la interfaz. */
HWND hEditor;
HWND hLista;
HWND hBtnAnalizar;
HWND hBtnLimpiar;
HWND hLabelCodigo;
HWND hLabelTokens;
HWND hLabelEstado;
HWND hTitulo;

/* HFONT identifica una fuente creada por el programa.
   Se libera al cerrar la ventana para no dejar recursos ocupados. */
HFONT fuenteNormal;
HFONT fuenteTitulo;


/* ============================================
   CREAR COLUMNAS DE LA TABLA
   ============================================ */

void crearColumnas()
{
    /* LVCOLUMNA describe una columna. mask indica qué datos se usan;
       cx es el ancho en píxeles y pszText es el encabezado visible. */
    LVCOLUMNA columna;

    ZeroMemory(&columna, sizeof(columna));

    columna.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    /* Columna 1 */

    columna.cx = 180;
    columna.pszText = "Lexema";
    columna.iSubItem = 0;

    ListView_InsertColumn(hLista, 0, &columna);

    /* Columna 2 */

    columna.cx = 230;
    columna.pszText = "Token";
    columna.iSubItem = 1;

    ListView_InsertColumn(hLista, 1, &columna);

    /* Columna 3 */

    columna.cx = 80;
    columna.pszText = "Linea";
    columna.iSubItem = 2;

    ListView_InsertColumn(hLista, 2, &columna);
}


/* ============================================
   MOSTRAR TOKENS EN LA TABLA
   ============================================ */

void mostrarTokens()
{
    /* Borra el resultado anterior y crea una fila por token.
       La fila tiene tres celdas: lexema, categoría y número de línea. */
    int cantidad;
    int i;

    char lineaTexto[20];

    ListView_DeleteAllItems(hLista);

    cantidad = obtener_cantidad_tokens();

    for (i = 0; i < cantidad; i++)
    {
        const Token *token;

        LVITEMA item;
        LVITEMA subItem;

        /* Solo consultamos el token: su memoria pertenece al analizador.
           const impide modificar sus datos mediante este puntero. */
        token = obtener_token(i);

        if (token == NULL)
            continue;

        /* -------------------------------
           INSERTAR LEXEMA
           ------------------------------- */

        /* Inicializar a cero evita usar campos con valores indeterminados.
           iItem selecciona la fila e iSubItem selecciona la columna. */
        ZeroMemory(&item, sizeof(item));

        item.mask = LVIF_TEXT;

        item.iItem = i;
        item.iSubItem = 0;

        item.pszText = (LPSTR) token->lexema;

        SendMessageA(
            hLista,
            LVM_INSERTITEMA,
            0,
            (LPARAM)&item
        );


        /* -------------------------------
           INSERTAR TIPO DE TOKEN
           ------------------------------- */

        ZeroMemory(&subItem, sizeof(subItem));

        subItem.iSubItem = 1;
        subItem.pszText = (LPSTR) token->tipo;

        SendMessageA(
            hLista,
            LVM_SETITEMTEXTA,
            i,
            (LPARAM)&subItem
        );


        /* -------------------------------
           INSERTAR LÍNEA
           ------------------------------- */

        snprintf(
            lineaTexto,
            sizeof(lineaTexto),
            "%d",
            token->linea
        );

        ZeroMemory(&subItem, sizeof(subItem));

        subItem.iSubItem = 2;
        subItem.pszText = lineaTexto;

        SendMessageA(
            hLista,
            LVM_SETITEMTEXTA,
            i,
            (LPARAM)&subItem
        );
    }

    /* Mostrar cantidad de tokens */

    {
        char mensaje[100];

        snprintf(
            mensaje,
            sizeof(mensaje),
            "Tokens encontrados: %d",
            cantidad
        );

        SetWindowTextA(
            hLabelEstado,
            mensaje
        );
    }
}


/* ============================================
   ANALIZAR EL CÓDIGO
   ============================================ */

void analizarCodigo(HWND ventana)
{
    /* Lee el contenido del editor, lo analiza y actualiza la tabla.
       ventana se usa como propietaria de los mensajes de aviso. */
    int longitud;

    char *texto;

    longitud = GetWindowTextLengthA(hEditor);

    if (longitud <= 0)
    {
        MessageBoxA(
            ventana,
            "Escribe algun codigo antes de analizar.",
            "Analizador Lexico",
            MB_OK | MB_ICONINFORMATION
        );

        return;
    }

    /* Se reserva un carácter adicional para el terminador '\0' de C. */
    texto = (char *)malloc(longitud + 1);

    if (texto == NULL)
    {
        MessageBoxA(
            ventana,
            "No se pudo reservar memoria.",
            "Error",
            MB_OK | MB_ICONERROR
        );

        return;
    }

    GetWindowTextA(
        hEditor,
        texto,
        longitud + 1
    );

    /* Enviar código a FLEX */

    analizar_texto(texto);

    /* Mostrar resultado */

    mostrarTokens();

    /* El texto temporal ya no se necesita: los lexemas fueron copiados
       al arreglo de tokens por agregar_token, en lexer.l. */
    free(texto);
}


/* ============================================
   LIMPIAR
   ============================================ */

void limpiar()
{
    /* Vacía los controles visibles y devuelve el cursor al editor.
       El arreglo interno se reinicia cuando se vuelve a analizar. */
    SetWindowTextA(
        hEditor,
        ""
    );

    ListView_DeleteAllItems(
        hLista
    );

    SetWindowTextA(
        hLabelEstado,
        "Tokens encontrados: 0"
    );

    SetFocus(hEditor);
}


/* ============================================
   AJUSTAR CONTROLES AL REDIMENSIONAR
   ============================================ */

void ajustarControles(HWND ventana)
{
    /* Calcula posiciones a partir del área interior de la ventana.
       El editor recibe el 45 por ciento del ancho disponible;
       la tabla recibe el resto. MoveWindow aplica cada posición. */
    RECT rect;

    int ancho;
    int alto;

    int margen = 20;
    int separacion = 20;

    int anchoDisponible;
    int anchoIzquierda;
    int anchoDerecha;

    int yEditor = 105;
    int alturaContenido;

    GetClientRect(
        ventana,
        &rect
    );

    ancho = rect.right;
    alto = rect.bottom;

    anchoDisponible =
        ancho -
        (margen * 2) -
        separacion;

    anchoIzquierda =
        (anchoDisponible * 45) / 100;

    anchoDerecha =
        anchoDisponible -
        anchoIzquierda;

    alturaContenido =
        alto - 190;

    if (alturaContenido < 200)
        alturaContenido = 200;


    MoveWindow(
        hTitulo,
        margen,
        15,
        ancho - 40,
        35,
        TRUE
    );


    MoveWindow(
        hLabelCodigo,
        margen,
        75,
        anchoIzquierda,
        25,
        TRUE
    );


    MoveWindow(
        hEditor,
        margen,
        yEditor,
        anchoIzquierda,
        alturaContenido,
        TRUE
    );


    MoveWindow(
        hLabelTokens,
        margen + anchoIzquierda + separacion,
        75,
        anchoDerecha,
        25,
        TRUE
    );


    MoveWindow(
        hLista,
        margen + anchoIzquierda + separacion,
        yEditor,
        anchoDerecha,
        alturaContenido,
        TRUE
    );


    MoveWindow(
        hBtnAnalizar,
        margen,
        alto - 65,
        140,
        38,
        TRUE
    );


    MoveWindow(
        hBtnLimpiar,
        margen + 150,
        alto - 65,
        140,
        38,
        TRUE
    );


    MoveWindow(
        hLabelEstado,
        ancho - 260,
        alto - 55,
        230,
        30,
        TRUE
    );
}


/* ============================================
   PROCEDIMIENTO PRINCIPAL DE LA VENTANA
   ============================================ */

/* Windows llama a esta función cuando ocurre un evento.
   mensaje identifica el evento; wParam y lParam aportan sus datos.
   Los eventos no atendidos se delegan a DefWindowProcA. */
LRESULT CALLBACK WindowProcedure(
    HWND ventana,
    UINT mensaje,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (mensaje)
    {

        /* ==================================
           CREAR INTERFAZ
           ================================== */

        case WM_CREATE:
        {
            /* Se ejecuta al crear la ventana. Los controles son ventanas
               hijas: WS_CHILD las asocia con la ventana principal y
               WS_VISIBLE hace que aparezcan en pantalla.
               Las funciones terminadas en A trabajan con texto char. */

            /* Fuente normal */

            fuenteNormal = CreateFontA(
                18,
                0,
                0,
                0,
                FW_NORMAL,
                FALSE,
                FALSE,
                FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,
                DEFAULT_PITCH,
                "Segoe UI"
            );


            /* Fuente del título */

            fuenteTitulo = CreateFontA(
                26,
                0,
                0,
                0,
                FW_BOLD,
                FALSE,
                FALSE,
                FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,
                DEFAULT_PITCH,
                "Segoe UI"
            );


            /* Título */

            hTitulo = CreateWindowA(
                "STATIC",
                "ANALIZADOR LEXICO",
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                20,
                15,
                900,
                35,
                ventana,
                NULL,
                NULL,
                NULL
            );


            /* Etiqueta del código */

            hLabelCodigo = CreateWindowA(
                "STATIC",
                "Codigo fuente",
                WS_CHILD | WS_VISIBLE,
                20,
                75,
                400,
                25,
                ventana,
                NULL,
                NULL,
                NULL
            );


            /* Editor */

            hEditor = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_CHILD |
                WS_VISIBLE |
                WS_VSCROLL |
                WS_HSCROLL |
                ES_MULTILINE |
                ES_AUTOVSCROLL |
                ES_AUTOHSCROLL |
                ES_WANTRETURN,
                20,
                105,
                430,
                450,
                ventana,
                (HMENU)(INT_PTR)ID_EDITOR,
                NULL,
                NULL
            );


            /* Etiqueta de los tokens */

            hLabelTokens = CreateWindowA(
                "STATIC",
                "Tokens encontrados",
                WS_CHILD | WS_VISIBLE,
                470,
                75,
                450,
                25,
                ventana,
                NULL,
                NULL,
                NULL
            );


            /* Tabla */

            hLista = CreateWindowExA(
                WS_EX_CLIENTEDGE,
                WC_LISTVIEWA,
                "",
                WS_CHILD |
                WS_VISIBLE |
                LVS_REPORT |
                LVS_SINGLESEL |
                LVS_SHOWSELALWAYS,
                470,
                105,
                500,
                450,
                ventana,
                (HMENU)(INT_PTR)ID_LISTA,
                NULL,
                NULL
            );


            /* Botón analizar */

            hBtnAnalizar = CreateWindowA(
                "BUTTON",
                "Analizar",
                WS_CHILD |
                WS_VISIBLE |
                BS_PUSHBUTTON,
                20,
                580,
                140,
                38,
                ventana,
                (HMENU)(INT_PTR)ID_ANALIZAR,
                NULL,
                NULL
            );


            /* Botón limpiar */

            hBtnLimpiar = CreateWindowA(
                "BUTTON",
                "Limpiar",
                WS_CHILD |
                WS_VISIBLE |
                BS_PUSHBUTTON,
                170,
                580,
                140,
                38,
                ventana,
                (HMENU)(INT_PTR)ID_LIMPIAR,
                NULL,
                NULL
            );


            /* Estado */

            hLabelEstado = CreateWindowA(
                "STATIC",
                "Tokens encontrados: 0",
                WS_CHILD | WS_VISIBLE | SS_RIGHT,
                700,
                585,
                250,
                30,
                ventana,
                NULL,
                NULL,
                NULL
            );


            /* Fuente */

            SendMessage(
                hTitulo,
                WM_SETFONT,
                (WPARAM)fuenteTitulo,
                TRUE
            );

            SendMessage(
                hLabelCodigo,
                WM_SETFONT,
                (WPARAM)fuenteNormal,
                TRUE
            );

            SendMessage(
                hLabelTokens,
                WM_SETFONT,
                (WPARAM)fuenteNormal,
                TRUE
            );

            SendMessage(
                hEditor,
                WM_SETFONT,
                (WPARAM)fuenteNormal,
                TRUE
            );

            SendMessage(
                hLista,
                WM_SETFONT,
                (WPARAM)fuenteNormal,
                TRUE
            );

            SendMessage(
                hBtnAnalizar,
                WM_SETFONT,
                (WPARAM)fuenteNormal,
                TRUE
            );

            SendMessage(
                hBtnLimpiar,
                WM_SETFONT,
                (WPARAM)fuenteNormal,
                TRUE
            );

            SendMessage(
                hLabelEstado,
                WM_SETFONT,
                (WPARAM)fuenteNormal,
                TRUE
            );


            /* Diseño tabla */

            ListView_SetExtendedListViewStyle(
                hLista,
                LVS_EX_FULLROWSELECT |
                LVS_EX_GRIDLINES
            );

            crearColumnas();


            break;
        }


        /* ==================================
           BOTONES
           ================================== */

        case WM_COMMAND:
        {
            /* LOWORD extrae el identificador del control que originó
               el evento. Los ID permiten distinguir los dos botones. */

            switch (LOWORD(wParam))
            {

                case ID_ANALIZAR:

                    analizarCodigo(
                        ventana
                    );

                    break;


                case ID_LIMPIAR:

                    limpiar();

                    break;
            }

            break;
        }


        /* ==================================
           REDIMENSIONAR
           ================================== */

        case WM_SIZE:
            /* Reubicar los controles cuando cambia el tamaño. */
            ajustarControles(
                ventana
            );

            break;


        /* ==================================
           CERRAR
           ================================== */

        case WM_DESTROY:
            /* Liberar fuentes y solicitar el fin del bucle de mensajes. */
            if (fuenteNormal)
                DeleteObject(fuenteNormal);

            if (fuenteTitulo)
                DeleteObject(fuenteTitulo);

            PostQuitMessage(0);

            break;


        default:

            return DefWindowProcA(
                ventana,
                mensaje,
                wParam,
                lParam
            );
    }

    return 0;
}


/* ============================================
   FUNCIÓN PRINCIPAL WINDOWS
   ============================================ */

/* Punto de entrada de la aplicación gráfica.
   hInstance identifica esta aplicación y nCmdShow indica cómo mostrarla.
   Registrar la clase asocia las ventanas con WindowProcedure. */
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    WNDCLASSA wc;

    HWND ventana;

    MSG mensaje;

    INITCOMMONCONTROLSEX controles;


    /* ========================================
       ACTIVAR CONTROLES COMUNES
       ======================================== */

    controles.dwSize =
        sizeof(INITCOMMONCONTROLSEX);

    controles.dwICC =
        ICC_LISTVIEW_CLASSES;

    InitCommonControlsEx(
        &controles
    );


    /* ========================================
       REGISTRAR VENTANA
       ======================================== */

    ZeroMemory(
        &wc,
        sizeof(wc)
    );

    wc.lpfnWndProc =
        WindowProcedure;

    wc.hInstance =
        hInstance;

    wc.lpszClassName =
        "AnalizadorLexicoWindow";

    wc.hCursor =
        LoadCursor(
            NULL,
            IDC_ARROW
        );

    wc.hIcon =
        LoadIcon(
            NULL,
            IDI_APPLICATION
        );

    wc.hbrBackground =
        (HBRUSH)(COLOR_WINDOW + 1);


    if (!RegisterClassA(&wc))
    {
        MessageBoxA(
            NULL,
            "No se pudo registrar la ventana.",
            "Error",
            MB_OK | MB_ICONERROR
        );

        return 0;
    }


    /* ========================================
       CREAR VENTANA
       ======================================== */

    ventana = CreateWindowExA(
        0,
        "AnalizadorLexicoWindow",
        "Analizador Lexico - FLEX + C + Win32 API",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1100,
        700,
        NULL,
        NULL,
        hInstance,
        NULL
    );


    if (ventana == NULL)
    {
        MessageBoxA(
            NULL,
            "No se pudo crear la ventana.",
            "Error",
            MB_OK | MB_ICONERROR
        );

        return 0;
    }


    ShowWindow(
        ventana,
        nCmdShow
    );

    UpdateWindow(
        ventana
    );


    /* ========================================
       BUCLE DE MENSAJES
       ======================================== */

    /* GetMessage espera eventos sin mantener un ciclo de espera activo.
       TranslateMessage prepara mensajes de caracteres del teclado;
       DispatchMessage entrega el evento al procedimiento de la ventana.
       Al recibir WM_QUIT, GetMessage devuelve 0 y termina el bucle. */
    while (
        GetMessageA(
            &mensaje,
            NULL,
            0,
            0
        ) > 0
    )
    {
        TranslateMessage(
            &mensaje
        );

        DispatchMessageA(
            &mensaje
        );
    }

    return (int)mensaje.wParam;
}
