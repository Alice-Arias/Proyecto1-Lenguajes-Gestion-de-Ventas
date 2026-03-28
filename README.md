# Sistema de Gestión de Eventos y Venta de Boletos

![Lenguaje](https://img.shields.io/badge/Lenguaje-C-blue)
![Estado](https://img.shields.io/badge/Estado-Finalizado-brightgreen)
![Versión](https://img.shields.io/badge/Versión-1.0-orange)
![Licencia](https://img.shields.io/badge/Licencia-Académico-lightgrey)

Sistema desarrollado en **lenguaje C** que permite gestionar eventos, sitios, espacios (sectores) y la compra de boletos desde consola.  

Incluye funciones administrativas y de cliente, almacenamiento en archivos de texto y generación de facturas con control de asientos disponibles.

---

##  Integrantes

- **Alice Arias Salazar**  
- **Luis Carlos Trejos**  

---

##  Información académica

- **Curso:** Lenguajes de Programación  
- **Semestre:** I Semestre, 2026  
- **Proyecto:** Proyecto Programado #1  
- **Fecha de entrega:** 28/03/2026  
- **Estatus:** Excelente  

---

##  Descripción

Este sistema facilita la organización y gestión de eventos, permitiendo administrar sitios, sectores y la venta de boletos de forma controlada.

El programa funciona completamente en consola y mantiene la información mediante archivos `.txt`.

---

##  Funcionalidades

###  Administrador
- Gestión de sitios de eventos  
- Gestión de espacios (sectores y asientos)  
- Gestión de eventos  
- Consulta del estado de eventos  
- Lista de facturas  
- Estadísticas del sistema  

###  Cliente
- Consulta de eventos  
- Compra de boletos  
- Generación de facturas  

---

##  Características técnicas

- Uso de `struct`  
- Memoria dinámica (`malloc`, `realloc`, `free`)  
- Modularidad con `.h` y `.c`  
- Persistencia con archivos `.txt`  
- Hash de contraseñas  
- Colores en consola  

---

##  Compilación y ejecución

```bash
cd "Programa"
gcc main.c admin/admin.c cliente/cliente.c config/sistema.c security/hash.c src/*.c -Iinclude -o sistema
.\sistema.exe
```

## Factura en PDF (punto extra)

- Al confirmar una compra, el sistema genera la factura en texto y tambien un PDF.
- Los archivos PDF se guardan en `Programa/data/pdfs` con formato `factura_000001.pdf`.
- La exportacion PDF esta implementada en C puro (sin librerias externas).