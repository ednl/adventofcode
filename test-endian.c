#include <stdio.h>

int main(void)
{
    unsigned short data = 0x0123;
    unsigned char c;

    FILE *f = tmpfile();
    if (!f) {
        perror("Could not create file.");
        return 1;
    }
    fwrite(&data, sizeof data, 1, f);
    printf("Data written    : 0x%04x\n", data);

    rewind(f);
    if (fread(&c, 1, 1, f) == 1) {
        printf("First byte read : 0x%02x\nEndianness      : ", c);
        if (c == (data & 0x00ff))
            puts("little-endian (starts at the little end)");
        else if (c == ((data & 0xff00) >> 8))
            puts("big-endian (starts at the big end)");
        else
            puts("unknown (corrupt data)");
    } else {
        perror("Could not read from file.");
        return 2;
    }

    return 0;
}
