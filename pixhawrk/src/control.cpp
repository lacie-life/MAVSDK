/**
 * @file offb_node.cpp
 * @brief Offboard control example node, written with MAVROS version 0.19.x, PX4 Pro Flight
 * Stack and tested in Gazebo SITL
 */
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>

mavros_msgs::State current_state;
geometry_msgs::PoseStamped custom_pose;


void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}

void *enterCustomPose(void *threadid)
{
    std::cout << "Pls Enter :" << std::endl;
    std::cin >> custom_pose.pose.position.x >> custom_pose.pose.position.y >> custom_pose.pose.position.z;
    std::cout << std::endl;
    pthread_exit(NULL);
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "offb_node");
    ros::NodeHandle nh;

    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("mavros/state", 10, state_cb);
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);

    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
            ("mavros/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
            ("mavros/set_mode");

    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);

    // wait for FCU connection
    while(ros::ok() && !current_state.connected){
        ros::spinOnce();
        rate.sleep();
    }
    geometry_msgs::PoseStamped poseStart;
    poseStart.pose.position.x = 0;
    poseStart.pose.position.y = 0;
    poseStart.pose.position.z = 2;

    geometry_msgs::PoseStamped poseStart1;
    poseStart1.pose.position.x = 0;
    poseStart1.pose.position.y = 2;
    poseStart1.pose.position.z = 2;

    geometry_msgs::PoseStamped poseStart2;
    poseStart2.pose.position.x = 0;
    poseStart2.pose.position.y = -2;
    poseStart2.pose.position.z = 2;

    geometry_msgs::PoseStamped poseStart3;
    poseStart3.pose.position.x = 2;
    poseStart3.pose.position.y = -2;
    poseStart3.pose.position.z = 2;

    geometry_msgs::PoseStamped poseStart4;
    poseStart4.pose.position.x = -2;
    poseStart4.pose.position.y = -2;
    poseStart4.pose.position.z = 2;

    geometry_msgs::PoseStamped customPose;

    customPose = poseStart;

    //send a few setpoints before starting
    for(int i = 100; ros::ok() && i > 0; --i){
        local_pos_pub.publish(poseStart);
        ros::spinOnce();
        rate.sleep();
    }



    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "OFFBOARD";

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

    ros::Time last_request = ros::Time::now();
    
    int poseIndex = 0;

    custom_pose = customPose;

    while(ros::ok()){

        if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))){
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.mode_sent){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();
        }
        else {
            if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(5.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
            }
        }

         if (current_state.mode == "OFFBOARD" && current_state.armed)
         {
             if (ros::Time::now() - last_request > ros::Duration(10.0))
             {

                pthread_t pthread;
                int threadState;
                int threadID = 0;
                threadState = pthread_create(&pthread, NULL, enterCustomPose, (void *)threadID);
                last_request = ros::Time::now();

        //         std::cout << "Change Pose" << std::endl;
        //         if (customPose.pose.position.x == poseStart.pose.position.x && customPose.pose.position.y == poseStart.pose.position.y && customPose.pose.position.z == poseStart.pose.position.z)
        //         {
        //             customPose = poseStart1;
        //             std::cout << "Pose 1" << std::endl;
        //             last_request = ros::Time::now();
        //         }

        //         else if (customPose.pose.position.x == poseStart1.pose.position.x && customPose.pose.position.y == poseStart1.pose.position.y && customPose.pose.position.z == poseStart1.pose.position.z)
        //         {
        //             customPose = poseStart2;
        //             std::cout << "Pose 2" << std::endl;
        //             last_request = ros::Time::now();
        //         }

        //         else if (customPose.pose.position.x == poseStart2.pose.position.x && customPose.pose.position.y == poseStart2.pose.position.y && customPose.pose.position.z == poseStart2.pose.position.z)
        //         {
        //             customPose = poseStart3;
        //             std::cout << "Pose 3" << std::endl;
        //             last_request = ros::Time::now();
        //         }

        //         else if (customPose.pose.position.x == poseStart3.pose.position.x && customPose.pose.position.y == poseStart3.pose.position.y && customPose.pose.position.z == poseStart3.pose.position.z)
        //         {
        //             customPose = poseStart4;
        //             std::cout << "Pose 4" << std::endl;
        //             last_request = ros::Time::now();
        //         }

        //         else if (customPose.pose.position.x == poseStart4.pose.position.x && customPose.pose.position.y == poseStart4.pose.position.y && customPose.pose.position.z == poseStart4.pose.position.z)
        //         {
        //             customPose = poseStart1;
        //             std::cout << "Pose 1_" << std::endl;
        //             last_request = ros::Time::now();
        //         }
        //         last_request = ros::Time::now();
             }

         }
        local_pos_pub.publish(custom_pose);

        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}