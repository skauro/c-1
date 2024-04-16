#include <iostream>
#include "DateTime.h"
#include "Items.h"
#include "Headers.h"
#include "ctype.h"
#include "string.h"
#include "ICS0017DataSource.h"

// IMPORTANT: follow the given order of *.h files: ICS0017DataSource.h must be
// the last

#pragma warning ( disable : 4996 )

void PrintDataStructure(HEADER_B* pHeaderB)
{
    int i = 1;
    while (pHeaderB != nullptr)
    {
       // printf("First word: %c\n", pHeaderB->cBegin);

        headerA* pA = pHeaderB->pHeaderA;  

        while (pA != nullptr)
        {
            //printf("Second word: %c\n", pA->cBegin);

            ITEM6* pItem6 = (ITEM6*)pA->pItems;
            while (pItem6 != nullptr)
            {
                printf("%d) %s %lu ", i, pItem6->pID, pItem6->Code);
                printf("%d %d %d\n", pItem6->Date.Day, pItem6->Date.Month, pItem6->Date.Year);

                pItem6 = pItem6->pNext;
                i++;
            }

            pA = pA->pNext;
            
        }
        pHeaderB = pHeaderB->pNext;
    }
    printf("\n");
    if (i == 0)
        printf("Empty\n");
}


// Function to check if an item with the same ID already exists in the data structure
bool IsIDUnique(HEADER_B* pHeaderB, const char* pNewItemID) 
{
    if (pNewItemID == nullptr)
    {
        return true;
    }
    while (pHeaderB != nullptr) 
    {
        HEADER_A* pA = pHeaderB->pHeaderA;
        while (pA != nullptr) 
        {
            ITEM6* pItem6 = (ITEM6*)pA->pItems;
            while (pItem6 != nullptr )
            {
                if (pItem6->pID != nullptr)
                {
                    if (strcmp(pItem6->pID, (char*)pNewItemID) == 0)
                    {
                        return false; // ID already exists
                    }
                }
                pItem6 = pItem6->pNext;
            }
            pA = pA->pNext;
        }
        pHeaderB = pHeaderB->pNext;
    }
    return true; // ID is unique
}

// Function to validate ID based on formatting rules 
bool IsValidID(const char* pID) 
{
    if (pID == nullptr)
        return true;
    // ID must be at least 2 characters long, and start with a letter
    if (strlen(pID) < 2 || !isalpha(pID[0])) 
    {
        return false;
    }
    return true;
}

HEADER_B* InsertItem(HEADER_B* p, char* pNewItemID = nullptr)
{
    ITEM6* newItem;

    if (pNewItemID == nullptr || pNewItemID[0] == '\0')
    {
        // Generate a new ID using GetItem
        newItem = (ITEM6*)(GetItem(6));
    }
    else
    {
        newItem = (ITEM6*)(GetItem(6, pNewItemID));
    }

    if (!IsIDUnique(p, pNewItemID) || !IsValidID(pNewItemID))
    {
        // Throw an exception if the ID is not unique or does not follow formatting rules
        throw std::runtime_error("Invalid or non-unique ID");
    }

    // Find the correct position to insert the new item in alphabetical order
    HEADER_A* pA = p->pHeaderA;
    if (pA != nullptr)
    {
        ITEM6* current = (ITEM6*)pA->pItems;
        ITEM6* previous = nullptr;

        while (current != nullptr)
        {
            if (strcmp(newItem->pID, current->pID) <= 0)
            {
                // Insert the new item before the current item
                newItem->pNext = current;
                if (previous != nullptr)
                {
                    previous->pNext = newItem;
                }
                else
                {
                    // The new item should be the first item in the list
                    pA->pItems = (void*)newItem;
                }
                return p;
            }

            previous = current;
            current = current->pNext;
        }
        // If the new item should be inserted at the end of the list, add it there
        previous->pNext = newItem;
    }
    else
    {
        // If pA is null, create a new pA and add the new item to it
        p->pHeaderA = (HEADER_A*)malloc(sizeof(HEADER_A));
        p->pHeaderA->pNext = nullptr;
        p->pHeaderA->pItems = (void*)newItem;
    }

    return p; // Return the pointer to the modified data structure
}


// Function to remove an item from the data structure
HEADER_B*  RemoveItem(HEADER_B* p, char* pItemID) 
{
    if (p == nullptr) 
    {
        throw std::runtime_error("Data structure is empty");
    }

    // Find the item to be removed and its parent
    ITEM6* prevItem = nullptr;
    ITEM6* currentItem = (ITEM6*)p->pHeaderA->pItems; 

    while (currentItem != nullptr) 
    {
        if (strcmp(currentItem->pID, pItemID) == 0) 
        {

            // Remove the item from the list
            if (prevItem != nullptr) 
            {
                prevItem->pNext = currentItem->pNext;
            }
            else 
            {
                p->pHeaderA->pItems = currentItem->pNext; // Update the header's item pointer
            }

            // Deallocate memory for the item
            delete[] currentItem->pID;
            delete currentItem;

            return p; // Return the modified data structure
        }

        prevItem = currentItem;
        currentItem = currentItem->pNext;
    }

    // If we reach here, the item was not found
    throw std::runtime_error("Item with the specified ID does not exist");
}

int main()
{
	HEADER_B* p = GetStruct1(6, 30);
	ITEM6* pNewItem = (ITEM6*)GetItem(6);

	PrintDataStructure(p);

    // Insert an item with a specific ID
    p = InsertItem(p, (char*)"New Id");

    PrintDataStructure(p);

    // Insert an item with an automatically generated ID
    p = InsertItem(p);

    PrintDataStructure(p);


    try 
    {
        // Remove an item by specifying its ID
        p = RemoveItem(p, (char*)"M Ba");
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }



	return 0;
}