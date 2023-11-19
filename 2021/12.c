#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define INPLINES 24
#define MAXNODES 16
#define MAXEDGES  8
#define STARTID   1
#define ENDID     2

typedef struct Node {
    int id, visits, edges, edge[MAXEDGES];
    bool smallcave, beentwice;
} Node;

static Node node[MAXNODES] = {0};
static int nodes = 0, startnode = 0, endnode = 0;
static int inp[INPLINES * 2] = {0};

static int make_id(const char *a)
{
    if (!strcmp(a, "start")) {
        return STARTID;
    }
    if (!strcmp(a, "end")) {
        return ENDID;
    }
    int id = 0;
    const char *c = a;
    while (*c) {
        id = (id << 8) | *c++;
    }
    return id;
}

static void print_id(int id)
{
    if (id == STARTID) {
        printf("start");
    } else if (id == ENDID) {
        printf("end");
    } else {
        char buf[6] = {0};
        int i = 0;
        while (id && i < 6) {
            buf[i++] = (char)(id & 127);
            id >>= 8;
        }
        while (i) {
            printf("%c", buf[--i]);
        }
    }
}

static int find_index(int id)
{
    for (int i = 0; i < nodes; ++i) {
        if (node[i].id == id) {
            return i;
        }
    }
    return -1;
}

static int add_node(char *name)
{
    int id = make_id(name);
    int i = find_index(id);
    if (i != -1) {
            return i;
    }
    if (nodes == MAXNODES) {
        fprintf(stderr, "Too many nodes.");
        return -1;
    }
    i = nodes++;
    node[i].id = id;
    node[i].smallcave = *name >= 'a';
    if (id == STARTID) {
        startnode = i;
    } else if (id == ENDID) {
        endnode = i;
    }
    return i;
}

static bool add_edge(int from, int to)
{
    if (from == -1 || to == -1) {
        return false;
    }
    if (from == endnode || to == startnode) {
        return true;
    }
    for (int i = 0; i < node[from].edges; ++i) {
        if (node[from].edge[i] == to) {
            return true;
        }
    }
    if (node[from].edges == MAXEDGES) {
        fprintf(stderr, "Too many edges.");
        return false;
    }
    node[from].edge[node[from].edges++] = to;
    return true;
}

static void print_graph(void)
{
    for (int i = 0; i < nodes; ++i) {
        print_id(node[i].id);
        if (node[i].edges) {
            printf(" -> ");
            print_id(node[node[i].edge[0]].id);
            for (int j = 1; j < node[i].edges; ++j) {
                printf(",");
                print_id(node[node[i].edge[j]].id);
            }
        }
        printf("\n");
    }
}

static unsigned int find_all_paths(int cur, int maxvisits)
{
    if (cur == endnode) {
        return 1;
    }
    if (++node[cur].visits == maxvisits) {
        node[cur].beentwice = true;
    }
    unsigned int paths = 0;
    for (int i = 0; i < node[cur].edges; ++i) {
        int nxt = node[cur].edge[i];
        if (node[nxt].smallcave == false || (node[nxt].beentwice == false /* && node[nxt].visits < maxvisits */)) {
            paths += find_all_paths(nxt, maxvisits);
        }
    }
    if (--node[cur].visits == 0) {
        node[cur].beentwice = false;
    }
    return paths;
}

int main(int argc, char *argv[])
{
    FILE *f = NULL;
    if (argc == 1) {
        f = fopen("../aocinput/2021-12-input.txt", "r");
    } else {
        switch (*argv[1]) {
            case '1': f = fopen("../aocinput/2021-12-example1.txt", "r"); break;
            case '2': f = fopen("../aocinput/2021-12-example2.txt", "r"); break;
            case '3': f = fopen("../aocinput/2021-12-example3.txt", "r"); break;
            default : f = fopen("../aocinput/2021-12-input.txt"    , "r"); break;
        }
    }

    char buf[2][6];
    int n = 0;
    while (fscanf(f, "%5[A-Za-z]-%5[A-Za-z] ", buf[0], buf[1]) == 2) {
        inp[n++] = add_node(buf[0]);
        inp[n++] = add_node(buf[1]);
    }
    fclose(f);

    for (int i = 0; i < n; i += 2) {
        add_edge(inp[i], inp[i + 1]);
        add_edge(inp[i + 1], inp[i]);
    }
    // print_graph();

    printf("Part 1: %u\n", find_all_paths(startnode, 1));  // Ex1: 10, Ex2: 19, Ex3: 226, Input: 3410
    printf("Part 2: %u\n", find_all_paths(startnode, 2));  // Ex1: 36, Ex2: 103, Ex3: 3509, Input: ?

    return 0;
}
