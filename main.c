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
AddElementToHashmap(Hashmap* hashmap, char* key, int value) 
{
    if (hashmap == NULL) {
        printf("PANIC provided hashmap is NULL\n");
        exit(125);
    }
    if (key == NULL) {
        printf("PANIC provided keys is NULL\n");
        exit(124);
    }

    unsigned int hash = CalculateHash(key);
    if (hashmap->list[hash] == NULL) {
        /* add element in position the empty position */
        Element *newElement = malloc(sizeof(Element));
        newElement->key = strdup(key);
        newElement->value = value;
        newElement->next = NULL;
        hashmap->list[hash] = newElement;
        printf("New element for empty position %d (%s:%d)\n", hash, key, value);
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
        newElement->key = strdup(key);
        newElement->value = value;
        element->next = newElement;
        printf("New element for existing position %d (%s:%d)\n", hash, key, value);
        return;
    }
    
    /* Should never arrive here */
    printf("PANIC!");
    exit(127);
}

void
PrintHashmap(Hashmap *hp) {
    if (hp == NULL || hp->list == NULL) {
        printf("PANIC hashmap or hashmap->list is NULL\n");
        exit(126);
    }
    for (int i = 0; i < CAPACITY; i++)
    {
        Element *element = hp->list[i];
        if (element == NULL) {
            printf("%p %d ==> NULL\n", hp, i);
        } else {
            printf("%p %d ==> '%s': %d (%p)\n", hp, i, element->key, element->value, element);
            while (element->next != NULL) {
                element = element->next;
                printf("                    \\_>"
                       " '%s': %d (%p) \n", element->key, element->value, element);
            }
        }
    }
}

/* returns pointer to value or NULL */
int *
GetValueHashmap(Hashmap *hp, char *key)
{
    if (hp == NULL) {
        printf("PANIC provided hashmap is NULL\n");
        exit(125);
    }
    if (key == NULL) {
        printf("PANIC provided keys is NULL\n");
        exit(124);
    }

    unsigned int hash = CalculateHash(key);
    Element *e = hp->list[hash];
    if (e == NULL) {
        return NULL;
    }
    while (e->next != NULL && strcmp(e->key, key) != 0) {
        e = e->next;
    }
    /* e is now a match, or the last element and there is no match */
    if (strcmp(e->key, key) == 0) {
        return &(e->value);
    }
    return NULL;
}

void
DeleteElementHashmap(Hashmap *hp, char *key)
{
    unsigned int hash = CalculateHash(key);
    Element *e = hp->list[hash];
    if (e == NULL) {
        return;
    }
    Element *previous = NULL;
    while (e->next != NULL && strcmp(e->key, key) != 0) {
        previous = e;
        e = e->next;
    }
    /* e is now a match, or the last element and there is no match */
    if (strcmp(e->key, key) == 0) {
        /* e is a match */
        if (previous == NULL) {
            /* Our match is the first element */
            hp->list[hash] = hp->list[hash]->next; // link to the next element
        } else {
            previous->next = e->next;
        }
        free(e->key);
        free(e);
        return;
    }
}

void
CleanupHashmap(Hashmap *hp)
{
    Element *e = NULL;
    for (size_t i = 0; i < hp->cap; i++)
    {
        if (hp->list[i] != NULL) {
            while (hp->list[i]->next != NULL) {
                e = hp->list[i];
                hp->list[i] = hp->list[i]->next;
                free(e->key);
                free(e);
            }
            free(hp->list[i]->key);
            free(hp->list[i]);
            hp->list[i] = NULL;
        }
    }
    free(hp->list);
    hp->list = NULL;
    free(hp);
}

int
main()
{
    char* a[] = {"abc", "xyz", "foo", "bar", "foobar", "oompa", "loompa", 
                 "a", "aa", "ab", "ba", "bb", "b", "1", "2", "3", "4",
                 "A", "AA", "AB", "BA", "BB", "B", "1", "2", "3", "4"};
    Hashmap *hp = NewHashmap();
    for (size_t i = 0; i < 27; i++)
    {
        printf("%s hash: %d\n", a[i], CalculateHash(a[i]));
        AddElementToHashmap(hp, a[i], strlen(a[i]));
    }
    
    PrintHashmap(hp);

    int *val = GetValueHashmap(hp, "abc");
    if (val != NULL) {
        printf("The value for 'abc' is %d\n", *val);
    } else {
        printf("No value found for 'abc'\n");
    }

    AddElementToHashmap(hp, "abc", 1234);

    val = GetValueHashmap(hp, "abc");
    if (val != NULL) {
        printf("The value for 'abc' is %d\n", *val);
    } else {
        printf("No value found for 'abc'\n");
    }

    val = GetValueHashmap(hp, "4");
    if (val != NULL) {
        printf("The value for '4' is %d\n", *val);
    } else {
        printf("No value found for '4'\n");
    }

    val = GetValueHashmap(hp, "b");
    if (val != NULL) {
        printf("The value for 'b' is %d\n", *val);
    } else {
        printf("No value found for 'b'\n");
    }

    val = GetValueHashmap(hp, "oompa");
    if (val != NULL) {
        printf("The value for 'oompa' is %d\n", *val);
    } else {
        printf("No value found for 'oompa'\n");
    }

    DeleteElementHashmap(hp, "oompa");

    val = GetValueHashmap(hp, "oompa");
    if (val != NULL) {
        printf("The value for 'oompa' is %d\n", *val);
    } else {
        printf("No value found for 'oompa'\n");
    }

    PrintHashmap(hp);

    CleanupHashmap(hp);
    hp = NULL;
    // PrintHashmap(hp);
    return 0;
}