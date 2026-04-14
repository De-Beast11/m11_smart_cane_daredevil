#include"FeedbackDevice.hpp"

class FeedbackSystem{
    private:
    feedbackMode modes;

    public:
    void feedback_loop(const std::array<float, 4> input_data, bool battery_status);
};