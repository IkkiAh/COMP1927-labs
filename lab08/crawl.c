// crawl.c ... build a graph of part of the web
// Written by John Shepherd, September 2015
// Uses the cURL library and functions by Vincent Sanders <vince@kyllikki.org>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <curl/curl.h>
#include "stack.h"
#include "set.h"
#include "graph.h"
#include "url.h"
#include "url_file.h"

// We want to use a queue
#include "queue.h"

#define BUFSIZE 1024

void setFirstURL(char *, char *);
void normalise(char *, char *, char *, char *, int);
void strlower(char *);

int main(int argc, char **argv)
{
	URL_FILE *handle;
	char buffer[BUFSIZE];
	char baseURL[BUFSIZE];
	char firstURL[BUFSIZE];
	char next[BUFSIZE];
	int  maxURLs;

	if (argc > 2) {
		strcpy(baseURL,argv[1]);
		setFirstURL(baseURL,firstURL);
		maxURLs = atoi(argv[2]);
		if (maxURLs < 40) maxURLs = 40;
	}
	else {
		fprintf(stderr, "Usage: %s BaseURL MaxURLs\n",argv[0]);
		exit(1);
	}
		
	// You need to modify the code below to implement:
	//
	// add firstURL to the ToDo list
    
    Queue ToDo = newQueue();
    enterQueue(ToDo, firstURL);
    
	// initialise Graph to hold up to maxURLs
    
    Graph storedURL = newGraph(maxURLs);
    
	// initialise set of Seen URLs
    
    Set seenURL = newSet();
    
    // while (ToDo list not empty and Graph not filled) {
    int counter = 0;
    
    // Set up the URL Barrier
    char * nxturl = leaveQueue(ToDo);
    char urlBarrier[100] = {0};
    int n = 0;
    int n2 = 0;
    while (n < 3) {
        int c = nxturl[n2];
        urlBarrier[n2] = c;
        if (c == 47) {
            n++;
        }
        n2++;
    }
    enterQueue(ToDo, nxturl);
    
    while (!(emptyQueue(ToDo)) && (nVertices(storedURL) < maxURLs)) {
        
        //    grab Next URL from ToDo list
        char * nxturl = leaveQueue(ToDo);
        
        //printf("**** %s\n", urlBarrier);
        //    if (not allowed) continue
        while (strncmp(nxturl, urlBarrier, n2) != 0) {
            if (!emptyQueue(ToDo)) {
                nxturl = leaveQueue(ToDo);
            } else {
                printf("Have a break, have a kitkat\n");
                break;
            }
        }
        
        if (!(handle = url_fopen(nxturl, "r"))) {
            fprintf(stderr,"Couldn't open %s\n", next);
            exit(1);
        }
        
        //       foreach line in the opened URL {
        //       foreach URL on that line {
        
        while(!url_feof(handle)) {
            url_fgets(buffer,sizeof(buffer),handle);
            //fputs(buffer,stdout);
            strlower(buffer);
            char *cur, link[BUFSIZE], full_link[BUFSIZE];
            cur = buffer;
            while ((cur = nextURL(cur)) != NULL) {
                
                getURL(cur, link, BUFSIZE-1);
                normalise(link, next, baseURL, full_link, BUFSIZE-1);
                
                // if (Graph not filled or both URLs in Graph)
                if ((nVertices(storedURL) != maxURLs) || (isConnected(storedURL, nxturl, full_link) == 1)) {
                    
                    // add an edge from Next to this URL
                    addEdge(storedURL, nxturl, full_link);
                    
                }
                
                // if (this URL not Seen already) {
                if (!isElem(seenURL, full_link)) {
                    
                    //printf("Time to add stuff \n");
                    //add it to the Seen set
                    insertInto(seenURL, full_link);
                    //add it to the ToDo list
                    enterQueue(ToDo, full_link);
                    //printf("+");
                    //showGraph(storedURL, 1);
                    //showQueue(ToDo);
                    if(strstr(full_link, "answers") != NULL) {
                        printf("%s\n", full_link);
                    }
                }
                
                //printf("Found %s\n",full_link);
                cur += strlen(link);
            }
        }
        
        //printf("\n");
        counter++;
        //printf("%d\n", counter);
        
        url_fclose(handle);
        //sleep(1);
        
    }
    
	//    foreach line in the opened URL {
	//       foreach URL on that line {
	//          if (Graph not filled or both URLs in Graph)
	//             add an edge from Next to this URL
	//          if (this URL not Seen already) {
	//             add it to the Seen set
	//             add it to the ToDo list
	//          }
	//       }
    //    }
	//    close the opened URL
	//    sleep(1)
	// }
    
    disposeQueue(ToDo);
    disposeGraph(storedURL);
    
	return 0;
}

// normalise(In,From,Base,Out,N)
// - converts the URL in In into a regularised version in Out
// - uses the base URL to handle URLs like "/..."
// - uses From (the URL used to reach In) to handle "../"
//   - assumes that From has already been normalised
void normalise(char *in, char *from, char *base, char *out, int n)
{
	if (strstr(in, "http") != NULL)
		strcpy(out, in);
	else if (in[0] == '/') {
		strcpy(out, base);
		strcat(out, in);
	}
	else {
		strcpy(out, base);
		strcat(out, "/");
		strcat(out, in);
	}
}

// setFirstURL(Base,First)
// - sets a "normalised" version of Base as First
// - modifies Base to a "normalised" version of itself
void setFirstURL(char *base, char *first)
{
	char *c;
	if ((c = strstr(base, "/index.html")) != NULL) {
		strcpy(first,base);
		*c = '\0';
	}
	else if (base[strlen(base)-1] == '/') {
		strcpy(first,base);
		strcat(first,"index.html");
		base[strlen(base)-1] = '\0';
	}
	else {
		strcpy(first,base);
		strcat(first,"/index.html");
	}
}

// strlower(Str)
// - set all chars in Str to lower-case
void strlower(char *s)
{
	for ( ; *s != '\0'; s++)
		*s = tolower(*s);
}
