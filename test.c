#define TEST(expr) if (!(expr)) { fail(__FILE__, __LINE__, #expr); }

static void fail(char *file, int line, char *cond)
{
  fprintf(stderr, "%s:%d: Test '%s' failed\n", file, line, cond);
}

static void test_list_init(void)
{
  struct list list;

  list_init(&list);

  TEST(list.first == NULL);
  TEST(list.last == NULL);
}

void memory_test(void)
{
  assert(sizeof(heap) == HEAP_SIZE);

  printf("Heap information:\n");
  printf("  heap size       : %lu bytes\n", sizeof(heap));
  printf("  block size      : %u bytes\n", BLOCK_SIZE);
  printf("  number of blocks: %d\n", NUMBER_OF_BLOCKS);
  printf("  start address   : %p\n", heap);
  printf("  end address     : %p\n", &(heap[HEAP_SIZE]));

  assert(NUMBER_OF_BLOCKS > 2);
  assert((NUMBER_OF_BLOCKS * BLOCK_SIZE) == sizeof(heap));

  test_list_init();
}
