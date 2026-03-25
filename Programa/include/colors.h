#ifndef COLORS_H
#define COLORS_H

// Códigos ANSI para colores de texto
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"
#define COLOR_RESET   "\x1b[0m"

// Estilos adicionales
#define STYLE_BOLD     "\x1b[1m"
#define STYLE_UNDERLINE "\x1b[4m"
#define STYLE_BLINK    "\x1b[5m"  // puede que no funcione 

// Colores de fondo
#define BG_RED     "\x1b[41m"
#define BG_GREEN   "\x1b[42m"
#define BG_YELLOW  "\x1b[43m"
#define BG_BLUE    "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN    "\x1b[46m"

// Combinaciones útiles
#define MENU_TITLE   COLOR_CYAN STYLE_BOLD
#define MENU_OPTION  COLOR_GREEN
#define MENU_ERROR   COLOR_RED STYLE_BOLD
#define MENU_SUCCESS COLOR_GREEN

#endif