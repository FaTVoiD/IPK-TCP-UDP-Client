## Changelog of IPK Calculator Protocol Client

### 1.1 Argument processing
- Added the implementation of arguments, the program now reads the arguments and verifies, if all required arguments have been filled out. 
- The arguments can now come in any order but each specification must follow the correct option argument ( e. g. hostname should follow the "-h" argument), otherwise the correct functionality isn't guaranteed.

### 2.1 TCP Communication and arguments modification

- Minor faults in processing the arguments have been sorted out.
- The program can now communicate with the host server using the TCP protocol.

### 2.2 Handling Ctrl-C signal in TCP

- The program now reacts to the Ctrl-C key combination by sending a *"BYE"* message to the server, after receiving the answer from the server, the program closes the socket and ends.  
- The Ctrl-C combination does not work occasionally.

### 3.0 UDP Communication and Ctrl-C handling modified

- The program can now communicate with the host server using the UDP protocol.
- Ctrl-C handling faults have been fixed.
- Ctrl-C terminates the UDP connection by closing the socket and ending the program run.

### 4.0 Platform Compatibility

- The program is now fully compatible with both **Unix** and **Windows** platforms.

### 5.0 Code Header and Code Commenting

- Added a header with the author name.
- Added code comments for better understandability of the source code.

### 5.1 Included Documentation of the Project

- README.md now holds the documentation of the project.