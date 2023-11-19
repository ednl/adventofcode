#include <stdio.h>
#include <stdlib.h>  // free

#define ALF 26

static const char *inp = "../aocinput/2020-06-input.txt";
static unsigned int question[ALF], part1, part2;  // static = init to zero

static void count_answers(unsigned int groupsize)
{
    for (unsigned int i = 0; i < ALF; ++i) {
        if (question[i]) {
            ++part1;          // count if any group member said yes
            if (question[i] == groupsize) {
                ++part2;      // count if all group members said yes
            }
            question[i] = 0;  // reset for next group
        }
    }
}

int main(void)
{
	FILE *fp;
	size_t t = 0;
	char *ch, *s = NULL;
    unsigned int groupsize = 0;

	if ((fp = fopen(inp, "r")) != NULL) {
		while (getline(&s, &t, fp) > 0) {
			ch = s;
			while (*ch != '\n' && *ch != '\r' && *ch != '\0') {
                question[*ch++ - 'a']++;  // record single answer
			}
            if (ch == s) {                // empty line = new group
                count_answers(groupsize);
                groupsize = 0;
            } else {                      // count group members
                ++groupsize;
            }
		}
		free(s);
		fclose(fp);
	}
    count_answers(groupsize);  // input does not end in empty line
    printf("%u %u\n", part1, part2);
    return 0;
}
