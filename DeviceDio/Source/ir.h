/**
 *  Functions for dealing with IR stuff
 */

#ifndef IR_H
#define IR_H

#include <stdint.h>
#include <stdbool.h>

/** initialize the IR hardware */
bool initialize_ir_hw(void);

/** call this in Device_vTick */
void ir_tick(void);


#endif

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
