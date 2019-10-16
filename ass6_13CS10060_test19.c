void swaping(int *x, int *y);
int main()
{
	int n1,n2,ep;
	prints("Enter first number (n1) : ");
	n1 = readi(&ep);
	prints("Enter second number (n2) : "); 
	n2 = readi(&ep);
	prints("\nBefore swapping values:\n");
	prints("\n\tn1=");printi(n1);prints(" \n\tn2=");printi(n2);
	swaping(&n1,&n2);
	prints("\nAfter swapping values:");
	prints("\n\tn1=");printi(n1);prints(" \n\tn2=");printi(n2);
	prints("\n");
	return 0;
}
void swaping(int *x, int *y)
{
  int z;
  z=*x;
  *x=*y;
  *y=z;
} 