#ifndef COLORS_H
#define COLORS_H

// RESET
#define RESET "\x1b[0m"

// ESTILOS
#define BOLD "\x1b[1m"
#define UNDERLINE "\x1b[4m"

// COLORES TEXTO
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"

// MENÚ
#define MENU_TITLE     CYAN BOLD
#define MENU_OPTION    GREEN
#define MENU_INPUT     YELLOW
#define MENU_BORDER    BLUE

// ENTIDADES
#define COLOR_SITIO    MAGENTA
#define COLOR_EVENTO   CYAN
#define COLOR_ASIENTO  YELLOW
#define COLOR_USUARIO  BLUE

// MENSAJES
#define MSG_ERROR      RED BOLD
#define MSG_SUCCESS    GREEN BOLD
#define MSG_WARNING    YELLOW BOLD
#define MSG_INFO       CYAN

// EXTRA
#define HIGHLIGHT      MAGENTA BOLD

#endif