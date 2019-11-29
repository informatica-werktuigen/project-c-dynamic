#include "memory_priv.h"

/* Initalizes the given list to be the empty list.
 *
 * This function is already implemented for you.
 */
static void list_init(struct list *list)
{
  list->first = NULL;
  list->last = NULL;
}

/* Returns true when the given block is valid.
 *
 * This function is already implemented for you.
 */
static bool block_is_valid(const struct block *block)
{
  return (   (block != NULL)
          && (block->address >= heap)
          && (block->address < &(heap[HEAP_SIZE]))
         );
}

/* Returns true if the given list contains the given block. Returns false
 * otherwise.
 */
static bool list_contains(const struct list *list, const struct block *block)
{
  return false;
}

/* Returns the length of the given list (the number of blocks it contains) */
static uint32_t list_get_length(const struct list *list)
{
  return 0;
}

/* Prints a human representation of the given list in forward order.
 *
 * Expected format:
 *
 *   Assuming a value for title of "free list", a list with two elements, with
 *   addresses 0x559cd4da1040 and 0x559cd4da1060 should be printed as follows:
 *
 *     free list:
 *       0x559cd4da1040->0x559cd4da1060->NULL
 *
 * Hint: the %p printf modifier can be used to print the value of a pointer
 */
static void list_print(struct list *list, const char *title)
{
}

/* Prints a human representation of the given list in reverse order.
 *
 * The same format as for the function "list_print" is expected.
 */
static void list_print_reverse(struct list *list, const char *title)
{
}

/* Returns the block for wich the given address falls within its address
 * range. The address range of a block starts with its address and ends
 * with its address + BLOCK_SIZE - 1.
 *
 * If the given list does not contain the block, returns NULL.
 */
static struct block *list_find_block_by_address(const struct list *list,
                                                const uint8_t *address)
{
  return NULL;
}

/* Returns true when the two given blocks are contiguous.
 *
 * Two blocks, left and right, are contiguous when
 *    1) the value of left's address field is less than the value of right's
 *    address field and 2) adding BLOCKS_SIZE to the value of left's address
 *    field gives the value of right's address field.
 */
static bool blocks_are_contiguous(const struct block *left,
                                  const struct block *right)
{
  return false;
}

/* Returns the number of contiguous blocks that is required to satisfy
 * an allocation request of size bytes.
 */
static uint32_t required_number_of_contiguous_blocks(uint32_t size)
{
  return 0xFFFFFFFF;
}

/* Returns true when the given block has at least count number of successors
 * and the first count successors of the given block are all contigous with
 * respect to their predecessors. Returns false otherwise.
 */
static bool has_number_of_contiguous_blocks(const struct block *block,
                                            uint32_t            count)
{
  return false;
}

/* Initalizes the given block with the given address and appends it to the
 * given list.
 *
 * Preconditions:
 *   - the given list is ordered by ascending address
 *   - when the list is not empty
 *     - the given address is greater than the address of the last block in
 *       the given list:
 *           address > list->last->address
 *     - the given address is BLOCK_SIZE bytes greater than the address of the
 *       last block in the given list:
 *           list->last->address + BLOCK_SIZE = address
 *
 * Postconditions:
 *   - the resulting list is ordered by ascending address
 *   - the last block in the resulting is the given block
 *   - the value of block->alloc_count is zero
 */
static void list_init_block(struct list  *list,
                            struct block *block,
                            uint8_t      *address)
{
}

/* Inserts a chain of blocks starting with the given block in the given list.
 *
 * Preconditions:
 *   - the given list is ordered by ascending address
 *   - no block in the chain of blocks that starts with the given block is
 *     already an element of the given list
 *   - the blocks in the given chain of blocks are ordered by ascending
 *     address
 *   - all blocks in the given chain of blocks are contiguous
 *
 * Postconditions:
 *   - the given list is ordered by ascending address
 *   - the given contains the given chain of blocks
 */
static void list_insert_chain(struct list* list, struct block *block)
{
}

/* Removes a chain of blocks starting with the given block from the given list,
 * without disconnnecting the individual links of the chain. At most
 * block_count blocks will be removed. This functions aims to remove
 * as many blocks as possible with block_count as the maximum.
 *
 * Preconditions:
 *   - the given block is an element of the given list
 *   - all blocks in the given chain of blocks are contiguous up to
 *     block_count or up to the end of the chain if the length of the chain
 *     is less than block_count.
 *
 * Returns the number of blocks that are actually removed.
 */
static uint32_t list_remove_chain(struct list  *list,
                                  struct block *block,
                                  uint32_t      block_count)
{
  return 0;
}

/* Initializes the dynamic memory and its bookeeping.
 *
 * This function is already implemented for you. Study this function
 * thoroughly in order to understand how the heap and its bookkeeping
 * is represented.
 */
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

/* Returns the amount of dynamic memory available in number of bytes */
uint32_t memory_available(void)
{
  return 0;
}

/* Returns the amount of dynamic memory used in number of bytes */
uint32_t memory_used(void)
{
  return 0;
}

/* Allocates size number of contiguous bytes and returns a pointer to the
 * allocated memory. The memory does not have to be initialized.
 *
 * contiguous memory region
 *
 * Returns NULL,
 *   if size is zero,
 *   or if not enough contiguous memory is available
 */
void *memory_allocate(uint32_t size)
{
  return NULL;
}

/* Releases the memory pointed to by the given pointer, which must have been
 * returned by a previous call to memory_allocate.
 *
 * Returns true when actual memory has been released, false otherwise.
 *
 * Possible reasons why the memory has not been released:
 *  - The given pointer is NULL.
 *  - The given pointer does not point to memory that was allocated by
 *    memory_allocate.
 */
bool memory_release(void *ptr)
{
  return false;
}
