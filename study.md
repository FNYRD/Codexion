# Codexion — Estudio del proyecto

> Documento de repaso. Primero verifica el cumplimiento del *subject* y luego explica el
> flujo completo de la simulación, indicando en cada paso qué función/archivo lo implementa.

---

## Parte 1 — Verificación del subject

### Resumen rápido

| Requisito del subject | Estado | Dónde se cumple |
|---|---|---|
| Escrito en C | ✅ | todo `srcs/` |
| Norminette | ✅ (según tu verificación) | — |
| Compila con `-Wall -Wextra -Werror -pthread` | ✅ | `Makefile` (`CFLAGS = cc -Wall -Wextra -Werror -g`, `PTHREAD = -pthread`) |
| Makefile con `NAME, all, clean, fclean, re` | ✅ | `Makefile` |
| Sin variables globales | ✅ | todo el estado vive en `t_data` y se pasa por puntero |
| 8 argumentos obligatorios (`argc == 9`) | ✅ | `main()` en `codexion.c:56` |
| Validación de entradas inválidas | ✅ | `verifier()` en `verifier.c` |
| Cada coder = un hilo (`pthread_create`) | ✅ | `starting_routines()` en `codexion.c:36` |
| Un dongle entre cada par de coders | ✅ | `creating_table()` en `initializing.c:73` |
| Estado del dongle protegido con mutex | ✅ | `general_mutex` (mutex único global) |
| Variable de condición para colas de espera | ✅ | `general_cond` |
| Cooldown del dongle obligatorio | ✅ | `wake_up()` en `wake_up.c` |
| Arbitraje justo: `fifo` / `edf` | ✅ | heap en `heaps.c` + `heap_helpers.c` |
| Priority queue (heap) propio, sin libs | ✅ | `heaps.c`, `heap_helpers.c` |
| Hilo monitor detecta burnout y para | ✅ | `monitor_routine()` en `routines.c:106` |
| Log de burnout dentro de 10 ms | ✅ | monitor con `usleep(500)` (0.5 ms de poll) |
| Logs serializados (no se mezclan) | ✅ | `print_mutex` en `printf_log()` |
| Para por burnout o por compilaciones completas | ✅ | `monitor_action()` en `routines.c:81` |
| Sin fugas de memoria | ✅ (valgrind) | `freedom()` en `general_helpers.c` |
| Funciones externas dentro de lo permitido | ✅ | ver abajo |

### Mapeo de argumentos

`assigning()` (`initializing.c:52`) traduce los `argv` a campos de `t_data`:

| Posición | Argumento del subject | Campo en `t_data` |
|---|---|---|
| `argv[1]` | number_of_coders | `n_workers` |
| `argv[2]` | time_to_burnout | `bornout` |
| `argv[3]` | time_to_compile | `compile` |
| `argv[4]` | time_to_debug | `debug` |
| `argv[5]` | time_to_refactor | `refactoring` |
| `argv[6]` | number_of_compiles_required | `uses_per_worker` |
| `argv[7]` | dongle_cooldown | `cooldown` |
| `argv[8]` | scheduler | `scheduler` (0=fifo, 1=edf) |

### Funciones externas usadas (todas permitidas)

`pthread_create`, `pthread_join`, `pthread_mutex_init/lock/unlock/destroy`,
`pthread_cond_init/wait/broadcast/destroy`, `gettimeofday`, `usleep`, `malloc`, `free`,
`printf`, `exit`. Escribiste tus propias `ft_atoi`, `ft_atol`, `ft_strcmp`, `len_str` (permitido).

> Nota: `pthread_cond_timedwait` está permitida pero no la usas; usas `pthread_cond_wait`
> con un monitor que hace polling cada 0.5 ms. Es válido.

### Mapeo de la nomenclatura (vocabulario del subject → tu código)

El código usa metáforas distintas a las del subject. Esto es clave para defenderlo:

| Subject | Tu código |
|---|---|
| coder | `worker` |
| compile | `compiling` (estado 2) + uso del dongle |
| burnout | `bornout` |
| dongle | `dongle` |
| compiling/debugging/refactoring | flags 2/3/4 en `printf_log` |

### Puntos finos a tener presentes en la defensa

1. **Mutex único (cumplido).** El subject (PDF pág. 11) dice: *"you must protect each
   dongle's state with a mutex (pthread_mutex_t)"* — **no** exige un mutex por dongle.
   Tu `general_mutex` protege el estado de cada dongle (`available`/`release_time`):
   toda lectura/escritura ocurre bajo ese mutex, así que cada dongle está protegido.
   Cumples la frase al pie de la letra. Además protege el heap, la cola de espera y
   `finished`, lo que da orden de bloqueo trivial y evita deadlock. Defensa: "un único
   mutex protege el estado de todos los dongles y del resto de recursos compartidos".

2. **Un solo coder se quema (correcto).** Con `n_workers == 1`, el `right_dongle` es `NULL`
   (`creating_table` → `initializing.c:81`) y en `steering` la condición `ready` exige
   `n_workers > 1`. El coder nunca consigue dos dongles y se quema en `time_to_burnout`.
   Es el comportamiento esperado (como un solo filósofo con un solo tenedor).
   Verificado: `./codexion 1 400 ...` imprime `400 1 burned out`.

3. **Desempate EDF (cumplido).** El subject (PDF pág. 12, recuadro "Note") solo pide que
   el desempate sea **determinista** — *no* exige que sea por id. Tu heap ordena por
   `last_compile` con `<` **estricto** (`heap_helpers.c:27` y `heap_helpers.c:72`): ante
   deadlines iguales **no hay swap**, así que se conserva el orden previo y gana el que
   entró antes → es un desempate **FIFO**, determinista. Como `time_to_burnout` es
   constante, ordenar por deadline equivale a ordenar por `last_compile`. Cumple el
   requisito; defiéndelo así: "en empate, el `<` estricto preserva el orden de llegada".

4. **`verifier` rechaza el `0`** en todos los argumentos numéricos (`argv[i][0] < '1'` en
   `verifier.c:95`). Es decir, no se aceptan tiempos ni contadores en 0. Decisión de diseño
   defendible.

---

## Parte 2 — Flujo de la simulación y el código que lo implementa

### Mapa de archivos

| Archivo | Responsabilidad |
|---|---|
| `codexion.c` | `main`, arranque/join de hilos, orquestación |
| `verifier.c` | validación de argumentos |
| `ft_atoil.c` | conversiones string→int/long |
| `lenght_helpers.c` | `strlen`/conteo de argumentos propios |
| `initializing.c` | reserva de memoria, mutexes, mesa de coders/dongles |
| `general_helpers.c` | liberación de recursos, `ft_strcmp`, init de dongles |
| `get_time_ms.c` | reloj en milisegundos |
| `routines.c` | rutina del worker y del monitor |
| `routine_helpers.c` | colas (heap/waiting), logging, condición de espera |
| `heaps.c` | push/pop del heap (FIFO/EDF) |
| `heap_helpers.c` | shift_up / shift_down del heap binario |
| `wake_up.c` | reactivación de dongles tras el cooldown |

### Estructuras de datos (`includes/codexion.h`)

- **`t_data`**: estado compartido global del programa (sin variables globales reales).
  Contiene parámetros, el array `workers`, el array `dongles`, el `heap` (cola de
  prioridad de quién compila), la cola `waiting` (quién aún no tiene los 2 dongles),
  y los mutex/cond.
- **`t_worker`**: un coder. Guarda `id`, `used` (compilaciones hechas), `ready`,
  `last_compile` (clave para EDF y burnout), punteros a su `lef_dongle`/`right_dongle`.
- **`t_dongle`**: `available` y `release_time` (para el cooldown).
- **`t_monitor`**: hilo vigilante; `finished` cuenta cuántos workers terminaron sus
  compilaciones requeridas.

---

### Paso 0 — Validación (`main` → `verifier`)

`main()` (`codexion.c:50`) primero comprueba `argc != 9 || verifier(argv)`.

`verifier()` (`verifier.c:77`):
1. El último argumento debe ser exactamente `fifo` o `edf` (`ft_strcmp`).
2. Recorre los argumentos numéricos:
   - `argv[1]` y `argv[6]` deben caber en `int` → `is_max_int()`.
   - el resto debe caber en `long` → `is_max_long()` (comparación lexicográfica contra
     `INT_MAX_STR`/`LONG_MAX_STR` — así evitas overflow al convertir).
   - cada carácter debe ser dígito (`is_num`) y el número debe ser `>= 1`.

Si algo falla devuelve 1 y `main` retorna sin reservar nada.

### Paso 1 — Inicialización (`assigning` + `creating_table`)

`main` marca `data.start_simulation = get_time_ms()` (origen de tiempos) y llama:

1. **`assigning()`** (`initializing.c:52`): vuelca los argumentos a `t_data`, fija
   `scheduler` (0/1), inicializa mutexes/cond (`mutexes()`), y reserva los 4 arrays
   (`reserving()`): `workers`, `dongles`, `heap`, `waiting`.
2. **`creating_table()`** (`initializing.c:73`):
   - `dongling()` (`general_helpers.c:53`) inicializa cada dongle como `available = 1`.
   - Asigna a cada worker su `lef_dongle` (`&dongles[j]`) y su `right_dongle`. La
     disposición circular: el último worker comparte con `dongles[0]`; si solo hay 1
     worker, `right_dongle = NULL` (un solo dongle en la mesa).
   - Fija `last_compile = get_time_ms()` como referencia inicial del burnout.

Luego `main` prepara el `monitor` (le da `data` y la lista de workers) y enlaza
`worker.monitor = &monitor` para todos.

### Paso 2 — Arranque de hilos (`starting_routines`)

`starting_routines()` (`codexion.c:29`):
- Crea N hilos worker con `worker_routine`.
- Crea 1 hilo `monitor` con `monitor_routine`.
- Hace `pthread_join` de todos los workers y luego del monitor.
- Si falla algún `pthread_create`, `helper_freedom()` pone `stop = 1`, despierta a todos
  y libera.

### Paso 3 — El ciclo de un coder (`worker_routine`)

`worker_routine()` (`routines.c:58`) repite mientras `used < uses_per_worker`:

**3a. Pedir turno** — `worker_waiting_routine()` (`routines.c:15`), bajo `general_mutex`:
- Si `stop`, sale.
- `steering()` (`routine_helpers.c:23`) decide dónde colocar al worker:
  - Si sus dos dongles están disponibles (`ready`), entra al **heap** (`heap_ready`).
  - Si no, entra a la cola **waiting** (`waiting_push`).
- Luego espera en bucle con `pthread_cond_wait` mientras `continue_while()` sea verdadero.

  `continue_while()` (`routine_helpers.c:100`) deja pasar al worker **solo si**: no hay
  `stop`, tiene ambos dongles disponibles, el heap no está vacío, y **él está en la cima
  del heap** (`heap[0]->id == worker->id`). Esto es el corazón del arbitraje justo: solo
  compila quien gana la cabeza de la cola de prioridad.

**3b. Tomar los dongles** — ya en cabeza, marca ambos dongles `available = 0`, imprime
dos veces `has taken a dongle` (flag 1, uno por dongle), saca su entrada del heap con
`heap_pop()`, y suelta el `general_mutex`.

**3c. Compilar / debug / refactor** — `worker_ending_routine()` (`routines.c:33`):
- Imprime `is compiling` (flag 2), actualiza `last_compile` (reinicia el reloj de burnout),
  y `usleep(compile)`.
- Bajo mutex: incrementa `used`; si llegó a `uses_per_worker`, suma a `monitor->finished`;
  libera ambos dongles marcando `available = 0` + `release_time = get_time_ms()` (empieza
  el **cooldown**); pone `ready = 0`; llama `waiting_end()` para reevaluar la cola; hace
  `pthread_cond_broadcast`.
- Imprime `is debugging` (flag 3), `usleep(debug)`, `is refactoring` (flag 4),
  `usleep(refactoring)`. Después vuelve al inicio del while.

> Detalle de cooldown: al liberar el dongle se pone `available = 0` y `release_time`.
> El dongle **no** queda libre inmediatamente; permanece bloqueado hasta que pase el
> cooldown. Quien lo reactiva es el hilo monitor vía `wake_up()`.

### Paso 4 — Las colas: waiting y heap

- **`steering`** reparte entre heap (listos) y waiting (no listos), evitando duplicados.
- **`waiting_end()`** (`routine_helpers.c:50`): recorre `waiting`, y a quien ya tenga sus
  dos dongles disponibles lo marca `ready`, lo mete al heap (`heap_ready`) y lo saca de
  waiting compactando el array.
- **`heap_ready` / `heap_pop`** (`heaps.c`):
  - En **FIFO** (`scheduler == 0`): el heap se usa como cola simple; push al final, pop
    desplazando todo hacia adelante → orden de llegada.
  - En **EDF** (`scheduler == 1`): se usa un heap binario real ordenado por
    `last_compile` (menor `last_compile` = deadline más cercano = más prioridad).
    `heap_ready` hace `shift_up()`; `heap_pop` sube el último a la raíz y hace
    `shift_down()`. Los índices padre/hijo los calcula `heap_relatives()` y la decisión
    de con qué hijo intercambiar la toma `swap_indexft()`.

### Paso 5 — El monitor (`monitor_routine` + `wake_up`)

`monitor_routine()` (`routines.c:106`) hace polling continuo:

- Para cada worker, `monitor_action()` (`routines.c:81`), bajo `general_mutex`:
  - Si `finished == n_workers` → **todos cumplieron** sus compilaciones: `stop = 1`,
    broadcast, termina.
  - Si `get_time_ms() - last_compile >= bornout` → **burnout**: `stop = 1`, broadcast,
    imprime `burned out` (flag 5) y termina. El poll cada `usleep(500)` (0.5 ms) garantiza
    el log dentro de los 10 ms exigidos.
- Tras revisar a todos, llama **`wake_up()`** (`wake_up.c`): recorre los dongles y, a los
  que ya cumplieron su cooldown (`get_time_ms() - release_time >= cooldown`), los pone
  `available = 1`, llama `waiting_end()` y hace broadcast para despertar a los workers
  que esperan.

### Paso 6 — Cierre y liberación (`freedom`)

Cuando algún hilo pone `stop = 1` y hace broadcast, los workers salen de
`pthread_cond_wait`, detectan `stop` y terminan; el monitor también termina. `main` hace
los `pthread_join` y llama **`freedom()`** (`general_helpers.c:15`): anula punteros de los
workers, destruye los dos mutex y la cond, libera los 4 arrays y hace `exit(code)`.

### Serialización del log (`printf_log`)

`printf_log()` (`routine_helpers.c:78`) toma `print_mutex` antes de imprimir, así dos
mensajes nunca se intercalan en una línea. Además, una vez impreso `burned out`, activa
`bornout_flag` para que **ningún** mensaje posterior se imprima (la simulación ya terminó).

---

## Diagrama del ciclo de un coder

```
        ┌─────────────────────────────────────────────┐
        │  worker_routine  (mientras used < required)   │
        └─────────────────────────────────────────────┘
                          │
            lock general_mutex
                          │
            steering ──► ¿2 dongles libres?
                          │ sí → heap (ready)   │ no → waiting
                          │
            cond_wait  mientras  continue_while:
              (¿soy la cabeza del heap y tengo mis 2 dongles?)
                          │  sí
            tomar dongles (available=0) + log "has taken a dongle" x2
            heap_pop
            unlock general_mutex
                          │
            log "is compiling" + usleep(compile)
            (last_compile = ahora)   ← reinicia burnout
                          │
            lock: used++, libera dongles (cooldown), waiting_end, broadcast
                          │
            log "is debugging"   + usleep(debug)
            log "is refactoring" + usleep(refactoring)
                          │
                     (repetir)

   monitor_routine (en paralelo):
     cada 0.5 ms → ¿finished==N? ¿algún last_compile vencido?
                 → wake_up(): reactiva dongles tras cooldown + broadcast
```

---

## Para tu defensa: las 3 condiciones de Coffman (deadlock)

- **Exclusión mutua**: los dongles son recursos exclusivos (`available`).
- **Hold and wait**: lo evitas porque un worker **no** toma un dongle hasta que sus **dos**
  están libres (`steering`/`continue_while` exigen ambos antes de marcarlos). No retiene
  uno mientras espera el otro.
- **Espera circular**: el arbitraje por heap (FIFO/EDF) impone un orden global de concesión;
  solo la cabeza del heap procede, rompiendo el ciclo.

Esto, sumado al mutex único (orden de bloqueo trivial), es lo que mantiene la simulación
libre de deadlock.
