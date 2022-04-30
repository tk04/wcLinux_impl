#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


//make linked list to store all words in file
struct word{ // node value in the linked list
    char *word; // pointer to word string
    int count; // # of times word appears in file
    int *indexes; // array of indexes where word appears in file
};

struct words{ //<-- specifies a node in the linked list
    struct word *word; 
    struct words *next; 
};

void append_char(char *str, char ch) {
    int str_len = strlen(str);
    str[str_len] = ch; 
    str[str_len + 1] = '\0'; // mark end of string
}


int convertCase(char *str, int caseType){ //caseType = 0 for lowercase, 1 for uppercase
    int i; 
    if(caseType == 1){
        for(i = 0; i < strlen(str); i++){ 
            str[i] = toupper(str[i]);
        }
    }
    else{ 
        for(i = 0; i < strlen(str); i++){
            str[i] = tolower(str[i]);
        }
    }
    return 0;
}
int removePunctuation(char *str){ 
    int j = 1;
    size_t size = 1;
    //initialize empty string
    char *newStr = malloc(size * sizeof(char));
    newStr[0] = '\0';
    for(int i = 0; i < strlen(str); i++){ // iterate and add all non-punctuation chars to newStr
        if(!ispunct(str[i])){
            size++;
            newStr = realloc(newStr, size * sizeof(char));
            append_char(newStr, str[i]);
            
        }
    }
    strcpy(str, newStr); 
    free(newStr); 
    return 0;
}
int removeNumbers(char *str){ 
    int j = 1;
    size_t size = 1;
    //initialize empty string
    char *newStr = malloc(size * sizeof(char)); 
    newStr[0] = '\0';
    for(int i = 0; i < strlen(str); i++){ 
        if(!isdigit(str[i])){ // add all non-numbers to new string
            size++;
            newStr = realloc(newStr, size * sizeof(char));
            append_char(newStr, str[i]);
        }
    }
    strcpy(str, newStr); 
    free(newStr); 
    return 0;
}


struct words *add_word(struct words *head, struct word *word){ 
    if(head == NULL){ // initiliaze head node if empty, set attributes (word, next)
        head = malloc(sizeof(struct words));
        head->word = word;
        head->next = NULL;
        return head;
    }
    else{ // if head not empty, iterate through list and add word to end
        struct words *temp = head; 

        // check if word == word in head node
        if(strcmp(temp->word->word, word->word) == 0){ 
            // increment head node count and add word index to head node indexes
            temp->word->count++;
            temp->word->indexes = realloc(temp->word->indexes, sizeof(int) * (temp->word->count));
            temp->word->indexes[temp->word->count - 1] = word->indexes[0];

            return head;
        }
        while(temp->next != NULL){
            // check if word is already in the linked-list
            if(strcmp(temp->word->word, word->word) == 0){
                // increment head node count and add word index to head node indexes
                temp->word->count++;
                temp->word->indexes = realloc(temp->word->indexes, sizeof(int) * (temp->word->count));
                temp->word->indexes[temp->word->count - 1] = word->indexes[0];

                return head;
            }
            temp = temp->next;
        }
        // if word not in list, add to end
        temp->next = malloc(sizeof(struct words));
        temp->next->word = word;
        temp->next->next = NULL; // set next to null, to indicate end of list
        
    }
    return head; 
}

void print_word_report(struct words *head, int include_positions, int include_count){
    printf("Word usage report:\n");
    struct words *temp = head; 
    while(temp != NULL){ // iterate through each node in the list, printing the word
        printf("%s\t", temp->word->word);
        if(include_count){ // if include_count is true, print the count
            printf("%d", temp->word->count);
        }
        
        int i;
        if(include_positions){ // if include_positions is true, iterate through the indexes array and print each index
            printf(" @ ");
            for(i = 0; i < temp->word->count; i++){
                if(i == temp->word->count - 1){
                    printf("%d\n", temp->word->indexes[i]);
                }
                else{
                    printf("%d, ", temp->word->indexes[i]);
                }
            }
            
        }else{
            printf("\n");
        }
        temp = temp->next; // grab the next node in the list
    } 
}

int main(int argc, char *argv[]){ 
    
    char *file = argv[1]; // grab file name (1st argument)
    // grab supported arguements from argv
    int contains_c = 0;
    int contains_w = 0;
    int contains_p = 0;
    int contains_v = 0;
    int contains_s = 0;
    if(argc < 2){ // arguements should always be >=2
        printf("Error: No file name given\n");
        exit(1);
    }
    
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-c") == 0){
            contains_c = 1;
        }
        if(strcmp(argv[i], "-w") == 0){
            contains_w = 1;
        }
        if(strcmp(argv[i], "-p") == 0){
            contains_p = 1;
        }
        if(strcmp(argv[i], "-v") == 0){
            fprintf(stderr,"--DEBUG MODE-- Processing option -v \n");
            contains_v = 1;
        }
        if(strcmp(argv[i], "-s") == 0){
            contains_s = 1;
        }
    }
    // -c, -w, and -p are mutually exclusive, exit if more than one is selected
    if((contains_c && contains_w) || (contains_c && contains_p) || (contains_w && contains_p)){
        printf("Error: -c, -w, and -p are mutually exclusive\n");
        exit(1);
    }
    
    
    FILE *fp;
    
    fp = fopen(file, "r"); // open file for reading and store in fp
    if(fp == NULL){ // check if file exists
        printf("Error: File not found\n");
        exit(1);
    }

    // initialize metric variables, and set them to 0    
    int word_count = 0; 
    int char_count = 0;
    int total_char_count = 0;
    size_t index_count = 1;

    struct words *head = NULL;
    char current_char = fgetc(fp); // grab the first character from the file
    char *curr_word= malloc(index_count * sizeof(char)); // holds the current word being read
    curr_word[0] = '\0'; // initialize the current word to empty string
    
    if(contains_v){
        fprintf(stderr,"--DEBUG MODE-- Processing Words\n");
    }
    while(current_char != EOF){ // while the end of the file has not been reached
        
        if(current_char == ' ' || current_char == '\n'){ 
            //remove numbers, punctuation, and make lowercase
            removeNumbers(curr_word);
            removePunctuation(curr_word);
            convertCase(curr_word, 0);
            if(strcmp(curr_word, "")!=0){
                if(contains_v){
                    fprintf(stderr,"--DEBUG MODE-- processing current word: %s\n", curr_word);
                }
                // create a word struct with the current word and set the count to 1, and set the index to the current index
                struct word *word1 = malloc(sizeof(struct word)); 
                word1->word = curr_word;
                word1->count = 1;
                word1->indexes = malloc(sizeof(int));
                word1->indexes[0] = word_count;
            
                head = add_word(head, word1); // add the word to the linked list
            
                
                word_count++;
            }
            char_count = -1; // reset char count (inc. to 0 this iteration)
            // reset the current word, initialize it to empty string
            curr_word = malloc(1 * sizeof(char));
            index_count = 1;
            curr_word[0] = '\0';
            
        }
        else{
            index_count++;
            curr_word = realloc(curr_word, index_count * sizeof(char)); // increase mem. size of the curr_word to fit the next character
            // if current_char not space or newline, append it to the current word
            append_char(curr_word, current_char); 
            
            
            
        }
        // inc the char count, and total char count
        char_count++;
        total_char_count++; 
        current_char = fgetc(fp); // get the next character from the file
    }

    
    fclose(fp); // close file

    
 


    if(contains_s){
        // sort words (from head) in alphabetic order        
        struct words *temp = head; 
        struct words *temp2; 
        while(temp != NULL){ 
            temp2 = temp->next; // grab the next node
            while(temp2 != NULL){ // iterate through all other words,except the one pointing to temp, and compare them to temp. Swap positions if necessary
                if(strcmp(temp->word->word, temp2->word->word) > 0){
                    struct word *temp_word = temp->word; 
                    temp->word = temp2->word; 
                    temp2->word = temp_word;
                }
            // move to the next nodes
                temp2 = temp2->next; 
            }
            temp = temp->next; 
        }
        
        if(!contains_p){
            print_word_report(head, 0,0);
        }
    }
    // print word/char/report(w/ positions) if specified
    if(contains_c){
        printf("\nChar count: %d\n\n", total_char_count);
    }else if(contains_w){
        printf("\nWord count: %d\n\n", word_count);
    }else if(contains_p){
        print_word_report(head, 1,0);
    }


    if(argc == 2 || (argc == 3 && contains_v)){ // if there is only one argument, or if there is a -v argument, print all info collected

        printf("\nWord count: %d\n", word_count);
        printf("\nChar count: %d\n\n", total_char_count);
        
        print_word_report(head, 1,1);
    }
    if(contains_v){
        fprintf(stderr,"\n--DEBUG MODE-- Freeing memory\n");
    }
    // free head and curr_word memory
    free(head);
    free(curr_word);
    if(contains_v){
        printf("--DEBUG MODE-- exiting with 0 status\n");
    }
    exit(0); 
    return 0;
}
