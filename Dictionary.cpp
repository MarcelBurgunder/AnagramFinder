// Dictionary.cpp

#include <iostream>
#include "Dictionary.h"
#include <string>
#include <list>
#include <cctype>
#include <utility>  // for swap
#include <vector>
#include <algorithm>
using namespace std;

// Here is an array of the first n number of primes, where n represents the size of the alphabet were using
const int ALPHABETSIZE = 26;
const int PRIMES[ALPHABETSIZE] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79,
83, 89, 97, 101 };
using integer = unsigned long long;	// avoids overflow for large composite numbers

class Node		/* every node holds the unique prime number to all sets of anagrams, and the anagrams associated
				to it in a list of strings */
{
public:
	Node(integer p, string word) 
		: m_p(p)
	{
		m_b.push_back(word);
	}
	integer prime() const { return m_p; }
	void addtonode(string word) { m_b.push_back(word); }	// adds a new word to the bucket
	const list<string> &words() const { return m_b; }
private:
	list<string> m_b;
	integer m_p;
};

class DictionaryImpl
{
public:
    DictionaryImpl() {}
    ~DictionaryImpl() {}
    void insert(string word);
    void lookup(string letters, void callback(string)) const;
	void loginsertion(const Node& x, string word, int pos, integer d = 4);
	const Node* findnode(integer pri, int pos, integer d = 4) const;
private:
	vector<Node> m_nodes;
};

integer primeproduct(string letters);
void clean(string& s);

void DictionaryImpl::insert(string word)
{
	clean(word);
	Node s(primeproduct(word), word);
	
	if (m_nodes.empty())
	{
		m_nodes.push_back(s);
		return;
	}

	loginsertion(s, word, m_nodes.size()/2);
	return;
}

void DictionaryImpl::lookup(string letters, void callback(string)) const
{
	clean(letters);
	const Node* s = findnode(primeproduct(letters), (m_nodes.size()/2)-1);

	if (s == nullptr)
		return;			// no anagrams were found
	
	for (list<string>::const_iterator p = s->words().cbegin(); p != s->words().cend(); p++)
		callback(*p);

	return;
}

void DictionaryImpl::loginsertion(const Node& x, string word, int pos, integer d)
{
	if (pos > m_nodes.size() - 1)
	{
		m_nodes.push_back(x);
		return;
	}

	if (x.prime() == m_nodes[pos].prime())
	{
		m_nodes[pos].addtonode(word);
		return;
	}

	if (pos == 0)
	{
		if (x.prime() > m_nodes[pos].prime())
		{
			m_nodes.insert(m_nodes.begin() + 1, x);
			return;
		}
		else
		{
			m_nodes.insert(m_nodes.begin(), x);
			return;
		}
	}
	
	if (x.prime() < m_nodes[pos].prime() && x.prime() > m_nodes[pos - 1].prime())
	{
		m_nodes.insert(m_nodes.begin() + pos, x);
		return;
	}

	integer half = max(m_nodes.size() / d, 1ull);

	if (x.prime() < m_nodes[pos].prime())
	{
		loginsertion(x, word, pos - half, 2 * d);
		return;
	}

	loginsertion(x, word, pos + half, 2 * d);	// if (x.prime() > m_nodes[pos].prime())
	return;
}

const Node* DictionaryImpl::findnode(integer pri, int pos, integer d) const
{
	if (pri == m_nodes[pos].prime())
		return &(m_nodes[pos]);

	integer half = max(m_nodes.size() / d, 1ull);

	if (pri < m_nodes[pos].prime())
	{
		if (pos == 0)
			return nullptr;	// nullptr is returned if prime is less than the least bucket prime
		if (pri > m_nodes[pos - 1].prime())
			return nullptr; // nullptr is returned if prime doesn't exist between two buckets
		return findnode(pri, pos - half, 2 * d);
	}

	if (pos == m_nodes.size() - 1)
		return nullptr;		// nullptr is returned if prime is larger the the largest bucket prime

	return findnode(pri, pos + half, 2 * d); // if (pri > m_nodes[pos].prime())
}

integer primeproduct(string letters)	// computes primeproduct of letters
{
	integer product = 1;
	for (int k = 0; k != letters.size(); k++)
		product *= PRIMES[letters[k] % ALPHABETSIZE];	/* allows every letter to correspond to a unique prime
	number, provided that all lowercase letters are consequtive in numbering (such as in ASCII) */
	return product;
}
    
void clean(string& s)
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
	s.erase(to, s.end());  // same function as removeNonLetters from the anagrams.zip file
} 

//******************** Dictionary functions ********************************

// These functions simply delegate to DictionaryImpl's functions.
// You probably don't want to change any of this code

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

/* Testing shortcuts
	C:\Users\Marcel\Documents\UCLA\Second Year\CS32\Project 4\Project 4\words.txt
*/