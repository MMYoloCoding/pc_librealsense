#include <librealsense2/rs.hpp> // Include the RealSense header file
#include <iostream>            // Include the standard input/output header file
#include "example.hpp"
using namespace std; using namespace rs2;
int main()
{
    window app(1280, 720, "Obstacle Detection");
    int max_distance = 1; // Declare number of pixels to stop
    pipeline pipeline;
    config config;
    pipeline.start(config);
    int count = 0;
    while (app) {
        frameset frames = pipeline.wait_for_frames();
        depth_frame depth_frame = frames.get_depth_frame();
        app.show_depth(depth_frame);
        int counter = 0;
        float obj_max_distance = 100000.0;
        bool goOrStop = true;
        for (int i = 0; i < depth_frame.get_width(); i++)
        {
            for (int j = 0; j < depth_frame.get_height(); j++)
            {
                float current_distnce = depth_frame.get_distance(i, j);
                if (current_distnce <= max_distance) { //max_distance = 1m
                   obj_max_distance = current_distnce; 
                   counter++;
                }
                if (counter == 50000) {
                    goOrStop = false;
                    break;
                }
            }
            if (counter == 50000) break;
        }
        if (goOrStop == true) cout << "Go" << endl; 
        else std::cout << "STOP: Detect object at " << obj_max_distance * 100 << "cm." << std::endl;
    }
    pipeline.stop();

    return 0;
}
