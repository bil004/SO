# Sistemi Operativi 2024/25

## Post Office Simulation

This project, developed for the **Operating Systems** course (a.y. 2024/2025), simulates the operation of a post office using concurrent processes, shared memory, semaphores, and message queues.

---

### 🧩 Key Objectives
- Simulate the daily management of a post office.
- Model the behavior of users, counters, and operators.
- Manage tickets and queues realistically.
- Collect daily and overall statistics.
- Implement simulation termination mechanisms (maximum duration or request overload).

---

### 🏗️ Main Components
- **Director Process**: Manages the simulation, creates processes, and collects statistics.
- **Ticket Dispenser Process**: Assigns tickets to users.
- **Operator Processes**: Manage specialized counters and serve users.
- **User Processes**: Go to the post office and request services.
- **Shared Resources**: Shared memory, semaphores, and message queues.

---

### ⚙️ Technologies and Tools
- Language: **C**
- IPC Mechanisms: **shared memory**, **semaphores**, **message queues**
- Compilation: **Makefile**
- Technical Documentation: **Doxygen**
- Statistics output in **CSV** format

---

### ▶️ Project Execution

#### Compilation
To compile all project components:
```bash
make all
