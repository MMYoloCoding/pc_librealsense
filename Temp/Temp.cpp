#include <librealsense2/rs.hpp> // Include the RealSense header file
#include <librealsense2/rs_advanced_mode.hpp> // Include the RealSense header file
#include <opencv2/opencv.hpp>  // Include the OpenCV header file
#include <iostream>            // Include the standard input/output header file
#include "example.hpp"
#include <fstream>
using namespace std;
using namespace rs2;

int main()
{
    window app(1280, 720, "Obstacle Detection");
    // Declare number of pixels to stop
    int max_distance = 1;
    // Declare a RealSense pipeline object
    rs2::context ctx;
    auto devices = ctx.query_devices();
    rs2::device dev = devices[0];
    rs2::pipeline pipeline;

    //rs2::hole_filling_filter hff = rs2::hole_filling_filter(1);
    //rs2::temporal_filter tf = rs2::temporal_filter(1);
    
    //rs2::disparity_transform depth2disparity(true);
   
    //rs2::threshold_filter thres_filter;
    //thres_filter.set_option(RS2_OPTION_MIN_DISTANCE, 0.15f);
    //thres_filter.set_option(RS2_OPTION_MAX_DISTANCE, 4.0f);
    

    // Declare a configuration object for the pipeline
    rs2::config config;
    std::string serial = dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
    std:string json_file_name = "play.json";
    if (dev.is<rs400::advanced_mode>())
    {
        auto advanced_mode_dev = dev.as<rs400::advanced_mode>();
        // Check if advanced-mode is enabled
        if (!advanced_mode_dev.is_enabled())
        {
            // Enable advanced-mode
            advanced_mode_dev.toggle_advanced_mode(true);
        }
        std::ifstream t(json_file_name, std::ifstream::in);
        std::string preset_json((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        advanced_mode_dev.load_json(preset_json);
    }
    else
    {
        std::cout << "Current device doesn't support advanced-mode!\n";
        return EXIT_FAILURE;
    }
    config.enable_device(serial);
    // Add the depth stream to the configuration
    config.enable_stream(RS2_STREAM_DEPTH,1280,720,RS2_FORMAT_ANY,15);

    // Start the pipeline with the configuration
    pipeline.start(config);



    //rs2::device dev = pipeline.get_active_profile().get_device();
    //dev.query_sensors()[1].set_option(rs2_option::RS2_OPTION_EMITTER_ENABLED, 1);
    // Declare a window to display the depth frames
    //cv::namedWindow("Depth Frame", cv::WINDOW_NORMAL);

    // Declare a counter for consecutive pixels within 200mm
    int count = 0;

    while (cv::waitKey(1) < 0 && app) // Wait for a key press
    {
        // Get the next depth frame from the pipeline
        rs2::frameset frames = pipeline.wait_for_frames();
        //rs2::frameset frames = pipeline.wait_for_frames();
        rs2::depth_frame depth_frame = frames.get_depth_frame();
        //cout << depth_frame.get_width() << endl << depth_frame.get_height() << endl;
        //depth_frame = thres_filter.process(depth_frame);
        //depth_frame = depth_frame.apply_filter(tf);
        //depth_frame = depth_frame.apply_filter(hff);
        //depth_frame = depth_frame.apply_filter(thres_filter);
    
        /*
        // Convert the depth frame to a grayscale OpenCV image
        cv::Mat depth_image(cv::Size(depth_frame.get_width(), depth_frame.get_height()), CV_16UC1, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
        cv::Mat hist(256, 1, CV_32FC1, cv::Scalar(0));
        const int histSize[] = { 256 };
        const float range[] = { 0, 256 };
        const float* ranges[] = { range };
        cv::calcHist(&depth_image, 1, 0, cv::Mat(), hist, 1, histSize, ranges, true, false);

        // Normalize the histogram using the normalize function
        //cv::normalize(hist, hist, 0, 255, cv::NORM_MINMAX, CV_32FC1);

        //cv::Mat depth_image_gray;
        //cv::convertScaleAbs(depth_image, depth_image_gray, 255.0 / 1000.0);

        // Display the depth frame
        cv::Mat plot = cv::Mat::zeros(cv::Size(256, 100), CV_8UC1);
        cv::normalize(hist, hist, 0, 100, cv::NORM_MINMAX);
        for (int i = 0; i < 256; i++) {
            cv::line(plot, cv::Point(i, 100), cv::Point(i, 100 - hist.at<float>(i)), cv::Scalar(255, 255, 255));
        }

        // Display the histogram
        cv::imshow("Histogram", plot);*/
        app.show_depth(depth_frame);
        
        // Scan the depth image for consecutive pixels within 200mm
        int counter = 0;
        float obj_max_distance = 100000.0;
        bool goOrStop = true;
        for (int i = 640; i < 1280; i++)
        {
            for (int j = 0; j < 720; j++)
            {
                float current_distnce = depth_frame.get_distance(i, j);
                if (current_distnce <= max_distance) { //max_distance = 1m
                    //cout << i << "," << j << endl;
                    //cout << "distance = " << depth_frame.get_distance(i, j) << endl;
                    if(current_distnce < obj_max_distance && current_distnce != 0) obj_max_distance = current_distnce;
                    counter++;
                }
                if (counter == 50000) {
                    //obj_distance = (obj_distance / 50000.0) * 100.0;
                    //std::cout << "STOP: Detect object at " << obj_distance <<"cm." << std::endl;
                    goOrStop = false;
                    break;
                }
                
            }
            if (counter == 50000) {
                break;
            }
        }
        if (goOrStop == true) {
            cout << "Go" << endl;
        }
        else {
            
            std::cout << "STOP: Detect object at " << obj_max_distance*100 << "cm." << std::endl;
            //std::cout << "STOP: Detect object!  " << std::endl;
        }
    }

    // Stop the pipeline
    pipeline.stop();

    return 0;
}
