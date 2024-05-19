#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

const unsigned char KEY[] = {0x3e, 0x71, 0x90, 0x27, 0xe1, 0x7b, 0x1f, 0x1d, 0xd5, 0x05, 0xf9, 0x12, 0x88, 0xe7, 0x89, 0xca};
const unsigned char SHUFFLE[] = {0x02, 0x06, 0x07, 0x01, 0x05, 0x0b, 0x09, 0x0e, 0x03, 0x0f, 0x04, 0x08, 0x0a, 0x0c, 0x0d, 0x00};
const unsigned char AND[] = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xe1, 0xde, 0xad, 0x13, 0x37, 0x13, 0x37, 0x67, 0x63, 0x74, 0x66};
const unsigned char XOR[] = {0x76, 0x58, 0xb4, 0x49, 0x8d, 0x1a, 0x5f, 0x38, 0xd4, 0x23, 0xf8, 0x34, 0xeb, 0x86, 0xf9, 0xaa};

const int KEY_AMOUNT = 100;

void nire_shuffle(unsigned char *buffer)
{
    unsigned char lag[28];
    int i;

    for (i = 0; i < 16; i++) {
        lag[i] = buffer[i];
    }
    for (i = 0; i < 16; i++) {
        buffer[i] = lag[(int)SHUFFLE[i]];
    }
}

void reverse_shuffle(unsigned char *buffer)
{
    unsigned char lag[28];
    int i;

    for (i = 0; i < 16; i++) {
        lag[i] = buffer[i];
    }
    for (i = 0; i < 16; i++) {
        buffer[(int)SHUFFLE[i]] = lag[i];
    }
}


void nire_xor(unsigned char *buffer)
{
    int i;

    for (i = 0; i < 16; i++) {
        buffer[i] = buffer[i] ^ XOR[i];
    }
}

void nire_and(unsigned char *buffer)
{
    int i;

    for (i = 0; i < 16; i++) {
        buffer[i] = buffer[i] & AND[i];
    }
}


void convert_to_binary_string(const unsigned char *buffer, int len, unsigned char *binaryBuffer) {
    for (int i = 0; i < len; i++) {
        unsigned char value = buffer[i];
        for (int j = 0; j < 8; j++) {
            int bit = (value >> (7 - j)) & 1; // Extract each bit from the value
            binaryBuffer[i * 8 + j] = (bit == 1) ? '1' : '0';
        }
    }
    binaryBuffer[len * 8] = '\0'; // Null-terminate the string
}

void binary_to_char(const unsigned char *binaryBuffer, int len, unsigned char *buffer) 
{
    int i; 
    for(i=0; i< len; i++)
    {
        unsigned char value = 0;
        for(int j=0; j<8; j++)
        {
            if(binaryBuffer[i*8 + j]=='1')
            {
                value += (int)pow(2, 7-j);
            }
        }
        buffer[i] = value;
    }
}

// kontatu zerrendako zeroak
int count_zeros(unsigned char *buffer, int len)
{
    int i;
    int kop=0;

    for(i=0; i<len; i++)
        if(buffer[i] == '0') kop ++;

    return kop;
}

// ahal den neurrian lortu AND eragiketaren alderantzizkoa
void partial_reverse_and(unsigned char *buffer, unsigned char *binary_AND, int len)
{
    int i=0;

    for(i=0; i<len; i++)
    {
        if(binary_AND[i] == '1' && buffer[i] == '0') buffer[i] = '0';
        else if(binary_AND[i] == '1' && buffer[i] == '1') buffer[i] = '1';
    }
}

int main(int argc, unsigned char argv[])
{
    unsigned char generated_key[128];
    unsigned char reversed_key[16];
    unsigned char reversed_key_lag[16];
    unsigned char reversed_key_binary[128];
    int len = 128;
    unsigned char keyXored[16];
    int and_zero_kop;
    unsigned char binary_AND[128];
    unsigned char binary_XOR[128];

    // pasa datu orokorrak bitarrera
    convert_to_binary_string(AND, 16, binary_AND);
    convert_to_binary_string(XOR, 16, binary_XOR);

    // lortu gako originaletik posible dena
    memcpy(reversed_key, KEY, 16);
    nire_xor(reversed_key);
    convert_to_binary_string(reversed_key, 16, reversed_key_binary);


    // and osoa ezin da alderantzizkatu, baina behar diren datuak lortu daitezke:
    partial_reverse_and(reversed_key_binary, binary_AND, 128);

    // orain KEY gakoari dagokion atal bat hasieratuta dugu (shuffle ostean lortu litzatekeena). Lortu ez den atala edozein izan daiteke, binary_AND-eko hori 0 denez
    // emaitza 0 izango baita

    // lortu aldatu daitezkeen indizeak
    and_zero_kop = 0;

    // pasatu AND bitarrera
    int zero_kop = count_zeros(binary_AND, 128);


    // gorde ordezkatu daitezkeen posizioen indizeak
    int *aldatzeko_indizeak = malloc(zero_kop * sizeof(int));
    
    // lortu indizeak
    int i;
    int j=0;
    for(i=0; i<128; i++)
    {
        if(binary_AND[i] == '0')
        {
            aldatzeko_indizeak[j] = i;
            j++;
        }
    }

    printf("Aldatzeko indizeak: %d\n", zero_kop);

    int aurkitutako_gakoak = 0;

    // inprimatu aldatu daitezkeen biten posizioak
    for(i = 0; i<zero_kop; i++)
    {
        printf("%d ", aldatzeko_indizeak[i]);
    }
    printf("\n");

    int lehenengoa_aurkituta = 0;

    while(aurkitutako_gakoak < 100)
    {
        if(lehenengoa_aurkituta==0)
        {
            // aldatu beharreko bit kopurua
            int bit_kop = 3;//rand() % zero_kop;

            for(i=0; i<bit_kop; i++)
            {
                int bit = rand() % zero_kop;

                if(reversed_key_binary[aldatzeko_indizeak[bit]] == '0') reversed_key_binary[aldatzeko_indizeak[bit]] = '1';
                else reversed_key_binary[aldatzeko_indizeak[bit]] = '0';
            }
        }

        // reversed_key_binary zerrendan, aldatzeko_indizeak posizioetako balioak edozein izan daitezke, gakoa lortuko baita halere.
        binary_to_char(reversed_key_binary, 16, reversed_key);
        
        // reverse shuffle aplikatu
        reverse_shuffle(reversed_key);

        // aztertu guztiak karaktereak direla
        int correct = 1;
        for(i = 0; i<16; i++)
        {
            if(reversed_key[i] < 97 || reversed_key[i] > 122)
                correct=0;
        }

        // gakoa karakterez osatuta badago probatu exekuzio arrunt batean onartuko lukeela
        if(correct==1)
        {
            memcpy(reversed_key_lag, reversed_key, 16);
            nire_shuffle(reversed_key_lag);
            nire_and(reversed_key_lag);
            nire_xor(reversed_key_lag);

            if(memcmp(reversed_key_lag, KEY, 16)==0)
            {
                for(i = 0; i<16; i++)
                {
                    printf("%c", reversed_key[i]);
                }
                printf("\n");   
                aurkitutako_gakoak ++;
            }
        }
        reverse_shuffle(reversed_key);
    }
}