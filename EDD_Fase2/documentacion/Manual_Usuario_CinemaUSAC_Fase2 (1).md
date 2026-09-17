# Manual de Usuario — CinemaUSAC Fase 2

**Sistema de gestión de cine**
Universidad San Carlos de Guatemala — Facultad de Ingeniería
Curso: Estructuras de Datos

**Estudiante:** Dalio Antonio Miranda Guerra
**Carné:** 202100116

---

## Índice

1. [Introducción](#1-introducción)
2. [Pantalla de inicio](#2-pantalla-de-inicio)
3. [Acceso como Administrador](#3-acceso-como-administrador)
4. [Panel Administrador — Películas](#4-panel-administrador--películas)
5. [Panel Administrador — Funciones (Fase 2)](#5-panel-administrador--funciones-fase-2)
6. [Panel Administrador — Clientes](#6-panel-administrador--clientes)
7. [Acceso como Cliente](#7-acceso-como-cliente)
8. [Funciones del Cliente](#8-funciones-del-cliente)
9. [Reportes con Graphviz](#9-reportes-con-graphviz)
10. [Mensajes de error y validaciones](#10-mensajes-de-error-y-validaciones)

---

## 1. Introducción

**CinemaUSAC** es una aplicación de escritorio desarrollada en C++ que permite administrar
la cartelera de un complejo de cines: películas, funciones, clientes, reservas y asientos.
El sistema cuenta con dos perfiles de acceso:

- **Administrador**: gestiona películas, funciones, clientes, reservas y genera reportes gráficos.
- **Cliente**: se registra, inicia sesión, consulta la cartelera, reserva y cancela asientos, y
  revisa su historial de reservas.

Toda la información se guarda de forma persistente en archivos JSON, por lo que los datos
se conservan entre distintas ejecuciones del programa.

---

## 2. Pantalla de inicio

Al abrir la aplicación se muestra la pantalla principal de bienvenida, desde donde se elige
el tipo de acceso al sistema.

![Pantalla de inicio](imagenes/06_pantalla_inicio.png)

**Opciones disponibles:**

- **Ingresar como Administrador**: lleva al panel de administración (requiere credenciales).
- **Ingresar como Cliente**: lleva a la pantalla de inicio de sesión / registro de clientes.

---

## 3. Acceso como Administrador

Para ingresar como administrador se deben utilizar las credenciales del sistema:

| Campo | Valor |
|---|---|
| Correo | `AdminCine@gmail.com` |
| Contraseña | `admin123Pass` |

Si las credenciales son incorrectas, el sistema mostrará un mensaje de advertencia y no
permitirá el acceso. Una vez validadas, se abre el **Panel Administrador**, organizado en
pestañas: *Películas*, *Promociones*, *Solicitudes*, *Funciones y Asientos (Fase 1)*,
*Funciones (Fase 2)*, *Clientes* y *Reportes*.

---

## 4. Panel Administrador — Películas

En esta pestaña se gestiona la cartelera completa del cine (Árbol Binario de Búsqueda).

![Panel de películas](imagenes/01_panel_admin_peliculas.png)

**Columnas mostradas:** Código, Título, Género, Duración, Clasificación, Idioma,
Fecha de Estreno, Fecha Fin y Estado (*En cartelera* / *Próximo a retirar*).

**Botones disponibles:**

| Botón | Función |
|---|---|
| **Agregar Película** | Abre un formulario para registrar una nueva película (código, título, género, duración, clasificación, idioma, fecha de estreno y fecha fin). El código se inserta en el Árbol Binario de Búsqueda. |
| **Eliminar** | Elimina la película seleccionada de la cartelera y del árbol. |
| **Cargar CSV** | Permite importar información complementaria desde un archivo CSV. |
| **Generar Reporte Graphviz** | Genera y abre la imagen del reporte gráfico del Árbol Binario de Búsqueda de películas. |

> **Nota:** las películas próximas a finalizar su cartelera (menos de 7 días) se resaltan en
> color **amarillo**; las que se encuentran vigentes se muestran en **verde**.

Desde esta pestaña también se puede seleccionar el orden en que se listan las películas:
**preorden, inorden o postorden**.

---

## 5. Panel Administrador — Funciones (Fase 2)

En esta pestaña se administran las funciones de cine, almacenadas en un **Árbol AVL**
usando el `codigo_funcion` como clave.

![Panel de funciones](imagenes/02_panel_admin_funciones.png)

**Columnas mostradas:** Código, Película, Horario, Sala, Filas x Columna y Archivo de
Asientos asociado.

**Botones disponibles:**

| Botón | Función |
|---|---|
| **Crear Función** | Abre el formulario para registrar una nueva función (ver detalle abajo). |
| **Editar Función** | Permite modificar los datos de la función seleccionada. |
| **Eliminar Función** | Elimina la función del Árbol AVL y borra su archivo `.json` de asientos. |
| **Cargar como Activa** | Carga la función seleccionada en la matriz dispersa activa (guarda primero la función anterior, si existía). |
| **Cargar JSON (Películas + Funciones)** | Permite hacer una carga masiva de películas junto con sus funciones desde un archivo JSON. |

Además, se puede elegir el **orden de recorrido** del listado: *preorden, inorden* (por
código, ascendente) o *postorden*.

### 5.1 Crear función

Al presionar **Crear Función** se abre el siguiente formulario:

![Formulario crear función](imagenes/03_crear_funcion.png)

**Campos a completar:**

1. **Película**: se selecciona de la lista de películas ya registradas (código - título).
2. **Horario**: hora en que se proyecta la función (formato `HH:MM`).
3. **Sala**: nombre o número de la sala asignada.
4. **Número de filas**: cantidad de filas de la sala.
5. **Número de columnas**: cantidad de columnas de la sala.

Al presionar **OK**, el sistema genera automáticamente el `codigo_funcion`, lo inserta en el
Árbol AVL y crea el archivo `F00X_funcion.json` correspondiente con la lista de asientos
ocupados vacía.

### 5.2 Mapa de asientos de la función activa

Debajo del listado se muestra el estado de la función que está actualmente cargada en la
matriz dispersa:

- **Verde** = asiento libre.
- **Rojo** = asiento ocupado (al hacer clic se muestra el detalle de la reserva y permite
  cancelarla).

También se indica en todo momento: nombre de la función activa, número de asientos
reservados, libres y el total de asientos de la sala.

---

## 6. Panel Administrador — Clientes

En esta pestaña se administra el catálogo de clientes del sistema, almacenado en un
**Árbol B de orden 4**, indexado por el `id` del cliente.

![Panel de clientes](imagenes/04_panel_admin_clientes.png)

**Columnas mostradas:** ID, Nombre, Correo, Teléfono y # de Reservas.

**Botones disponibles:**

| Botón | Función |
|---|---|
| **Agregar Cliente** | Registra un nuevo cliente en el Árbol B. |
| **Editar Cliente** | Modifica los datos del cliente seleccionado. |
| **Eliminar Cliente** | Elimina al cliente del Árbol B y, en cascada, todas sus reservas asociadas en la Tabla Hash. |
| **Ver Reservas** | Muestra el detalle de todas las reservas realizadas por el cliente seleccionado. |
| **Cargar JSON (Clientes + Reservas)** | Permite una carga masiva de clientes junto con sus reservas desde un archivo JSON. |

### 6.1 Ver reservas de un cliente

Al presionar **Ver Reservas** se abre una ventana emergente con el listado de reservas del
cliente seleccionado:

![Reservas de un cliente](imagenes/05_reservas_cliente.png)

**Columnas mostradas:** Código de reserva, Función, Fecha, Fila y Columna del asiento
reservado. Esta información se obtiene combinando el Árbol B (referencias del cliente) con
la Tabla Hash de reservas (datos de cada reserva por `codigo_reserva`).

---

## 7. Acceso como Cliente

Al elegir **Ingresar como Cliente** desde la pantalla de inicio, se muestra la siguiente
pantalla, que permite tanto iniciar sesión como registrarse:

![Acceso de cliente](imagenes/07_acceso_cliente.png)

### 7.1 Iniciar sesión

Se debe ingresar:

- **Correo**
- **Password**

Y presionar **Iniciar Sesión**. Si las credenciales no son correctas, el sistema muestra un
mensaje de advertencia indicando el error.

### 7.2 Registrarse

Para crear una cuenta nueva se deben completar los siguientes campos:

| Campo | Descripción |
|---|---|
| **ID** | Identificador único del cliente. |
| **Nombre** | Nombre completo del cliente. |
| **Correo** | Correo electrónico (no puede estar repetido en el sistema). |
| **Teléfono** | Número de teléfono de contacto. |
| **Password** | Contraseña de acceso. |

Al presionar **Registrarse**, el cliente se inserta en el Árbol B de clientes. Si el correo ya
existe en el sistema, el registro es rechazado y se muestra un mensaje indicando el motivo.

---

## 8. Funciones del Cliente

Una vez autenticado, el cliente accede a un panel con las siguientes opciones:

### 8.1 Consultar cartelera y reservar asiento

1. El cliente elige una **película** de la cartelera (proveniente del Árbol Binario de
   Búsqueda).
2. Selecciona una **función** de esa película (búsqueda resuelta mediante el Árbol AVL).
3. El sistema carga el archivo `F00X_funcion.json` correspondiente y llena la matriz
   dispersa con los asientos ya ocupados.
4. Se muestra el **mapa de asientos** (tabla) con los asientos libres y ocupados.
5. El cliente selecciona el asiento deseado y confirma la reserva.
6. El sistema actualiza la matriz en memoria, la persiste al archivo de la función y registra
   la reserva en la Tabla Hash general (clave `codigo_reserva`).

### 8.2 Ver mi historial de reservas

El cliente puede consultar todas las reservas realizadas. Para cada una se muestra:

| Código | Función | Fecha | Fila | Columna | Estado |
|---|---|---|---|---|---|
| R001 | F001 | 2026-03-01 | 2 | 5 | VALIDA |
| R008 | F005 | 2026-03-05 | 7 | 10 | VENCIDA |

El **estado** se calcula comparando la fecha de la función con la fecha actual: **VALIDA**
si aún no ha pasado, **VENCIDA** si ya se realizó. Esta consulta no requiere cargar los
archivos de asientos de las funciones.

### 8.3 Cancelar reserva

El cliente busca su reserva por `codigo_reserva`. El sistema:

1. Elimina la reserva de la Tabla Hash y del archivo `F00X_funcion.json` correspondiente.
2. Carga la matriz dispersa de esa función (si no era la que ya estaba activa).
3. Marca el asiento como libre y persiste el cambio al archivo de la función.

---

## 9. Reportes con Graphviz

Desde la pestaña **Reportes** del Panel Administrador (y también desde el botón de reporte
disponible en la pantalla de la función activa) se pueden generar los siguientes reportes
gráficos en tiempo real:

| Reporte | Estructura | Contenido |
|---|---|---|
| **Reporte 1** | Árbol Binario de Búsqueda | Cartelera completa de películas (código, título, duración, clasificación). Verde = en cartelera, amarillo = próximo a retirar. |
| **Reporte 2** | Árbol AVL | Funciones registradas (código, película, horario, sala). Verde = programada, rojo = ya realizada. |
| **Reporte 3** | Árbol B de orden 4 | Clientes registrados, con hasta 4 hijos por nodo, diferenciando raíz, nodos internos y hojas. |
| **Reporte 4** | Tabla Hash | Distribución de reservas por bucket, mostrando colisiones (listas enlazadas) y estadísticas generales (tamaño, buckets ocupados, colisiones). |
| **Reporte 5** | Matriz Dispersa | Mapa de asientos (libres/ocupados) de la función actualmente cargada en memoria, con estadísticas de asientos libres, ocupados y totales. |

Cada reporte se genera como archivo `.dot`, procesado por Graphviz para producir una
imagen (`.png` o `.pdf`) que se abre automáticamente desde la aplicación.

---

## 10. Mensajes de error y validaciones

El sistema valida toda la información ingresada, tanto desde los formularios de la interfaz
como desde los archivos JSON cargados. Ante un dato inválido se muestra un mensaje
indicando **qué campo presenta el error** y **qué formato o valor se espera**. Algunos
ejemplos:

- Fechas con formato inválido o inexistentes (ej. `2026-13-40`).
- Campos numéricos (filas, columnas, duración, ID) con texto en vez de números.
- Campos de texto obligatorios dejados en blanco.
- Correos ya registrados al intentar crear un cliente duplicado.
- Referencias inconsistentes entre película/función/cliente/reserva al cargar un JSON.

Los registros que no cumplan las reglas de validación son **rechazados** e informados al
usuario, sin afectar la información válida ya almacenada en el sistema.

---

*Fuente de las capturas de pantalla: elaboración propia, 2026.*
