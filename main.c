#include <stdio.h>
#include <stdint.h> // Essential for uint32_t
#include <string.h>
#define MEM_SIZE (1024*1024)// 1MB = 1024KB, 1KB = 1024bytes

// Structure
typedef struct {
    uint8_t memory[MEM_SIZE]; // entire simulated RAM.
    uint32_t pc;
    uint32_t regs[32];// register file

}RISKVstate;

RISKVstate rv;

int init(void){
    memset(&rv,0,sizeof(rv));// fill memmory or set memory with 0's
    rv.pc=0x00000000;

    return 0;
}
// memory read 32 bit
uint32_t mem_read32(RISKVstate *cpu, uint32_t addr){
    return *(uint32_t *)&cpu->memory[addr];//pointer casting      
}

// write value into memory at addr 32 bit
void mem_write32(RISKVstate *cpu, uint32_t addr, uint32_t value){
    *(uint32_t *)&cpu->memory[addr] = value;
}

// 8 bit memory
uint8_t mem_read8(RISKVstate * cpu, uint32_t addr){
    return cpu->memory[addr];
}

void mem_write8(RISKVstate * cpu,uint32_t addr, uint8_t value){
    cpu->memory[addr]=value;
}

//16 bit memory
uint16_t mem_read16(RISKVstate * cpu, uint32_t addr){
    return *(uint16_t *) &cpu->memory[addr];
}

void mem_write16(RISKVstate * cpu,uint32_t addr,  uint16_t value){
    *(uint16_t *)&cpu->memory[addr]=value;
}

//reg_read & reg_write 
uint32_t reg_read32(RISKVstate * cpu, uint32_t reg){
    if (reg==0) return 0; //x0 always return 0
    return cpu->regs[reg];
}

void reg_write32(RISKVstate *cpu, uint32_t reg, uint32_t value){
    if (reg==0) return; //x0 ignores writes
    cpu->regs[reg]= value;
}

void cpu_loop(RISKVstate *cpu){
    //fetch - read instruction at current pc
    uint32_t inst = mem_read32(cpu, cpu->pc);

    //decode
    uint32_t opcode = inst & 0x7F;
    uint32_t  rd    = (inst >> 7) & 0x1F;
    uint32_t funct3 = (inst >> 12) & 0x07;
    uint32_t rs1    = (inst >> 15) & 0x1F; 
    uint32_t rs2    = (inst >> 20) & 0x1F;
    uint32_t funct7 = (inst >> 25) & 0x7F;

    //execute
    

    
    // 3. EXECUTE - switch on opcode 

    // 4. advance pc by 4
}
