#include "ros/ros.h"
#include "gazebo_msgs/ModelStates.h"
#include "gazebo_msgs/ModelState.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Twist.h"

#include<cstdio>
#include <sstream>
#include <iostream>
/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */

using namespace std;

gazebo_msgs::ModelStates current_states;
gazebo_msgs::ModelState target_state;


void state_cb(const gazebo_msgs::ModelStates::ConstPtr& msg){
	current_states = * msg;
	string name  = current_states.name[1];
	geometry_msgs::Pose pose = current_states.pose[1]; 
	cout << name << endl;

	

	target_state.model_name = name;
	target_state.pose       = pose;
	target_state.twist      = current_states.twist[0]; 
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "mover");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;
  ros::Subscriber state_sub = n.subscribe<gazebo_msgs::ModelStates>("/gazebo/model_states", 10 , state_cb);

  ros::Publisher state_pub = n.advertise<gazebo_msgs::ModelState>("/gazebo/set_model_state", 1000);

  ros::Rate loop_rate(10);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  int count = 0;
 // int default_x = target_state.pose.position.x;  
    int default_y = target_state.pose.position.y;
  
	while (ros::ok())
  {
	target_state.pose.position.x = 0;
	if(count%10<5){
	target_state.pose.position.y = count% 10;
	}
	else{
		target_state.pose.position.y =10 - (count%10);
	}
		
	target_state.pose.position.z = 2;
    
	state_pub.publish(target_state);
    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}
