#include "first.h"
#include<stdio.h>


int main() {
  int x,y ; /* Allocation des entrées */
  First__myfun_out o ; /* Allocation des sorties */
  for(;;) { /* Boucle infinie */
    printf("Inputs:"); scanf("%d%d",&x,&y) ; /* Lecture des entrées */
    First__myfun_step(x,y,&o) ; /* Calculs */
    printf("Result: z=%d t=%d\n",o.z,o.t) ; /* Ecriture des sorties */
  }
}
