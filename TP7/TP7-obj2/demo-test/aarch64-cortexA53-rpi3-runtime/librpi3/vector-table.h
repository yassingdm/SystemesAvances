#ifndef VECTOR_TABLE_H
#define VECTOR_TABLE_H

// The three vector tables (for EL3, EL2, and EL1) that are
// defined in assembly and used in C.
extern uint64_t VectorTableEL1[0x100] ;
extern uint64_t VectorTableEL2[0x100] ;

// While I don't control EL3...
//extern uint64_t VectorTableEL3[0x100] ;

#endif
