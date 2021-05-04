

/* 
 * File:   main.c
 * Author: Vilens
 *         N01064501
 */ 

 
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <stdint.h> 
#include <stdbool.h> 
#include <math.h> 
#include <ctype.h> 




#define TABLE_SIZE 26 


char line [1500];
char iline[1500];
char operand[120],opcode[120],symbol[120], instrc[100];
char foperand[120], fopcode[7], fsymbol[7], finstruction[7], fadd[100];
char filename[20];
char objfname[22];
int locCounter;
int lineCurser = 0;
int flineC = 0;
int programLength;
int startaddress;
int found = 0;
int ENDcounter = 0;
int STARTcounter = 0;
int lineCounter = 0;
int l = 0;




FILE *input, *opTable, *intermediate, *intermF;

static char directives [7][6] = {"START", "END", "BYTE", "WORD", "RESB", "RESW"};


struct Node{
    char name[25];
    int address;
    struct Node *next;
};
typedef struct Node node; 




node *hash_table[TABLE_SIZE]; 
node *hash_table2[TABLE_SIZE];




unsigned int hashing(char *word){
 int hash_value = 0;
 char name[26];
 strcpy(name, word);
 
 for(int i = 0; i < 26; i++){
        
     if(name[0] >= 'A' && name[0] <= 'Z'){
         hash_value = (name[0] - 65);
     }
     if(name[0] >= 0 && name[0] <= 27){
         return -1;
     } 
 }
   return hash_value;
}
 

 



void print_table(){
    
    for(int i = 0; i < TABLE_SIZE; i++){
        
        if(hash_table2[i] == NULL){
            printf("\t[%i]\t---\n", i);
        }
        
        else{
            printf("\t[%i]\t", i); 
            node *temp = hash_table2[i];
            while(temp != NULL){
                printf("   %s - %x     ", temp->name, temp->address);
                temp = temp->next;
                }
            printf("\n");
        }
    }
}



node * createNode(char *name, int address) 
{
  node *newNode = (node*) malloc(sizeof(node));
  strcpy(newNode->name, name);
  newNode->address = address;
  return newNode;
}



bool hash_table_insert(node *p){
    
    int index = hashing(p->name);
    p->next = hash_table[index];
    hash_table[index] = p;

    return true;
}

bool hash_table_insert2(node *p){
    
    int index = hashing(p->name);
    p->next = hash_table2[index];
    hash_table2[index] = p;

    return true; 
}



node *symbol_lookup(char *name){
     int index = hashing(name);
    
    node *temp = hash_table[index];
    int len = strlen(name);
    while(temp != NULL && strncmp(temp->name, name, len)!= 0){
        temp = temp->next;
    }
    return temp;
}

node *opTable_lookup(char *name){
     int index = hashing(name);
    
    node *temp = hash_table2[index];
    int len = strlen(name);
    while(temp != NULL && strncmp(temp->name, name, len)!= 0){
        temp = temp->next;
    }
    return temp;
}

int to_hex(char *hexString){
    int x;
    sscanf(hexString, "%x", &x);
    //printf("value x: %x - %d", x, x);
    return x;
}


void trimString(char *string)
{
    size_t i = 0, j = strlen(string);
    while (j > 0 && isspace((unsigned char)string[j - 1])) string[--j] = '\0';
    while (isspace((unsigned char)string[i])) i++;
    if (i > 0) memmove(string, string + i, j - i + 1);
}


void create_column(){
    char tokens[1024]; 
  
    
    int inc = 0; 
    int walk = 0;
    
    for(int i = 0; i < strlen(line); i++){
        if (line[i] != '\t' && line[i] != '\n'){
            tokens[inc++] = line[i];
        }
        
        else{
            if(walk == 0 && line[i] != ' '){
               tokens[inc] = '\0';
                strcpy(symbol, tokens);
                trimString(symbol);
                walk++;
                inc = 0;
            }
            
            else if(walk == 1 && line[i] != ' '){
               tokens[inc] = '\0';
                strcpy(opcode, tokens);
                trimString(opcode);
                walk++;
                inc = 0;
            }
            
            else if(walk == 2 && line[i] != ' '){ 
               tokens[inc] = '\0';
                strcpy(operand, tokens);
                trimString(operand);
                walk++;
                inc = 0;
            }
        }
    }

}



void create_column2(){
    char token[1024];
    
    int inc = 0;
    int walk = 0;
    
    for(int i = 0; i < strlen(iline); i++){
        if (iline[i] != '\t' && iline[i] != '\n'){
            token[inc++] = iline[i];
        }
        
        else{
            if(walk == 0 && iline[i] != ' '){
               token[inc] = '\0';
                strcpy(fadd, token);
                walk++;
                inc = 0;
            }
            
            else if(walk == 1 && iline[i] != ' '){
               token[inc] = '\0';
                strcpy(fsymbol, token);
                walk++;
                inc = 0;
            }
            
            else if(walk == 2 && iline[i] != ' '){ 
               token[inc] = '\0';
                strcpy(fopcode, token);
                walk++;
                inc = 0;
            }
            else if(walk == 3 && iline [i] != ' '){
                token[inc] = '\0';
                strcpy(foperand, token);
                walk++;
                inc = 0;
            }
        }
    }


}



void create_opTable(){
    opTable = fopen("opcList.txt","r");
    char name[8], add[8];
    int hexadd;
    
    while(!feof(opTable)){
        fscanf(opTable,"%s\t%s", name, add);
        trimString(add);
        trimString(name);
        hexadd = to_hex(add);
        node *temp = createNode(name, hexadd);
        hash_table_insert2(temp);
    }
}


void read_next_line2()
{   
    intermF = fopen("intermediate.txt","r");
    if(!feof(intermF))
    {
        fseek(intermF , flineC , SEEK_CUR);
        fgets(iline, sizeof(iline), intermF);
        flineC = ftell(intermF);
    }
}

void read_next_line()
{    
    input = fopen(filename,"r");
    if(!feof(input))
    {
        fseek(input , lineCurser , SEEK_CUR);
        fgets(line, sizeof(line), input); 
        lineCurser = ftell(input);
    }
}


int main(int argc, char* argv[]) { 
    strcpy(filename, argv[1]);
    strcpy(objfname, filename);
    strcat(objfname, ".obj");
    create_opTable();
    
    
        
   input = fopen(filename, "r");
   intermediate = fopen("intermediate.txt", "w");
  
  
   if(input == NULL){
       printf("\nFile not found\n");
       fclose(input);
       exit(0);  
   }
  
   if(!feof(input)){
    read_next_line();
    create_column();
    
    lineCounter++;
    while (line[0] == 35){
        read_next_line();
        create_column();
        lineCounter++;
    }
   
     //printf(" %s\t%s\t%s\n ",symbol,opcode,operand);
      if(strlen(symbol) > 6){
        printf("\nLine %d: symbols cannot be more than than 6 characters long, program will terminate.\n", lineCounter);
    }

        for(int i = 0; i < sizeof(directives); i++){
          if(strcmp(symbol, directives[i]) == 0){
            printf("\nLine %d: symbol has same name as directive, program will terminate.\n", lineCounter);
            exit(0);
           }
        }
  
    }

   if(strcmp(symbol, "START") == 0){
       printf("\nLine %d: No file name, program will exit, program will terminate.\n", lineCounter);
       exit(0);
   }
   if(strcmp(opcode, "START") == 0){
       STARTcounter = 1;
       startaddress = to_hex(operand);
       locCounter = startaddress;
       if(locCounter > 32768){
          printf("\nLine %d: not enough memory.\n", lineCounter);
          exit(0);
       }
       

       node *temp = createNode(symbol, locCounter);
       hash_table_insert(temp);
       //printf("%s\t%x\n", temp->name, temp->address);
       fprintf(intermediate,"%x\t%s\t%s\t%s\n", locCounter, symbol, opcode, operand);
       
       if(!feof(input)){
           read_next_line();
           create_column();
           lineCounter++;
       }
   }

   else {  
       printf("\nLine %d: no start directive, program will terminate.\n", lineCounter);
       exit(0);
   }
   
   if(strcmp(operand, " ") == 0){
       locCounter = 0;
   }
   
   
   while(strcmp(opcode, "END") != 0){
           
     if(strlen(symbol) > 6){
     printf("\nLine %d: Symbols cannot be more than than 6 characters long, program will terminate.\n", lineCounter);
     exit(0);
    }

        if ( line[0] != 35 ) {
            if(strcmp(symbol, "") != 0){
       
                node *temp2 = symbol_lookup(symbol); 
                  
               if(temp2 != NULL){ 
                   printf("\nLine %d: duplicate symbol, program will terminate.\n", lineCounter); 
                   exit(0); 
               }
               else{
                   for(int i = 0; i < sizeof(directives); i++){
                      if(strcmp(symbol, directives[i]) == 0){
                          printf("\nLine %d: symbol has same name as directive, program will terminate.\n", lineCounter);
                          exit(0);
                       }
                }

                node *temp = createNode(symbol, locCounter);
                hash_table_insert(temp);
                //printf("%s\t%x\n", temp->name, temp->address);
                   

               }
            }
            fprintf(intermediate,"%x\t%s\t%s\t%s\n", locCounter, symbol, opcode, operand);
            if(strcmp(opcode, "START") == 0){
                printf("\nLine %d: cannot have 2 'START' directives, program will terminate.\n", lineCounter);
                exit(1);
            }
            
             node *temp10 = opTable_lookup(opcode);
             if(temp10 != NULL){
                 locCounter+= 3;
             }
            
            else if(strcmp(opcode, "WORD") == 0){
                int x = atoi(operand);
                if(x > 8388608){
                    printf("\nLine %d: Word is too big, program will terminate.\n", lineCounter);
                    exit(0);
                }
               
                locCounter += 3; 
            }
            else if(strcmp(opcode, "RESW") == 0){
                locCounter = locCounter + atoi(operand) * 3;
            }
            else if(strcmp(opcode,"RESB") == 0){
                locCounter = locCounter + atoi(operand);
            }
            else if(strcmp(opcode,"BYTE") == 0){
                char *c = strtok(operand, "'");
                       c = strtok(NULL, "'");
                           
                if(operand[0]=='C'){
                    
                    locCounter += strlen(c);
                }             
                        else if(operand[0] == 'X'){ 
                            for(int i = 0; i < strlen(c); i++){
                                char a = c[i];
                                if((( a >= 65) && (a <= 70)) || ((a >= 48) && (a <= 57))){
                                    continue;
                                }
                                else{
                                    printf("\nLine %d: invalid hex, program will terminate.\n", lineCounter);
                                    exit(0);
                                }
                               
                            }
                             locCounter += strlen(c) / 2;
                        }
                       
            }
            else if(strcmp(opcode, "END") == 0){
                continue;
            }
            else{ 
                printf("\nLine %d:  %s is an invalid operation code, program will terminate.\n", lineCounter, opcode); 
                exit(0);
            }
         
         if(!feof(input)){
            read_next_line();
                while (line[0] == 35){
                read_next_line();
                lineCounter++;
         }
            create_column();
           
            lineCounter++;
    }
            

       for(int i = 0; i < strlen(symbol); i++){
           if((( symbol[i] >= 65) && (symbol[i] <= 91)) || ((symbol[i] >= 48) && (symbol[i] <= 57)) || ((symbol[i] >= 97) && (symbol[i] <= 122))){
                    continue;
           }
           else{
               printf("\nLine %d: '%c' is an invalid token , program will terminate.\n", lineCounter, symbol[i]);
               exit(0);
           }
       }
         
          if(locCounter > 32768){
          printf("\nLine %d: not enough memory, program will terminate.\n", lineCounter);
          exit(0);
      }
   }
          
   }
   
   fprintf(intermediate,"%x\t%s\t%s\t%s\n", locCounter, symbol, opcode, operand);
   
   programLength = locCounter - startaddress;


  fclose(input);
  fclose(intermediate);


    
    
    
    
    
    int flocCounter = 0X0, start=0X0, Xbit=0X0, increment=0X0, record_len=0X0;
    int lineCounter = 0;
    int tStart = 0;
    int passone = 0;
    int endaddress = 0;
    int Erecord = 0;
    int maxSize;
    char str[400] = {'\0'};
    char ss[400] = {'\0'};
    char substring[400] = {'\0'};
    char obj[1024] = {'\0'};

    int opAdd, operandAdd;
    FILE *opTable;
    bool indexadd, RES;  
    
    
    
    intermF = fopen("intermediate.txt", "r");
    opTable = fopen("opcList.txt", "r");
    
    if(intermF == NULL){
        printf("\nERROR: Intermediate file missing!\n");
        exit(0);
    }

    
    read_next_line2();
    create_column2();
    lineCounter++;
    
    
    flocCounter = to_hex(fadd);
    start = to_hex(foperand);
    tStart = start;
    if(strcmp(fopcode, "START") == 0){
         sprintf(ss, "H%s  %06x%06x",fsymbol, start, programLength);
         strcat(obj, ss);
         memset(ss, 0, sizeof(ss));
         
         
    }
    
    while(!feof(intermF)){
        read_next_line2();
        create_column2();
        flocCounter = to_hex(fadd);
        lineCounter++;


       
       node *tempest = opTable_lookup(fopcode);
       
        if(tempest){
            if(strlen(foperand) != 0){
                char doperand[50] = "";  
                indexadd = false;
                
                for(int i = 0; i < strlen(foperand) - 2; i++){
                    doperand[i] = foperand[i];
                    
                    if(foperand[i+1] == ',' && foperand[i+2] == 'X'){
                        indexadd = true;
                        break;
                    }
                }
                
                if(endaddress == 0){
                    Erecord = flocCounter;
                    endaddress++;
                }
                
                if(indexadd){
                    node *temp = symbol_lookup(doperand);
                    if(temp == NULL){ 
                    printf("\nLine %d: symbol '%s' is not in symbol table, pass 2 will terminate.\n", lineCounter, doperand); 
                    exit(0); 
                    }
                    else{
                        node *temp2 = opTable_lookup(fopcode);
                        
                        if(temp2){
                            char s[50];
                            opAdd = temp2->address;
                            operandAdd = temp->address + 0X8000;
                            sprintf(s,"%02x%04x", opAdd, operandAdd);
                            strcat(str, s);
                            
                        }
                        else{
                            printf("\nLine %d:  '%s' is an invalid opcode, program will terminate\n",lineCounter, fopcode);
                            exit(0); 
                        }   
                    }
                }
                
                else if(!indexadd){
                     node *temp3 = opTable_lookup(fopcode);
                     node *temp4 = symbol_lookup(foperand);
                     
                     if(temp4){ 
                         char s[50];
                      sprintf(s,"%02x%04x", temp3->address, temp4->address);
                      strcat(str, s);
                         
                     }  
                     else{
                      printf("\nLine %d: symbol '%s' is not in symbol table, pass 2 will terminate.\n", lineCounter, foperand); 
                        exit(0); 
                     }
                }

            }
            else{
                char s[50];
                sprintf(s,"%x0000", tempest->address);
                strcat(str, s);
            }
            increment = 3;
        }
        else if(strcmp(fopcode, "BYTE") == 0 || strcmp(fopcode, "WORD") == 0){
            char s[10];
            if(foperand[0] == 'C'){
                int ascii = 0X0;
                for(int i = 2; i < strlen(foperand) - 1; i++){  
                    int c = 0X0;
                    c =  (int)foperand[i];
                    
                    sprintf(s,"%x", c);
                    strcat(str, s);
                    if(c == 39){
                        break;
                    }
                }
                increment = strlen(foperand)-3;
            }
            else if(foperand[0] == 'X'){
                char s[25];
                char constants[8] = {'\0'};
                for(int i = 0; i < strlen(foperand) - 2; i++){ 
                    constants[i] = foperand[i+2];
                    if(constants[i] == 39){
                        break;
                    }
                    
                    sprintf(s,"%c", constants[i]);
                    strcat(str, s);
                }               
                increment = (strlen(foperand)-3) / 2;
            }
            else{
                char s[50];
                int x = atoi(foperand); 
                
                sprintf(s,"%06x", x);
                strcat(str, s);
                increment = 3;
            }
        }
       
       else if(strcmp(fopcode, "RESW") == 0){
           increment = atoi(foperand) * 3;
           continue;
       }
       else if(strcmp(fopcode, "RESB") == 0){
           increment = atoi(foperand);
           continue;
       }
       
       else if(strcmp(fopcode, "END") == 0){
           node *temp5 = symbol_lookup(foperand);
           if(!temp5){
               printf("\nLine %d: symbol '%s' is not in symbol table, pass 2 will terminate.\n", lineCounter, foperand); 
               exit(0);
           }
       }
       
        record_len += increment;
            maxSize  = 60;
        
            if(strlen(str) > maxSize){
                
                
               start = flocCounter;
              
              if(passone == 0){
                  start = tStart;
                  passone++;
              }
              
              record_len = record_len - maxSize/2;
              strncpy(substring, str, maxSize);
              memset(str,' ', maxSize);
              trimString(str);
              
              int size = strlen(substring) / 2;
              
              sprintf(ss, "\nT%06x%02x%s", start, size, substring);
              strcat(obj, ss);
              memset(substring, 0, sizeof(substring));
             
            }    
        
    }
            start = flocCounter;
    
            sprintf(ss, "\nT%06x%02x%s", start, record_len, str);
            strcat(obj, ss);
            memset(ss, 0, sizeof(ss));
         
            memset(str, 0, sizeof(str));
            record_len = 0;
            start = flocCounter; 
            
             sprintf(ss, "\nE%06X",Erecord);
            strcat(obj, ss);
            memset(ss, 0, sizeof(ss));

            
            FILE *objF;
            objF = fopen(objfname, "w");
            
            fprintf(objF, "%s", obj);
           // printf("%s", obj);
            
    
    fclose(intermF);
    fclose(objF); 
    fclose(opTable);
    
    return  0;

}



