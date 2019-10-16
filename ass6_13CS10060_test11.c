int binary(int a[],int n,int m,int l,int u);
int main()
{
    int a[100],i,n,m,c,l,u,ep,k;

    prints("Enter the size of an array: ");
    n = readi(&ep);

    prints("Enter the elements of the array: " );
    for(i=0;i<n;i++){
        a[i] = readi(&ep);
    }

    prints("Enter the number to be search: ");
    m = readi(&ep);

    l=0,u=n-1;
    c = binary(a,n,m,l,u);
    if(c < 0)
        prints("Number is not found.\n");
    else
    {
        prints("Number is found at position ");
        printi(c+1);
        prints("\n");
    }

    return 0;
}

int binary(int a[],int n,int m,int l,int u){
    int mid,c = 0;
    if(l<=u){
        mid=(l+u)/2;
        if(m==a[mid]){
          c = mid;
        }
        else if(m<a[mid]){
          return binary(a,n,m,l,mid-1);
        }
        else
          return binary(a,n,m,mid+1,u);
    }
    else
        return -1;
}
