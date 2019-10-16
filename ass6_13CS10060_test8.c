int main()
{
   int n, reverse = 0, temp,ep;
 
   prints("Enter a number to check if it is a palindrome or not\n");
   n = readi(&ep);
 
   temp = n;
 
   while( temp != 0 )
   {
      reverse = reverse * 10;
      reverse = reverse + temp%10;
      temp = temp/10;
   }
   printi(n);
   if ( n == reverse )
      prints(" is a palindrome number.\n");
   else
      prints(" is not a palindrome number.\n");
 
   return 0;
}