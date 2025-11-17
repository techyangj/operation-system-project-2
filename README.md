# Operating Systems Project 2 — Threads \& Synchronization  
Process Threads + Dining Philosophers (C / pthread)

A small C project for an Operating Systems course that extends Project 1.  

- Step 1: reuse `processes.txt`, turn each process into a **thread** and simulate its CPU burst using `sleep()`.  
- Step 2: implement the **Dining Philosophers** synchronization problem using **mutex locks**, and print detailed thread activity.

Designed for macOS / Linux. Processes are read from a text file `processes.txt` in the project root.

---

## Requirements

- macOS or Linux (tested on common distributions)  
- `gcc` (7.0+ recommended)  
- POSIX threads (`pthread`) support (standard on macOS/Linux)

---

## Build

```bash
# In the project root
gcc os_project2.c -o os_project2
