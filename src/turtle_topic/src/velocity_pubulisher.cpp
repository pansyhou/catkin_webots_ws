// #include <iostream>
// #include <string>
// #include <sstream>
// #include "ros/ros.h"
// #include "geometry_msgs/Twist.h"
// using namespace std;

// #include "ros/ros.h"
// #include "std_msgs/String.h"

// int main(int argc, char** argv)
// {
//     //节点初始化
//     ros::init(argc, argv, "talker");
//     //节点句柄
//     ros::NodeHandle n;
//     //创建pubulisher
//     ros::Publisher chatter_pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 100);

//     //设定循环频率
//     ros::Rate loop_rate(10);
//     int count = 0;
//     while(ros::ok())
//     {
//         // std_msgs::String msg;
//         // std::stringstream ss;
//         // ss << "hello ROS !" << count;
//         // msg.data = ss.str();
//         // ROS_INFO("%s", msg.data.c_str());
//         // chatter_pub.publish(msg);
//         // ros::spinOnce();
//         //初始化geometry_msgs::Twist类型消息
//         geometry_msgs::Twist vel_msg;
//         vel_msg.linear.x=0.5;
//         vel_msg.angular.z=0.2;

//         chatter_pub.publish(vel_msg);
//         ROS_INFO("pubulish command [%0.2f m/s , %0.2f rad/s ]", vel_msg.linear.x,vel_msg.angular.z);
//         loop_rate.sleep();
//         // count++;
//     }
//     return 0;
// }



#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"
#include <cmath>
#include <turtlesim/Kill.h>
#include "turtlesim/Spawn.h"
#include "std_srvs/Empty.h"
// 目标坐标点列表
std::vector<std::pair<float, float>> waypoints = {
    // {5.54, 8.54},
    // {6.54, 7.54},
    // {5.54, 6.54},
    // {6.54, 5.54}
    {2.0,2.0},{4.0,0.0},{6.0,2.0},{8.0,0.0}

};

// 当前小乌龟的位置
turtlesim::Pose current_pose;

void poseCallback(const turtlesim::Pose::ConstPtr& pose_msg)
{
    current_pose = *pose_msg;
}

float distanceToTarget(float target_x, float target_y)
{
    return std::sqrt(std::pow(target_x - current_pose.x, 2) + std::pow(target_y - current_pose.y, 2));
}

void moveToGoal(float target_x, float target_y, ros::Publisher& cmd_vel_pub)
{
    ros::Rate rate(10);

    while (distanceToTarget(target_x, target_y) > 0.02)
    {

        // if(current_pose.x==0)break;
        // 计算线速度和角速度
        float linear_speed = 0.1 * distanceToTarget(target_x, target_y);
        float desired_angle = std::atan2(target_y - current_pose.y, target_x - current_pose.x);
        float angular_speed = 7 * (desired_angle - current_pose.theta);

        // 发布速度控制命令
        geometry_msgs::Twist cmd_vel;
        cmd_vel.linear.x = linear_speed;
        cmd_vel.angular.z = angular_speed;
        cmd_vel_pub.publish(cmd_vel);
        ROS_INFO("position [%0.2f x , %0.2f y ,%0.2ftheta ]", current_pose.x,current_pose.y,current_pose.theta);
        ROS_INFO("pubulish command [%0.2f m/s , %0.2f rad/s ]", linear_speed,angular_speed);
        rate.sleep();
        ros::spinOnce();
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "turtle_controller");

    ros::NodeHandle nh;
    //初始化现在位置，不然会跑飞
    current_pose.x=0.0f;
    current_pose.y=0.0f;
    //先删除原有乌龟，再生成新的
    ros::ServiceClient kill_client = nh.serviceClient<turtlesim::Kill>("/kill");
    turtlesim::Kill srv;
    srv.request.name = "turtle1";
    kill_client.call(srv);
    //清楚痕迹
    ros::ServiceClient clearClient = nh.serviceClient<std_srvs::Empty>("/clear");
    std_srvs::Empty srv1;
    clearClient.call(srv1);
     // 创建一个ServiceClient，请求生成一只新的乌龟
    ros::ServiceClient client= nh.serviceClient<turtlesim::Spawn>("/spawn");
    turtlesim::Spawn spawn;

    spawn.request.x=current_pose.x;
    spawn.request.y=current_pose.y;
    spawn.request.theta=0;
    spawn.request.name="turtle1";

    //   发送请求
    client.waitForExistence();
    bool flag=client.call(spawn);  
    // 处理响应
    if(flag){
        ROS_INFO("乌龟生成成功，新乌龟名：%s",spawn.response.name.c_str());
    }
    else{
        ROS_INFO("请求失败！");
    }



    ros::Subscriber pose_sub = nh.subscribe("/turtle1/pose", 1000, poseCallback);
    ros::Publisher cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1000);

    while (pose_sub.getNumPublishers() == 0 || cmd_vel_pub.getNumSubscribers() == 0)
    {
        ROS_INFO("Waiting for publishers and subscribers...");
        ros::Duration(1.0).sleep();
    }
    ros::Duration(1.0).sleep();

    for (const auto& waypoint : waypoints)
    {
        moveToGoal(waypoint.first, waypoint.second, cmd_vel_pub);

    }
    //删除乌龟
    kill_client.call(srv);
    ros::spin();

    return 0;
}

