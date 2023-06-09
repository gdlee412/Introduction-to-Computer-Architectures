# Implementing true LRU cache replacement algorithm

1. Use vectors to save the tags instead of array
2. In the *check_tag* function, rotate the indices of the tags such that the currently accessed vector is the most recent (index [0])
3. In the *victimize* function, retrieve the last index of the current set. Then insert the tag again to the most front (since it has been accessed as a victim)

**RISCV-ISA-SIM repository came from https://github.com/riscv-software-src/riscv-isa-sim**
