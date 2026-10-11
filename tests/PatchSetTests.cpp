// DK2VR 0.8.62 - vtable slot patch logic: install, call-through original, repair after the runtime rewrote the slot.
#include "../src/plugin/PatchSet.hpp"
#include <cassert>
#include <cstdio>
static void f0() {} static void f1() {} static void f2() {} static void hookA() {} static void hookB() {} static void foreign() {}
struct Obj {void** vtable;};
int main() {
    void* vtA[8]={nullptr,nullptr,nullptr,nullptr,nullptr,(void*)&f0,(void*)&f1,(void*)&f2};
    Obj a{vtA};
    PatchSet slot5,slot6;
    // install keeps the original per vtable and puts the hook in
    assert(slot5.install(&a,5,(void*)&hookA)&&vtA[5]==(void*)&hookA&&slot5.original(&a)==(void*)&f0);
    assert(slot6.install(&a,6,(void*)&hookB)&&vtA[6]==(void*)&hookB&&slot6.original(&a)==(void*)&f1);
    assert(slot5.install(&a,5,(void*)&hookA)&&slot5.count==1); // second install on the same table: nothing new
    // nothing rewrote the slot: refresh does nothing
    assert(!slot5.refresh(&a,5,(void*)&hookA));
    // the runtime refills the table (what creating the second device did in the 0.8.61 report)
    vtA[5]=(void*)&f0;vtA[6]=(void*)&f1;
    void* prev=nullptr;
    assert(slot5.refresh(&a,5,(void*)&hookA,&prev)&&prev==(void*)&f0&&vtA[5]==(void*)&hookA&&slot5.original(&a)==(void*)&f0);
    assert(slot6.refresh(&a,6,(void*)&hookB)&&vtA[6]==(void*)&hookB&&slot6.original(&a)==(void*)&f1);
    // somebody else put its own function there: it becomes the function to call through
    vtA[5]=(void*)&foreign;
    assert(slot5.refresh(&a,5,(void*)&hookA,&prev)&&prev==(void*)&foreign&&vtA[5]==(void*)&hookA&&slot5.original(&a)==(void*)&foreign);
    // a table that was never patched is not touched by refresh
    void* vtB[8]={nullptr,nullptr,nullptr,nullptr,nullptr,(void*)&f0,(void*)&f1,(void*)&f2};Obj b{vtB};
    assert(!slot5.refresh(&b,5,(void*)&hookA)&&vtB[5]==(void*)&f0&&slot5.original(&b)==nullptr);
    // ... and install handles it as a second table
    assert(slot5.install(&b,5,(void*)&hookA)&&slot5.count==2&&slot5.original(&b)==(void*)&f0&&vtB[5]==(void*)&hookA);
    // limit of 16 tables
    PatchSet many;void* tables[20][8];Obj objs[20];
    for(int i=0;i<20;++i) {for(int k=0;k<8;++k) tables[i][k]=(void*)&f0;objs[i].vtable=tables[i];}
    int ok=0;for(int i=0;i<20;++i) ok+=many.install(&objs[i],3,(void*)&hookA)?1:0;
    assert(ok==16);
    puts("PASS: PatchSet install, original, repair after a runtime rewrite, foreign hooks, second table, limit.");
}
