#ifndef GAME_H
#define GAME_H

#include "Circle.h"
#include <map>
#include <memory>

struct Window
{
	long width;
	long length;
	Window(long _v_x, long _v_y)
	{
		width = _v_x;
		length = _v_y;
	};
};

class Game
{
private:    
    bool m_stop_while_moving, m_stop_while_minimized;
	HWND        m_hwnd;
	HBRUSH      m_black_brush, m_red_brush;
	HPEN        m_black_pen, m_red_pen;
    std::shared_ptr<KillerCircle> m_killer_circle;
    POINT m_center;
    POINT m_down_right_border;
    POINT m_top_left_border;
	std::multimap<long int, SimpleCircle> m_x_sorted_circles;
	
    void CalculateCollisions(HDC _hdc);
    Game(HWND _window);
public:
    static Game& GetGame(HWND _window);
	void CalculateChange();
    void CreateSimpleCircle(POINT& _circle_center);
	void RedrawAll(const HDC _hdc) const;
	void ChangeBorder();  
    void SetWindowMoving(bool move);
    void CangeWindowPos();
    HWND GetWindow() const;
    long int random_int(long int lower_bound, long int upper_bound) const;
};
#endif
