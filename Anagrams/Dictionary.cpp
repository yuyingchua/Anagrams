#include "Dictionary.h"
#include <string>
#include <list>
#include <cctype>
#include <utility>  // for swap
#include <iostream>
using namespace std;

void removeNonLetters(string& s);

// This class does the real work of the implementation
class DictionaryImpl
{
public:
    DictionaryImpl() {}
    ~DictionaryImpl() {}
    void insert(string word);
    void lookup(string letters, void callback(string)) const;
    
    // hash function - returns the index at which item will be inserted in the array
    int getHashIndex(const string& word) const
    {
        unsigned long long hashIndex = 1;
        unsigned long wordLength = word.length();
        for (int i = 0; i < wordLength; i++)
        {
            hashIndex *= primes[word[i] - 'a'];
        }
        return hashIndex % 49999; //chose prime number for fewer collisions
    }
    
private:
    list<list<string>> anagrams[49999];
    
    int primes[26] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101 }; // 26 prime numbers representing each char of the alphabet
    
    bool isAnagram(const string& s1, const string& s2) const
    {
        // eliminate words with differing lengths from the start
        if (s1.length() != s2.length())
            return false;
        
        int frequencies[26] = {0};
        for (int i = 0; i < s1.length(); i++)
        {
            frequencies[s1[i] - 'a']++;
        }
        for (int i = 0; i < s2.length(); i++)
        {
            if (frequencies[s2[i] - 'a'] == 0)
                return false;
            frequencies[s2[i] - 'a']--;
        }
        return true;
    }
};

void DictionaryImpl::insert(string word)
{
    removeNonLetters(word);
    
    int hashIndex = getHashIndex(word);
    if ( ! word.empty())
    {
        if (anagrams[hashIndex].empty())
        {
            anagrams[hashIndex].push_back(list<string>());
            list<list<string>>::iterator itr = anagrams[hashIndex].end();
            --itr;  // itr now points to the last list<string> in the list<list<string>> that anagrams[element] holds.
            
            // chose to set the iterator to end and decrement instead of setting it to begin, as the first
            // word to be mapped to a bucket is more likely to have the most anagrams
            // -- makes lookup more efficient
            
            (*itr).push_back(word);    // newly-created empty list of strings holds all anagrams of the word
        }
        else //there's already a list<string> pushed back onto the list, with anagrams
        {
            list<list<string>>::iterator listOfStringsp;
            for ( listOfStringsp = anagrams[hashIndex].begin(); listOfStringsp != anagrams[hashIndex].end(); listOfStringsp++)
            {
                // push the word onto the end of the current
                // listOfStrings if it is an Anagram of the words
                // currently in that listOfStrings
                if (isAnagram(word, (*(*listOfStringsp).begin())))
                {
                    (*listOfStringsp).push_back(word);
                    break;
                }
            }
            
            // there is not already a list<string> with an anagram of the current word.
            // push_back a new list of strings
            if (listOfStringsp == anagrams[hashIndex].end())
            {
                anagrams[hashIndex].push_back(list<string>());
                list<list<string>>::iterator itr = anagrams[hashIndex].end();
                --itr;
                (*itr).push_back(word);
            }
            
        }
    }
}

void DictionaryImpl::lookup(string letters, void callback(string)) const
{
    if (callback == nullptr)
        return;
    
    removeNonLetters(letters);
    if (letters.empty())
        return;
    
    int hashIndex = getHashIndex(letters);
    list<list<string>>::const_iterator listOfStringsp = anagrams[hashIndex].begin();
    
    for ( ; listOfStringsp != anagrams[hashIndex].end(); listOfStringsp++)
    {
        if (isAnagram(letters, (*(*listOfStringsp).begin())))
        {
            // print out each word in the list<string> that listOfStringsp points to
            // set the wordp iterator to the beginning of the list<string>
            for ( list<string>::const_iterator wordp = (*listOfStringsp).begin(); wordp != (*listOfStringsp).end(); wordp++)
            {
                callback(*wordp);
            }
            break; // after printing out all relevant anagrams
        }
    }
}


void removeNonLetters(string& s)
{
    string::iterator to = s.begin();
    for (string::const_iterator from = s.begin(); from != s.end(); from++)
    {
        if (isalpha(*from))
        {
            *to = tolower(*from);
            to++;
        }
    }
    s.erase(to, s.end());  // chop everything off from "to" to end.
}


//******************** Dictionary functions ********************************

// These functions delegate to DictionaryImpl's functions.

Dictionary::Dictionary()
{
    m_impl = new DictionaryImpl;
}

Dictionary::~Dictionary()
{
    delete m_impl;
}

void Dictionary::insert(string word)
{
    m_impl->insert(word);
}

void Dictionary::lookup(string letters, void callback(string)) const
{
    m_impl->lookup(letters,callback);
}
