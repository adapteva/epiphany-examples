#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <e-hal.h>

#include "common.h"

#if 0
#define e_read(dev, row, col, addr, dst, size)\
    do {\
        printf("DEBUG: e_read\t(0x%p, %d, %d, 0x%x, 0x%p, %u)",\
               (dev), (row), (col), (addr), (dst), (size));\
        fflush(stdout);\
        /*usleep(100000);*/\
        e_read((dev), (row), (col), (addr), (dst), (size));\
        printf(" = 0x%08x.\n", *((uint32_t *) (dst)));\
        fflush(stdout);\
    } while (0)

#define e_write(dev, row, col, addr, src, size)\
    do {\
        printf("DEBUG: e_write\t(0x%p, %d, %d, 0x%x, 0x%p, %u)",\
               (dev), (row), (col), (addr), (src), (size));\
        fflush(stdout);\
        /*usleep(100000);*/\
        e_write((dev), (row), (col), (addr), (src), (size));\
        printf(".\n");\
        fflush(stdout);\
    } while (0)
#endif

// TODO
bool verify(int phase)
{
    return true;
}

int main(int argc, char *argv[])
{
    e_platform_t platform;
    e_epiphany_t dev;

    uint32_t flag, addr, val, phase, prev_phase = 0, expect, ack;
    const uint32_t zero = 0, one = 1;

    //Initalize Epiphany device
    if (e_init(NULL))
        abort();
    if (e_reset_system()) // reset Epiphany
        abort();
    usleep(10000);
    if (e_get_platform_info(&platform))
        abort();
    // TODO: Open all cores, not just one
    if (e_open(&dev, 0, 0, 1, 1)) //open all cores
        abort();
    usleep(10000);

    e_write(&dev, 0, 0, DEV_ADDR_FLAG, &one, sizeof(one));


    //Load program to cores and run
    //TODO: Run on all cores
    if (e_load_group("e_task.elf", &dev, 0, 0, 1, 1, E_TRUE))
        abort();

    //usleep(10000);

    //usleep(10000);
    e_write(&dev, 0, 0, DEV_ADDR_HOST_STOP, &zero, sizeof(zero));
    //usleep(10000);
    e_write(&dev, 0, 0, DEV_ADDR_HOST_ACK, &zero, sizeof(zero));
    //usleep(10000);
    e_write(&dev, 0, 0, DEV_ADDR_FLAG, &zero, sizeof(zero));
    //usleep(10000);

    //Check if all cores are done
    while (1) {
        //usleep(1000000);
        e_read(&dev, 0, 0, DEV_ADDR_FLAG, &flag, sizeof(flag));
        //usleep(10000);
        e_read(&dev, 0, 0, DEV_ADDR_VAL, &val,  sizeof(val));
        //usleep(10000);
        e_read(&dev, 0, 0, DEV_ADDR_ADDR, &addr, sizeof(addr));
        //usleep(10000);
        e_read(&dev, 0, 0, DEV_ADDR_PHASE, &phase, sizeof(phase));
        //usleep(10000);
        e_read(&dev, 0, 0, DEV_ADDR_EXPECT, &expect, sizeof(expect));
        //usleep(10000);

        if (flag)
            break;

        if (prev_phase != phase) {
            if (!verify(phase)) {
                // stop core
                e_write(&dev, 0, 0, DEV_ADDR_HOST_STOP, &one, sizeof(one));
                //usleep(100000);
                //e_write(&dev, 0, 0, DEV_ADDR_HOST_STOP, &one, sizeof(one));
                //usleep(100000);
                flag = FAIL;
                break;
            }

            ack = phase;
            printf("Progress: %d\n", phase);
            //usleep(10000);
            //usleep(100000);
            e_write(&dev, 0, 0, DEV_ADDR_HOST_ACK, &ack, sizeof(ack));
            //usleep(100000);
            //e_write(&dev, 0, 0, DEV_ADDR_HOST_ACK, &ack, sizeof(ack));
            //usleep(100000);
            prev_phase = phase;
        }
    }

    if (flag != OK) {
        printf("Error: addr: 0x%08x val: 0x%08x expect: 0x%08x phase: %d\n",
               addr, val, expect, phase);
        return 1;
    }

    printf("OK\n");
    return 0;
}
