int main();

int fibo(int n){
	//prints("Inside fibo: n=  ");
	//printi(n);
	//prints("\n");
	if(n < 1)	
		return 0;
	if(n == 1 || n == 2)
		return 1;
	int sum = fibo(n-1)+fibo(n-2);
	prints("The f number : ");
	printi(sum);
	prints("\n");
	return sum;
}

int main()
{
	int ep;
	prints("Enter a number to get the nth Fibonacci Number:");
	int t = readi(&ep);
	t = fibo(t);
	prints("The nth fibonacci number is : ");
	printi(t);
	prints("\n");
	return 0;
}