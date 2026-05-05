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
    while(1){
        //fetch - read instruction at current pc
    uint32_t inst = mem_read32(cpu, cpu->pc);

    //decode
    uint32_t opcode = inst & 0x7F;
    uint32_t  rd    = (inst >> 7) & 0x1F;
    uint32_t funct3 = (inst >> 12) & 0x07;
    uint32_t rs1    = (inst >> 15) & 0x1F; 
    uint32_t rs2    = (inst >> 20) & 0x1F;
    uint32_t funct7 = (inst >> 25) & 0x7F;

    int32_t imm = (int32_t)inst >> 20; //I-type immediate is bits [31:20], so shift right by 20

    uint32_t shamt = (inst >> 20) & 0x1F; //shamt is just the lower 5 bits of the imm

    //execute
    switch(opcode){
        case 0x33:// R-type: operations between two registers → result goes into a third register.
            switch(funct3){
                case 0x0: // ADD SUB
                    if (funct7==0x00)// ADD 
                        reg_write32(cpu,rd, reg_read32(cpu, rs1)+ reg_read32(cpu, rs2));

                    else if (funct7==0x20)// SUB
                        reg_write32(cpu,rd, reg_read32(cpu, rs1)- reg_read32(cpu, rs2));
                    break;

                case 0x1://SLL 
                    reg_write32(cpu, rd, reg_read32(cpu, rs1)<< reg_read32(cpu,rs2));
                    break;

                case 0x2://SLT (signed comparison)
                    reg_write32(cpu, rd, (int32_t)reg_read32(cpu, rs1) < (int32_t)reg_read32(cpu,rs2) ? 1:0);
                    break;

                case 0x3://SLTU (unsigned comparison)
                    reg_write32(cpu, rd, reg_read32(cpu, rs1) < reg_read32(cpu,rs2) ? 1:0);
                    break;

                case 0x4://XOR
                    reg_write32(cpu, rd, reg_read32(cpu, rs1) ^ reg_read32(cpu,rs2));
                    break;

                case 0x5:
                    if (funct7==0x00)//SRL
                        reg_write32(cpu, rd, reg_read32(cpu, rs1) >> reg_read32(cpu,rs2));
                    else if(funct7==0x20)//SRA
                        reg_write32(cpu, rd, (int32_t)reg_read32(cpu, rs1) >> (int32_t)reg_read32(cpu,rs2));
                    break;

                case 0x6://OR
                    reg_write32(cpu, rd, reg_read32(cpu, rs1) | reg_read32(cpu,rs2));
                    break;

                case 0x7:
                    reg_write32(cpu, rd, reg_read32(cpu, rs1) & reg_read32(cpu,rs2));
                    break;
                default:
                    printf("Unknown error at 0x33 opcode");
            }    

            break;

        //3.2 - Integer Register-Immediate
        case 0x13:
            switch(funct3){
                case(0x0):
                    reg_write32(cpu,rd, reg_read32(cpu, rs1) + imm);
                    break;
                
                case(0x7):
                    reg_write32(cpu,rd, reg_read32(cpu, rs1) & imm);
                    break;
                
                case(0x6):
                    reg_write32(cpu,rd, reg_read32(cpu, rs1) | imm);
                    break;

                case(0x4):
                    reg_write32(cpu,rd, reg_read32(cpu, rs1) ^ imm);
                    break;
                case(0x2):
                    reg_write32(cpu,rd, (int32_t)reg_read32(cpu, rs1) < imm ? 1:0);
                    break;
                case(0x3):
                    reg_write32(cpu,rd, reg_read32(cpu, rs1) < (uint32_t)imm ? 1:0);
                    break;
                case(0x1):
                    if (funct7==0x00)
                        reg_write32(cpu,rd, reg_read32(cpu, rs1) << shamt);
                    break;
                case(0x5):
                    if (funct7==0x00)
                        reg_write32(cpu,rd, reg_read32(cpu, rs1) >> (uint32_t)shamt);
                    else if (funct7==0x20)
                        reg_write32(cpu,rd, (int32_t)reg_read32(cpu, rs1) >> shamt);
                    break;

            }
            break;

        // Loads
        case 0x03:
            {
                int32_t addr = reg_read32(cpu, rs1) + imm ;// address = base + offset
                switch(funct3){
                    case(0x0):
                        {
                            int8_t byte = (int8_t)mem_read8(cpu, addr);
                            reg_write32(cpu, rd, (int32_t)byte);
                        }
                        break;
                    case(0x1):
                        {
                            int16_t half = (int16_t)mem_read16(cpu, addr);
                            reg_write32(cpu, rd, (int32_t)half);
                        }
                        break;
                    case(0x2):
                        reg_write32(cpu, rd, mem_read32(cpu, addr));
                        break;
                    case(0x4):
                        reg_write32(cpu, rd, (uint32_t)mem_read8(cpu, addr));
                        break;
                    case(0x5):
                        reg_write32(cpu, rd, (uint32_t)mem_read16(cpu, addr));
                        break;
                }
            }
            break;
        
        case 0x23: // Stores  S-type
            {
                int32_t imm_s = ((int32_t)inst >> 20 & ~0x1F) | ((inst >> 7) & 0x1F);
                uint32_t addr =  reg_read32(cpu, rs1) + imm_s;
                uint32_t val = reg_read32(cpu, rs2);

                switch(funct3){
                    case 0x0:
                        mem_write8(cpu, addr,  (uint8_t)(val & 0xFF));//8-bit  8-bit_memory
                        break;
                    case 0x1:
                        mem_write16(cpu, addr, (uint16_t)(val & 0xFFFF));
                        break;
                    case 0x2://x[rs2][31:0]  (all 4 bytes)
                        mem_write32(cpu, addr, val);//32-bit rs2
                        break;
                }
            }
            break;

        case 0x63:// branches (B-type).
            {
                int32_t imm_b = ((int32_t)(inst & 0x80000000) >> 19)
                                | ((inst & 0x00000080) << 4)
                                | ((inst >> 20) & 0x7E0)
                                | ((inst >> 7)  & 0x1E);
                            
                int check=0;//flag

                switch(funct3){
                    case(0x0):
                        check= (reg_read32(cpu,rs1)==reg_read32(cpu,rs2));
                        break;  
                    case(0x1):
                        check=(reg_read32(cpu,rs1)!=reg_read32(cpu,rs2));
                        break;
                    case(0x4):
                        check=((int32_t)reg_read32(cpu,rs1)<(int32_t)reg_read32(cpu,rs2));
                        break;
                    case(0x5):
                        check=((int32_t)reg_read32(cpu,rs1)>=(int32_t)reg_read32(cpu,rs2));
                        break;
                    case(0x6):
                        check=(reg_read32(cpu,rs1)<reg_read32(cpu,rs2));
                        break;
                    case(0x7):
                        check=(reg_read32(cpu,rs1)>=reg_read32(cpu,rs2));
                        break;
                }
                if (check){
                    cpu->pc += imm_b;
                    cpu->regs[0]=0;
                    continue;
                }
            }
            break;
        
        case 0x6F://J-type
            {
                int32_t imm_j = ((int32_t)(inst & 0x80000000) >> 11)  // bit 20 of imm (sign)
                                | (inst & 0x000FF000)                 // bits 19:12 of imm
                                | ((inst >> 9)  & 0x800)              // bit 11 of imm ← inst[20]
                                | ((inst >> 20) & 0x7FE);             // bits 10:1 of imm
                
                uint32_t return_addr = cpu->pc + 4;// save before modifying pc
                uint32_t new_pc      = cpu->pc + imm_j;
                reg_write32(cpu, rd, return_addr);
                cpu->pc = new_pc;
                cpu->regs[0] = 0;
                continue;
            }
            break;
            
        
        case 0x67://I-type
            {
                uint32_t target = (reg_read32(cpu,rs1) + imm) & ~1U;// compute target first
                reg_write32(cpu,rd, cpu->pc +4);
                cpu->pc = target;
                cpu->regs[0]=0;
                continue;

            }
            break;// continue skips everything anyway, but it's cleaner and prevents compiler warnings.
            

        case 0x37:// U-type Upper Immediate
            {
                uint32_t imm_u = inst & 0xFFFFF000;
                reg_write32(cpu, rd, imm_u);
            }
            break;
        
        case 0x17:// U-type Upper Immediate
            {
                uint32_t imm_u = inst & 0xFFFFF000;
                reg_write32(cpu, rd, cpu->pc + imm_u);
            }
            break;

        case 0x73:
            {
                uint32_t which = (inst >> 20) & 0xFFF; // imm field

                if (which == 0){//ECALL   
                    uint32_t syscall= reg_read32(cpu, 17);// a7 = x17
                    uint32_t arg0 = reg_read32(cpu, 10);// a0 = x10
                    uint32_t arg1    = reg_read32(cpu, 11); // a1 = x11

                    if(syscall == 10 || syscall == 93){     // exit
                        printf("[ECALL] exit(%d)\n", arg0);
                        return;
                    }
                    if(syscall == 1){                        // print integer
                        printf("%d", (int32_t)arg1);         // a1 holds the number
                    }
                    if(syscall == 4){                        // print string
                        uint32_t addr = arg1;                // a1 holds the address
                        while(1){
                            uint8_t ch = mem_read8(cpu, addr);
                            if(ch == 0) break;
                            putchar(ch);
                            addr++;
                        }
                    }

                }else if(which == 1){ // EBREAK
                    printf("[EBREAK] debugger trap at PC=0x%08X\n", cpu->pc);
                    return;
                }
                break;
            }
        
        default:
            printf(
                "Unknown opcode: 0x%02X at PC: 0x%08X", opcode, cpu->pc
            ); break;
    }
    

    // 4. advance pc by 4
     cpu->pc+=4;
     cpu->regs[0]=0;
    }
}
