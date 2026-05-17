#ifndef INTCODE_H
#define INTCODE_H
#include <stdint.h>
#include <stdbool.h>

// VM constants
#define VM_QSIZE 32

// VM uses 64-bit signed ints
typedef int64_t VMType;

typedef enum vmstate VMState;

// Day 2: "Encountering an unknown opcode means something went wrong."
typedef enum vmopcode VMOpCode;
typedef enum vmparam VMParam;

typedef struct vmqueue VMQueue;

// Memory, instruction pointer, offset
// for parmode=2, size of memory, state
typedef struct vm VM;

// Translation of parameter modes and opcode from
// decimal to binary with added info: parameter count
// MSByte = opcode
// LSByte = 3x 2-bit parmode, 1x 2-bit parcount
extern const uint16_t vm_decode[];

// Parse one-line CSV of signed ints from input file to VM data
extern int vm_parse(const char *const input, VMType *const code);

// Resize (only larger) or free (size=0)
extern bool vm_resize(VM *vm, const int newsize);

// Full VM initialisation
extern bool vm_init(VM *const vm, const VMType *const code, const int codesize);

// Queue management
extern bool vm_enq(VMQueue *const vmq, const VMType val);
extern bool vm_deq(VMQueue *const vmq, VMType *const val);
extern bool vm_write_input(VM *const vm, const VMType inputval);
extern bool vm_read_output(VM *const vm, VMType *const outputval);

extern VMState run(VM *const vm);

#endif
