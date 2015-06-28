# Example Proposal - Embedded Operating System
## Team
- Group F2
- Jeramie Vens (NETID: vens, Section F)
- Kris Hall (NETID: pkhall, Section F)

## Project Idea
We are going to create an embedded operating system to run on a ARM TIVA microcontroller.  
The operating system will at a minimum contain the following components:

- A task library with a clear and simple API.
- A scheduler system and context switch allowing for premptive switching of the running task at regular intervals.
- Mutual exclusion and semaphores to protect memory accessed by multiple tasks.
- A example program which will blink an LED, read from an analog in pin, and transmit via serial to a computer.
- Extra Credit: A loadable driver system for peripheral with at least one example driver.

## Topics From Class
The following class topics will be included in this project:

- Mutual Exclusion
- Scheduling
- Mutli-threaded programming
- Loadable Drivers (extra credit)

## Timeline

- Week 1:
    - Install needed developement software and learn TIVA libraries
    - Start development of the task library
- Week 2:
    - Continue development of task library
    - Add scheduler and context switching
- Week 3:
    - Finish task library and scheduler
    - Add mutual exclusion
    - Write example program
    - Present
    

## Rubric
The Grading rubric is shown on the next page.

+------------------+-----------------------------------+-----------------------------------+-----------------------------+
| Part             | Unsatisfactory                    | Satisfactory                      | Beyond Satisfactory         |
+==================+===================================+===================================+=============================+
| Task Library     | A simple task library that can    | Tasks can have priority and have  | Tasks have owners, parents, |
|                  | create and switch tasks [0-25pts] | their life cycles controlled      | and other POSIX ideas       |
|                  |                                   | [26-50pts]                        | [51-75pts]                  |
+------------------+-----------------------------------+-----------------------------------+-----------------------------+
|                  |                                   |                                   |                             |
+------------------+-----------------------------------+-----------------------------------+-----------------------------+
| Mutual Exclusion | Mutual exclusion by disabling     | Mutual exclusion based on an      | Mutual exclusion based on a |
|                  | interrupts [0-25pts]              | algorithm from class [26-50pts]   | more efficent new algorithm |
|                  |                                   |                                   | found through research      |
|                  |                                   |                                   | [51-75pts]                  |
+------------------+-----------------------------------+-----------------------------------+-----------------------------+
|                  |                                   |                                   |                             |
+------------------+-----------------------------------+-----------------------------------+-----------------------------+
| Example Program  | Simple test bench program         | Interesting multi-task program    | Useful program that is      |
|                  | [0-25pts]                         | which shows the capabilities of   | interactive and can create  |
|                  |                                   | the OS [26-50]                    | and end tasks based on user |
|                  |                                   |                                   | interactions [51-75pts]     |
+------------------+-----------------------------------+-----------------------------------+-----------------------------+
|                  |                                   |                                   |                             |
+------------------+-----------------------------------+-----------------------------------+-----------------------------+
| Extra Cedit      | N/A                               | Working loadable peripheral       | Additional peripheral       |
|                  |                                   | driver system with one driver     | drivers [10pts / driver]    |
|                  |                                   | [20pts]                           |                             |  
+------------------+-----------------------------------+-----------------------------------+-----------------------------+
|                  |                                   |                                   |                             |
+------------------+-----------------------------------+-----------------------------------+-----------------------------+
| Demo & Report    | Rushed report, very little        | Good presentation and report      | Amazing presentation with   |
|                  | information. Boring presentation  | [21-50pts]                        | interactive demo [51-60pts] |
|                  | [0-20pts]                         |                                   |                             |
+------------------+-----------------------------------+-----------------------------------+-----------------------------+


