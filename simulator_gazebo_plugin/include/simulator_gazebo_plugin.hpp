// Copyright 2019 Bold Hearts
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SIMULATOR_GAZEBO_PLUGIN__SIMULATOR_GAZEBO_PLUGIN_HPP_
#define SIMULATOR_GAZEBO_PLUGIN__SIMULATOR_GAZEBO_PLUGIN_HPP_

#include <gazebo/common/PID.hh>
#include <gazebo/common/Plugin.hh>
#include <rclcpp/rclcpp.hpp>
#include <simulator_msgs/msg/control_command.hpp>
#include <simulator_gazebo_joint.hpp>

//mx_joint_controller_msgs

namespace gokart_gazebo_plugin
{

class GokartGazeboPlugin : public gazebo::ModelPlugin
{
public:
  GokartGazeboPlugin();

  void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf);

private:
  void Update();

  using ControlCommand = simulator_msgs::msg::ControlCommand;

  std::string robot_namespace_;

  gazebo::physics::ModelPtr model_;
  gazebo::physics::WorldPtr world_;

  gazebo::common::Time last_sim_time_;
  gazebo::common::Time last_update_time_;
  double update_period_ms_;

  gazebo::event::ConnectionPtr update_connection_;

  std::string fl_steering_joint_name;
  std::string fr_steering_joint_name;
  std::string rl_wheel_joint_name;
  std::string rr_wheel_joint_name;

  std::map<std::string, std::pair<gazebo::physics::JointPtr, gazebo::common::PID>> joints_;
  std::map<std::string, double> joint_targets_;   

  double desired_steering_angle;
  double desired_velocity;

  // std::map<std::string, double> joint_targets_;

  rclcpp::Node::SharedPtr ros_node_;
  rclcpp::Subscription<ControlCommand>::SharedPtr control_command_sub_;
};

}

#endif  // SIMULATOR_GAZEBO_PLUGIN__GAZEBO_PLUGIN_HPP_
