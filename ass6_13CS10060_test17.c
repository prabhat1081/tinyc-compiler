int MAX = 100;

int main () {

   int arr[100],n,ep,i, *ptr,*endptr;
   prints("Enter the size of an array: ");
      n = readi(&ep);

   prints("Enter the elements of the array: " );
   for(i=0;i<n;i++){
      arr[i] = readi(&ep);
   }  

   /* let us have array address in pointer */
   ptr = arr;
   endptr = &arr[n-1];
   i = 0;

	
   while(ptr <= endptr) {

      prints("Address of var[");printi(i);prints("] = ");printi(ptr);prints("\n");
      prints("Value of var[");printi(i);prints("] = ");printi(*ptr);prints("\n");
      i++;
      /* move to the next location */
      ptr++;
   }
	
   return 0;
}