//
// Created by swirta on 02.05.2021.
//

#ifndef SCZR_SNAKE_SNAKE_HPP
#define SCZR_SNAKE_SNAKE_HPP

#include <opencv4/opencv2/imgproc.hpp>
#include <deque>

typedef std::pair<cv::Point2f, float> Circle;

const int fruitRadius = 10;

enum GAME_STATE{
    PREPARING,
    PLAYING,
    PAUSED,
    LOST,
    ENDED
};

class Snake{
public:
    void grow(cv::Point chunk);
    void move(cv::Point point);
    int length();
    void draw(cv::Mat &frame);
    bool check_snake();
    bool eat_fruit(cv::Point fruit);
    cv::Point get_point(int i);

private:
    bool is_intersected(const cv::Point &a,const  cv::Point &b,const cv::Point &c,const cv::Point &d);
    int orientation(const cv::Point &a, const cv::Point &b, const cv::Point &c);
    std::deque<cv::Point> snakeBody;
    int maxLength;
};

class Game{
public:
    void draw(cv::Mat &frame);
    void create_snake(cv::Point head);
    void generate_fruit();
    void run();

    Game(int sizeX_, int sizeY_) : gameState(PREPARING), points(0), lives(3), sizeX(sizeX_), sizeY(sizeY_) {}
private:
    GAME_STATE gameState;
    int points, lives;
    int sizeX, sizeY;
    cv::Point fruit;
    Snake snake;

};

#endif //SCZR_SNAKE_SNAKE_HPP
