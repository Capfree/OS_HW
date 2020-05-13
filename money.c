/**
 * COSC 6270 - Project 1
 *
 * A simple international currency converter. 
 * The converter will take an unformatted list of five allowable currencies, convert each to their equivalent in U.S. dollars. 
 * Print a final total when done.
 *
 */

#include <stdio.h>

#define OTHER 0
#define USD 1
#define UKD 2
#define JPY 3
#define EUR 4
#define IR 5

//Ratio
#define UKD2USD 1000/766
#define JPY2USD 1000/109940
#define EUR2USD 1000/897
#define IR2USD 1000/70845

int getCurrencyType(char* type);
void printMoney(double usd, int type);
int total = 0;

char ukSign[] = "£";
char jpSign[] = "¥";
char euroSign[] = "€";
char irSign[] = "₹";

int main(){
    printf("=== Currency Converter ===\n");
    int digit = 0;
    int value = 0;

    char input = getchar();

    int type = getCurrencyType(&input);

    while(input != EOF){
        if(input >= '0' && input <= '9'){
            digit = input - '0';
            value = value * 10 + digit;
            //printf("%d\t%d \n", digit, value);
        }

        input = getchar();

        if(input == '\n'){
            if(0 == type){ //Others
                printf("Unsupported type of money! \n");
            }else if(0 < type && 6 > type){
                printMoney((double)value, type);                
            }else{//Error happens
                printf("Unexpected errors!\n");
            }
            
            //New round
            value = 0;
            input = getchar();
            if(input == EOF) {
                printf("TOTAL = $%d\n", total);
                return 0;
            }

            type = getCurrencyType(&input);
        }
    }
    printf("TOTAL = $%d\n", total);
    return 0;
}

void printMoney(double value, int type){
    //printf("%d\n", type);
    if(USD == type){//US dollar
        total += (int)value;
        printf("$%d = $%d \n", (int)value, (int)value);

    }else if(UKD == type){//UK pound
        total += (int)(value*UKD2USD);
        printf("£%d = $%d \n", (int)value, (int)(value*UKD2USD));

    }else if(JPY == type){//JP Yen
        total += (int)(value*JPY2USD);
        printf("¥%d = $%d \n", (int)value, (int)(value*JPY2USD));

    }else if(EUR == type){//EU
        total += (int)(value*EUR2USD);
        printf("€%d = $%d \n", (int)value, (int)(value*EUR2USD));

    }else if(IR == type){//Indian Rupee
        total += (int)(value*IR2USD);
        printf("₹%d = $%d \n", (int)value, (int)(value*IR2USD));

    }else{

    }
}

int getCurrencyType(char* type){
    if(*type == '$'){
        *type = getchar();
        return USD;
    }

    if(*type == ukSign[0]){
        *type = getchar();
        if(*type == ukSign[1]){
            *type = getchar();
            return UKD;
        }else if(*type == jpSign[1]){
            return JPY;
        }
    }

    if(*type == euroSign[0]){
        *type = getchar();
        if(*type == euroSign[1]){
            *type = getchar();
            if(*type == euroSign[2]){
                *type = getchar();
                return EUR;
            }else if(*type = irSign[2]){
                *type = getchar();
                return IR;
            }
        }
    }

    *type = getchar();
    return OTHER;
}
