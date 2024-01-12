#include "ros/ros.h"
#include <learning_topic/Student.h>
int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    ros::init(argc ,argv, "laodeng");
    ros::NodeHandle nh;
    ros::Publisher pub=nh.advertise<learning_topic::Student>("stu_msg",10);

    learning_topic::Student stu;
    stu.name="彭城";
    stu.class_time=0;

    ros::Rate r(1);
    while (ros::ok()) {
        pub.publish(stu);
        ROS_INFO("我叫%s,这是我的第%d次上课",stu.name.c_str(),stu.class_time);
        stu.class_time++;
        r.sleep(); 
        ros::spinOnce();
    }
        return 0;
    }