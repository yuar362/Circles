#include "Game.h"
#include <ctime>
#include <algorithm>

Game::Game(HWND _window) :
m_stop_while_moving(false),
m_stop_while_minimized(false),
m_hwnd(_window),
m_black_brush(CreateSolidBrush(RGB(0, 0, 0))),
m_red_brush(CreateSolidBrush(RGB(255, 0, 0))),
m_black_pen(CreatePen(PS_SOLID, 1, RGB(0, 0, 0))),
m_red_pen(CreatePen(PS_SOLID, 1, RGB(255, 0, 0)))
{
	srand(clock());
    RECT window_rect;
    GetClientRect(m_hwnd, &window_rect);
    m_down_right_border = { window_rect.right - window_rect.left, window_rect.bottom - window_rect.top };
    m_center = { (LONG)(0.5 * m_down_right_border.x), (LONG)(0.5 * m_down_right_border.y) };
    m_top_left_border = { 0, 0 };
    ClientToScreen(m_hwnd, &m_down_right_border);
    ClientToScreen(m_hwnd, &m_top_left_border);
    ClientToScreen(m_hwnd, &m_center);

    POINT velocity = { 0, 0 };
    while (!velocity.x && !velocity.y)
    {
        velocity.x = random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
        velocity.y = random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
    }


    m_killer_circle = std::make_shared<KillerCircle>(KillerCircle(m_center, random_int(SimpleCircle::MIN_RADIUS, SimpleCircle::MAX_RADIUS_BLACK), this, velocity));

    HDC hdc = GetDC(m_hwnd);
    m_killer_circle->Draw(hdc, m_black_brush, m_black_pen);
    ReleaseDC(m_hwnd, hdc);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Game& Game::GetGame(HWND _window)
{
    static Game game(_window);
    return game;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::CalculateChange()
{
    if (m_stop_while_moving || m_stop_while_minimized)
        return;

    if (!m_killer_circle)
    {
        POINT velocity = { 0, 0 };
        while (!velocity.x && !velocity.y)
        {
            velocity.x = random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
            velocity.y = random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
        }

        m_killer_circle = std::make_shared<KillerCircle>(KillerCircle(m_center, random_int(SimpleCircle::MIN_RADIUS, SimpleCircle::MAX_RADIUS_BLACK), this,
                                                         velocity));
    }

    HDC hdc = GetDC(m_hwnd);    
    SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
    SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    m_killer_circle->DefaultDraw(hdc);
    m_killer_circle->Move(m_top_left_border, m_down_right_border);
    CalculateCollisions(hdc);
    m_killer_circle->Draw(hdc, m_black_brush, m_black_pen);
    ReleaseDC(m_hwnd, hdc);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::CreateSimpleCircle(POINT & _circle_center)
{
    ClientToScreen(m_hwnd, &_circle_center);
    SimpleCircle circle = SimpleCircle(_circle_center, random_int(SimpleCircle::MIN_RADIUS, SimpleCircle::MAX_RADIUS_RED), this);
    HDC hdc = GetDC(m_hwnd);
    SelectObject(hdc, (HPEN)GetStockObject(BLACK_PEN));
    SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));
    circle.Draw(hdc, m_red_brush, m_red_pen);
    m_x_sorted_circles.insert(std::make_pair(_circle_center.x, circle));
    ReleaseDC(m_hwnd, hdc);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RedrawAll(const HDC _hdc) const
{
    SelectObject(_hdc, (HPEN)GetStockObject(BLACK_BRUSH));
    SelectObject(_hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));

    if (m_killer_circle)
        m_killer_circle->Draw(_hdc, m_black_brush, m_black_pen);

    SelectObject(_hdc, m_red_pen);
    SelectObject(_hdc, m_red_brush);
    
	for (const auto& circle : m_x_sorted_circles)
		circle.second.DefaultDraw(_hdc);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::ChangeBorder()
{
    CangeWindowPos();
    RECT client_rect;
    GetClientRect(m_hwnd, &client_rect);

    m_down_right_border = { client_rect.right - client_rect.left, client_rect.bottom - client_rect.top };
    m_center = { (LONG)(0.5 * m_down_right_border.x), (LONG)(0.5 * m_down_right_border.y) };

    ClientToScreen(m_hwnd, &m_down_right_border);
    ClientToScreen(m_hwnd, &m_center);
    if (m_killer_circle)
    {
        POINT killer_circle_center = m_killer_circle->GetCenter();
        const long int killer_circle_radius = m_killer_circle->GetRadius();
        if (killer_circle_center.x + killer_circle_radius > m_down_right_border.x || killer_circle_center.y + killer_circle_radius > m_down_right_border.y ||
            killer_circle_center.x - killer_circle_radius < m_top_left_border.x || killer_circle_center.y - killer_circle_radius < m_top_left_border.y)
        {
            m_killer_circle = nullptr;
        }
    }

    if (!m_killer_circle)
    {
        POINT velocity = { 0, 0 };
        while (!velocity.x && !velocity.y)
        {
            velocity.x = random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
            velocity.y = random_int(-KillerCircle::MAX_VELOCITY, KillerCircle::MAX_VELOCITY);
        }

        m_killer_circle = std::make_shared<KillerCircle>(KillerCircle(m_center, random_int(SimpleCircle::MIN_RADIUS, SimpleCircle::MAX_RADIUS_BLACK), this,
            velocity));
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::CalculateCollisions(HDC _hdc)
{
    const long int killer_circle_radius = m_killer_circle->GetRadius();
    const POINT& killer_circle_center = m_killer_circle->GetCenter();
    long int min_x = killer_circle_center.x - killer_circle_radius - SimpleCircle::MAX_RADIUS_RED;
    long int max_x = killer_circle_center.x + killer_circle_radius + SimpleCircle::MAX_RADIUS_RED;

    SelectObject(_hdc, (HPEN)GetStockObject(WHITE_PEN));
    SelectObject(_hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    bool no_collision = true;
    for (auto circle = m_x_sorted_circles.lower_bound(min_x), upper = m_x_sorted_circles.upper_bound(max_x); circle != upper;)
        {
            const POINT& circle_center = circle->second.GetCenter();
            long int circle_radius = circle->second.GetRadius();
            if ((killer_circle_center.x - circle_center.x)*(killer_circle_center.x - circle_center.x) +
                (killer_circle_center.y - circle_center.y)*(killer_circle_center.y - circle_center.y) <=
                (circle_radius + killer_circle_radius) * (circle_radius + killer_circle_radius))
            {
                circle->second.DefaultDraw(_hdc);
                circle = m_x_sorted_circles.erase(circle);
                no_collision = false;
            }
            else
                ++circle;
        }

    if (no_collision)
        return;

    min_x = killer_circle_center.x - killer_circle_radius - 3 * SimpleCircle::MAX_RADIUS_RED;
    max_x = killer_circle_center.x + killer_circle_radius + 3 * SimpleCircle::MAX_RADIUS_RED;
    SelectObject(_hdc, m_red_pen);
    SelectObject(_hdc, m_red_brush);
    for (auto circle = m_x_sorted_circles.lower_bound(min_x), upper = m_x_sorted_circles.upper_bound(max_x); circle != upper; ++circle)
    {
        circle->second.DefaultDraw(_hdc);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::SetWindowMoving(bool move)
{
    m_stop_while_moving = move;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::CangeWindowPos()
{
    POINT prev_top_left_border = m_top_left_border;
    m_top_left_border = { 0, 0 };

    ClientToScreen(m_hwnd, &m_top_left_border);
    POINT shift = { m_top_left_border.x - prev_top_left_border.x, m_top_left_border.y - prev_top_left_border.y };

    if (shift.x || shift.y)
    {
        std::multimap<long int, SimpleCircle> new_x_sorted_circles;
        for (auto circle = m_x_sorted_circles.begin(), end = m_x_sorted_circles.end(); circle != end; ++circle)
        {
            POINT center_pos = circle->second.GetCenter();
            center_pos.x += shift.x;
            center_pos.y += shift.y;
            circle->second.SetCenter(center_pos);
            new_x_sorted_circles.insert(std::make_pair(center_pos.x, circle->second));
        }
        m_x_sorted_circles = new_x_sorted_circles;

        
        if (m_killer_circle)
        {
            POINT killer_circle_center = m_killer_circle->GetCenter();
            killer_circle_center.x += shift.x;
            killer_circle_center.y += shift.y;
            m_killer_circle->SetCenter(killer_circle_center);
        }        
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HWND Game::GetWindow() const
{
    return m_hwnd;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long int Game::random_int(long int lower_bound, long int upper_bound) const
{
    if (lower_bound >= upper_bound)
        return lower_bound;

    return lower_bound + rand() % (upper_bound - lower_bound + 1);
}