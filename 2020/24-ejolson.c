/*  advent of code day 24 https://adventofcode.com/2021/day/24
    written March 2, 2021 by eric olson */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef struct { uchar x,y; } coord;
static const int maxbit=16;
static const int maxint=(int)((~(unsigned int)1)/2);
static char dirs[]="nenwwswsee";
static ushort state[256][16];
static ushort neigh[6][256];

static long tstart;
static void tic(){
    tstart=clock();
}
static void toc(){
    long tstop=clock();
    long tsec=tstop-tstart;
    long tcen=10000l*(tsec%CLOCKS_PER_SEC);
    tsec/=CLOCKS_PER_SEC;
    tcen=(tcen+CLOCKS_PER_SEC/2)/CLOCKS_PER_SEC;
    printf("\nTotal execution time %lu.%04lu seconds.\n",tsec,tcen);
}

static void syntax(char *s){
    printf("Unknown direction starting in %s\n",s);
    exit(1);
}
static int move(char *s, coord *p){
    int r=1;
    if(s[0]=='n'){
        r++; if(s[1]=='e'){ (*p).y++;
        } else if(s[1]=='w'){ (*p).y++; (*p).x--;
        } else { syntax(s); }
    } else if(s[0]=='s'){
        r++; if(s[1]=='e'){ (*p).y--; (*p).x++;
        } else if(s[1]=='w'){ (*p).y--;
        } else { syntax(s); }
    } else if(s[0]=='e'){ (*p).x++;
    } else if(s[0]=='w'){ (*p).x--;
    } else { syntax(s); }
    return r;
}

static int bitget(coord *p){
    int i=(*p).y/maxbit;
    int j=(*p).y%maxbit;
    return (state[(*p).x][i]&(1<<j))>>j;
}
static void bitset(coord *p){
    int i=(*p).y/maxbit;
    int j=(*p).y%maxbit;
    state[(*p).x][i]|=1<<j;
}
static void bitclr(coord *p){
    int i=(*p).y/maxbit;
    int j=(*p).y%maxbit;
    state[(*p).x][i]&=~(1<<j);
}
static void bitflip(coord p){
    int r=bitget(&p);
    if(r) bitclr(&p);
    else bitset(&p);
}

static void update(int h){
    int x=h&3;
    coord p; p.x=h; p.y=0;
    do {
        int c=neigh[x][p.y];
        if(c>2||c==0){
            bitclr(&p);
        } else if(c==2){
            bitset(&p);
        }
        neigh[x][p.y]=0;
        p.y++;
    } while(p.y);
}
static void dostep(){
    int x,y,g,h;
    for(x=0;x<4;x++){
        for(y=0;y<256;y++){
            neigh[x][y]=0;
        }
    }
    for(h=0;h<256;h++){
        if(h==254){
            for(y=0;y<256;y++){
                neigh[3][y]=neigh[4][y];
            }
        } else if(h==255){
            for(y=0;y<256;y++){
                neigh[0][y]=neigh[5][y];
            }
        }
        for(g=0;g<16;g++){
            int i,j;
            ushort z=state[h&255][g];
            if(!z) continue;
            for(j=0;j<16;j++){
                if(z&(1<<j)){
                    coord p; p.x=h; p.y=g*16+j;
                    for(i=0;dirs[i];){
                        coord q; q=p;
                        i+=move(&dirs[i],&q);
                        neigh[q.x&3][q.y]++;
                    }
                }
            }
        }
        if(h==0){
            for(y=0;y<256;y++){
                neigh[4][y]=neigh[3][y];
                neigh[3][y]=0;
            }
        } else if(h==1){
            for(y=0;y<256;y++){
                neigh[5][y]=neigh[0][y];
                neigh[0][y]=0;
            }
        } else if(h>=2){
            update(h-1);
        }
    }
    update(255);
    update(256);
}

static coord decode(char *s){
    int i;
    coord p={0,0};
    for(i=0;s[i]&&s[i]!='\n';i+=move(&s[i],&p));
    return p;
}
static void doinit(){
    FILE *fp;
    fp=fopen("../aocinput/2020-24-input.txt","r");
    if(!fp){
        printf("Error opening input24.txt for import!\n");
        exit(1);
    }
    for(;;){
        char buf[128];
        char *s=fgets(buf,sizeof(buf),fp);
        if(!s) break;
        if(!buf[0]) continue;
        else bitflip(decode(buf));
    }
    fclose(fp);
}

static int count(){
    int r=0,h,i;
    for(h=0;h<256;h++){
        for(i=0;i<16;i++){
            ushort n=state[h][i];
            while(n){
                n&=(n-1);
                r++;
            }
        }
    }
    return r;
}

static void part1() {
    printf("Part 1 Total of %d tiles are black\n",count());
}

static void part2() {
    int t;
    for(t=1;t<=100;t++) dostep();
    printf("Part 2 Total of %d tiles are black\n",count());
}

int main() {
    tic();
    printf("Advent of Code Day 24\n\n");
    doinit();
    part1();
    part2();
    toc();
    return 0;
}
