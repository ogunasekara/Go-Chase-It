#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "ball_chaser/DriveToTarget.h"

class DriveBotNode
{
    public:
        DriveBotNode()
        {
            _motor_command_publisher = _n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

            _command_robot_srv_server = _n.advertiseService("ball_chaser/command_robot", &DriveBotNode::handleDriveRequest, this);
        }

        bool handleDriveRequest(ball_chaser::DriveToTargetRequest& req, ball_chaser::DriveToTargetResponse& res)
        {
            ROS_INFO("DriveToTarget received - linear: %1.2f, angular: %1.2f", (float)req.linear_x, (float)req.angular_z);

            geometry_msgs::Twist cmd;
            cmd.linear.x = req.linear_x;
            cmd.angular.z = req.angular_z;

            _motor_command_publisher.publish(cmd);

            res.msg_feedback = "Wheel velocities set - linear: " + std::to_string(req.linear_x) + ", angular: " + std::to_string(req.angular_z);
            ROS_INFO_STREAM(res.msg_feedback);
            
            return true;
        }

    private:
        ros::NodeHandle _n; 
        ros::Publisher _motor_command_publisher;
        ros::ServiceServer _command_robot_srv_server;
};

int main(int argc, char **argv)
{
    // Initialize ROS node
    ros::init(argc, argv, "drive_bot");

    // create DriveBotNode object
    DriveBotNode node;

    ros::spin();

    return 0;
}