#include "CubeFaces.hpp"
#include <cstdio>
#include <cstdlib>
int main(int argc,char** argv){ // 9 floats of A0 (engine layout) + rect l r t b + guard -> prints valid, then for each face A (9) Q (9) N (12), then focal and rect
    if(argc<15) return 2; float A0[9];for(int i=0;i<9;++i) A0[i]=strtof(argv[1+i],0);
    float rect[4];for(int i=0;i<4;++i) rect[i]=strtof(argv[10+i],0);float guard=strtof(argv[14],0);
    dk2vr::CubeFaces c;bool ok=dk2vr::buildCubeFaces(A0,c);printf("%d\n",ok?1:0);
    for(int k=0;k<6;++k){for(int i=0;i<9;++i)printf("%.9g ",c.A[k][i]);for(int i=0;i<9;++i)printf("%.9g ",c.Q[k][i]);for(int p=0;p<4;++p)for(int r=0;r<3;++r)printf("%.9g ",c.N[k][p][r]);printf("\n");}
    float f,o[4];bool lo=dk2vr::cubeFaceLens(rect,guard,f,o);printf("%d %.9g %.9g %.9g %.9g %.9g\n",lo?1:0,f,o[0],o[1],o[2],o[3]);
}
