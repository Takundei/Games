/*
    CH08-320143
    Hand.cpp
    Takundei Gora
    t.gora@jacobs-university.de


*/

#include "Hand.h"

using namespace std;

Hand::Hand()
{
    m_Cards.reserve(7);
}

Hand::~Hand()
{
    Clear();
}

void Hand::Add(Card* pCard)
{
    m_Cards.push_back(pCard);
}

void Hand::Clear()
{
    //iterate through vector, freeing all memory on the heap
    vector<Card*>::iterator iter = m_Cards.begin();
    for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
    {
        delete *iter;
        *iter = 0;
    }
    //clear vector of pointers
    m_Cards.clear();
}

int Hand::GetTotal() const
{
    int total = 0;
    bool has_ace = false;
    int card_inHand;
    vector<Card*>::const_iterator iter = m_Cards.begin();

    for (iter = m_Cards.begin(); iter != m_Cards.end(); iter++)
    {
        card_inHand = (*iter)->GetValue();
        if(card_inHand == 1)
        {
            has_ace = true;//player has an ace
        }
        total+= card_inHand;

    }
    if(has_ace && total < 12)  //if player has ace and total is low enough
    {
        total+= 10;//ace with value of 1 was already added
    }

    return total;
}
