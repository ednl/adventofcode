/*  advent of code day 22 https://adventofcode.com/2021/day/22
    written february 14, 2020 by eric olson
    modified february 15, 2020 for cc65 on the pet
    micro-optimizations february 16, 2020 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DSIZE 51
#define HSIZE 10000

typedef struct {
    int m,n;
    char x[DSIZE];
} hand;

struct {
    int m;
    char x[HSIZE];
} seen;
int hmax=0;

#ifdef PET
#define fopen myfopen
#define fgets myfgets
#define fclose myfclose

char *myday22[] = {
    "Player 1:\n",
    "18\n",
    "19\n",
    "16\n",
    "11\n",
    "47\n",
    "38\n",
    "6\n",
    "27\n",
    "9\n",
    "22\n",
    "15\n",
    "42\n",
    "3\n",
    "4\n",
    "21\n",
    "41\n",
    "14\n",
    "8\n",
    "23\n",
    "30\n",
    "40\n",
    "13\n",
    "35\n",
    "46\n",
    "50\n",
    "\n",
    "Player 2:\n",
    "39\n",
    "1\n",
    "29\n",
    "20\n",
    "45\n",
    "43\n",
    "12\n",
    "2\n",
    "37\n",
    "33\n",
    "49\n",
    "32\n",
    "10\n",
    "26\n",
    "36\n",
    "17\n",
    "34\n",
    "44\n",
    "25\n",
    "28\n",
    "24\n",
    "5\n",
    "48\n",
    "31\n",
    "7\n",
    0 };

int myline=0;
FILE *myfopen(char *fn,char *mode){
    myline=0;
    return (FILE*)1;
}
char *fgets(char *s,int n,FILE *fp){
    if(myday22[myline]){
        strncpy(s,myday22[myline],n);
        myline++;
        return s;
    }
    return 0;
}
int myfclose(FILE *fp){
    return 0;
}

long tstart;
void tic(){
    tstart=clock();
}
void toc(){
    long tstop=clock();
    long tsec=tstop-tstart;
    long tcen=10000l*(tsec%CLOCKS_PER_SEC);
    tsec/=CLOCKS_PER_SEC;
    tcen=(tcen+CLOCKS_PER_SEC/2)/CLOCKS_PER_SEC;
    printf("\nTotal execution time %lu.%04lu seconds.\n",tsec,tcen);
}

#else

static struct timespec tic_start;
void tic() {
    clock_gettime(CLOCK_MONOTONIC_RAW,&tic_start);
}
void toc() {
    struct timespec tic_stop;
    clock_gettime(CLOCK_MONOTONIC_RAW,&tic_stop);
    double sec=tic_stop.tv_sec-tic_start.tv_sec;
    double r=sec+(tic_stop.tv_nsec-tic_start.tv_nsec)*1.0e-9;
    printf("\nTotal execution time %g seconds.\n",r);
}

#endif

void doinit(hand *cards) {
    int p=0,l=0,n;
    FILE *fp=fopen("day22.dat","r");
    if(fp==0){
        printf("Error opening day22.dat for import!\n");
        exit(1);
    }
    for(n=0;n<DSIZE;){
        char buf[128];
        if(!fgets(buf,sizeof(buf),fp)){
            if(n>0){
                cards[p].m=0; cards[p].n=n;
            }
            break;
        }
        l+=1;
        if(buf[0]=='\n'){
            continue;
        } else if(buf[0]=='P'){
            if(n>0){
                cards[p].m=0; cards[p].n=n;
                p+=1; n=0;
            }
        } else {
            int x=atoi(buf);
            cards[p].x[n]=x;
            if(!x) {
                printf(
                    "Error converting %s to a number in line %d!\n",
                    buf,l); exit(1);
            }
            n+=1;
        }
    }
    fclose(fp);
}

int getcard(hand *cards,int p){
    int j=cards[p].m;
    if(j!=cards[p].n) {
        cards[p].m=(j+1)%DSIZE;
        return cards[p].x[j];
    }
    return 0;
}

void putcard(hand *cards,int p,int q){
    int j=cards[p].n;
    cards[p].x[j]=q;
    cards[p].n=(j+1)%DSIZE;
}

int countcards(hand *h){
    int r=(*h).n-(*h).m;
    if(r<0) { r+=DSIZE; }
    return r;
}

char *gethash(hand *cards){
    hand *h;
    char *r;
    int c[2],nmax,p,m,n;
    for(p=0;p<2;p++) c[p]=countcards(&cards[p]);
    if(c[0]>c[1]) p=1;
    else p=0;
    h=&cards[p]; nmax=c[p];
    r=seen.x+seen.m;
    for((m=(*h).m),n=0;n<nmax;(m=(m+1)%DSIZE),n+=1){
        r[n]=(*h).x[m];
    }
    r[nmax+1]=0;
    seen.m+=nmax+1;
    if(hmax<seen.m) hmax=seen.m;
    return r;
}

int findhash(int m,char *r){
    char *s=seen.x+m;
    while(s<r){
        int f=1,n;
        for(n=0;s[n];n++){
            if(s[n]!=r[n]) f=0;
        }
        if(s[n]!=r[n]) f=0;
        if(f) return f;
        s+=n+1;
    }
    return 0;
}

int shortcut(hand *card){
    int cmax=0,p;
    for(p=0;p<2;p++){
        int n;
        for(n=card[p].m;n!=card[p].n;n=(n+1)%DSIZE){
            if(cmax<card[p].x[n]) {
                if(p) return 0;
                cmax=card[p].x[n];
            }
        }
    }
    if(cmax>countcards(&card[0])+countcards(&card[1])-2) return 1;
    return 0;
}

extern int dogame(hand *cards,int f);
extern int dogame2(hand *cards,int f);
int turn(hand *cards,int sp,int f) {
    int p,c[2];
    char *hash;
    for(p=0;p<2;p++){
        if(countcards(&cards[p])==0) {
            return (p+1)%2;
        }
    }
    hash=gethash(cards);
    if(findhash(sp,hash)){
        return 0;
    }
    for(p=0;p<2;p++) { c[p]=getcard(cards,p); }
    if(f&&c[0]<=countcards(&cards[0])&& 
        c[1]<=countcards(&cards[1])){
        hand save[2];
        memcpy(save,cards,sizeof(save)); 
        for(p=0;p<2;p++){
            cards[p].n=(cards[p].m+c[p])%DSIZE;
        }
        p=dogame2(cards,f);
        memcpy(cards,save,sizeof(save));
        putcard(cards,p,c[p]); putcard(cards,p,c[(p+1)%2]);
    } else if(c[0]>c[1]){
        putcard(cards,0,c[0]); putcard(cards,0,c[1]);
    } else {
        putcard(cards,1,c[1]); putcard(cards,1,c[0]);
    }
    return 2;
}

int turn2(hand *cards,int f) {
    int p,c[2];
    for(p=0;p<2;p++){
        if(countcards(&cards[p])==0) {
            return (p+1)%2;
        }
    }
    for(p=0;p<2;p++) { c[p]=getcard(cards,p); }
    if(f&&c[0]<=countcards(&cards[0])&& 
        c[1]<=countcards(&cards[1])){
        hand save[2];
        memcpy(save,cards,sizeof(save)); 
        for(p=0;p<2;p++){
            cards[p].n=(cards[p].m+c[p])%DSIZE;
        }
        p=dogame2(cards,f);
        memcpy(cards,save,sizeof(save));
        putcard(cards,p,c[p]); putcard(cards,p,c[(p+1)%2]);
    } else if(c[0]>c[1]){
        putcard(cards,0,c[0]); putcard(cards,0,c[1]);
    } else {
        putcard(cards,1,c[1]); putcard(cards,1,c[0]);
    }
    return 2;
}

int equalcards(hand *h,hand *h2) {
    int m,n;
    if(countcards(h)!=countcards(h2)) return 0;
    for((m=(*h).m),n=(*h2).m; n!=(*h2).n;
        (m=(m+1)%DSIZE),n=(n+1)%DSIZE){
        if((*h).x[m]!=(*h2).x[n]) return 0;
    }
    return 1;
}

int dogame2(hand *cards,int f){
    int sp=seen.m;
    hand loop[2];
    if(shortcut(cards)) return 0;
    memcpy(loop,cards,sizeof(loop));
    for(;;){
        int p=turn2(cards,f);
        if(p<2){
            seen.m=sp;
            return p;
        }
        p=turn2(loop,f);
        p=turn2(cards,f);
        if(p<2){
            seen.m=sp;
            return p;
        }
        if(equalcards(&loop[0],cards)){
            return 0;
        }
    }
}

int dogame(hand *cards,int f){
    int sp=seen.m;
    for(;;){
        int p=turn(cards,sp,f);
        if(p<2){
            seen.m=sp;
            return p;
        }
    }
}

long doscore(hand *cards,int p){
    int w=1,n;
    long r=0;
    for(n=cards[p].n;n!=cards[p].m;){
        n=(n+DSIZE-1)%DSIZE;
        r+=cards[p].x[n]*w;
        w+=1;
    }
    return r;
}

void part1(hand *cards){
    int p=dogame(cards,0);
    long r=doscore(cards,p);
    printf("Part 1 Player %d won with score %ld\n",p+1,r);
}

void part2(hand *cards){
    int p=dogame(cards,1);
    long r=doscore(cards,p);
    printf("Part 2 Player %d won with score %ld\n",p+1,r);
}

int main() {
    hand cards[2],start[2];
    tic();
    printf("Advent of Code Day 22\n\n");
    doinit(cards);
    memcpy(start,cards,sizeof(start));
    part1(cards);
    memcpy(cards,start,sizeof(cards));
    seen.m=0;
    part2(cards);
    toc();
    printf("Used %d bytes of %d to detect infinite loops.\n",hmax,HSIZE);
    exit(0);
}
