# YAC Sim - Yet Another Cache Simulator
### Simulates a cache with user defined attributes and simulates accesses from user input.
Created for the purposes of Computer Organization (CS-225) course CSD, University of Crete.

## How To Use YAC Simulator
After runing the simulator executable  

- Insert the memory characteristics: 
  1. **Memory Size** in **Bytes** 
  2. **Word Size** in **Bytes**  
  
- Insert the cache characteristics:
   1. **Cache Size** in **Bytes**
   3. **Block** (Line) **Size** in **Bytes**
   2. **Associativity**,1 is direct mapped up to 16 associativity levels

Now the user can insert different addresses or some commands for the simulation.  
Αddresses can be inserted through the command line (one by one) or by a file.  

### Available commands:
- **source < filename >**:  Reads addresses from a file with a given filename and reports Hits & Misses.  
- **display**:  Displays the contents of the Cache and the Statistics (Hit/Miss Ratio).  
- **clear**:  Clears the contents of the Cache and the resets the Stastistics  
- **cmd**:  Display the available commands  
- **exit**: Display the Statistics and exits the YAC Simulator  
- **Typing a valid address in decimal** e.g. "100" YAC Sim will simulate the access of that address

#### Copyright (C) 2018  Ioannis Vardas - vardas@ics.forth.gr



