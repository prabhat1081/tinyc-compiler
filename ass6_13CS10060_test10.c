

int main(void) {
  int n, 
    lcv, 
    flag,
    ep; 
 
  prints("Enter value of N > ");
  n = readi(&ep);
  flag = 1;
  for (lcv=2; lcv <= (n / 2); lcv++) {
    if ((n % lcv) == 0) {
      if (flag)
	       prints("The non-trivial factors are: \n");
      flag = 1 > 2;
      printi(lcv);
      prints("\n");
    }
  }
  if (flag){
    printi(n);
    prints(" is prime\n");
  }
  return 0;
}
