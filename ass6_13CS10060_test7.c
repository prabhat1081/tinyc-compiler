int main()
{
  int year,ep;
 
  prints("Enter a year to check if it is a leap year\n");
  year = readi(&ep);
  printi(year);
  if ( year%400 == 0)
    prints(" is a leap year.\n");
  else if ( year%100 == 0)
    prints(" is not a leap year.\n");
  else if ( year%4 == 0 )
    prints(" is a leap year.\n");
  else
    prints(" is not a leap year.\n");  
 
  return 0;
}