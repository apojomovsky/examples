// #include <chrono>
// #include <thread>

// #include "content-filter-test.hpp"
// #include "helper_macros.hpp"
// #include "rclcpp/rclcpp.hpp"
// #include "std_msgs/msg/string.hpp"

// int main(int argc, char * argv[])
// {
//   rclcpp::init(argc, argv);
//   size_t count = 0;

//   auto node = rclcpp::Node::make_shared("content_filtering_node");
//   auto publisher = node->create_publisher<std_msgs::msg::String>("topic", 10);

//   rclcpp::SubscriptionOptions sub_options;
//   sub_options.content_filter_options.filter_expression = "data = 'Hello, world!'";
//   sub_options.content_filter_options.expression_parameters = {};

//   auto subscriber = node->create_subscription<std_msgs::msg::String>("topic", 10,
//     [node, &count](const std_msgs::msg::String::SharedPtr msg) {
//       RCLCPP_INFO(node->get_logger(), "Received message: %s", msg->data.c_str());
//       ++count;
//     },
//     sub_options);

//   auto executor = std::make_unique<rclcpp::executors::SingleThreadedExecutor>();

//   executor->add_node(node);
//   std::thread executor_thread([&executor]() {executor->spin();});

//   auto msg1 = std_msgs::msg::String();
//   msg1.data = "Hello, world!";
//   auto msg2 = std_msgs::msg::String();
//   msg2.data = "Goodbye, world!";


//   RCLCPP_INFO(node->get_logger(), "**********FIRST ITERATION***********");
//   for (int i = 0; i < 5; ++i) {
//     publisher->publish(msg1);
//     publisher->publish(msg2);
//     std::this_thread::sleep_for(std::chrono::milliseconds(1));
//   }

//   subscriber->set_content_filter("data = 'Goodbye, world!'", {});

//   RCLCPP_INFO(node->get_logger(), "**********SECOND ITERATION***********");
//   for (int i = 0; i < 5; ++i) {
//     publisher->publish(msg1);
//     publisher->publish(msg2);
//     std::this_thread::sleep_for(std::chrono::milliseconds(1));
//   }

//   executor->cancel();
//   executor_thread.join();

//   return 0;
// }

#include <chrono>
#include <memory>
#include <vector>
#include <iostream>
#include <thread>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "content-filter-test.hpp"

class PublisherNode : public rclcpp::Node
{
public:
  PublisherNode() : Node("publisher_node")
  {
    publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(5),  // 5ms delay
      std::bind(&PublisherNode::publish_message, this));
    msg1_.data = "Hello, world!";
    msg2_.data = "Goodbye, world!";
  }

private:
  void publish_message()
  {
    publisher_->publish(msg1_);
    // publisher_->publish(msg2_);
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  std_msgs::msg::String msg1_;
  std_msgs::msg::String msg2_;
};

class SubscribersNode : public rclcpp::Node
{
public:
  SubscribersNode(size_t num_subscribers, bool use_content_filter)
    : Node("subscribers_node"), received_messages_(0)
  {
    for (size_t i = 0; i < num_subscribers; ++i)
    {
      rclcpp::SubscriptionOptions options;
      if (use_content_filter)
      {
        options.content_filter_options.filter_expression = "data <> data";
        options.content_filter_options.expression_parameters = {};
      }
      auto subscription = this->create_subscription<std_msgs::msg::String>(
        "topic", 10,
        [this](std_msgs::msg::String::SharedPtr msg) { this->topic_callback(msg); },
        options);
      subscriptions_.push_back(subscription);
    }
  }

  size_t get_received_messages() const { return received_messages_; }

private:
  void topic_callback(const std_msgs::msg::String::SharedPtr)
  {
    received_messages_++;
  }

  std::vector<rclcpp::Subscription<std_msgs::msg::String>::SharedPtr> subscriptions_;
  std::atomic<size_t> received_messages_;
};

void run_scenario(size_t num_subscribers, bool use_content_filter)
{
    auto pub_node = std::make_shared<PublisherNode>();
    auto sub_node = std::make_shared<SubscribersNode>(num_subscribers, use_content_filter);

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(pub_node);
    executor.add_node(sub_node);

    // Measure CPU usage
    double start_cpu_time = get_cpu_time();
    auto start_wall_time = std::chrono::steady_clock::now();
    
    // Run for 1 seconds of wall clock time
    const auto duration = std::chrono::seconds(2);
    while (std::chrono::steady_clock::now() - start_wall_time < duration)
    {
        executor.spin_some(std::chrono::milliseconds(0));
    }

    double end_cpu_time = get_cpu_time();
    auto end_wall_time = std::chrono::steady_clock::now();
    
    double cpu_time_used = end_cpu_time - start_cpu_time;
    auto wall_time_used = std::chrono::duration_cast<std::chrono::duration<double>>(end_wall_time - start_wall_time).count();

    // Print results
    std::cout << "Scenario: " << (use_content_filter ? "With" : "Without") << " content filter" << std::endl;
    std::cout << "Number of subscribers: " << num_subscribers << std::endl;
    std::cout << "Wall time elapsed: " << std::fixed << std::setprecision(6) << wall_time_used << " seconds" << std::endl;
    std::cout << "CPU time used: " << std::fixed << std::setprecision(6) << cpu_time_used << " seconds" << std::endl;
    std::cout << "CPU usage: " << (cpu_time_used / wall_time_used) * 100.0 << "%" << std::endl;
    std::cout << "Messages received: " << sub_node->get_received_messages() << std::endl;
    std::cout << "Messages received per second: " << sub_node->get_received_messages() / wall_time_used << std::endl;

    executor.remove_node(pub_node);
    executor.remove_node(sub_node);
}

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);

  std::vector<size_t> subscriber_counts = {50/*, 100, 1000, 10000*/};

  for (auto count : subscriber_counts)
  {
    std::cout << "\nRunning scenario with " << count << " subscribers, without content filter..." << std::endl;
    run_scenario(count, false);

    std::cout << "\nRunning scenario with " << count << " subscribers, with content filter..." << std::endl;
    run_scenario(count, true);
  }

  rclcpp::shutdown();
  return 0;
}
