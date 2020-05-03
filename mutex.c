#define locked 1
#define unlocked 0


void lock_mutex(void * mutex)
{
    __asm__("LDR     r1, =locked\n\t"
    "1:   LDREX   r2, [r0]\n\t"
    "CMP     r2, r1\n\t"
    "BEQ     2f\n\t" 
    "STREXNE r2, r1, [r0]\n\t" 
    "CMPNE   r2, #1\n\t"
    "BEQ     1b \n\t"
    "DMB\n\t" 
    "BX      lr\n\t"
    "2:     B       1b\n\t");
}


void unlock_mutex(void * mutex)
{
    __asm__("LDR     r1, =unlocked\n\t"
    "DMB\n\t"
    "STR     r1, [r0]\n\t"
    "BX      lr\n\t");
}

