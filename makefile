# Author Name: Abhijeet Dutt Srivastava
# Description: Makes executable for Q 2.5 of homework 2

all: hamming_enc.c 
	gcc hamming_enc.c -o hmgenc -Wall -Werror
clean:
	rm -f hmgenc