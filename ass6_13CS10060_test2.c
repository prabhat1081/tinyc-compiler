void bubblesort(int b[],int n);
int main()
{
	int b[100],n,ep,i;
    prints("Enter the size of an array: ");
    n = readi(&ep);

    prints("Enter the elements of the array: " );
    for(i=0;i<n;i++){
        b[i] = readi(&ep);
    }

	prints("Array Before Sorting:\n");
	int *q = &b[0];
	for(i=0;i<n;i++)
	{
		printi(q[i]);
		prints("\n");
	}
	bubblesort(&b[0],n);
	prints("Array After Sorting:\n");
	for(i=0;i<n;i++)
	{
		printi(b[i]);
		prints("\n");
	}

	return 0;
}

void bubblesort(int b[],int n){
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n-1;j++)
		{
			if(b[j] > b[j+1])
			{
				int temp = b[j];
				b[j] = b[j+1];
				b[j+1] = temp;
			}
		}
	}
}