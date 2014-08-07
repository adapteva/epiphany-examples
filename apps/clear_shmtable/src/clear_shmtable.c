/*
  clear_shmtable.c

  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Yaniv Sapir <yaniv@adapteva.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/

// This program clears all existing shared memory regions from the 
// Epiphany SHM Table.

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#include <e-hal.h>

typedef struct region
{
    e_shmseg_t *segment;
    struct region *next;
} region_t;

region_t *rgn_list = NULL;

static int add_region(region_t *element)
{
    assert( element );

    if ( !element ) {
        return -1;
    }
    
    if ( !rgn_list ) {
        rgn_list = element;
    } else {
        element->next = rgn_list;
        rgn_list = element;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int i = 0, rgncnt = 0;
    e_shmtable_t  *tbl = NULL;

    e_set_host_verbosity(H_D0);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	if ( E_OK != e_init(NULL) ) {
        fprintf(stderr, "Epiphany HAL initialization failed\n");
        return EXIT_FAILURE;
    }

	if ( E_OK != e_reset_system() ) {
        fprintf(stderr, "Epiphany system reset failed\n");
        return EXIT_FAILURE;
    }
    
    tbl = e_shm_get_shmtable();

    if ( tbl ) {
        for ( i = 0; i < MAX_SHM_REGIONS; ++i ) {
            if ( tbl->regions[i].valid ) {
                region_t *shm_reg = (region_t*)malloc(sizeof(region_t));
                shm_reg->segment = &tbl->regions[i].shm_seg;
                shm_reg->next = NULL;

                add_region(shm_reg);
                ++rgncnt;
            }
        }
    }

    printf("Found %d allocated regions\n", rgncnt);

    rgncnt = 0;

    region_t *rgn = rgn_list; 
    while ( rgn ) {
        region_t *tmp = NULL;

		printf("Releasing segment: %s\n", rgn->segment->name);

        if ( E_OK != e_shm_release(rgn->segment->name) ) {
            printf("Error releasing segment %s", rgn->segment->name);
        }

        ++rgncnt;

        tmp = rgn;
        rgn = rgn->next;

        free(tmp);
    }

    printf("Released %d allocated regions\n", rgncnt);

	e_finalize();

	return 0;
}

