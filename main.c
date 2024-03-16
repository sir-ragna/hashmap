#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct element {
    char *key;
    int value;
    struct element *next;
} Element;

typedef struct hashmap {
    unsigned int len;
    unsigned int cap;
    Element** list;
} Hashmap;

#define CAPACITY 10

Hashmap*
NewHashmap() {
    Hashmap* hp = malloc(sizeof(Hashmap));
    hp->len = 0;
    hp->cap = CAPACITY;
    hp->list = calloc(sizeof(Element), CAPACITY);
    return hp;
}

unsigned int
CalculateHash(char* key) {
    unsigned int result = 0;
    for (; *key != '\0'; key++)
    {
        char c = key[0] - 65;
        result += c * c;
    }
    return result % CAPACITY;
}

void
AddElementToHashmap(Hashmap* hashmap, char* key, int value) {
    if (hashmap->len == hashmap->cap) {
        printf("Hashmap is full\n");
        return;
    }

    unsigned int hash = CalculateHash(key);
    if (hashmap->list[hash] == NULL) {
        /* add element in position the empty position */
        Element *newElement = malloc(sizeof(Element));
        newElement->key = malloc(strlen(key) + 1);
        strcpy(newElement->key, key);
        newElement->key[strlen(key)] = '\0';
        newElement->value = value;
        newElement->next = NULL;
        hashmap->list[hash] = newElement;
        hashmap->len++;
        printf("added element in posistion %d\n", hash);
        return;
    }

    /* another element already exists here, search through the linked list
     * for the first match or NULL */
    Element *element = hashmap->list[hash];
    while (element->next != NULL && strcmp(element->key, key) != 0) {
        element = element->next;
    }
    /* element is now either a match, or the last non-matching element */
    if (strcmp(element->key, key) == 0) {
        printf("Updating the value for %s from %d to %d\n", key, element->value, value);
        element->value = value;
        return;
    }
    if (element->next == NULL) {
        /* we don't have any match in this position
         * add another element to the linked list */
        Element *newElement = (Element *)malloc(sizeof(Element));
        newElement->next = NULL;
        newElement->key = key;
        newElement->value = value;
        element->next = newElement;
        printf("Created new element %p, %s=%d\n", newElement, key, value);
        return;
    }
    
    /* Should never arrive here */
    printf("PANIC!");
    exit(127);
}

void
PrintHashmap(Hashmap *hp) {
    for (int i = 0; i < CAPACITY; i++)
    {
        Element *element = hp->list[i];
        if (element == NULL) {
            printf("%p %d ==> NULL\n", hp, i);
        } else {
            printf("%p %d ==> '%s': %d (%p)\n", hp, i, element->key, element->value, element);
            while (element->next != NULL) {
                element = element->next;
                printf("                  -> '%s': %d (%p) \n", element->key, element->value, element);
            }
        }
    }
}

int
main()
{
    char* a[] = {"abc", "xyz", "foo", "bar", "foobar", "oompa", "loompa", "a", "aa", "ab", "ba", "bb", "b"};
    Hashmap *hp = NewHashmap();
    for (size_t i = 0; i < 13; i++)
    {
        //printf("%s hash: %d\n", a[i], CalculateHash(a[i]));
        AddElementToHashmap(hp, a[i], strlen(a[i]));
    }

    
    PrintHashmap(hp);
    return 0;
}