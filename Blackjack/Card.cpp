/*
    CH08-320143
    Card.cpp
    Takundei Gora
    t.gora@jacobs-university.de


*/

#include "Card.h"
using namespace std;

Card::Card(rank r, suit s, bool ifu):  m_Rank(r), m_Suit(s), m_IsFaceUp(ifu)
{}

int Card::GetValue() const
{
    int value ;

    if(!m_IsFaceUp)
    {
        value = 0;
    }
    else
    {
        if(m_Rank <= 10)
        {
            value = m_Rank;
        }
        else
        {
            value = 10;
        }
    }


    return value;
}

void Card::Flip()
{
    m_IsFaceUp = !(m_IsFaceUp);
}

//overloads << operator so Card object can be sent to cout
ostream& operator<<(ostream& os, const Card& aCard)
{
    const string RANKS[] = {"0", "A", "2", "3", "4", "5", "6", "7", "8", "9",
                            "10", "J", "Q", "K"
                           };
    const string SUITS[] = {"c", "d", "h", "s"};

    if (aCard.m_IsFaceUp)
    {
        os << RANKS[aCard.m_Rank] << SUITS[aCard.m_Suit];
    }
    else
    {
        os << "XX";
    }

    return os;
}
