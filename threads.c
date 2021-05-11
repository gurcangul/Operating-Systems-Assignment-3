#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>

#define BOOKNUMBER 10
#define MAX_THREAD 19
#define BILLION 1E9
#define LINE_NUMBER 851	//number of line in "englishstopwords.txt"

char *bookList[BOOKNUMBER];
char *allWordsArray[LINE_NUMBER];
int frequencyList[LINE_NUMBER][BOOKNUMBER];

void readingFileToMemory(){
	// all words in "englishstopwords.txt" file are assigned to allWordsArray
  FILE *fptr = NULL;
  int i = 0;
  int tot = 0;
	char *listOfWords;
  fptr = fopen("englishstopwords.txt", "r");
  while(i<LINE_NUMBER)
	{
		listOfWords=(char *)malloc(sizeof(char) * 30);
		fscanf(fptr, "%s", listOfWords);
		allWordsArray[i] = (char *)malloc(sizeof(char) * strlen(listOfWords));
		allWordsArray[i] = listOfWords;
    i++;
		if(feof(fptr))
			break;
  }
	fclose(fptr);
}

void books(){
	// this function assigns all books' name to bookList array
	char bookName[10]="bookX.txt";
    char *x;

  	int l, i;
	for (i = 1; i < 10; i++) {
		bookName[4] = i + '0' ;
		l = strlen(bookName);
		x = (char *)malloc(l + 1);
		strcpy(x, bookName);
		bookList[i-1] = x;
	}
	bookList[9]="book10.txt";
}

char *helperSubstring(char *string, int location, int length){
    char *p;
    int c;
    p = malloc(length+1);

    if (p == NULL)
    {
        printf("Unable to allocate memory.\n");
        exit(1);
    }
    for (c = 0; c < length; c++)
    {
        *(p+c) = *(string+location-1);
        string++;
    }
    *(p+c) = '\0';
    return p;
}


int find(char *dword,char *listOfWords){
	// this method finds all substring of all words in all books
    int location=1;
    char  *p;
    int strLength=strlen(listOfWords);
    int a=strLength;
    int length=1;
    int bool=0;
		int c;

    while (location <= strLength)
    {
        while (length <= a)
        {
            p = helperSubstring(listOfWords, location, length);
            int result = strcmp(dword,p);
            if(result == 0){
							 bool=1;
						}
            length++;
						free(p);
        }
        a--;
        location++;
        length = 1;
    }
    return bool;
}

void bookOperations(char *bookName, int i){
	FILE *fptr = NULL;
	char *listOfWords;
	char *listOfWords2;
	fptr = fopen(bookName, "r");
	listOfWords=(char *)malloc(255*sizeof(char));
	int numberOfWords=1; // numberOfWords is a number of words in a book
	while(1){
		if(feof(fptr))
				break;
		fscanf(fptr, "%s", listOfWords);
		numberOfWords++;
	}
	fclose(fptr);
	fptr = fopen(bookName, "r");
	char *listOfWordsArray[numberOfWords];
	for(int s=0; s<numberOfWords; s++)
	{
			listOfWords=(char *)malloc(255*sizeof(char));
			listOfWords2=(char *)malloc(255*sizeof(char));
			fscanf(fptr, "%s", listOfWords);
			listOfWordsArray[s] = (char *)malloc(sizeof(char)*255);
			int t = 0;
			while (listOfWords[t]!='\0')
			{
					listOfWords2[t]=tolower(listOfWords[t]);	// converted to all characters to lowercase character
					t++;
			}
			listOfWordsArray[s] = listOfWords2;	// converted word is assigned to listOfWordsArray that is hold all words in a book
	}
	fclose(fptr);
	for(int a=0; a<LINE_NUMBER; a++){
		int frequency = 0;	// number of searching word in a book
		for(int x = 0; x<numberOfWords; x++){

			int h = find(allWordsArray[a],listOfWordsArray[x]);
			if(h==1)	// if the word is equal to word in the book
				frequency++;
		}
		frequencyList[a][i] = frequency;
	}
}

void *readAllBooks(void *argv){
	books();
	int k= *((int*)argv);
	for(int i =k * BOOKNUMBER / MAX_THREAD; i<(k+1) * BOOKNUMBER / MAX_THREAD;i++){	// one word is searching in all books.
		bookOperations(bookList[i], i);
	}
}

int main()
{
	  readingFileToMemory();

		pthread_t thread_array[MAX_THREAD]; 	// number of thread array

		struct timespec requestStart, requestEnd;
    // Timer is started
    clock_gettime(CLOCK_REALTIME, &requestStart);
    printf("frequency calculation started\n");

		// number of MAX_THREAD threads are created
		for(int i=0;i<MAX_THREAD;i++){
			int* j = malloc(sizeof(int));
      *j=i;	// j is thread number
			pthread_create(&thread_array[i],NULL,readAllBooks,j);
		}


		for(int i=0;i<MAX_THREAD;i++)
			pthread_join(thread_array[i],NULL);


		// Timer is ended
		clock_gettime(CLOCK_REALTIME, &requestEnd);
    printf("frequency calculation ended\n");

    // Calculate time
    double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;

		printf("frequency of the word %s in all books:\n",allWordsArray[5]);
    for(int i=0; i<BOOKNUMBER; i++){
        printf("%d - ", frequencyList[5][i]);
    }
    printf("\nfrequency of the word %s in all books:\n", allWordsArray[745]);
    for(int i=0; i<BOOKNUMBER; i++){
        printf("%d - ", frequencyList[745][i]);
    }


		printf("\nTotal time spent %lf, for %d threads\n", accum, MAX_THREAD);

	return 0;
}
