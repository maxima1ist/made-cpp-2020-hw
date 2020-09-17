#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

//function to make some tests
double convertToDouble (uint64_t number) {
    return *((double *)(&number));
}

uint64_t convertToUint64 (double number) {
    return *((uint64_t *)(&number));
}

bool getBit (const uint64_t number, const uint8_t index) {
    uint64_t mask = (uint64_t)1 << index;
    uint64_t result = mask & number;
    result >>= index;
    return result;
}

bool checkForPlusZero (uint64_t number) {
    return number == 0x0000000000000000;
}

bool checkForMinusZero (uint64_t number) {
    return number == 0x8000000000000000;
}

bool checkForPlusInf (uint64_t number) {
    return number == 0x7FF0000000000000;
}

bool checkForMinusInf (uint64_t number) {
    return number == 0xFFF0000000000000;
}

bool checkForNormal (uint64_t number) {
    bool wasZeroInF = true;
    for(int8_t i = 1; i <= 11; ++i)
        if(getBit(number, 63 - i) == 1) {
            wasZeroInF = false;
            break;
        }
    if(wasZeroInF)
        return false;

    bool wasInfInF = true;
    for(int8_t i = 1; i <= 11; ++i)
        if(getBit(number, 63 - i) == 0) {
            wasInfInF = false;
            break;
        }
    if(wasInfInF)
        return false;

    return true;
}

bool checkForPlusNormal (uint64_t number) {
    if(getBit(number, 63) == 1)
        return false;

    return checkForNormal(number);
}

bool checkForMinusNormal (uint64_t number) {
    if(getBit(number, 63) == 0)
        return false;

    return checkForNormal(number);
}

bool checkForDenormal (uint64_t number) {
    for(int8_t i = 1; i <= 11; ++i)
        if(getBit(number, 63 - i) == 1)
            return false;

    return (getBit(number, 0) == 1);
}

bool checkForPlusDenormal (uint64_t number) {
    if(getBit(number, 63) == 1)
        return false;

    return checkForDenormal(number);
}

bool checkForMinusDenormal (uint64_t number) {
    if(getBit(number, 63) == 0)
        return false;

    return checkForDenormal(number);
}

bool checkForSignalingNan (uint64_t number) {
    for(int8_t i = 1; i <= 11; ++i)
        if(getBit(number, 63 - i) == 0)
            return false;

    return (getBit(number, 51) == 0 && getBit(number, 0) == 1);
}

bool checkForQuietNan (uint64_t number) {
    for(int8_t i = 1; i <= 11; ++i)
        if(getBit(number, 63 - i) == 0)
            return false;

    return (getBit(number, 51) == 1);
}


void classify (double number) {
    if (checkForPlusZero(convertToUint64(number))) {
        printf("Plus zero\n");
    }

    else if (checkForMinusZero(convertToUint64(number))) {
        printf("Minus zero\n");
    }

    else if (checkForPlusInf(convertToUint64(number))) {
        printf("Plus inf\n");
    }

    else if (checkForMinusInf(convertToUint64(number))) {
        printf("Minus inf\n");
    }

    else if (checkForPlusNormal(convertToUint64(number))) {
        printf("Plus normal\n");
    }

    else if (checkForMinusNormal(convertToUint64(number))) {
        printf("Minus normal\n");
    }

    else if (checkForPlusDenormal(convertToUint64(number))) {
        printf("Plus Denormal\n");
    }

    else if (checkForMinusDenormal(convertToUint64(number))) {
        printf("Minus Denormal\n");
    }

    else if (checkForSignalingNan(convertToUint64(number))) {
        printf("Signailing NaN\n");
    }

    else if (checkForQuietNan(convertToUint64(number))) {
        printf("Quiet NaN\n");
    }

    else {
        printf("Error.\n");
    }
}

int main(void) {

    double x = 0;
    classify(+x);

    classify(-x);

    x = INFINITY;
    classify(x);

    classify(-x);

    x = 1e12;
    classify(x);

    classify(-x);

    x = 2.2205921692043672e-308;
    classify(x);

    classify(-x);

    uint64_t a = 0x7FF7F7BFFBDFFFFF;
    x = convertToDouble(a);
    classify(x);

    a = 0x7FFFF7BFFBDFFFFF;
    x = convertToDouble(a);
    classify(x);

    return 0;
}
