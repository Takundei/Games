/*
    CH08-320143
    Player.cpp
    Takundei Gora
    t.gora@jacobs-university.de


*/


#include <iostream>
#include "Player.h"

using namespace std;

Player::Player(const string& name): 
    GenericPlayer(name)
{}

Player::~Player()
{}
    
bool Player::IsHitting() const
{
    cout << m_Name << ", do you want a hit? (Y/N): ";
    char response;
    cin >> response;
    return (response == 'y' || response == 'Y');
}

void Player::Win() const
{
    cout << m_Name <<  " wins.\n";
}

void Player::Lose() const
{
    cout << m_Name <<  " loses.\n";
}

void Player::Push() const
{
    cout << m_Name <<  " pushes.\n";
}

