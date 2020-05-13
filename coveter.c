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

//Ratio on 1/16/2020
#define UKD2USD 1.3055
#define JPY2USD 0.0091
#define EUR2USD 1.1148
#define IR2USD 0.0141

int getCurrencyType(char* type);
void printMoney(double usd);

char* ukSign = "£";
char* jpSign = "¥";
char* euroSign = "€";
char* irSign = "₹";


int main(){
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

            }else if(1 == type){//US dollar
                printMoney((double)value);

            }else if(2 == type){//UK pound
                double tmp = (double)value*UKD2USD;
                printMoney(tmp);

            }else if(3 == type){//JP Yen
                double tmp = (double)value*JPY2USD;
                printMoney(tmp);

            }else if(4 == type){//EU
                double tmp = (double)value*EUR2USD;
                printMoney(tmp);

            }else if(5 == type){//Indian Rupee
                double tmp = (double)value*IR2USD;
                printMoney(tmp);

            }else{//Error happens
                printf("Unexpected errors!\n");
            }
            
            //New round
            value = 0;
            input = getchar();
            if(input == EOF) return 0;

            type = getCurrencyType(&input);
        }
    }
    return 0;
}

void printMoney(double usd){
    printf("%.2f dollars\t%.2f British pounds\t%.2f Japanese Yens\t%.2f Euros\t%.2f Indian Rupees\n", usd, usd/UKD2USD, usd/JPY2USD, usd/EUR2USD, usd/IR2USD);
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
