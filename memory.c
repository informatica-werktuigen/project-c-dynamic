#include "memory_priv.h"

static void list_init(struct list *list)
{
  list->first = NULL;
  list->last = NULL;
}

static void list_init_block(struct list  *list,
                            struct block *block,
                            uint8_t      *address)
{
}

static bool block_is_valid(const struct block *block)
{
  return false;
}

static void list_print(struct list *list, const char *title)
{
}

static void list_print_reverse(struct list *list, const char *title)
{
}

static uint32_t list_get_length(const struct list *list)
{
  return 0;
}

static struct block *list_find_block_by_address(const struct list *list,
                                                const uint8_t *address)
{
  return NULL;
}

static uint32_t required_number_of_contiguous_blocks(uint32_t size)
{
  return 0xFFFFFFFF;
}

static bool blocks_are_contiguous(const struct block *left,
                                  const struct block *right)
{
  return false;
}

static bool has_number_of_contiguous_blocks(const struct block *block,
                                            uint32_t            count)
{
  return false;
}

static uint32_t list_remove_chain(struct list  *list,
                                  struct block *block,
                                  uint32_t      block_count)
{
  return 0;
}

static void list_insert_chain(struct list* list, struct block *block)
{
}

void memory_initialize(void)
{
  list_init(&free_list);
  list_init(&used_list);

  uint8_t *address = heap;
  for (int i=0; i < (sizeof(pool_of_blocks)/sizeof(pool_of_blocks[0])); i++)
  {
    list_init_block(&free_list, &(pool_of_blocks[i]), address);
    address += BLOCK_SIZE;
  }
  assert(address == &(heap[HEAP_SIZE]));
}

uint32_t memory_available(void)
{
  return 0;
}

uint32_t memory_used(void)
{
  return 0;
}

void *memory_allocate(uint32_t size)
{
  return NULL;
}

bool memory_release(void *ptr)
{
  return false;
}
