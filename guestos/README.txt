Sushant Hiray 110050009
Sanchit Garg  110050035

Q1)
The boot() function is defined in file src/guestos.c
It performs the following actions:
-> install signals: Installs the basic signals
-> set defaults: It gets the following values from the .config file
                    Instruction slice
                    Number of Heads
                    Number of Tracks
                    Number of Sectors


Q3)

a) The system calls performed by the executed application are intercepted by Multi2Sim and emulated in file 'syscall.c' in the function syscall_do

syscall_do function is invoked in machine.c in function op_int_imm8_impl

b) 327 syscalls are present: found in src/libm2skernel/syscall.dat. Out of which 1st 326 are handled by host OS. In the current system only the last call namely syscall_code=400 is handled by host OS. However it can support 11 syscalls (400-410)

c) The syscall_do handles the system call.The system call code is in eax. Depending on the syscall code,(if < 325 it is host os syscall else guest os syscall) it performs switch case, and then does appropriate handling depending whether the system call is a host os syscall or is a guest os syscall.

d) The initial step is to get the host file descriptor. 
This it does by first getting the guest file descriptor using the ebx parameter.
The host_fd is found using fd=fdt_entry_get(isa_ctx->fdt, guest_fd);
host_fd=fd->host_fd;

Now non-blocking read is performed using: read(host_fd, buf, count) 
If all goes well the data from isa_mem (which is a struct) is read and written into an allocated buffer.

In this way memory is accessed.



Q4)
a) The struct kernel_t has pointers to head and tail of PCB's of context,running,free,suspended,zombie,finished and allocated. All the PCB's can be found by iterating over the list from the head  by using ->next.

b) The struct ctx_t contains a field instr_slice which holds the instruction slice of the process.

c) Scheduling is handling in function ke_run() in the file src/libm2skernel/m2skernel.c
It executes one instruction from each running context.
It starts with the running_list_head. It executes instr_slice no. of instructions for the head. For other processes, it runs a single instruction and breaks.

Once all the processes in the running list are looped over once, it frees up all the finished contexts. 

After freeing up the finished contexts, it calls the ke_process_events which processes the set of suspended contexts, ie looks up the list of suspended contexts and tries to find the process that needs to be woken up.


Q5)
1) a) test file: test.c: just checks syscall
   b) test file: 1.c 2.c 
   		run as follows:
   			./src/guestos/
   			no of processes = 2
   			/pathto/1.out
   			/pathto/2.out

   			We can notice that there 2 is printed 10 times (= time slice) and 1 is printed once as only 1 instruction for 1 is executed. (see explanation for scheduling)
