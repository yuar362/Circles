#include "Circle.h"
#include "Game.h"

long int SimpleCircle::MIN_RADIUS = 5;
long int SimpleCircle::MAX_RADIUS_BLACK = 10;
long int SimpleCircle::MAX_RADIUS_RED = 100;

long int KillerCircle::MAX_VELOCITY = 10;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleCircle::SimpleCircle(const POINT _center, const long int _radius, Game* _game) :
m_center(_center),
m_radius(_radius),
m_game(_game)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const POINT& SimpleCircle::GetCenter() const
{
	return m_center;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long int SimpleCircle::GetRadius() const
{
	return m_radius;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SimpleCircle::Draw(HDC _dc, HBRUSH _brush, HPEN _pen) const
{
	SelectObject(_dc, _pen);
	SelectObject(_dc, _brush);	
    DefaultDraw(_dc);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SimpleCircle::DefaultDraw(HDC _dc) const
{
    POINT center_client = m_center;
    ScreenToClient(m_game->GetWindow(), &center_client);
    Ellipse(_dc,
        center_client.x - m_radius,
        center_client.y + m_radius,
        center_client.x + m_radius,
        center_client.y - m_radius);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SimpleCircle::SetCenter(const POINT& _new_center)
{
    m_center = _new_center;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
KillerCircle::KillerCircle(const POINT _center, const long int _radius, Game* _game, const POINT _velocity) : SimpleCircle(_center, _radius, _game),
m_velocity(_velocity)
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KillerCircle::Move(const POINT& _top_left_border_point, const POINT& _down_right_border_point)
{
	m_center.x += m_velocity.x;
	m_center.y += m_velocity.y;

    if (m_center.x <= m_radius + _top_left_border_point.x && m_center.y <= m_radius + _top_left_border_point.y)
    {
        long int speed = 0;
        speed = m_game->random_int(1, KillerCircle::MAX_VELOCITY);
        m_velocity.x = speed;
        m_velocity.y = speed;
    }
    else if (m_center.x <= m_radius + _top_left_border_point.x && m_center.y >= _down_right_border_point.y - m_radius)
    {
        long int speed = 0;
        speed = m_game->random_int(1, KillerCircle::MAX_VELOCITY);
        m_velocity.x = speed;
        m_velocity.y = -speed;
    }
    else if (m_center.x >= _down_right_border_point.x - m_radius && m_center.y <= m_radius + _top_left_border_point.y)
    {
        long int speed = 0;
        speed = m_game->random_int(1, KillerCircle::MAX_VELOCITY);
        m_velocity.x = -speed;
        m_velocity.y = speed;
    }
    else if (m_center.x >= _down_right_border_point.x - m_radius && m_center.y >= _down_right_border_point.y - m_radius)
    {
        long int speed = 0;
        speed = m_game->random_int(1, KillerCircle::MAX_VELOCITY);
        m_velocity.x = -speed;
        m_velocity.y = -speed;
    }
    else if (m_center.x <= m_radius + _top_left_border_point.x)
	{
        POINT velocity = { 0, 0 };
        while (!velocity.x && !velocity.y)
        {
            velocity.x = m_game->random_int(0, KillerCircle::MAX_VELOCITY);
            velocity.y = m_game->random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
        }
        m_velocity = velocity;
	}
    else if (m_center.x >= _down_right_border_point.x - m_radius)
    {
        POINT velocity = { 0, 0 };
        while (!velocity.x && !velocity.y)
        {
            velocity.x = m_game->random_int(-KillerCircle::MAX_VELOCITY, 0);
            velocity.y = m_game->random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
        }
        m_velocity = velocity;
    }
    else if (m_center.y <= m_radius + _top_left_border_point.y)
	{
        POINT velocity = { 0, 0 };
        while (!velocity.x && !velocity.y)
        {
            velocity.x = m_game->random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
            velocity.y = m_game->random_int(0, KillerCircle::MAX_VELOCITY);
        }
        m_velocity = velocity;
	}
    else if (m_center.y >= _down_right_border_point.y - m_radius)
    {
        POINT velocity = { 0, 0 };
        while (!velocity.x && !velocity.y)
        {
            velocity.x = m_game->random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
            velocity.y = m_game->random_int(-KillerCircle::MAX_VELOCITY, 0);
        }
        m_velocity = velocity;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const POINT & KillerCircle::GetVelocity() const
{
	return m_velocity;
}
