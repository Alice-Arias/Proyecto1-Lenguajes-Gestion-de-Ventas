#ifndef ADMIN_H
#define ADMIN_H

#define MAX_NOMBRE 50
#define ARCHIVO_USUARIOS "data/usuarios.txt"

typedef struct {
    char usuario[MAX_NOMBRE];
    unsigned long hash;// Almacena el hash de la contraseña
} Usuario;

int IniciarSesionAdmin();
void menuAdmin();
void cargarUsuarios();
void liberarUsuarios();

#endif