# YAC Sim - Yet Another Cache Simulator

### Created for the purposes of Computer Organization (CS-225) course CSD, University of Crete.
### Creates a cache with user defined attributes and simulates accesses from user input
#### Copyright (C) 2018  Ioannis Vardas - vardas@ics.forth.gr

## How To Use YAC Simulator
After runing the simulator executable  

1. Insert the memory characteristics, **Memory Size** and **Word Size** in **Bytes**.
2. Insert the cache characteristics, **Cache Size** and **Block** (Line) **Size** in **Bytes**, **Associativity** (1 is direct mapped) up to 16 associativity levels  

Now the cache memory is created, the user can insert different addresses for the simulation
addresses can be inserted through the command line (one by one) or by a file.  
### Available commands:
- **source < filename >**:  Reads addresses from a file with a given filename and reports hits or misses.  
- **display**:  Displays the contents of the Cache and the Statistics(Hit/Miss Ratio).  
- **clear**:  Clears the contents of the Cache and the resets the Stastistics  
- **cmd**:  Display the available commands  
- **exit**: Display the Statistics and exits the YAC Simulator  
- **By typing a valid address in decimal** e.g. "100" YAC Sim will simulate the access of that address



