#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory.h"

/****************************************************************************
 * De configuratie van de heap
 ****************************************************************************/

#define HEAP_SIZE   1024
#define BLOCK_SIZE  64

#define NUMBER_OF_BLOCKS  ((HEAP_SIZE) / (BLOCK_SIZE))

/****************************************************************************
 * Interne gegevensstructuren die gebruikt worden om de boekhouding van het
 * dynamisch geheugengebruik bij te houden.
 ****************************************************************************/

struct block
{
  uint8_t *address;
  uint32_t alloc_count;
  struct block *prev;
  struct block *next;
};

struct list
{
  struct block *first;
  struct block *last;
};

/****************************************************************************
 * Interne variabelen om de heap voor te stellen en om de interne boekhouding
 * van het geheugengebruik te doen.
 ****************************************************************************/

static uint8_t heap[HEAP_SIZE];

static struct block pool_of_blocks[NUMBER_OF_BLOCKS];

static struct list free_list;

static struct list used_list;

/****************************************************************************
 * Declaraties van de interne functies.
 ****************************************************************************/

static void list_init(struct list *list);

static bool block_is_valid(const struct block *block);

static bool list_contains(const struct list *list, const struct block *block);

static uint32_t list_get_length(const struct list *list);

static void list_print(struct list *list, const char *title);

static void list_print_reverse(struct list *list, const char *title);

static struct block *list_find_block_by_address(const struct list *list,
                                                const uint8_t *address);

static bool blocks_are_contiguous(const struct block *left,
                                  const struct block *right);

static uint32_t required_number_of_contiguous_blocks(uint32_t size);

static bool has_number_of_contiguous_blocks(const struct block *block,
                                            uint32_t            count);

static void list_init_block(struct list  *list,
                            struct block *block,
                            uint8_t      *address);

static void list_insert_chain(struct list* list, struct block *block);

static uint32_t list_remove_chain(struct list  *list,
                                  struct block *block,
                                  uint32_t      block_count);

#include "test.c"
