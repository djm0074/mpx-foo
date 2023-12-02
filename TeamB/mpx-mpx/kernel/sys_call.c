#include <mpx/sys_call.h>
#include <pcb.h>
#include <sys_req.h>
#include <string.h>

struct pcb *current_process = NULL; 
struct pcb *next_process = NULL;  
struct context *initial_context = NULL;  
int insert_flag = 0;

struct context *sys_call(struct context *ctx) {

    unsigned int operation = ctx->eax;
    if (operation == IDLE) {
        if (initial_context == NULL) {  
            initial_context = ctx;
        }
        // Handle IDLE 
        // if current_process != null  (something was running)
        // set current_prcess to ready
        // set current_process -> stackptr = ctx;
        // insert current_process into ready queue
        //ctx->eax = (uint32_t) 0;
        if (current_process != NULL) {
            current_process->execution_state = READY;
            current_process->stack_ptr = (unsigned char *) ctx;
            insert_flag = 1;
        }
    }

    else if (operation == EXIT) {
        // Handle EXIT
        // Delete current_process
        // Load next process context or initial_context if no other process
        // Set return value in ctx->eax to 0
        if (current_process != NULL) {
            pcb_remove(current_process); // Remove current_process from its queue
            pcb_free(current_process); // Deallocate PCB resources
            current_process = NULL;
        }
    } else {
        ctx->eax = (uint32_t) -1;  // Unsupported operation
        return ctx;
    }
    
    if (get_ready_q() != NULL && get_ready_q()->front != NULL) {
            next_process = get_ready_q()->front;
            ctx = (struct context *) next_process->stack_ptr;
            pcb_remove(next_process); // Remove current_process from its queue
            pcb_free(next_process); // Deallocate PCB resources
            if (insert_flag == 1) {
                pcb_insert(current_process);
                insert_flag = 0;
            }
            current_process = next_process;
    }
    else { // if no process, load initial context
        ctx = initial_context;
        initial_context = NULL; // reset initial_context as it's now being used
    }
    ctx->eax = (uint32_t) 0;

    return ctx;
}
