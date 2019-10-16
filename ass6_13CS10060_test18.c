int main()
{
   int array[100], n, c, d, position, swap,ep;
 
   prints("Enter number of elements\n");
   n = readi(&ep);
 
   prints("Enter the integers\n");
 
   for ( c = 0 ; c < n ; c++ )
      array[c] = readi(&ep);
 
   for ( c = 0 ; c < ( n - 1 ) ; c++ )
   {
      position = c;
 
      for ( d = c + 1 ; d < n ; d++ )
      {
         if ( array[position] > array[d] )
            position = d;
      }
      if ( position != c )
      {
         swap = array[c];
         array[c] = array[position];
         array[position] = swap;
      }
   }
 
   prints("Sorted list in ascending order:\n");
 
   for ( c = 0 ; c < n ; c++ ){
      printi(array[c]);
      prints("\n");
   }
 
   return 0;
}