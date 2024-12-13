// // #include <chrono>
// // #include <memory>
// // #include <vector>
// // #include <iostream>
// // #include <thread>
// // #include "rclcpp/rclcpp.hpp"
// // #include "std_msgs/msg/string.hpp"
// // #include "content-filter-test.hpp"

// // class PublisherNode : public rclcpp::Node
// // {
// // public:
// //   PublisherNode() : Node("publisher_node")
// //   {
// //     publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
// //     timer_ = this->create_wall_timer(
// //       std::chrono::milliseconds(5),  // 5ms delay
// //       std::bind(&PublisherNode::publish_message, this));
// //     msg1_.data = "Hello, world!";
// //     msg2_.data = "Goodbye, world!";
// //   }

// // private:
// //   void publish_message()
// //   {
// //     publisher_->publish(msg1_);
// //     // publisher_->publish(msg2_);
// //   }

// //   rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
// //   rclcpp::TimerBase::SharedPtr timer_;
// //   std_msgs::msg::String msg1_;
// //   std_msgs::msg::String msg2_;
// // };

// // class SubscribersNode : public rclcpp::Node
// // {
// // public:
// //   SubscribersNode(size_t num_subscribers, bool use_content_filter)
// //     : Node("subscribers_node"), received_messages_(0)
// //   {
// //     for (size_t i = 0; i < num_subscribers; ++i)
// //     {
// //       rclcpp::SubscriptionOptions options;
// //       if (use_content_filter)
// //       {
// //         options.content_filter_options.filter_expression = "data <> data";
// //         options.content_filter_options.expression_parameters = {};
// //       }
// //       auto subscription = this->create_subscription<std_msgs::msg::String>(
// //         "topic", 10,
// //         [this](std_msgs::msg::String::SharedPtr msg) { this->topic_callback(msg); },
// //         options);
// //       subscriptions_.push_back(subscription);
// //     }
// //   }

// //   size_t get_received_messages() const { return received_messages_; }

// // private:
// //   void topic_callback(const std_msgs::msg::String::SharedPtr)
// //   {
// //     received_messages_++;
// //   }

// //   std::vector<rclcpp::Subscription<std_msgs::msg::String>::SharedPtr> subscriptions_;
// //   std::atomic<size_t> received_messages_;
// // };

// // void run_scenario(size_t num_subscribers, bool use_content_filter)
// // {
// //     auto pub_node = std::make_shared<PublisherNode>();
// //     auto sub_node = std::make_shared<SubscribersNode>(num_subscribers, use_content_filter);

// //     rclcpp::executors::SingleThreadedExecutor executor;
// //     executor.add_node(pub_node);
// //     executor.add_node(sub_node);

// //     // Measure CPU usage
// //     double start_cpu_time = get_cpu_time();
// //     auto start_wall_time = std::chrono::steady_clock::now();
    
// //     // Run for 1 seconds of wall clock time
// //     const auto duration = std::chrono::seconds(2);
// //     while (std::chrono::steady_clock::now() - start_wall_time < duration)
// //     {
// //         executor.spin_some(std::chrono::milliseconds(0));
// //     }

// //     double end_cpu_time = get_cpu_time();
// //     auto end_wall_time = std::chrono::steady_clock::now();
    
// //     double cpu_time_used = end_cpu_time - start_cpu_time;
// //     auto wall_time_used = std::chrono::duration_cast<std::chrono::duration<double>>(end_wall_time - start_wall_time).count();

// //     // Print results
// //     std::cout << "Scenario: " << (use_content_filter ? "With" : "Without") << " content filter" << std::endl;
// //     std::cout << "Number of subscribers: " << num_subscribers << std::endl;
// //     std::cout << "Wall time elapsed: " << std::fixed << std::setprecision(6) << wall_time_used << " seconds" << std::endl;
// //     std::cout << "CPU time used: " << std::fixed << std::setprecision(6) << cpu_time_used << " seconds" << std::endl;
// //     std::cout << "CPU usage: " << (cpu_time_used / wall_time_used) * 100.0 << "%" << std::endl;
// //     std::cout << "Messages received: " << sub_node->get_received_messages() << std::endl;
// //     std::cout << "Messages received per second: " << sub_node->get_received_messages() / wall_time_used << std::endl;

// //     executor.remove_node(pub_node);
// //     executor.remove_node(sub_node);
// // }

// // int main(int argc, char** argv)
// // {
// //   rclcpp::init(argc, argv);

// //   std::vector<size_t> subscriber_counts = {50/*, 100, 1000, 10000*/};

// //   for (auto count : subscriber_counts)
// //   {
// //     std::cout << "\nRunning scenario with " << count << " subscribers, without content filter..." << std::endl;
// //     run_scenario(count, false);

// //     std::cout << "\nRunning scenario with " << count << " subscribers, with content filter..." << std::endl;
// //     run_scenario(count, true);
// //   }

// //   rclcpp::shutdown();
// //   return 0;
// // }

// #include <chrono>
// #include <memory>
// #include <vector>
// #include <iostream>
// #include <iomanip>
// #include "rclcpp/rclcpp.hpp"
// #include "std_msgs/msg/string.hpp"
// #include <time.h>

// double get_cpu_time()
// {
//     struct timespec ts;
//     if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0)
//     {
//         perror("clock_gettime");
//         return 0;
//     }
//     return ts.tv_sec + ts.tv_nsec * 1e-9;
// }

// class PublisherNode : public rclcpp::Node
// {
// public:
//     PublisherNode(const std::string& name) : Node(name), toggle_(false)
//     {
//         publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
//         timer_ = this->create_wall_timer(
//             std::chrono::milliseconds(5),  // 5ms delay
//             std::bind(&PublisherNode::publish_message, this));
//     }

// private:
//     void publish_message()
//     {
//         auto message = std_msgs::msg::String();
//         message.data = toggle_ ? "Goodbye, world!" : "Hello, world!";
//         publisher_->publish(message);
//         toggle_ = !toggle_;
//     }

//     rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
//     rclcpp::TimerBase::SharedPtr timer_;
//     bool toggle_;
// };

// class SubscriberNode : public rclcpp::Node
// {
// public:
//     SubscriberNode(bool use_content_filter)
//         : Node("subscriber_node"), received_messages_(0)
//     {
//         rclcpp::SubscriptionOptions options;
//         if (use_content_filter)
//         {
//             options.content_filter_options.filter_expression = "data <> 'Goodbye, world!'";
//             options.content_filter_options.expression_parameters = {};
//         }
//         subscription_ = this->create_subscription<std_msgs::msg::String>(
//             "topic", 10,
//             [this](std_msgs::msg::String::SharedPtr msg) { this->topic_callback(msg); },
//             options);
//     }

//     size_t get_received_messages() const { return received_messages_; }

// private:
//     void topic_callback(const std_msgs::msg::String::SharedPtr)
//     {
//         received_messages_++;
//     }

//     rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
//     std::atomic<size_t> received_messages_;
// };

// void run_scenario(size_t num_publishers, bool use_content_filter)
// {
//     // auto context = std::make_shared<rclcpp::Context>();

//     // rclcpp::ExecutorOptions executor_options;
//     // executor_options.context = context;
//     auto executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();

//     std::vector<std::shared_ptr<PublisherNode>> pub_nodes;
//     for (size_t i = 0; i < num_publishers; ++i)
//     {
//         auto node = std::make_shared<PublisherNode>("publisher_node_" + std::to_string(i));
//         pub_nodes.push_back(node);
//         executor->add_node(node);
//     }

//     auto sub_node = std::make_shared<SubscriberNode>(use_content_filter);
//     executor->add_node(sub_node);

//     // Measure CPU usage
//     double start_cpu_time = get_cpu_time();
//     auto start_wall_time = std::chrono::steady_clock::now();
    
//     // Run for 10 seconds of wall clock time
//     const auto duration = std::chrono::seconds(10);
//     while (std::chrono::steady_clock::now() - start_wall_time < duration)
//     {
//         executor->spin_some(std::chrono::milliseconds(0));
//     }

//     double end_cpu_time = get_cpu_time();
//     auto end_wall_time = std::chrono::steady_clock::now();
    
//     double cpu_time_used = end_cpu_time - start_cpu_time;
//     auto wall_time_used = std::chrono::duration_cast<std::chrono::duration<double>>(end_wall_time - start_wall_time).count();

//     // Print results
//     std::cout << "Scenario: " << (use_content_filter ? "With" : "Without") << " content filter" << std::endl;
//     std::cout << "Number of publishers: " << num_publishers << std::endl;
//     std::cout << "Wall time elapsed: " << std::fixed << std::setprecision(6) << wall_time_used << " seconds" << std::endl;
//     std::cout << "CPU time used: " << std::fixed << std::setprecision(6) << cpu_time_used << " seconds" << std::endl;
//     std::cout << "CPU usage: " << (cpu_time_used / wall_time_used) * 100.0 << "%" << std::endl;
//     std::cout << "Messages received: " << sub_node->get_received_messages() << std::endl;
//     std::cout << "Messages received per second: " << sub_node->get_received_messages() / wall_time_used << std::endl;

//     // Clean up
//     executor->remove_node(sub_node);
//     for (auto& node : pub_nodes)
//     {
//         executor->remove_node(node);
//     }
// }

// int main(int argc, char** argv)
// {
//     rclcpp::init(argc, argv);

//     std::vector<size_t> publisher_counts = {1, 10, 100, 1000};

//     for (auto count : publisher_counts)
//     {
//         std::cout << "\nRunning scenario with " << count << " publishers, without content filter..." << std::endl;
//         run_scenario(count, false);

//         std::cout << "\nRunning scenario with " << count << " publishers, with content filter..." << std::endl;
//         run_scenario(count, true);
//     }

//     rclcpp::shutdown();
//     return 0;
// }

#include <chrono>
#include <memory>
#include <vector>
#include <iostream>
#include <iomanip>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <time.h>

double get_cpu_time()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0)
    {
        perror("clock_gettime");
        return 0;
    }
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

class PublisherNode : public rclcpp::Node
{
public:
    PublisherNode(const std::string& name) : Node(name)
    {
        msg_.data = "Hello, world!";
        publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(5),  // 5ms delay
            std::bind(&PublisherNode::publish_message, this));
    }

private:
    void publish_message()
    {
        publisher_->publish(msg_);
    }

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    std_msgs::msg::String msg_;
};

class SubscriberNode : public rclcpp::Node
{
public:
    SubscriberNode(const std::string& name, bool use_content_filter)
        : Node(name), received_messages_(0)
    {
        rclcpp::SubscriptionOptions options;
        if (use_content_filter)
        {
            options.content_filter_options.filter_expression = "data <> data";
            options.content_filter_options.expression_parameters = {};
        }
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "topic", 10,
            [this](std_msgs::msg::String::SharedPtr msg) { this->topic_callback(msg); },
            options);
    }

    size_t get_received_messages() const { return received_messages_; }

private:
    void topic_callback(const std_msgs::msg::String::SharedPtr)
    {
        received_messages_++;
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    std::atomic<size_t> received_messages_;
};

void run_scenario_publishers(size_t num_publishers, bool use_content_filter)
{
    auto executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();

    std::vector<std::shared_ptr<PublisherNode>> pub_nodes;
    for (size_t i = 0; i < num_publishers; ++i)
    {
        auto node = std::make_shared<PublisherNode>("publisher_node_" + std::to_string(i));
        pub_nodes.push_back(node);
        executor->add_node(node);
    }

    auto sub_node = std::make_shared<SubscriberNode>("subscriber_node", use_content_filter);
    executor->add_node(sub_node);

    // Measure CPU usage
    // double start_cpu_time = get_cpu_time();
    auto start_wall_time = std::chrono::steady_clock::now();
    
    // Run for 10 seconds of wall clock time
    const auto duration = std::chrono::seconds(10);
    while (std::chrono::steady_clock::now() - start_wall_time < duration)
    {
        executor->spin_some(std::chrono::milliseconds(0));
    }

    // double end_cpu_time = get_cpu_time();
    // auto end_wall_time = std::chrono::steady_clock::now();
    
    // double cpu_time_used = end_cpu_time - start_cpu_time;
    // auto wall_time_used = std::chrono::duration_cast<std::chrono::duration<double>>(end_wall_time - start_wall_time).count();

    // Print results
    // std::cout << "Scenario: " << (use_content_filter ? "With" : "Without") << " content filter" << std::endl;
    // std::cout << "Number of publishers: " << num_publishers << std::endl;
    // std::cout << "Wall time elapsed: " << std::fixed << std::setprecision(6) << wall_time_used << " seconds" << std::endl;
    // std::cout << "CPU time used: " << std::fixed << std::setprecision(6) << cpu_time_used << " seconds" << std::endl;
    // std::cout << "CPU usage: " << (cpu_time_used / wall_time_used) * 100.0 << "%" << std::endl;
    // std::cout << "Messages received: " << sub_node->get_received_messages() << std::endl;
    // std::cout << "Messages received per second: " << sub_node->get_received_messages() / wall_time_used << std::endl;

    // Clean up
    executor->remove_node(sub_node);
    for (auto& node : pub_nodes)
    {
        executor->remove_node(node);
    }
}

void run_scenario_subscribers(size_t num_subscribers, bool use_content_filter)
{
    auto executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();

    auto pub_node = std::make_shared<PublisherNode>("publisher_node");
    executor->add_node(pub_node);

    std::vector<std::shared_ptr<SubscriberNode>> sub_nodes;
    for (size_t i = 0; i < num_subscribers; ++i)
    {
        auto node = std::make_shared<SubscriberNode>("subscriber_node_" + std::to_string(i), use_content_filter);
        sub_nodes.push_back(node);
        executor->add_node(node);
    }

    // Measure CPU usage
    // double start_cpu_time = get_cpu_time();
    auto start_wall_time = std::chrono::steady_clock::now();
    
    // Run for 10 seconds of wall clock time
    const auto duration = std::chrono::seconds(10);
    while (std::chrono::steady_clock::now() - start_wall_time < duration)
    {
        executor->spin_some(std::chrono::milliseconds(0));
    }

    // double end_cpu_time = get_cpu_time();
    // auto end_wall_time = std::chrono::steady_clock::now();
    
    // double cpu_time_used = end_cpu_time - start_cpu_time;
    // auto wall_time_used = std::chrono::duration_cast<std::chrono::duration<double>>(end_wall_time - start_wall_time).count();

    // Print results
    // std::cout << "Scenario: " << (use_content_filter ? "With" : "Without") << " content filter" << std::endl;
    // std::cout << "Number of subscribers: " << num_subscribers << std::endl;
    // std::cout << "Wall time elapsed: " << std::fixed << std::setprecision(6) << wall_time_used << " seconds" << std::endl;
    // std::cout << "CPU time used: " << std::fixed << std::setprecision(6) << cpu_time_used << " seconds" << std::endl;
    // std::cout << "CPU usage: " << (cpu_time_used / wall_time_used) * 100.0 << "%" << std::endl;
    // size_t total_messages_received = 0;
    // for (auto& node : sub_nodes)
    // {
    //     total_messages_received += node->get_received_messages();
    // }
    // std::cout << "Messages received: " << total_messages_received << std::endl;
    // std::cout << "Messages received per second: " << total_messages_received / wall_time_used << std::endl;

    // Clean up
    for (auto& node : sub_nodes)
    {
        executor->remove_node(node);
    }
    executor->remove_node(pub_node);
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <num_publishers> <num_subscribers>" << std::endl;
        return 1;
    }

    size_t num_publishers = std::stoi(argv[1]);
    size_t num_subscribers = std::stoi(argv[2]);
    bool with_content_filter = std::stoi(argv[3]);

    rclcpp::init(argc, argv);

    if (num_subscribers > 0)
    {
        if (with_content_filter)
        {
            std::cout << "\nRunning scenario with " << num_subscribers << " subscribers, with content filter..." << std::endl;
            run_scenario_subscribers(num_subscribers, true);
        }
        else
        {
            std::cout << "\nRunning scenario with " << num_subscribers << " subscribers, without content filter..." << std::endl;
            run_scenario_subscribers(num_subscribers, false);
        }
    }
    else if (num_publishers > 0)
    {

        if (with_content_filter)
        {
            std::cout << "\nRunning scenario with " << num_publishers << " publishers, with content filter..." << std::endl;
            run_scenario_publishers(num_publishers, true);
        }
        else
        {
            std::cout << "\nRunning scenario with " << num_publishers << " publishers, without content filter..." << std::endl;
            run_scenario_publishers(num_publishers, false);
        }
    }

    rclcpp::shutdown();
    return 0;
}