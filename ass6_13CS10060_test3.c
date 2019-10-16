int isVowel(char c){
	if(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c =='u'||
		c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U')
		return 1;
	return 0;
}

int main(){
	char c;
	int ep;
	prints("Enter a character: ");
	c = readchar(&ep);
	printchar(c);
	if(isVowel(c)){
		prints(" is a vowel\n");
	}
	else
		prints(" is not a vowel\n");
	return 0;
}