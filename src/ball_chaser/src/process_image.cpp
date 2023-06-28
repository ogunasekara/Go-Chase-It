#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "ball_chaser/DriveToTarget.h"

class ProcessImageNode
{
    public:
        ProcessImageNode()
        {
            _image_subscriber = _n.subscribe("/camera/rgb/image_raw", 10, &ProcessImageNode::processImageCallback, this);

            _command_robot_srv_client = _n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
        }

        void processImageCallback(const sensor_msgs::Image img)
        {
            int white_pixel = 255;
            int img_width = img.step;

            for (int i = 0; i < img.data.size(); i++)
            {
                if (img.data[i] == white_pixel)
                {
                    // determine drive behavior based on column of white pixel in image
                    int pixel_col = i % img_width;
                    if (pixel_col < (img_width / 3))
                    {
                        // drive left
                        drive_robot(0.25, 0.5);
                        return;
                    } 
                    else if (pixel_col < (2 * img_width / 3))
                    {
                        // drive forward
                        drive_robot(0.5, 0);
                        return;
                    }
                    else 
                    {
                        // drive right
                        drive_robot(0.25, -0.5);
                        return;
                    }
                }
            }

            // stop if no white pixel
            drive_robot(0, 0);
        }

        void drive_robot(float lin_x, float ang_z)
        {
            ball_chaser::DriveToTarget srv;
            srv.request.linear_x = lin_x;
            srv.request.angular_z = ang_z;
            if (_command_robot_srv_client.call(srv))
            {
                ROS_INFO("DriveToTarget sent - linear: %1.2f, angular: %1.2f", (float)lin_x, (float)ang_z);
            }
            else
            {
                ROS_ERROR("Failed to call /ball_chaser/command_robot.");
            }
        }

    private:
        ros::NodeHandle _n; 
        ros::Subscriber _image_subscriber;
        ros::ServiceClient _command_robot_srv_client;
};

int main(int argc, char **argv)
{
    // Initialize ROS node
    ros::init(argc, argv, "process_image");

    // create ProcessImageNode object
    ProcessImageNode node;

    ros::spin();

    return 0;
}