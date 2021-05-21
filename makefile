default: arraylist

arraylist:
	gcc -Wall -o prog -g mapElem.c mapArrayList.c main.c 

clean:
	rm -f ./prog

