*This project has been created as part of the 42 curriculum by jericard.*

## Description

Codexion is a concurrency project focused on coordinating multiple worker
threads that compete for shared resources. Its goal is to simulate a controlled
parallel execution environment where each worker must acquire two dongles before
compiling, while the program preserves data integrity and avoids deadlocks and
data races.

The project uses mutexes and condition variables to synchronize access to shared
state, including worker status, dongle availability, logging, and scheduler
queues. A monitor thread supervises the simulation and detects when a worker
burns out by missing its allowed time window.

Codexion also implements two scheduling policies. The FIFO scheduler handles
ready workers in arrival order, while the EDF scheduler prioritizes the ready
worker with the earliest deadline. Workers that cannot acquire both required
resources are kept out of the ready queue until they become executable, so the
scheduler only orders workers that can actually make progress.

## Instructions

Clone the repository and enter the project directory:

```sh
git clone https://github.com/FNYRD/Codexion
cd Codexion
```

Compile the project with:

```sh
make
```

During development, the project will be compiled with ThreadSanitizer enabled to
check for data races between threads:

```make
-fsanitize=thread -g
```

The executable expects the following arguments:

```sh
./codexion number_of_workers time_to_burnout time_to_compile time_to_debug time_to_refactor uses_per_worker cooldown scheduler
```

The scheduler must be either `fifo` or `edf`.

Normal execution examples:

```sh
./codexion 5 1000 100 50 50 2 10 fifo
./codexion 5 1000 100 50 50 2 10 edf
```

Border-style examples:

```sh
./codexion 1 1000 100 50 50 1 10 fifo
./codexion 1 1000 100 50 50 1 10 edf
```

Burnout examples:

```sh
./codexion 5 80 100 50 50 2 10 fifo
./codexion 5 80 100 50 50 2 10 edf
```

Remove generated object files and the executable with:

```sh
make fclean
```

## Blocking Cases Handled

Deadlocks are prevented by avoiding partial resource acquisition. A worker is
only allowed to take dongles when it is at the front of the ready heap and both
required dongles are available. If either condition is false, the worker waits
on a condition variable instead of holding one dongle while waiting for another.

This breaks the circular-wait pattern described by Coffman's deadlock
conditions. Mutual exclusion still exists because each dongle is a shared
resource, but the program avoids hold-and-wait by making resource acquisition an
all-or-nothing operation protected by the general mutex.

Starvation prevention is handled through the scheduler queues. Ready workers are
kept in a heap ordered by the selected policy: FIFO for arrival order, or EDF
for earliest deadline first. Workers that are not able to acquire both dongles
are kept in a separate waiting queue, so a worker with a more urgent deadline
cannot block another worker that is ready to make progress.

Cooldown is handled as part of the worker cycle. After compiling and releasing
both dongles, the worker completes its debugging and refactoring phases before
being scheduled again. This gives other ready workers a chance to acquire the
resources and prevents the same worker from immediately monopolizing the
dongles.

Burnout detection is handled by a dedicated monitor thread. The monitor
periodically checks every worker against its last compile timestamp and stops
the simulation as soon as a worker exceeds `time_to_burnout`. The monitor also
broadcasts on the shared condition variable so sleeping workers can wake up and
exit cleanly.

Log serialization is handled through a single logging function protected by a
print mutex. This guarantees that two threads cannot interleave messages on the
same output line. The same function also suppresses normal logs after burnout,
ensuring that the burnout message is the final visible event of a failed
simulation.

## Thread Synchronization Mechanisms

The project uses `pthread_mutex_t` to protect shared state. The main shared
simulation state is guarded by a general mutex, including dongle availability,
worker readiness, scheduler queues, the stop flag, and monitor-related counters.
Before a worker checks whether it can compile, it locks this mutex, verifies the
current state, updates the dongles and queues if it can proceed, and then
unlocks the mutex.

This prevents race conditions where two workers could otherwise observe the same
dongles as available and take them at the same time. The same protection is used
when workers release dongles, update their compile count, change their readiness
state, or move waiting workers back into the ready heap.

The project also uses `pthread_cond_t` to avoid active waiting. When a worker
cannot proceed because its dongles are unavailable, the heap is empty, or it is
not its turn, it waits on a condition variable. When resources are released, or
when the monitor stops the simulation, a broadcast wakes the sleeping workers so
they can re-check the shared state safely under the mutex.

Logging is protected by a dedicated print mutex. All messages go through a
single logging function, so only one thread can write to standard output at a
time. This keeps each log entry complete and prevents messages from different
threads from being mixed on the same line.

Communication between workers and the monitor is done through shared structures
protected by the general mutex. The monitor reads each worker's last compile
timestamp and the global finished counter to determine whether the simulation
should continue. If a burnout is detected, it sets the shared stop flag and
broadcasts on the condition variable. Workers check this flag after waking up
and exit cleanly when the simulation has ended.

## Resources

References used while studying threads, synchronization, race conditions, and
concurrent execution in C:

- <https://medium.com/@akshatarhabib/understanding-threads-in-c-c9feb5e9372a>
- <https://www.geeksforgeeks.org/c/multithreading-in-c/>
- <https://www.youtube.com/watch?v=FY9livorrJI>
- <https://www.youtube.com/watch?v=MqnpIwN7dz0>
- <https://www.youtube.com/watch?v=BEyzoRtQDEs>

AI was used as a development assistant during the project. Its main uses were
generating repetitive support code such as test cases, reviewing code behavior,
clarifying doubts about concurrency concepts, and helping design a small
practice path focused on the specific topics required for the project. It was
also used to generate and organize test commands, review edge cases, and explain
issues related to mutexes, condition variables, data races, heaps, and scheduler
logic.
