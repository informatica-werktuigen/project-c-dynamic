#include <stdlib.h>
#include <stdio.h>

#include "memory.h"

int main(int argc, char *argv[])
{
  memory_test(); /* LAAT DEZE LIJN ONGEWIJZIGD */

  /*************************************************************************
   * Hierna volgt voorbeeld code voor het gebruik van de dynamische geheugen 
   * API .
   *************************************************************************/

  /* Initialiseer het dynamische geheugen */
  memory_initialize();

  /* Alloceer voldoende geheugen voor tien gehele getallen van 32 bits */
  uint32_t *ptr_to_int = (uint32_t *) memory_allocate(10 * sizeof(uint32_t));

  /* Controleer op succesvolle toekenning */
  if (ptr_to_int != NULL)
  {
    printf("Pointer naar het toegekend geheugen: %p\n", ptr_to_int);

    /* Vanaf hier kan je met het toegekende geheugen werken */
    ptr_to_int[0] = 10;

    /* Geef het eerder toegekende geheugen weer vrij */
    memory_release(ptr_to_int);
  }
  else
  {
    printf("Toekenning van geheugen mislukt.\n");
  }

  return 0;
}
