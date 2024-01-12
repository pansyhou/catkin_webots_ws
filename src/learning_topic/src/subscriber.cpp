#include"ros/ros.h"
#include <learning_topic/Student.h>


void Post_stumsg(const learning_topic::Student::ConstPtr& stu){
    ROS_INFO("这上课的学生叫%s，他已经上课了%d次",stu->name.c_str(),stu->class_time);
}


int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");
    ros::init(argc,argv,"laoshi");
    ros::NodeHandle nh;
    ros::Subscriber sub=nh.subscribe<learning_topic::Student>("stu_msg",10,Post_stumsg);
    ros::spin();
    return 0;

}