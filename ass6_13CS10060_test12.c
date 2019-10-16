void main ()
{
	char ch;
	int num1,num2,ep;
	prints("What do you want to do?\n");
	prints("Addition, Subtraction, Multiplication or Division or Remainder(A,S,M,D,R): ");
	ch = readchar(&ep);
	prints("Enter first number: ");
	num1 = readi(&ep);
	prints("Enter second number: ");
	num2 = readi(&ep);
	prints("\n");


	if(ch=='A')  {
		prints("The sum is ");
		printi(num1+num2);
	}

	if(ch=='S')  {
		prints("The difference is ");
		printi(num1-num2);
	}
	if(ch=='M')  {
		prints("The product is ");
		printi(num1*num2);
	}
	if(ch=='D')  {
		prints("The quotient is ");
		printi(num1/num2);
	}
	if(ch=='R')  {
		prints("The remainder is ");
		printi(num1%num2);
	}
	prints("\n");
	return 0;



}