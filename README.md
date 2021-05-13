# Multiprocess Snake Game Project
## Authors: Bartosz Świrta, Radosław Radziukiewicz
### Dependencies: CMake, OpenCV, POSIX

# Project Overview

This project was created as the part of real-time systems
course (SCZR) realised on Warsaw University of Technology.
Project implements custom snake game, which is performed 
by 5 processes. It was necessary to implement 
proper synchronization between those processes. Our system implements
two methods of synchronization.

---

# How to run the project

After downloading and extracting this repository, one should
create directory for cmake initialization. After creating such 
directory (ex. build) one should type: <br>
`cd build` <br>
`cmake ..` <br>
These commands will initialize project structure. After that one 
has to compile all processes. To do so, one has to type all these commands: <br>
`make capture` <br>
`make info` <br>
`make start` <br>
`make process` <br>
`make game` <br>
To start the system, one has to start process name 'start' (present in bin directory)
with argument 0 or 1 (represents synchronization type).

---

# Game description

System implements custom snake game. Player controls head of the snake
via special marker. Such marker can be anything (pen, notebook or cat).
The only requirement is that marker is uni-colour (enables easy detection on the frame). <br> 
Process responsible for image processing will look for the marker on the frame
After spotting it, it will send marked point as the current position for 
snake's head. When the system starts, one is capable of setting marking
object with live preview of the result of marking. <br>
Player's target is to gain as high score as he can. Score increases
after consuming fruit, which appears on the screen.

---

# Processes Overview

As mentioned above, system consists of 5 separate processes:
>Process A (frame capturing) - loads frame from camera connected to the computer and sends it to process B. <br>
>Process B (image processing) - process finds marker position and sends it to process C along with frame. <br>
>Process C (game logic) - process responsible for controlling game and displaying it. <br>
>Process D (info gathering) - process collects information about overall system speed. <br>
>Process E (starting the system) - allocates necessary resources and monitors system functioning. <br>

---

# Diagram of the system

Below system architecture diagram is being presented:

![](pictures/System_diagram.png)


---

# Processes synchronization

System implements synchronization via shared memory blocks.
Access to these blocks is synchronized with semaphores (synchronization
mode 0) or with queues (synchronization mode 1). In order to allocate 
and manage proper resources, operaing system should support POSIX style
queues, semaphores and shared memory allocation.

---

# System in action

Below we present some visual data showing lunched system: <br>

**Marker adjustment** <br>
![](pictures/marker.gif)

**Brilliant gameplay** <br>
![](pictures/gameplay.gif)



