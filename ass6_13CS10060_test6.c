int main()
{
  int m, n, p, q, c, d, k, sum = 0,ep;
  int first[10][10], second[10][10], multiply[10][10];
 
  prints("Enter the number of rows and columns of first matrix\n");
  m = readi(&ep);
  n = readi(&ep);
  prints("Enter the elements of first matrix\n");
 
  for (c = 0; c < m; c++)
    for (d = 0; d < n; d++)
      first[c][d] = readi(&ep);
 
  prints("Enter the number of rows and columns of second matrix\n");
  p = readi(&ep);
  q = readi(&ep);
 
  if (n != p)
    prints("Matrices with entered orders can't be multiplied with each other.\n");
  else
  {
    prints("Enter the elements of second matrix\n");
 
    for (c = 0; c < p; c++)
      for (d = 0; d < q; d++)
        second[c][d] = readi(&ep);
 
    for (c = 0; c < m; c++) {
      for (d = 0; d < q; d++) {
        for (k = 0; k < p; k++) {
          sum = sum + first[c][k]*second[k][d];
        }
 
        multiply[c][d] = sum;
        sum = 0;
      }
    }
 
    prints("Product of entered matrices:-\n");
 
    for (c = 0; c < m; c++) {
      for (d = 0; d < q; d++){
        printi(multiply[c][d]);
        prints("\t");
      }
 
      prints("\n");
    }
  }
 
  return 0;
}