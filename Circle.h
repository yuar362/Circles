#ifndef CIRCLE_H
#define CIRCLE_H

#include <Windows.h>

class Game;

class SimpleCircle
{
protected:	
	POINT m_center;
	long int m_radius;
    Game* m_game;
public:
    static long int MIN_RADIUS;
    static long int MAX_RADIUS_BLACK;
    static long int MAX_RADIUS_RED;

    SimpleCircle(const POINT _center, const long int _radius, Game* _game);
	const POINT& GetCenter() const;
	long int GetRadius() const;
	void Draw(HDC _dc, HBRUSH _brush, HPEN _pen) const;
    void DefaultDraw(HDC _dc) const;            ///< Рисует заданными пером и кистью
    void SetCenter(const POINT& _new_center);
};



class KillerCircle : public SimpleCircle
{
protected:
	POINT m_velocity;
public:
    static long int MAX_VELOCITY;

    KillerCircle(const POINT _center, const long int _radius, Game* _game, const POINT _velocity);
    void Move(const POINT& _top_left_border_point, const POINT& _down_right_border_point);
	const POINT& GetVelocity() const;
};

#endif