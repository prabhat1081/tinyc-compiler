int prints(char * str);
int printi(int i);
int readi(int *eP);
int sum(int a,int b, int c,int d, int e, int f,int g, int h);

int main()
{
	int suma = 0;
	int p[8],i,ep;
	prints("Enter eight numbers to find sum:\n");
	for( i = 0;i < 8;i++){
		p[i] = readi(&ep);
	}
	suma = sum(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
	prints("The sum is :");
	printi(suma);
	prints("\n");
	return 0;
}

int sum(int a,int b, int c,int d, int e, int f,int g, int h){
	prints("The paarmeters are:\n");
	printi(a);
	prints("\n");
	printi(b);
	prints("\n");
	printi(c);
	prints("\n");
	printi(d);
	prints("\n");
	printi(e);
	prints("\n");
	printi(f);
	prints("\n");
	printi(g);
	prints("\n");
	printi(h);
	prints("\n");
	return (a+b+c+d+e+f+g+h);
}