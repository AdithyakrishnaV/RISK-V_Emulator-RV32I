#include <stdio.h>
#include <stdint.h> // Essential for uint32_t
#include <string.h>
#define MEM_SIZE (1024*1024)// 1MB = 1024KB, 1KB = 1024bytes

// Structure
typedef struct {
    uint8_t memory[MEM_SIZE]; // entire simulated RAM.
    uint32_t PC;
    uint32_t regs[32];// register file

}RISKVstate;

RISKVstate rv;

int init(void){
    memset(&rv,0,sizeof(rv));// fill memmory or set memory with 0's
    rv.PC=0x00000000;

    return 0;
}
// memory read
uint32_t mem_read32(RISKVstate *cpu, uint32_t addr){
    return *(uint32_t *)&cpu->memory[addr];//pointer casting 
      
}

uint32_t mem_wite32()