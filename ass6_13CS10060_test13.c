int add(int a,int b){
	return (a+b);
}
int diff(int a,int b)
{
	return a -b;
}
int s = 10;
int *g = &s;

int test(int (*fptr)(int a,int b),int a,int b){
	return fptr(a,b);
}

int main()
{
	int a = 10,b=15;
	int (*f)(int a,int b);
	f = &add;
	printi(f(a,b));
	prints("\n");
	f = &diff;
	printi(f(a,b));
	prints("\n");
	printi(test(f,a,b));
	prints("\n");
	return 0; 
}