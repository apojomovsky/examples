#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <thread>
#include <chrono>

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  size_t count = 0;

  auto node = rclcpp::Node::make_shared("content_filtering_node");
  auto publisher = node->create_publisher<std_msgs::msg::String>("topic", 10);

  rclcpp::SubscriptionOptions sub_options;
  // sub_options.content_filter_options.filter_expression = "";
  // sub_options.content_filter_options.expression_parameters = {};

  auto subscriber = node->create_subscription<std_msgs::msg::String>("topic", 10,
    [node, &count](const std_msgs::msg::String::SharedPtr msg) {
      RCLCPP_INFO(node->get_logger(), "Received message: %s", msg->data.c_str());
      ++count;
    },
    sub_options);

  auto executor = std::make_unique<rclcpp::executors::SingleThreadedExecutor>();

  executor->add_node(node);
  std::thread executor_thread([&executor]() {executor->spin();});
  // const std::string equalityFilter = ContentFilterGenerator<std_msgs::msg::String>::generateEqualityFilter();
  // std::cout << "Equality filter: '" << equalityFilter << "'" << std::endl;
  // subscriber->set_content_filter("", {});
  subscriber->set_content_filter("data = 'Hello, world!'", {});


  auto msg1 = std_msgs::msg::String();
  msg1.data = "Hello, world!";
  auto msg2 = std_msgs::msg::String();
  msg2.data = "Goodbye, world!";


  RCLCPP_INFO(node->get_logger(), "**********FIRST ITERATION***********");
  for (int i = 0; i < 5; ++i) {
    publisher->publish(msg1);
    publisher->publish(msg2);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  // executor->cancel();
  // executor_thread.join();

  // const std::string inequalityFilter = ContentFilterGenerator<std_msgs::msg::String>::generateInequalityFilter();
  // std::cout << "Inequality filter: '" << inequalityFilter << "'" << std::endl;
  // subscriber->set_content_filter(inequalityFilter, {});
  // // subscriber->set_content_filter("", {});

  // subscriber->set_content_filter("data = data", {});

  subscriber->set_content_filter("data = 'Goodbye, world!'", {});
  // subscriber->set_content_filter("", {});
  // subscriber->set_content_filter("", {});
  // executor_thread = std::thread([&executor]() {executor->spin();});

  RCLCPP_INFO(node->get_logger(), "**********SECOND ITERATION***********");
  for (int i = 0; i < 5; ++i) {
    publisher->publish(msg1);
    publisher->publish(msg2);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  executor->cancel();
  executor_thread.join();

  return 0;
}
