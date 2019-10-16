int gcd(int a,int b){
	if(b == 0)
		return a;
	return gcd(b,a%b);
}

int main()
{
	int a,b;
	int ep;
	prints("Enter two numbers\n");
	a = readi(&ep);
	b = readi(&ep);
	prints("The gcd of ");
	printi(a);
	prints(" and ");
	printi(b);
	prints(" is ");
	printi(gcd(a,b));
	prints("\n");

	return 0;
}