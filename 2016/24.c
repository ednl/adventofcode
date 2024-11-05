#include <stdio.h>    // fopen, fclose, printf, putchar, puts
#include <string.h>   // memset, strcpy
#include <stdbool.h>  // bool

#define W 179      // width
#define H 43       // height
#define N 8        // points of interest 0..7
#define FAC7 5040  // 7! = 5040
#define QSIZE 100  // for my input: max=69 (nice)

typedef struct {
    int x, y;
} vec2_t;

typedef struct {
    vec2_t pos;
    int dist;
} state_t;

typedef struct {
    size_t len, pop, ins;
    state_t q[QSIZE];
} queue_t;

const static vec2_t dir[4] = {{-1,0},{1,0},{0,-1},{0,1}};
static queue_t queue;
static int grid[H][W];
static bool seen[H][W];
static vec2_t poi[N];       // position of POI 0..7
static int dist[N][N];      // distance between POI i and j
static char perm[FAC7][N];  // all permutations of "1234567" (always start at 0)

// Dequeue = pop off the tail of the queue
static bool deq(queue_t * const q, state_t * const val)
{
    if (!q || !q->len)
        return false;
    q->len--;
    *val = q->q[q->pop++];
    if (q->pop == QSIZE)
        q->pop = 0;
    return true;
}

// Enqueue = push onto the head of the queue
static bool enq(queue_t * const q, const state_t val)
{
    if (!q || q->len == QSIZE)
        return false;
    q->len++;
    q->q[q->ins++] = val;
    if (q->ins == QSIZE)
        q->ins = 0;
    return true;
}

static vec2_t add(const vec2_t a, const vec2_t b)
{
    return (vec2_t){a.x + b.x, a.y + b.y};
}

static void flood(const int start)
{
    memset(seen, 0, sizeof seen);
    seen[poi[start].y][poi[start].x] = true;
    state_t s = {poi[start], 0};
    enq(&queue, s);
    while (deq(&queue, &s)) {
        int c = grid[s.pos.y][s.pos.x];
        if (c >= '0' && c < '0' + N)
            dist[start][c - '0'] = dist[c - '0'][start] = s.dist;
        for (int i = 0; i < (int)(sizeof dir / sizeof *dir); ++i) {
            vec2_t next = add(s.pos, dir[i]);
            if (grid[next.y][next.x] != '#' && !seen[next.y][next.x]) {
                seen[next.y][next.x] = true;
                if (!enq(&queue, (state_t){next, s.dist + 1}))
                    puts("***** QUEUE OVERFLOW *****");
            }
        }
    }
}

static void swap(char *a, char *b)
{
    char t = *a;
    *a = *b;
    *b = t;
}

static void permute(char *s, int l, int r)
{
    static int count = 0;

    if (l == r)
        strcpy(perm[count++], s);
    else
        for (int i = l; i <= r; ++i) {
            swap(s + l, s + i);
            permute(s, l + 1, r);
            swap(s + l, s + i);  // backtrack
        }
}

static void showdist(void)
{
    printf("\n           |");
    for (int i = 0; i < N; ++i)
        printf("%4d", i);
    putchar('\n');
    puts("-----------+--------------------------------");
    for (int i = 0; i < N; ++i) {
        printf("%d (%3d,%2d) |", i, poi[i].x, poi[i].y);
        for (int j = 0; j < N; ++j)
            printf("%4d", dist[i][j]);
        putchar('\n');
    }
    putchar('\n');
}

static void mindist(const int part)
{
    int sum, min = W * H, ix = 0;
    for (int i = 0; i < FAC7; ++i) {
        sum = part == 2 ? dist[0][(int)perm[i][0]] : 0;  // part 2: return to '0'
        for (int j = 0; j < N - 1; ++j)
            sum += dist[(int)perm[i][j]][(int)perm[i][j + 1]];  // perm[i][7] = '\0'
        if (sum < min) {
            min = sum;
            ix = i;
        }
    }
    char s[8] = {0};
    strcpy(s, perm[ix]);
    for (int i = 0; i < 7; ++i)
        s[i] += '0';
    int i = 0, j = N - 2;
    while (i < j)
        swap(s + i++, s + j--);
    printf("Part %d: \"0%s%s\" = %d\n", part, s, part == 2 ? "0" : "", min);  // 490, 744
}

int main(void)
{
    FILE *f = fopen("../aocinput/2016-24-input.txt", "r");
    if (!f)
        return 1;
    int c, x = 0, y = 0;
    while (y < H && (c = fgetc(f)) != EOF) {
        if (c == '\n') {
            x = 0;
            ++y;
        } else {
            if (c >= '0' && c < '0' + N)
                poi[c - '0'] = (vec2_t){x, y};
            if (x < W)
                grid[y][x++] = (char)c;
        }
    }
    fclose(f);

    for (int i = 0; i < N; ++i)
        flood(i);
    showdist();

    char s[] = "1234567";
    for (int i = 0; i < N - 1; ++i)
        s[i] -= '0';
    permute(s, 0, N - 2);

    mindist(1);
    mindist(2);
    return 0;
}
