#include <ros/ros.h>
#include <pr2_controllers_msgs/Pr2GripperCommandAction.h>
#include <actionlib/client/simple_action_client.h>
#include <stdio.h>

// Our Action interface type, provided as a typedef for convenience
typedef actionlib::SimpleActionClient<pr2_controllers_msgs::Pr2GripperCommandAction> GripperClient;

class Gripper{
private:
  GripperClient* gripper_client_;  

public:
  //Action client initialization
  Gripper(bool isleft){

    //Initialize the client for the Action interface to the gripper controller
    //and tell the action client that we want to spin a thread by default
    if(isleft)
      gripper_client_ = new GripperClient("l_gripper_controller/gripper_action", true);
    else
      gripper_client_ = new GripperClient("r_gripper_controller/gripper_action", true);
    
    //wait for the gripper action server to come up 
    while(!gripper_client_->waitForServer(ros::Duration(5.0))){
      ROS_INFO("Waiting for the r_gripper_controller/gripper_action action server to come up");
    }
  }

  ~Gripper(){
    delete gripper_client_;
  }

  //Open the gripper
  void open(){
    pr2_controllers_msgs::Pr2GripperCommandGoal open;
    open.command.position = 0.07;
    open.command.max_effort = -1.0;  // Do not limit effort (negative)
    
    ROS_INFO("Sending open goal");
    gripper_client_->sendGoal(open);
    gripper_client_->waitForResult();
    if(gripper_client_->getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
     ROS_INFO("The gripper opened!");
    else
      ROS_INFO("The gripper failed to open.");
  }

  //Close the gripper
  void close(){
    pr2_controllers_msgs::Pr2GripperCommandGoal squeeze;
    squeeze.command.position = 0.0;
    squeeze.command.max_effort = 50.0;  // Close gently
    
    ROS_INFO("Sending squeeze goal");
    gripper_client_->sendGoal(squeeze);
    gripper_client_->waitForResult();
    if(gripper_client_->getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("The gripper closed!");
    else
      ROS_INFO("The gripper failed to close.");
  }
};

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_gripper");

  bool isleft = false;
  if (argc == 1) {
    printf("simple_gripper [close/open] [left/right]\n");  
    return 0;
  }
  
  if(argc == 3) 
      isleft = strcmp("left", argv[2]) == 0 || strcmp("l", argv[2]) == 0 ;
  Gripper gripper(isleft);

  if (strcmp(argv[1], "close") == 0){ 
    gripper.close();
  }
  else if (strcmp(argv[1], "open") == 0){ 
    gripper.open();
  }
  //ros::Duration(10.0).sleep();
  return 0;
}

