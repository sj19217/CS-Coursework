int main()
{
    printf("Hello, world!\n");
    char character;
    unsigned char unsigned_char;
    short short_ = 5;
    unsigned short ushort_;
    int integer;
    unsigned int unsigned_int = -50;
    float f = 5.2;

    character = 7;
    unsigned_char = 6;
    short_ = character + unsigned_char;
    ushort_ = character - unsigned_char;
    integer = character * unsigned_char;
    f = (float) character / (float) unsigned_char;
    unsigned_int = character % unsigned_char;

    short_ = 0;
    short_ = character & unsigned_char;
    short_ = character | unsigned_char;
    short_ = character ^ unsigned_char;
    short_ = ~character;
    short_ = character << 3;
    short_ = unsigned_int >> 2;

    integer = ushort_ * ((short_ >> 2) + unsigned_char);

    char cmp;
    char a = 5, b = 5, c = 6;
    cmp = a == b;
    cmp = a != c;
    cmp = a > c;
    cmp = a < c;
    cmp = a <= b;
    cmp = a >= b;

    if (a && b) {
        printf("a && b\n");
    } else if (a || b) {
        printf("a || b\n;");
    } else {
        if (!1) {
            printf("!1\n");
        }
    }

    for (int i = 0; i < 100; i++) {
        c += i;
    }

    while (i < 0) {
        f = f * 1.01;
        i--;
    }
}