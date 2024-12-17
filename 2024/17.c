#include <stdio.h>
#include <stdint.h>

static int fwd(int A)
{
    int B = 0, C = 0, res = 0;
    do {
        B = A & 7;   // 1: take 3LSB of A
        B ^= 2;      // 2: -> 2,3,0,1,6,7,4,5
        C = A >> B;  // 3: shift A by that, store in C
        B ^= C;      // 4: flip bits B in A shifted by B, store in B
        B ^= 3;      // flip 2LSB of B
        putchar("01234567"[B & 7]); putchar(A ? ',' : '\n');  // output 3LSB of B
    } while (A >>= 3);
    return res;
}

int main(void)
{
    // Part 1
    int A = 35200350, B = 0, C = 0;  // 10 000 110 010 001 110 101 011 110

    // Part 2
    // prog: 2,4,1,2,7,5,4,7,1,3,5,5,0,3,3,0
    // rev : 0,3,3,0,5,5,3,1,7,4,5,7,2,1,4,2
    // bin : 000 011 011 000 101 101 011 001 111 100 101 111 010 001 100 010
    //     | 010 100 001 010 111 101 100 111 001 011 101 101 000 011 011 000
    // 001 111 010 001 100 110 111 100 010 000 110 110 011 000 000 011
    return 0;
}
