/*
Copyright (c) 2016 Florent VAIN, Thomas BLANC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:


The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/*!
 * \file heal.cpp
 * \brief Heal c++ source file
 */

#include "heal.h"
#include "character.h"

/*!
 * \brief Constructor
 * \param x float : X position
 * \param y float : Y position
 * \param w int : Width of the heal
 * \param h int : height of the heal
 * \param dx float : X speed
 * \param dy float : Y speed
 * \param healingPower int : Health value given back to the player.
 */
Heal::Heal(float x, float y, int w, int h, float dx, float dy, int healingPower)
    : Bonus(x,y,w,h,dx,dy,BONUS_TYPE_HEALTH),
      m_healingPower(healingPower)
{
}

/*!
 * \brief Apply the heal effect on a Character.
 * \param charact Character on wich the effect of the heal is applied.
 */
void Heal::apply(Character *player) {
    player->addLife(m_healingPower);
}
