int s;
int *p;

int main()
{
	int a[10],i;
	p = &a[1];
	p[1] = 1;
	prints("Testing Globals\n");
	printi(p[1]);
	prints("\n");
	p[1]++;
	i = p[1];
	printi(p[1]);
	prints("\n");
	//i = p[2];
	return 0;
}