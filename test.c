#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define TEST(ctxt, expr)                                              \
  if ( (expr) ) ctxt->success_count++, fprintf(stdout, ".");          \
  else          ctxt->fail_count++, fail(__FILE__, __LINE__, #expr);

typedef struct context_s
{
  char name[80];
  int success_count;
  int fail_count;
} context_t;

static context_t contexts[100];

static void print_separator(void)
{
  fprintf(stdout,
  "  ------------------------------------------------------------------\n");
}

static context_t *new_context(const char *name)
{
  static int idx = 0;

  #define PREFIX "test_"
  assert(strncmp(name, PREFIX, strlen(PREFIX)) == 0);
  const char *n = name+strlen(PREFIX);

  print_separator();
  printf("  %s\n  ", n);

  assert(idx < (sizeof(contexts)/sizeof(contexts[0])));
  context_t * result = &(contexts[idx++]);

  strncpy(result->name, name, sizeof(result->name));
  result->name[sizeof(result->name)-1] = '\0';
  result->success_count = 0;
  result->fail_count    = 0;

  return result;
}

static uint32_t _list_get_length(const struct list *list)
{
  uint32_t result = 0;

  struct block *block = list->first;
  while (block != NULL)
  {
    result++;
    block = block->next;
  }

  return result;
}

/****************************************************************************/
static void fail(char *file, int line, char *cond)
{
#if 1
  fprintf(stderr, "%s:%d: Test '%s' failed\n", file, line, cond);
#endif
}

/****************************************************************************/
static void list_append(struct list *list, struct block *block)
{
  block->next = NULL;
  block->alloc_count = 0;

  if (list->first == NULL)
  {
    assert(list->last == NULL);

    list->first = block;
    list->last = block;
    block->prev = NULL;
    block->address = heap;
  }
  else
  {
    assert(list->last != NULL);

    block->prev = list->last;
    block->prev->next = block;
    list->last = block;
    block->address = block->prev->address + BLOCK_SIZE;
  }
}

/****************************************************************************/
static struct block orphin_block;
static struct list  list0;

static struct list list1;
static struct block block1a;

static struct list list2;
static struct block block2a;
static struct block block2b;

static struct list list3;
static struct block block3a;
static struct block block3b;
static struct block block3c;

static void initialize_globals()
{
  memset(&orphin_block, 0, sizeof(orphin_block));

  list_init(&list0);

  list_init(&list1);
  list_append(&list1, &block1a);

  list_init(&list2);
  list_append(&list2, &block2a);
  list_append(&list2, &block2b);

  list_init(&list3);
  list_append(&list3, &block3a);
  list_append(&list3, &block3b);
  list_append(&list3, &block3c);

}

static void print_summary(const context_t * ctxt)
{
  float ratio = 1;

  if ((ctxt->success_count + ctxt->fail_count) > 0)
  {
    ratio = (float)
      ctxt->success_count / (ctxt->success_count + ctxt->fail_count);
  }

  fprintf(stdout, "\n");
  fprintf(stdout,
      "  ratio=%.2f (pass=%02d fail=%02d)\n",
      ratio,
      ctxt->success_count,
      ctxt->fail_count);
}

/****************************************************************************/
static void test_list_init(void)
{
  context_t * ctxt = new_context(__func__);

  struct list list;

  list_init(&list);

  TEST(ctxt, list.first == NULL);
  TEST(ctxt, list.last == NULL);

  print_summary(ctxt);
}

/****************************************************************************/
static void test_block_is_valid(void)
{
  context_t * ctxt = new_context(__func__);

  TEST(ctxt, !block_is_valid(NULL));

  struct block block;

  block.address = heap;
  TEST(ctxt, block_is_valid(&block));

  block.address = heap + BLOCK_SIZE;
  TEST(ctxt, block_is_valid(&block));

  block.address++;
  TEST(ctxt, !block_is_valid(&block));

  block.address = &(heap[HEAP_SIZE]);
  TEST(ctxt, !block_is_valid(&block));

  block.address = &(heap[HEAP_SIZE]) - BLOCK_SIZE;
  TEST(ctxt, block_is_valid(&block));

  print_summary(ctxt);
}

static void test_list_contains(void)
{
  context_t *ctxt = new_context(__func__);

  TEST(ctxt, list_contains(&list1, &block1a));
  TEST(ctxt, list_contains(&list2, &block2a));
  TEST(ctxt, list_contains(&list2, &block2b));
  TEST(ctxt, list_contains(&list3, &block3a));
  TEST(ctxt, list_contains(&list3, &block3b));
  TEST(ctxt, list_contains(&list3, &block3c));

  TEST(ctxt, !list_contains(&list1, &orphin_block));
  TEST(ctxt, !list_contains(&list2, &block1a));
  TEST(ctxt, !list_contains(&list3, &block2b));

  print_summary(ctxt);
}

static void test_list_get_length(void)
{
  context_t *ctxt = new_context(__func__);

  TEST(ctxt, list_get_length(&list0) == 0);
  TEST(ctxt, list_get_length(&list1) == 1);
  TEST(ctxt, list_get_length(&list2) == 2);
  TEST(ctxt, list_get_length(&list3) == 3);

  print_summary(ctxt);
}

static void test_list_print(void)
{
  (void) new_context(__func__);

  list_print(&list0, "list0");
  list_print(&list1, "list1");
  list_print(&list3, "list3");
}

static void test_list_print_reverse(void)
{
  (void) new_context(__func__);

  list_print_reverse(&list0, "list0");
  list_print_reverse(&list1, "list1");
  list_print_reverse(&list3, "list3");
}

static void test_find_block_by_address(void)
{
  context_t *ctxt = new_context(__func__);

  TEST(ctxt, list_find_block_by_address(&list0, heap) == NULL);
  TEST(ctxt, list_find_block_by_address(&list1, heap) == &block1a);
  TEST(ctxt, list_find_block_by_address(&list2, heap) == &block2a);
  TEST(ctxt, list_find_block_by_address(&list3, heap) == &block3a);
  TEST(ctxt, list_find_block_by_address(&list0, heap+1) == NULL);
  TEST(ctxt, list_find_block_by_address(&list1, heap+1) == &block1a);
  TEST(ctxt, list_find_block_by_address(&list2, heap+1) == &block2a);
  TEST(ctxt, list_find_block_by_address(&list3, heap+1) == &block3a);
  TEST(ctxt, list_find_block_by_address(&list0, heap+BLOCK_SIZE) == NULL);
  TEST(ctxt, list_find_block_by_address(&list1, heap+BLOCK_SIZE) == NULL);
  TEST(ctxt, list_find_block_by_address(&list2, heap+BLOCK_SIZE) == &block2b);
  TEST(ctxt, list_find_block_by_address(&list3, heap+BLOCK_SIZE) == &block3b);
  TEST(ctxt, list_find_block_by_address(&list0, heap+2*BLOCK_SIZE) == NULL);
  TEST(ctxt, list_find_block_by_address(&list1, heap+2*BLOCK_SIZE) == NULL);
  TEST(ctxt, list_find_block_by_address(&list2, heap+2*BLOCK_SIZE) == NULL);
  TEST(ctxt, list_find_block_by_address(&list3, heap+2*BLOCK_SIZE) == &block3c);

  print_summary(ctxt);
}

static void test_blocks_are_contiguous(void)
{
  context_t *ctxt = new_context(__func__);

  TEST(ctxt, !blocks_are_contiguous(&block1a, &block1a));
  TEST(ctxt, blocks_are_contiguous(&block2a, &block2b));
  TEST(ctxt, blocks_are_contiguous(&block3a, &block3b));
  TEST(ctxt, blocks_are_contiguous(&block3b, &block3c));
  TEST(ctxt, !blocks_are_contiguous(&block3a, &block3c));

  print_summary(ctxt);
}

static void test_required_number_of_contiguous_blocks(void)
{
  context_t *ctxt = new_context(__func__);

  TEST(ctxt, required_number_of_contiguous_blocks(0) == 0);
  TEST(ctxt, required_number_of_contiguous_blocks(1) == 1);
  TEST(ctxt, required_number_of_contiguous_blocks(BLOCK_SIZE) == 1);
  TEST(ctxt, required_number_of_contiguous_blocks(BLOCK_SIZE+1) == 2);
  TEST(ctxt, required_number_of_contiguous_blocks(2*BLOCK_SIZE) == 2);
  TEST(ctxt, required_number_of_contiguous_blocks(2*BLOCK_SIZE+1) == 3);

  print_summary(ctxt);
}

static void test_has_number_of_contiguous_blocks(void)
{
  context_t *ctxt = new_context(__func__);

  TEST(ctxt, has_number_of_contiguous_blocks(&block1a, 0));
  TEST(ctxt, has_number_of_contiguous_blocks(&block1a, 1));
  TEST(ctxt, !has_number_of_contiguous_blocks(&block1a, 2));
  TEST(ctxt, has_number_of_contiguous_blocks(&block2a, 2));
  TEST(ctxt, !has_number_of_contiguous_blocks(&block2a, 3));
  TEST(ctxt, has_number_of_contiguous_blocks(&block3a, 1));
  TEST(ctxt, has_number_of_contiguous_blocks(&block3a, 2));
  TEST(ctxt, has_number_of_contiguous_blocks(&block3a, 3));
  TEST(ctxt, !has_number_of_contiguous_blocks(&block3a, 4));
  TEST(ctxt, has_number_of_contiguous_blocks(&block3b, 1));
  TEST(ctxt, has_number_of_contiguous_blocks(&block3b, 2));
  TEST(ctxt, !has_number_of_contiguous_blocks(&block3b, 3));

  print_summary(ctxt);
}

static void test_list_init_block(void)
{
  context_t *ctxt = new_context(__func__);

  struct list list;
  struct block block1;
  struct block block2;
  struct block block3;

  list_init(&list);

  list_init_block(&list, &block1, heap);
  TEST(ctxt, list.first == &block1);
  TEST(ctxt, list.last == &block1);
  TEST(ctxt, block1.address == heap);
  TEST(ctxt, block1.alloc_count == 0);
  TEST(ctxt, block1.next == NULL);

  list_init_block(&list, &block2, heap+BLOCK_SIZE);
  TEST(ctxt, list.first == &block1);
  TEST(ctxt, list.last == &block2);
  TEST(ctxt, block1.address == heap);
  TEST(ctxt, block1.alloc_count == 0);
  TEST(ctxt, block1.next == &block2);
  TEST(ctxt, block2.address == heap+BLOCK_SIZE);
  TEST(ctxt, block2.alloc_count == 0);
  TEST(ctxt, block2.next == NULL);
  TEST(ctxt, block2.prev == &block1);

  list_init_block(&list, &block3, heap+2*BLOCK_SIZE);
  TEST(ctxt, list.first == &block1);
  TEST(ctxt, list.last == &block3);
  TEST(ctxt, block1.address == heap);
  TEST(ctxt, block1.alloc_count == 0);
  TEST(ctxt, block1.next == &block2);
  TEST(ctxt, block2.address == heap+BLOCK_SIZE);
  TEST(ctxt, block2.alloc_count == 0);
  TEST(ctxt, block2.next == &block3);
  TEST(ctxt, block2.prev == &block1);
  TEST(ctxt, block3.address == heap+2*BLOCK_SIZE);
  TEST(ctxt, block3.alloc_count == 0);
  TEST(ctxt, block3.next == NULL);
  TEST(ctxt, block3.prev == &block2);

  print_summary(ctxt);
}

static void test_list_insert_chain(void)
{
  context_t *ctxt = new_context(__func__);

  struct list list;
  struct block block1;
  //struct block block2;
  struct block block3;
  struct block block4;
  struct block block5;
  struct block block6;
  struct block block7;
  struct block block8;

  list_init(&list);

  block3.address = heap+3*BLOCK_SIZE;
  block3.next = &block4;
  block3.prev = NULL;
  block4.address = heap+4*BLOCK_SIZE;
  block4.next = NULL;
  block4.prev = &block3;

  list_insert_chain(&list, &block3);
  TEST(ctxt, list.first == &block3);
  TEST(ctxt, block3.prev == NULL);
  TEST(ctxt, block3.next == &block4);
  TEST(ctxt, block4.prev == &block3);
  TEST(ctxt, block4.next == NULL);
  TEST(ctxt, list.last == &block4);

  block1.address = heap+BLOCK_SIZE;
  block1.next = NULL;
  block1.prev = NULL;
  list_insert_chain(&list, &block1);
  TEST(ctxt, list.first == &block1);
  TEST(ctxt, block1.prev == NULL);
  TEST(ctxt, block1.next == &block3);
  TEST(ctxt, block3.prev == &block1);
  TEST(ctxt, block3.next == &block4);
  TEST(ctxt, block4.prev == &block3);
  TEST(ctxt, block4.next == NULL);
  TEST(ctxt, list.last == &block4);

  block8.address = heap+8*BLOCK_SIZE;
  block8.next = NULL;
  block8.prev = NULL;
  list_insert_chain(&list, &block8);
  TEST(ctxt, list.first == &block1);
  TEST(ctxt, block1.prev == NULL);
  TEST(ctxt, block1.next == &block3);
  TEST(ctxt, block3.prev == &block1);
  TEST(ctxt, block3.next == &block4);
  TEST(ctxt, block4.prev == &block3);
  TEST(ctxt, block4.next == &block8);
  TEST(ctxt, block8.prev == &block4);
  TEST(ctxt, list.last == &block8);

  block5.address = heap+5*BLOCK_SIZE;
  block5.prev = NULL;
  block5.next = &block6;
  block6.address = heap+6*BLOCK_SIZE;
  block6.prev = &block5;
  block6.next = &block7;
  block7.address = heap+7*BLOCK_SIZE;
  block7.prev = &block6;
  block7.next = NULL;
  list_insert_chain(&list, &block5);
  TEST(ctxt, list.first == &block1);
  TEST(ctxt, block1.prev == NULL);
  TEST(ctxt, block1.next == &block3);
  TEST(ctxt, block3.prev == &block1);
  TEST(ctxt, block3.next == &block4);
  TEST(ctxt, block4.prev == &block3);
  TEST(ctxt, block4.next == &block5);
  TEST(ctxt, block5.prev == &block4);
  TEST(ctxt, block5.next == &block6);
  TEST(ctxt, block6.prev == &block5);
  TEST(ctxt, block6.next == &block7);
  TEST(ctxt, block7.prev == &block6);
  TEST(ctxt, block7.next == &block8);
  TEST(ctxt, block8.prev == &block7);
  TEST(ctxt, block8.next == NULL);
  TEST(ctxt, list.last == &block8);
  TEST(ctxt, block1.address == heap+1*BLOCK_SIZE);
  TEST(ctxt, block1.next->address == heap+3*BLOCK_SIZE);
  TEST(ctxt, block1.next->next->address == heap+4*BLOCK_SIZE);
  TEST(ctxt, block1.next->next->next->address == heap+5*BLOCK_SIZE);
  TEST(ctxt, block1.next->next->next->next->address == heap+6*BLOCK_SIZE);
  TEST(ctxt, block1.next->next->next->next->next->address == heap+7*BLOCK_SIZE);
  TEST(ctxt,
       block1.next->next->next->next->next->next->address == heap+8*BLOCK_SIZE);

  print_summary(ctxt);
}

static void test_list_remove_chain(void)
{
  context_t *ctxt = new_context(__func__);

  struct list list;
  struct block block1;
  struct block block2;
  struct block block3;
  struct block block4;
  struct block block5;
  struct block block6;
  struct block block7;
  struct block block8;

  list_init(&list);
  list_append(&list, &block1);
  list_append(&list, &block2);
  list_append(&list, &block3);
  list_append(&list, &block4);
  list_append(&list, &block5);
  list_append(&list, &block6);
  list_append(&list, &block7);
  list_append(&list, &block8);

  block1.address = heap+0*BLOCK_SIZE;
  block2.address = heap+1*BLOCK_SIZE;
  block3.address = heap+2*BLOCK_SIZE;
  block4.address = heap+3*BLOCK_SIZE;
  block5.address = heap+4*BLOCK_SIZE;
  block6.address = heap+5*BLOCK_SIZE;
  block7.address = heap+6*BLOCK_SIZE;
  block8.address = heap+7*BLOCK_SIZE;

  TEST(ctxt, list_remove_chain(&list, &block1, 1) == 1);
  TEST(ctxt, list.first == &block2);
  TEST(ctxt, list_remove_chain(&list, &block8, 2) == 1);
  TEST(ctxt, list.last == &block7);
  TEST(ctxt, list_remove_chain(&list, &block3, 1) == 1);
  TEST(ctxt, block2.next == &block4);
  TEST(ctxt, block4.prev == &block2);
  TEST(ctxt, list_remove_chain(&list, &block4, 3) == 3);
  TEST(ctxt, _list_get_length(&list) == 2);
  TEST(ctxt, list_remove_chain(&list, &block2, 3) == 2);
  TEST(ctxt, _list_get_length(&list) == 0);
  TEST(ctxt, list.first == NULL);
  TEST(ctxt, list.last == NULL);

  list_append(&list, &block1);
  TEST(ctxt, list_remove_chain(&list, &block1, 10) == 1);
  TEST(ctxt, _list_get_length(&list) == 0);
  TEST(ctxt, list.first == NULL);
  TEST(ctxt, list.last == NULL);

  print_summary(ctxt);
}

static void test_memory_initialize(void)
{
  context_t *ctxt = new_context(__func__);

  memory_initialize();

  TEST(ctxt, used_list.first == NULL);
  TEST(ctxt, used_list.last == NULL);
  TEST(ctxt, free_list.first != NULL);
  TEST(ctxt, free_list.last != NULL);

  uint8_t * expected = heap;
  for (struct block * p = free_list.first; p != NULL; p = p->next)
  {
    TEST(ctxt, p->address == expected);
    TEST(ctxt, p->alloc_count == 0);
    expected += BLOCK_SIZE;
  }

  expected = &heap[HEAP_SIZE] - BLOCK_SIZE;
  for (struct block * p = free_list.last; p != NULL; p = p->prev)
  {
    TEST(ctxt, p->address == expected);
    expected -= BLOCK_SIZE;
  }

  print_summary(ctxt);
}

static void test_memory_allocate(void)
{
  context_t *ctxt = new_context(__func__);

  list_init(&used_list);
  list_init(&free_list);

  struct block block0;
  struct block block2;
  struct block block3;
  struct block block4;
  struct block block6;
  struct block block7;
  struct block block9;
  struct block block10;
  struct block block11;
  struct block block12;

  list_append(&free_list, &block0);
  list_append(&free_list, &block2);
  list_append(&free_list, &block3);
  list_append(&free_list, &block4);
  list_append(&free_list, &block6);
  list_append(&free_list, &block7);
  list_append(&free_list, &block9);
  list_append(&free_list, &block10);
  list_append(&free_list, &block11);
  list_append(&free_list, &block12);

  block0.address = heap+0*BLOCK_SIZE;

  block2.address = heap+2*BLOCK_SIZE;
  block3.address = heap+3*BLOCK_SIZE;
  block4.address = heap+4*BLOCK_SIZE;

  block6.address = heap+6*BLOCK_SIZE;
  block7.address = heap+7*BLOCK_SIZE;

  block9.address = heap+9*BLOCK_SIZE;
  block10.address = heap+10*BLOCK_SIZE;
  block11.address = heap+11*BLOCK_SIZE;
  block12.address = heap+12*BLOCK_SIZE;

  assert(block_is_valid(&block0));
  assert(block_is_valid(&block2));
  assert(block_is_valid(&block3));
  assert(block_is_valid(&block4));
  assert(block_is_valid(&block6));
  assert(block_is_valid(&block7));
  assert(block_is_valid(&block9));
  assert(block_is_valid(&block10));
  assert(block_is_valid(&block11));
  assert(block_is_valid(&block12));

  uint8_t *p1;

  p1 = (uint8_t *) memory_allocate(0);
  TEST(ctxt, p1 == NULL);
  p1 = (uint8_t *) memory_allocate(4*BLOCK_SIZE+1); 
  TEST(ctxt, p1 == NULL);
  p1 = (uint8_t *) memory_allocate(5*BLOCK_SIZE); 
  TEST(ctxt, p1 == NULL);

  p1 = (uint8_t *) memory_allocate(4*BLOCK_SIZE); 
  TEST(ctxt, p1 == block9.address);
  TEST(ctxt, block9.alloc_count == 4);
  TEST(ctxt, _list_get_length(&free_list) == 6);
  TEST(ctxt, _list_get_length(&used_list) == 4);
  TEST(ctxt, free_list.first == &block0);
  TEST(ctxt, free_list.last == &block7);
  TEST(ctxt, used_list.first == &block9);
  TEST(ctxt, used_list.last == &block12);

  p1 = (uint8_t *) memory_allocate(7*BLOCK_SIZE); 
  TEST(ctxt, p1 == NULL);

  p1 = (uint8_t *) memory_allocate(2*BLOCK_SIZE+1); 
  TEST(ctxt, p1 == block2.address);
  TEST(ctxt, block2.alloc_count == 3);
  TEST(ctxt, _list_get_length(&free_list) == 3);
  TEST(ctxt, _list_get_length(&used_list) == 7);
  TEST(ctxt, free_list.first == &block0);
  TEST(ctxt, free_list.last == &block7);
  TEST(ctxt, used_list.first == &block2);
  TEST(ctxt, used_list.last == &block12);

  p1 = (uint8_t *) memory_allocate(4*BLOCK_SIZE); 
  TEST(ctxt, p1 == NULL);

  p1 = (uint8_t *) memory_allocate(BLOCK_SIZE-1); 
  TEST(ctxt, p1 == block0.address);
  TEST(ctxt, block0.alloc_count == 1);
  TEST(ctxt, _list_get_length(&free_list) == 2);
  TEST(ctxt, _list_get_length(&used_list) == 8);
  TEST(ctxt, free_list.first == &block6);
  TEST(ctxt, free_list.last == &block7);
  TEST(ctxt, used_list.first == &block0);
  TEST(ctxt, used_list.last == &block12);

  p1 = (uint8_t *) memory_allocate(3*BLOCK_SIZE); 
  TEST(ctxt, p1 == NULL);

  p1 = (uint8_t *) memory_allocate(BLOCK_SIZE+3); 
  TEST(ctxt, p1 == block6.address);
  TEST(ctxt, block6.alloc_count == 2);
  TEST(ctxt, _list_get_length(&free_list) == 0);
  TEST(ctxt, _list_get_length(&used_list) == 10);
  TEST(ctxt, free_list.first == NULL);
  TEST(ctxt, free_list.last == NULL);
  TEST(ctxt, used_list.first == &block0);
  TEST(ctxt, used_list.last == &block12);

  p1 = (uint8_t *) memory_allocate(1);
  TEST(ctxt, p1 == NULL);

  print_summary(ctxt);
}

static void test_memory_release(void)
{
  context_t *ctxt = new_context(__func__);

  list_init(&used_list);
  list_init(&free_list);

  struct block block0;
  struct block block2;
  struct block block3;
  struct block block4;
  struct block block6;
  struct block block7;
  struct block block9;
  struct block block10;
  struct block block11;
  struct block block12;

  list_append(&used_list, &block0);
  list_append(&used_list, &block2);
  list_append(&used_list, &block3);
  list_append(&used_list, &block4);
  list_append(&used_list, &block6);
  list_append(&used_list, &block7);
  list_append(&used_list, &block9);
  list_append(&used_list, &block10);
  list_append(&used_list, &block11);
  list_append(&used_list, &block12);

  block0.address = heap;
  block0.alloc_count = 1;
  uint8_t * p0 = block0.address;

  block2.address = heap+2*BLOCK_SIZE;
  block3.address = heap+3*BLOCK_SIZE;
  block4.address = heap+4*BLOCK_SIZE;
  block2.alloc_count = 3;
  block3.alloc_count = 0;
  block4.alloc_count = 0;
  uint8_t * p2 = block2.address;

  block6.address = heap+6*BLOCK_SIZE;
  block7.address = heap+7*BLOCK_SIZE;
  block6.alloc_count = 2;
  block7.alloc_count = 0;
  uint8_t * p6 = block6.address;

  block9.address = heap+9*BLOCK_SIZE;
  block10.address = heap+10*BLOCK_SIZE;
  block11.address = heap+11*BLOCK_SIZE;
  block12.address = heap+12*BLOCK_SIZE;
  block9.alloc_count = 4;
  block10.alloc_count = 0;
  block11.alloc_count = 0;
  block12.alloc_count = 0;
  uint8_t * p9 = block9.address;

  assert(block_is_valid(&block0));
  assert(block_is_valid(&block2));
  assert(block_is_valid(&block3));
  assert(block_is_valid(&block4));
  assert(block_is_valid(&block6));
  assert(block_is_valid(&block7));
  assert(block_is_valid(&block9));
  assert(block_is_valid(&block10));
  assert(block_is_valid(&block11));
  assert(block_is_valid(&block12));

  TEST(ctxt, memory_release(p0));
  TEST(ctxt, memory_release(p2));
  TEST(ctxt, memory_release(p6));
  TEST(ctxt, memory_release(p9));
  TEST(ctxt, !memory_release(NULL));
  TEST(ctxt, !memory_release(heap-1));
  TEST(ctxt, !memory_release(heap+1));

  print_summary(ctxt);
}

static void test_memory_available(void)
{
  uint32_t available = HEAP_SIZE;
  void *p1, *p2, *p3;

  context_t *ctxt = new_context(__func__);

  memory_initialize();

  TEST(ctxt, memory_available() == available);
  p1 = memory_allocate(1); available -= BLOCK_SIZE;
  TEST(ctxt, memory_available() == available);
  memory_allocate(BLOCK_SIZE); available -= BLOCK_SIZE;
  TEST(ctxt, memory_available() == available);
  p2 = memory_allocate(2*BLOCK_SIZE+1); available -= 3*BLOCK_SIZE;
  TEST(ctxt, memory_available() == available);
  p3 = memory_allocate(3*BLOCK_SIZE); available -= 3*BLOCK_SIZE;
  TEST(ctxt, memory_available() == available);
  memory_allocate(3*BLOCK_SIZE+10); available -= 4*BLOCK_SIZE;
  TEST(ctxt, memory_available() == available);

  memory_release(p2); available += 3*BLOCK_SIZE;
  TEST(ctxt, memory_available() == available);
  memory_release(p1); available += BLOCK_SIZE;
  TEST(ctxt, memory_available() == available);
  memory_release(p3); available += 3*BLOCK_SIZE;
  TEST(ctxt, memory_available() == available);

  p1 = memory_allocate(1); available -= BLOCK_SIZE;
  TEST(ctxt, memory_available() == available);

  print_summary(ctxt);
}

static void test_memory_used(void)
{
  uint32_t used = 0;
  void *p1, *p2, *p3;

  context_t *ctxt = new_context(__func__);

  memory_initialize();

  TEST(ctxt, memory_used() == used);
  memory_allocate(10); used += BLOCK_SIZE;
  TEST(ctxt, memory_used() == used);
  p1 = memory_allocate(2*BLOCK_SIZE); used += 2*BLOCK_SIZE;
  TEST(ctxt, memory_used() == used);
  p2 = memory_allocate(3*BLOCK_SIZE+1); used += 4*BLOCK_SIZE;
  TEST(ctxt, memory_used() == used);
  memory_allocate(2*BLOCK_SIZE); used += 2*BLOCK_SIZE;
  TEST(ctxt, memory_used() == used);
  p3 = memory_allocate(5*BLOCK_SIZE+10); used += 6*BLOCK_SIZE;
  TEST(ctxt, memory_used() == used);

  memory_release(p2); used -= 4*BLOCK_SIZE;
  TEST(ctxt, memory_used() == used);
  memory_release(p1); used -= 2*BLOCK_SIZE;
  TEST(ctxt, memory_used() == used);
  memory_release(p3); used -= 6*BLOCK_SIZE;
  TEST(ctxt, memory_used() == used);

  p1 = memory_allocate(1); used += BLOCK_SIZE;
  TEST(ctxt, memory_used() == used);

  print_summary(ctxt);
}

/****************************************************************************/
static void run(void (*test)(void))
{
  if (fork() == 0)
  {
    test();
    exit(0);
  }

  int status;
  while (wait(&status) > 0);
}

/****************************************************************************/
void memory_test(void)
{
  assert(NUMBER_OF_BLOCKS > 2);
  assert(sizeof(heap) == HEAP_SIZE);
  assert((NUMBER_OF_BLOCKS * BLOCK_SIZE) == sizeof(heap));

  initialize_globals();

  printf("Heap information:\n");
  printf("  heap size       : %lu bytes\n", sizeof(heap));
  printf("  block size      : %u bytes\n", BLOCK_SIZE);
  printf("  number of blocks: %d\n", NUMBER_OF_BLOCKS);
  printf("  start address   : %p\n", heap);
  printf("  end address     : %p\n", &(heap[HEAP_SIZE]));

  printf("Test results:\n");

  run(test_list_init);
  run(test_block_is_valid);
  run(test_list_contains);
  run(test_list_get_length);
  run(test_find_block_by_address);
  run(test_blocks_are_contiguous);
  run(test_required_number_of_contiguous_blocks);
  run(test_has_number_of_contiguous_blocks);
  run(test_list_init_block);
  run(test_list_insert_chain);
  run(test_list_remove_chain);
  run(test_memory_initialize);
  run(test_memory_allocate);
  run(test_memory_release);
  run(test_memory_available);
  run(test_memory_used);
  run(test_list_print);
  run(test_list_print_reverse);
}
