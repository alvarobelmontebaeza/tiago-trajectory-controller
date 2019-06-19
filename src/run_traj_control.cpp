/*
 * Software License Agreement (Modified BSD License)
 *
 *  Copyright (c) 2013, PAL Robotics, S.L.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of PAL Robotics, S.L. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Alessandro Di Fava. */

// C++ standard headers
#include <exception>
#include <string>
#include <fstream>

// Boost headers
#include <boost/shared_ptr.hpp>

// ROS headers
#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <control_msgs/FollowJointTrajectoryAction.h>
#include <ros/topic.h>
#include "std_msgs/Float64MultiArray.h"




// Our Action interface type for moving TIAGo's head, provided as a typedef for convenience
typedef actionlib::SimpleActionClient<control_msgs::FollowJointTrajectoryAction> arm_control_client;
typedef boost::shared_ptr< arm_control_client>  arm_control_client_Ptr;


// Create a ROS action client to move TIAGo's arm
void createArmClient(arm_control_client_Ptr& actionClient)
{
  ROS_INFO("Creating action client to arm controller ...");

  actionClient.reset( new arm_control_client("/arm_dynamic_controller/follow_joint_trajectory") );

  int iterations = 0, max_iterations = 3;
  // Wait for arm controller action server to come up
  while( !actionClient->waitForServer(ros::Duration(2.0)) && ros::ok() && iterations < max_iterations )
  {
    ROS_DEBUG("Waiting for the arm_controller_action server to come up");
    ++iterations;
  }

  if ( iterations == max_iterations )
    throw std::runtime_error("Error in createArmClient: arm controller action server not available");
}


// Generates a simple trajectory with two waypoints to move TIAGo's arm 
void waypoints_arm_goal(control_msgs::FollowJointTrajectoryGoal& goal)
{
  // The joint names, which apply to all waypoints
  goal.trajectory.joint_names.push_back("arm_1_joint");
  goal.trajectory.joint_names.push_back("arm_2_joint");
  goal.trajectory.joint_names.push_back("arm_3_joint");
  goal.trajectory.joint_names.push_back("arm_4_joint");
  /*goal.trajectory.joint_names.push_back("arm_5_joint");
  goal.trajectory.joint_names.push_back("arm_6_joint");
  goal.trajectory.joint_names.push_back("arm_7_joint");*/

  // Three waypoints in this goal trajectory
  goal.trajectory.points.resize(3);

  // First trajectory point
  // Positions
  int index = 0;
  goal.trajectory.points[index].positions.resize(4);
  goal.trajectory.points[index].positions[0] = 0.15;
  goal.trajectory.points[index].positions[1] = -0.90;
  goal.trajectory.points[index].positions[2] = -2.52;
  goal.trajectory.points[index].positions[3] = 1.64;
  /*goal.trajectory.points[index].positions[4] = -1.57;
  goal.trajectory.points[index].positions[5] = -0.5;
  goal.trajectory.points[index].positions[6] = 0.0;*/
  // Velocities
  goal.trajectory.points[index].velocities.resize(4);
  for (int j = 0; j < 4; ++j)
  {
    goal.trajectory.points[index].velocities[j] = 0.2;
  }
  // To be reached 2 second after starting along the trajectory
  goal.trajectory.points[index].time_from_start = ros::Duration(8.0);

  // Second trajectory point
  // Positions
  index += 1;
  goal.trajectory.points[index].positions.resize(4);
  goal.trajectory.points[index].positions[0] = 1.57;
  goal.trajectory.points[index].positions[1] = -0.06;
  goal.trajectory.points[index].positions[2] = -0.88;
  goal.trajectory.points[index].positions[3] = 0.49;
  /*goal.trajectory.points[index].positions[4] = 1.0;
  goal.trajectory.points[index].positions[5] = -0.5;
  goal.trajectory.points[index].positions[6] = 0.0;*/
  // Velocities
  goal.trajectory.points[index].velocities.resize(4);
  for (int j = 0; j < 4; ++j)
  {
    goal.trajectory.points[index].velocities[j] = 0.2;
  }
  // To be reached 4 seconds after starting along the trajectory
  goal.trajectory.points[index].time_from_start = ros::Duration(13.0);

  // Third trajectory point
  // Positions
  index += 1;
  goal.trajectory.points[index].positions.resize(4);
  goal.trajectory.points[index].positions[0] = 2.30;
  goal.trajectory.points[index].positions[1] = 0.82;
  goal.trajectory.points[index].positions[2] = 1.00;
  goal.trajectory.points[index].positions[3] = 1.69;
  /*goal.trajectory.points[index].positions[4] = 1.0;
  goal.trajectory.points[index].positions[5] = -0.5;
  goal.trajectory.points[index].positions[6] = 0.0;*/
  // Velocities
  goal.trajectory.points[index].velocities.resize(4);
  for (int j = 0; j < 4; ++j)
  {
    goal.trajectory.points[index].velocities[j] = 0.2;
  }
  // To be reached 4 seconds after starting along the trajectory
  goal.trajectory.points[index].time_from_start = ros::Duration(17.0);

}

std::ofstream error_file;
std::vector<double> error_msg;
int error_cont = 0;
void errorCallback(const std_msgs::Float64MultiArray::ConstPtr& msg)
{
  error_cont++;
  
  error_file << error_cont << " ";
  for(int i=0;i<msg->data.size();i++)
    error_file << msg->data[i] << " ";
  
  error_file << "\n";
  
}

std::ofstream torque_file;
std::vector<double> torque_msg;
int torque_cont = 0;
void torqueCallback(const std_msgs::Float64MultiArray::ConstPtr& msg)
{
  torque_cont++;
   
  torque_file << torque_cont << " ";
  for(int i=0;i<msg->data.size();i++)
    torque_file << msg->data[i] << " ";
  
  torque_file << "\n";
}

// Entry point
int main(int argc, char** argv)
{
  // Init the ROS node
  ros::init(argc, argv, "run_traj_control");

  ROS_INFO("Starting run_traj_control application ...");
 
  // Precondition: Valid clock
  ros::NodeHandle nh;
  if (!ros::Time::waitForValid(ros::WallDuration(10.0))) // NOTE: Important when using simulated clock
  {
    ROS_FATAL("Timed-out waiting for valid time.");
    return EXIT_FAILURE;
  }

  // Create an arm controller action client to move the TIAGo's arm
  arm_control_client_Ptr ArmClient;
  createArmClient(ArmClient);

  // Generates the goal for the TIAGo's arm
  control_msgs::FollowJointTrajectoryGoal arm_goal;
  waypoints_arm_goal(arm_goal);

  // Sends the command to start the given trajectory 1s from now
  arm_goal.trajectory.header.stamp = ros::Time::now() + ros::Duration(1.0);
  ArmClient->sendGoal(arm_goal);

  // Open data files
  error_file.open("/tmp/error.data");
  torque_file.open("/tmp/torques.data");

  ros::Subscriber error_sub = nh.subscribe("/arm_dynamic_controller/position_error", 1000, errorCallback);
  ros::Subscriber torque_sub = nh.subscribe("/arm_dynamic_controller/torques", 1000, torqueCallback);

  // Wait for trajectory execution
  while(!(ArmClient->getState().isDone()) && ros::ok())
  {
    ros::spinOnce();
  }

  error_file.close();
  torque_file.close();

  return EXIT_SUCCESS;
}

