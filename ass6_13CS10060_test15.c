/**
*  This file is used for testing conditional statements
*  It calculates the maximu of four numbers
*/

int main()
{
    int a,b,c,d,ep;
    prints("Enter four numbers:\n");
    a = readi(&ep);
    b = readi(&ep);
    c = readi(&ep);
    d = readi(&ep);
    int max;

    if(a>b)
    {

        if(a>c)
        {

            if(a>d)
                max = a;
            else
                max = d;

        }
        else
        {
            if(c>d)
                max = c;
            else
                max = d;
        }
    }
    else
    {
        if(b>c)
        {
            if(b>d)
                max = b;
            else
                max = d;
        }
        else
        {
            if(c>d)
                max = c;
            else
                max = d;
        }
    }
    prints("The maximum of the four is : ");
    printi(max);
    prints("\n");
	return 0;
}

