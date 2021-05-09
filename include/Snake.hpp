//
// Created by swirta on 02.05.2021.
//

#ifndef SCZR_SNAKE_SNAKE_HPP
#define SCZR_SNAKE_SNAKE_HPP

#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <deque>
#include <optional>

const int fruitRadius = 20;
const unsigned int M_BUTTON = 109;
const unsigned int R_BUTTON = 114;
const unsigned int ESC_BUTTON = 27;
const unsigned int ENTER_BUTTON = 13;

enum GAME_STATE{
    PREPARING,
    PLAYING,
    LOST,
    ENDED,
    QUIT
};

class Snake{
public:
    void clear_snake();
    void grow(const cv::Point &chunk);
    void move(const cv::Point &point);
    int length();
    void draw(cv::Mat &frame);
    bool check_snake();
    bool eat_fruit(cv::Point &fruit);
    std::optional<cv::Point> get_point(unsigned long i);

private:
    static bool is_intersected(const cv::Point &a,const  cv::Point &b,const cv::Point &c,const cv::Point &d);
    static int orientation(const cv::Point &a, const cv::Point &b, const cv::Point &c);
    std::deque<cv::Point> snakeBody;
};

class Game{
public:
    void draw(cv::Mat &frame);
    void draw_lost_message(cv::Mat &frame) const;
    void draw_info_message(cv::Mat &frame) const;
    void create_snake(cv::Point &head);
    void generate_fruit();
    void run(cv::Point &marker, cv::Mat &frame);
    GAME_STATE get_state();

    Game(cv::Point windowSize_) : gameState(PREPARING), points(0), lives(3), windowSize(windowSize_) {}
private:
    GAME_STATE gameState;
    int points, lives;
    cv::Point windowSize;
    cv::Point fruit;
    Snake snake;

};

#endif //SCZR_SNAKE_SNAKE_HPP
