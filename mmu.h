#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

int cpumem_read();
int cpumem_write();
int mmu_init(char *romfile);
int ppumem_read();
int ppumem_write();

#endif // MMU_H_INCLUDED
