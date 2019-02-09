#ifndef FLAGHELPER
#define FLAGHELPER

//Бинарный сдвиг единицы влево на 'n' позиций
#define SHL(n) (1 << n)

#define FLAG_EQUAL(value, flag) ((value & flag) == flag)
//#define FLAG_PRESENT(value, flag) ((value & flag) != 0)

#endif // FLAGHELPER

