// Takes a char* and reads 4 bytes from it, turning them into an integer
unsigned int convert_to_uint(const unsigned char* str)
{
    unsigned int result = 0;
    result += str[0] << 24;
    result += str[1] << 16;
    result += str[2] << 8;
    result += str[3];

    return result;
}