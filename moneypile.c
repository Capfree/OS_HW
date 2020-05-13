/**
 * COSC 6270 - Project 2
 *
 * Reads in an arbitrarily long sequence of positive integer currency values 
 * and outputs each category of currency in reverse order.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#define OTHER 5
#define USD 0
#define UKD 1
#define JPY 2
#define EUR 3
#define IR 4

//Ratio
#define UKD2USD 1.3055
#define JPY2USD 0.0091
#define EUR2USD 1.1148
#define IR2USD 0.0141

int getCurrencyType(char* type);
void add2Pile(int usd, int type, int* money);
char* getMoneySign(int type);

char* ukSign = "£";
char* jpSign = "¥";
char* euroSign = "€";
char* irSign = "₹";
int pileSize[5] = {0, 0, 0, 0, 0};

int main(){
    printf("=== Virtual Money Pile ===\n");
    int digit = 0;
    int value = 0;
    //int i = 0;

    char input = getchar();

    int type = getCurrencyType(&input);

    int i;
    int** moneyArr = (int**)malloc(5*sizeof(int*));
    for(i = 0; i < 5; ++i){
        moneyArr[i] = (int*)malloc(1000*sizeof(int));
    }

    while(input != EOF){

        if(input >= '0' && input <= '9'){
            digit = input - '0';
            value = value * 10 + digit;
            //printf("%d\t%d \n", digit, value);
        }

        input = getchar();

        if(input == '\t' || input == '\n'){
            if(5 == type){ //Others
               // printf("Unsupported type of money! \n");
                continue;
            }else if(0 <= type && 5 > type){
                add2Pile(value, type, moneyArr[type]);                
            }else{//Error happens
                continue;
                //printf("Unexpected errors!\n");
            }
            
            //New round
            value = 0;
            input = getchar();
            if(input == EOF) break;
            while(input == '\t' || input == '\n')input = getchar();
            type = getCurrencyType(&input);
        }

        //pile space check
        for(i = 0; i < 5; i++){
            if(999 == (pileSize[i] % 1000)){
                moneyArr[i] = (int*)realloc(moneyArr[i], (pileSize[i] + 1000) * sizeof(int));
                if(NULL == moneyArr[i]){
                    printf("Memory realloc failed!");
                    exit(0);
                }
            }
        }
    }

    /*
    for(i = 0; i < 5; ++i){
        printf("%d\n", pileSize[i]);
    }*/
    
    printf("\n");

    int j;
    for(i = 0; i < 5; ++i){
        char* moneySign = getMoneySign(i);

        printf("\nVirtual %s pile:\n", moneySign);
        
        for(j = pileSize[i]-1; j >= 0; --j){
            printf("%s%d\n", moneySign, moneyArr[i][j]);
        }
    }
    return 0;
}

void add2Pile(int value, int type, int* money){
    //printf("%d\n", type);
    if(USD == type){//US dollar
        *(money + pileSize[0]) = value;
        pileSize[0] = pileSize[0] + 1;
        printf("Putting $%d in $ pile.\n", value);

    }else if(UKD == type){//UK pound £
        *(money + pileSize[1]) = value;
        pileSize[1] = pileSize[1] + 1;
        printf("Putting £%d in £ pile.\n", value);

    }else if(JPY == type){//JP Yen ¥
        *(money + pileSize[2]) = value;
        pileSize[2] = pileSize[2] + 1;
        printf("Putting ¥%d in ¥ pile.\n", value);

    }else if(EUR == type){//EU €
        *(money + pileSize[3]) = value;
        pileSize[3] = pileSize[3] + 1;
        printf("Putting €%d in € pile.\n", value);

    }else if(IR == type){//Indian Rupee ₹
        *(money + pileSize[4]) = value;
        pileSize[4] = pileSize[4] + 1;
        printf("Putting ₹%d in ₹ pile.\n", value);

    }else{

    }
}

char* getMoneySign(int type){
    char* moneySign;

    if(USD == type){
        moneySign = "$";
    }else if(UKD == type){
        moneySign = "£";
    }else if(JPY == type){
        moneySign = "¥";
    }else if(EUR == type){
        moneySign = "€";
    }else if(IR == type){
        moneySign = "₹";
    }

    return moneySign;
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
    if(*type != EOF){
        //printf("%d/t%c", *type, *type);
        return getCurrencyType(type);
    }else{
        return OTHER;
    }
}

