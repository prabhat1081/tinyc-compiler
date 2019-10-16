#include "myl.h"
#define BUFF 30
#define FBUFF 100
#define IBUFF 20
#define PRECISION 6
int printi(int n){
    char buff[BUFF], zero='0';
    int i=0, j, k, bytes;

    if(n == 0) buff[i++]=zero;
    else{
       if(n < 0) {
          buff[i++]='-';                     // Adding - sign for negative numbers
          n = -n;
       }
       while(n){
          int dig = n%10;                   // Extracting digits and adding to the buffer
          buff[i++] = (char)(zero+dig);
          n /= 10;
       }
       if(buff[0] == '-') j = 1;
       else j = 0;
       k=i-1;
       while(j<k){                         // Reversing the number to correct the sequence
          char temp=buff[j];
          buff[j++] = buff[k];
          buff[k--] = temp;
       }
    } 
    bytes = i;

    __asm__ __volatile__ (
          "movl $1, %%eax \n\t"
          "movq $1, %%rdi \n\t"
          "syscall \n\t"
          :
          :"S"(buff), "d"(bytes)
    ) ;  // $1: write, $1: on stdin
    return bytes;
}


int prints(char *str)
{
  int bytes = 0;
  while(str[bytes++]!='\0');   // Counting the number of characters
  bytes--;                     // Ignoring the null character
  __asm__ __volatile__ (
          "movl $1, %%eax \n\t"
          "movq $1, %%rdi \n\t"
          "syscall \n\t"
          :
          :"S"(str), "d"(bytes)
    ) ;  // $1: write, $1: on stdin
    return bytes;

}

int printchar(char ch)
{
  int bytes = 0;
  char str[2];
  str[0] = ch;
  str[1] = '\0';
  while(str[bytes++]!='\0');   // Counting the number of characters
  bytes--;                     // Ignoring the null character
  __asm__ __volatile__ (
          "movl $1, %%eax \n\t"
          "movq $1, %%rdi \n\t"
          "syscall \n\t"
          :
          :"S"(str), "d"(bytes)
    ) ;  // $1: write, $1: on stdin
    return bytes;

}

int printd(float number)
{
    char buff[BUFF], zero='0';
    int i=0, j, k, bytes;
    if(number < 0)
    { 
        buff[i++]='-';
        number = -number;                     // Adding - to buffer and making the number positive
    }
    long long int integer_part = (long long int)number;   // Taking the integer part of the fraction
    float fraction_part = number - integer_part;
    
    if(integer_part == 0) buff[i++]=zero;     // Storing the integer part in the buffer
    else{
       while(integer_part){
          int dig = integer_part%10;
          buff[i++] = (char)(zero+dig);
          integer_part /= 10;
       }
       if(buff[0] == '-') j = 1;
       else j = 0;
       k=i-1;
       while(j<k){
          char temp=buff[j];
          buff[j++] = buff[k];
          buff[k--] = temp;
       }
    } 
    buff[i++] = '.';
    
    for(j=0;j<PRECISION;j++)                // Get PRECISION no of digits after the decimal to print
    {
      fraction_part*=10;
      int temp = (int)fraction_part;
      buff[i++] = (char)(temp+zero);
      fraction_part -= temp;
    }
    int sigdigit = (int)(fraction_part*10);  // Getting the PRECISION+1 digit for rounding off
    if(sigdigit >= 5)
    { 
        int k = i;
        i--;
        int carry = 1;
        while(i>=0){                        // Propagating the carry backwards
          if(buff[i] == '.') {i--;continue;}
          int value = buff[i]-zero + carry;
          if(value < 10)
          {
            buff[i] = (char)(value+zero);
            carry = 0;
            break;
          }
          buff[i] = zero;
          carry = 1;
          i--;
        }
        if(carry == 1)     //Inserting one more digit if carry is still 1
        {
          for(i=k;i>=1;i--)
            buff[i] = buff[i-1];
          buff[0] = carry;
          k++;
        }
        i = k;
    }
    bytes = i;

    __asm__ __volatile__(
        "movl $1, %%eax \n\t"
        "movq $1, %%rdi \n\t"
        "syscall \n\t"
        :
        :"S"(buff), "d"(bytes)
      );
    return bytes;
}

char readchar(int *eP)
{
	char buff[2];
    int bytes=0;
     *eP = ERR;
    __asm__ __volatile__(
        "movl $0, %%eax \n\t"
        "movq $1, %%rdi \n\t"
        "syscall \n\t"
        :"=a"(bytes)
        :"S"(buff), "d"(2)
      );
      if(bytes == 0)
      	*eP = ERR;
      return buff[0];
}
int readi(int *eP)
{
    char buff[IBUFF];
    int bytes=0;
     *eP = ERR;
    __asm__ __volatile__(
        "movl $0, %%eax \n\t"
        "movq $1, %%rdi \n\t"
        "syscall \n\t"
        :"=a"(bytes)
        :"S"(buff), "d"(IBUFF)
      );
   
    int num = 0,neg = 0,i=0;
    char zero = '0';      
    if(buff[0] == '-')           // If no is negative then setting negative flag to 1
    { 
      neg = 1;
      i++;
    }
    else if(buff[0] == '+')i++;
    while(i<bytes)
    {
        char c = buff[i];
        if('0' <= c && c <= '9')
        {   
            num = num*10 + (int)(c -zero);    // Adding to the number if the character is valid digit
        }
        else if(c == '\n')
        {
            *eP = OK;                     // IF the input ends with newline a succesful read has been done
            break;
        }
        else
          break;
        i++;
    }
    if(*eP != OK) return -1;           // Return -1 if error
    if(neg) num = -num;                
    return num;
}
int readf(float* number)
{
    char buff[FBUFF];
    int bytes=0;

    __asm__ __volatile__(
        "movl $0, %%eax \n\t"
        "movq $1, %%rdi \n\t"
        "syscall \n\t"
        :"=a"(bytes)
        :"S"(buff), "d"(FBUFF)
      );
    int eP = ERR;
    float num = 0;
    int neg = 0,i=0;
    char zero = '0';
    if(buff[0] == '-')            // If no is negative then setting negative flag to 1
    { 
      neg = 1;
      i++;
    }
    else if(buff[0] == '+')i++;
    while(i<bytes)
    {
        char c = buff[i];
        if('0' <= c && c <= '9')        // Adding to the number if the character is valid digit
        {   
            num = num*10 + (float)(c -zero);
        }
        else if(c == '\n')            // IF the input ends with newline a succesful read has been done
        {
            eP = OK;
            break;
        }
        else
          break;
        i++;
    }
    if(buff[i] == '.'){       // Read the digits after decimal if decimal point is there
      i++;
      float multiplier = 10;  // The weight^-1 of the current digit
      while(i<bytes)
      {
          char c = buff[i];
          if('0' <= c && c <= '9')
          {   
              num = num + (float)(c -zero)/multiplier;  // Add the digit to the number with its weight
              multiplier *=10;
          }
          else if(c == '\n')
          {
              eP = OK;
              break;
          }
          else
            break;
          i++;
      }
    }
    if(neg) num = -num;
    *number = num;
    return eP;
}
