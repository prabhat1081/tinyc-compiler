 int main()
{
   int first, second, *p, *q, sum, ep;
 
   prints("Enter two integers to add\n");
   first = readi(&ep);
   second = readi(&ep);
 
   p = &first;
   q = &second;
 
   sum = *p + *q;
 
   prints("Sum of entered numbers = ");
   printi(sum);
   prints("\n");
 
   return 0;
}