/*  advent of code day 25 https://adventofcode.com/2020/day/10
    written march 5, 2021 by eric olson using the kangaroo method */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define M 20201227

#ifdef PET

typedef unsigned long ui32;
typedef unsigned char ui8;

#define fopen myfopen
#define fgets myfgets
#define fclose myfclose
static char *myday25[] = {
    "6270530\n",
    "14540258\n",
    0 };

static int myline=0;
static FILE *myfopen(char *,char *){
    myline=0;
    return (FILE *)1;
}
static char *myfgets(char *s,int n,FILE *){
    if(myday25[myline]){
        strncpy(s,myday25[myline],n);
        myline++;
        return s;
    }
    return 0;
}
static int myfclose(FILE *){
    return 0;
}

static ui32 tstart;
static void tic(){
    tstart=clock();
}
static void toc(){
    ui32 tstop=clock();
    ui32 tsec=tstop-tstart;
    ui32 tcen=10000ul*(tsec%CLOCKS_PER_SEC);
    tsec/=CLOCKS_PER_SEC;
    tcen=(tcen+CLOCKS_PER_SEC/2)/CLOCKS_PER_SEC;
    printf("\nTotal execution time %lu.%04lu seconds.\n",tsec,tcen);
}

static ui32 r[13];
/*  this 32-bit modmul is seven times slower than slow */
static ui32 modmul(ui32 x,ui32 y){
    int i,j;
    ui8 p[7],q[7];
    ui32 z;
    for(i=0;i<7;i++){
        p[i]=x&15; x>>=4;
        q[i]=y&15; y>>=4;
    }
    z=0;
    for(i=0;i<7;i++){
        ui32 t=0;
        for(j=0;j<7;j++){
            t+=q[j]*r[i+j];
        }
        z+=p[i]*(t%M);
    }
    return z%M;
}

#else

typedef unsigned int ui32;
typedef unsigned char ui8;

static struct timespec tic_start;
static void tic(){
    clock_gettime(CLOCK_MONOTONIC_RAW,&tic_start);
}
static void toc(){
    struct timespec tic_stop;
    clock_gettime(CLOCK_MONOTONIC_RAW,&tic_stop);
    double sec=tic_stop.tv_sec-tic_start.tv_sec;
    double r=sec+(tic_stop.tv_nsec-tic_start.tv_nsec)*1.0e-9;
    printf("\nTotal execution time %g seconds.\n",r);
}

static ui32 r[13];
static ui32 modmul(ui32 x,ui32 y){
    return ((unsigned long long)x*y)%M;
}

#endif

static ui32 sqrtM,shake[2];

static ui32 ui32sqr(ui32 x){
    ui32 a=0,b=65536;
    for(;;){
        ui32 c=(a+b)/2;
        if(c*c>=x) b=c;
        else a=c;
        if(b-a<=1) return b;
    }
}

static void doinit(){
    FILE *fp;
    int i;
    r[0]=1;
    for(i=1;i<13;i++){
        r[i]=(16*r[i-1])%M;
    }
    sqrtM=ui32sqr(M);
    /*
    fp=fopen("day25.dat","r");
    if(fp==0){
        printf("Error opening day25.dat for import!\n");
        exit(1);
    }
    for(i=0;i<2;i++){
        char buf[128];
        if(!fgets(buf,sizeof(buf),fp)){
            printf("Error reading day25.dat at line %d!\n",i);
            exit(1);
        }
        shake[i]=atol(buf);
    }
    fclose(fp);
    */

    // ejolson
    // shake[0] = 6270530UL;
    // shake[1] = 14540258UL;

    // ednl
    // shake[0] = 15113849UL;
    // shake[1] = 4206373UL;

    // lurk101
    shake[0] = 16915772UL;
    shake[1] = 18447943UL;
}

static ui32 modpow(ui32 p,ui32 n){
    ui32 r;
    for(r=1;n!=0;p=modmul(p,p)){
        if((n&1)!=0) r=modmul(r,p);
        n>>=1;
    }
    return r;
}

static ui32 rooparm[2]={ 3659326121ul, 3048033998ul };
static void rooseed(){
    int k;
    for(k=0;k<2;k++){
        ui32 s;
        while((s=rand())<1024);
        while((s&1)!=1) s>>=1;
        rooparm[k]=s;
    }
}
static ui32 roojump(ui32 x){
    x=x*(x+rooparm[1])+rooparm[0];
    return ((x>>16)|(x<<16))%sqrtM+1;
}

static ui32 xi,yi,d,dn,alpha,beta;
static void rootame(){
    int i;
    xi=1;
    d=0;
    for(i=0;i<sqrtM;i++){
        ui32 fi=roojump(xi);
        d+=fi;
        xi=modmul(xi,modpow(alpha,fi));
    }
}
static ui32 roowild(){
    yi=beta;
    dn=0;
    for(;;){
        ui32 fi;
        if(yi==xi) return 1;
        fi=roojump(yi);
        dn+=fi;
        yi=modmul(yi,modpow(alpha,fi));
        if(dn>=d+M) return 0;
    }
}
static ui32 modlog(ui32 base,ui32 y){
    int k;
    alpha=base;
    beta=y;
    for(k=0;k<sqrtM;k++){
        rootame();
        if(roowild()){
            return (M-1+d-dn)%(M-1);
        }
        rooseed();
    }
    return 0;
}

static void part1(){
    ui32 x=modlog(7,shake[0]);
    ui32 key=modpow(shake[1],x);
    printf("Part 1 The encryption key is %lu\n",(unsigned long)key);
}

int main(){
    tic();
    printf("Advent of Code Day 25\n\n");
    doinit();
    part1();
    toc();
    return 0;
}
