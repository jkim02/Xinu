
#include <thread.h>
#include <printprocstks.h>

int printprogZ() {
 return printprocZ();
  }

int printprocZ() {
return 13;
}

void printprocstks(){
int thr1, thr2, thr3;

thr1 = create(printprocZ, 1024, 20, "EVIL", 0);
thr2 = create(printprocZ, 1024, 20, "NEVER", 0);
thr3 = create(printprocZ, 1024, 20, "DIES", 0);

printf("Proc [%s]. Tid = %d.\n     Stack: Base = %d\n            Len = %d\n            Limit = %d\n            StkPtr = %d\n\n", thrtab[thr1].name, thr1, thrtab[thr1].stkbase, thrtab[thr1].stklen, thrtab[thr1].stkbase - thrtab[thr1].stklen, thrtab[thr1].stkptr); 

printf("Proc [%s]. Tid = %d.\n     Stack: Base = %d\n            Len = %d\n            Limit = %d\n            StkPtr = %d\n\n", thrtab[thr2].name, thr2, thrtab[thr2].stkbase, thrtab[thr2].stklen, thrtab[thr2].stkbase - thrtab[thr2].stklen, thrtab[thr2].stkptr); 


printf("Proc [%s]. Tid = %d.\n     Stack: Base = %d\n            Len = %d\n            Limit = %d\n            StkPtr = %d\n\n", thrtab[thr3].name, thr3, thrtab[thr3].stkbase, thrtab[thr3].stklen, thrtab[thr3].stkbase - thrtab[thr3].stklen, thrtab[thr3].stkptr); 

resume(thr1);
resume(thr2);
resume(thr3);

return;
}

