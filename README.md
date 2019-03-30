# YAC Sim - Yet Another Cache Simulator
### Emulates a simple Cache scheme with user defined attributes and simulates accesses from user input.
Created for the purposes of Computer Organization (CS-225) course CSD, 
University of Crete.

## How To Use YAC Simulator
### Before running the simulator executable:  
The simulator executable can be run with a configuration file as argument e.g.:   
**./yac_sim config.txt**  
the configuration file can contain all memory and cache parameters (see below).  
You are encouraged to use config.txt as an example to create your own
configurations.   
If the configuration file does not specify all the parameters the user will be  
prompted to input them after running the simulator.

Form of a config file (all sizes are in Bytes):    
memsize \<Size of Memory\>  
wordsize \<Size of Word\>  
cachesize \<Size of Cache\>  
cachelinesize  \<Size of Cache Line(Block)\>  
asso  \<Associativity level\>

### After running the simulator executable:
If yac_sim was run with a configuration file some of the following steps in the  
configuration phase will not be executed.  
**Configuration phase:**  

- Insert the **memory** parameters: 
  1. **Memory Size** in **Bytes** 
  2. **Word Size** in **Bytes, should be less than Memory Size**  
  
- Insert the **cache** parameters:
   1. **Cache Size** in **Bytes**
   3. **Block** (Line) **Size** in **Bytes**
   2. **Associativity**,1 is direct mapped up to 16 associativity levels  
   


**Simulation phase:**  
Now the user can insert different addresses or some commands (see below) for the
simulation.  
Αddresses can be inserted through the command line (one by one) or by a file.  

**Available Simulation commands:**  
- **source < filename >**:  Reads addresses from a file with a given filename and reports Hits & Misses  (Sample files trace.txt, trace1.txt).
- **display**:  Displays the contents of the Cache and the Statistics (Hit/Miss Ratio).  
- **clear**:  Clears the contents of the Cache and the resets the Stastistics  
- **cmd**:  Display the available commands  
- **exit**: Display the Statistics and exits the YAC Simulator  
- **Typing a valid address in decimal** e.g. "100" YAC Sim will simulate the access of that address

#### Copyright (C) 2018  Ioannis Vardas - vardas@ics.forth.gr
