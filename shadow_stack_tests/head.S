# initialize a stack for C routines
# Because we might be launched without bios, $sp must be initialized to valid
# value before entering C

#define STACK_SIZE 32768

.data

stack: .space STACK_SIZE

.text

.globl kernel_entry
kernel_entry:
        # skip hardware inits for qemu

        # initialize $sp
        la $sp, stack
        li.d $t0, STACK_SIZE
        add.d $sp, $sp, $t0

        bl start_kernel

