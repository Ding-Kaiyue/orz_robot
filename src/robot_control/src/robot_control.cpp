#include "robot_control.h"
#include "cubic_spline.h"

vector<double> time_from_start_;
vector<double> p_joint1_;
vector<double> p_joint2_;
vector<double> p_joint3_;
vector<double> p_joint4_;
vector<double> p_joint5_;
vector<double> p_joint6_;
vector<double> p_joint7_;
vector<double> v_joint1_;
vector<double> v_joint2_;
vector<double> v_joint3_;
vector<double> v_joint4_;
vector<double> v_joint5_;
vector<double> v_joint6_;
vector<double> v_joint7_;
vector<double> a_joint1_;
vector<double> a_joint2_;
vector<double> a_joint3_;
vector<double> a_joint4_;
vector<double> a_joint5_;
vector<double> a_joint6_;
vector<double> a_joint7_;

/* 存储的结构体 p2*/
struct vel_data
{
    int vector_len;    //toatal num
    int vector_cnt;    //current num
};

/* 数据收发结构体 */
struct vel_data p2;

/* 初始化输入输出速度加速度 */
double acc = 0, vel = 0;
double x_out = 0, y_out = 0;

/* 判断路点数据是否改变 */
bool point_changed = false;

/*三次样条插值后周期*/
double rate = 0.020;    // 5ms

float min_interval = 20;  //透传周期,单位:秒
float wait_move_finish_time = 1.5;  //等待运动到位时间,单位:秒
int count_keep_send = 0;
int count_final_joint = 0;

/* 三次样条无参构造 */
cubicSpline::cubicSpline()
{

}

/* 析构 */
cubicSpline::~cubicSpline()
{
    releaseMem();
}

/* 初始化参数 */
void cubicSpline::initParam()
{
    x_sample_ = y_sample_ = M_ = NULL;
    sample_count_ = 0;
    bound1_ = bound2_ = 0;
}
/* 释放参数 */
void cubicSpline::releaseMem()
{
    delete x_sample_;
    delete y_sample_;
    delete M_;

    initParam();
}

/* 加载关节位置数组等信息 */
bool cubicSpline::loadData(double *x_data, double *y_data, int count, double bound1, double bound2, BoundType type)
{
    if ((NULL == x_data) || (NULL == y_data) || (count < 3) || (type > BoundType_Second_Derivative) || (type < BoundType_First_Derivative))
    {
        return false;
    }
    initParam();

    x_sample_ = new double[count];
    y_sample_ = new double[count];
    M_        = new double[count];
    sample_count_ = count;

    memcpy(x_sample_, x_data, sample_count_*sizeof(double));
    memcpy(y_sample_, y_data, sample_count_*sizeof(double));

    bound1_ = bound1;
    bound2_ = bound2;

    return spline(type);
}

/* 计算样条插值 */
bool cubicSpline::spline(BoundType type)
{
    if ((type < BoundType_First_Derivative) || (type > BoundType_Second_Derivative))
    {
        return false;
    }

    //  追赶法解方程求二阶偏导数
    double f1=bound1_, f2=bound2_;

    double *a=new double[sample_count_];                //  a:稀疏矩阵最下边一串数
    double *b=new double[sample_count_];                //  b:稀疏矩阵最中间一串数
    double *c=new double[sample_count_];                //  c:稀疏矩阵最上边一串数
    double *d=new double[sample_count_];

    double *f=new double[sample_count_];

    double *bt=new double[sample_count_];
    double *gm=new double[sample_count_];

    double *h=new double[sample_count_];

    for(int i=0;i<sample_count_;i++)
        b[i]=2;                                //  中间一串数为2
    for(int i=0;i<sample_count_-1;i++)
    {
        if (x_sample_[i+1] == x_sample_[i])
        {
            h[i] = 0.005;
        }
        else
        {
            h[i]=x_sample_[i+1]-x_sample_[i];      // 各段步长
        }
    }
    for(int i=1;i<sample_count_-1;i++)
        a[i]=h[i-1]/(h[i-1]+h[i]);
    a[sample_count_-1]=1;

    c[0]=1;
    for(int i=1;i<sample_count_-1;i++)
        c[i]=h[i]/(h[i-1]+h[i]);

    for(int i=0;i<sample_count_-1;i++)
    {
        if (x_sample_[i+1] == x_sample_[i])
        {
            f[i]=(y_sample_[i+1]-y_sample_[i])/0.005;
        }
        else
        {
            f[i]=(y_sample_[i+1]-y_sample_[i])/(x_sample_[i+1]-x_sample_[i]);
        }
    }

    for(int i=1;i<sample_count_-1;i++)
        d[i]=6*(f[i]-f[i-1])/(h[i-1]+h[i]);

    //  追赶法求解方程
    if(BoundType_First_Derivative == type)
    {
        d[0]=6*(f[0]-f1)/h[0];
        d[sample_count_-1]=6*(f2-f[sample_count_-2])/h[sample_count_-2];

        bt[0]=c[0]/b[0];
        for(int i=1;i<sample_count_-1;i++)
            bt[i]=c[i]/(b[i]-a[i]*bt[i-1]);

        gm[0]=d[0]/b[0];
        for(int i=1;i<=sample_count_-1;i++)
            gm[i]=(d[i]-a[i]*gm[i-1])/(b[i]-a[i]*bt[i-1]);

        M_[sample_count_-1]=gm[sample_count_-1];
        for(int i=sample_count_-2;i>=0;i--)
            M_[i]=gm[i]-bt[i]*M_[i+1];
    }
    else if(BoundType_Second_Derivative == type)
    {
        d[1]=d[1]-a[1]*f1;
        d[sample_count_-2]=d[sample_count_-2]-c[sample_count_-2]*f2;

        bt[1]=c[1]/b[1];
        for(int i=2;i<sample_count_-2;i++)
            bt[i]=c[i]/(b[i]-a[i]*bt[i-1]);

        gm[1]=d[1]/b[1];
        for(int i=2;i<=sample_count_-2;i++)
            gm[i]=(d[i]-a[i]*gm[i-1])/(b[i]-a[i]*bt[i-1]);

        M_[sample_count_-2]=gm[sample_count_-2];
        for(int i=sample_count_-3;i>=1;i--)
            M_[i]=gm[i]-bt[i]*M_[i+1];

        M_[0]=f1;
        M_[sample_count_-1]=f2;
    }
    else
        return false;

    delete a;
    delete b;
    delete c;
    delete d;
    delete gm;
    delete bt;
    delete f;
    delete h;

    return true;
}
/* 得到速度和加速度数组 */
bool cubicSpline::getYbyX(double &x_in, double &y_out)
{
    int klo,khi,k;
    klo=0;
    khi=sample_count_-1;
    double hh,bb,aa;

    //  二分法查找x所在区间段
    while(khi-klo>1)
    {
        k=(khi+klo)>>1;
        if(x_sample_[k]>x_in)
            khi=k;
        else
            klo=k;
    }
    hh=x_sample_[khi]-x_sample_[klo];

    aa=(x_sample_[khi]-x_in)/hh;
    bb=(x_in-x_sample_[klo])/hh;

    y_out=aa*y_sample_[klo]+bb*y_sample_[khi]+((aa*aa*aa-aa)*M_[klo]+(bb*bb*bb-bb)*M_[khi])*hh*hh/6.0;

    //test
    acc = (M_[klo]*(x_sample_[khi]-x_in) + M_[khi]*(x_in - x_sample_[klo])) / hh;
    vel = M_[khi]*(x_in - x_sample_[klo]) * (x_in - x_sample_[klo]) / (2 * hh)
          - M_[klo]*(x_sample_[khi]-x_in) * (x_sample_[khi]-x_in) / (2 * hh)
          + (y_sample_[khi] - y_sample_[klo])/hh
          - hh*(M_[khi] - M_[klo])/6;

    return true;
}

RobotControl::RobotControl(std::string name) : Node(name)
{
    using namespace std::placeholders;
    
    joint_msg.position.resize(6);
    joint_msg.velocity.resize(6);
    //  初始位姿
    joint_msg.position[0] = 0.0;
    joint_msg.position[1] = 0.0;
    joint_msg.position[2] = 0.0;
    joint_msg.position[3] = 0.0;
    joint_msg.position[4] = 0.0;
    joint_msg.position[5] = 0.0;

    //  初始速度
    joint_msg.velocity[0] = 0.0;
    joint_msg.velocity[1] = 0.0;
    joint_msg.velocity[2] = 0.0;
    joint_msg.velocity[3] = 0.0;
    joint_msg.velocity[4] = 0.0;
    joint_msg.velocity[5] = 0.0;

    State_Timer = this->create_wall_timer(std::chrono::milliseconds(20), 
        std::bind(&RobotControl::timer_callback,this));

    this->action_server_ = rclcpp_action::create_server<FollowJointTrajectory>(
                this, "/arm_controller/follow_joint_trajectory",
                std::bind(&RobotControl::handle_goal, this, _1, _2),
                std::bind(&RobotControl::handle_cancel, this, _1),
                std::bind(&RobotControl::handle_accepted, this, _1));
    joint_pos_publisher = this->create_publisher<sensor_msgs::msg::JointState>("motor_cmd", 10);

    // joint_pos_publisher->publish(joint_msg);
}

rclcpp_action::GoalResponse RobotControl::handle_goal(const rclcpp_action::GoalUUID &uuid, std::shared_ptr<const FollowJointTrajectory::Goal> goal)
{
    std::cout << "---handle goal" << goal->trajectory.joint_names.size() << std::endl;
    std::cout << goal->trajectory.header.frame_id.c_str() << goal->trajectory.header.stamp.sec << goal->trajectory.header.stamp.nanosec << std::endl;
    // 轨迹点数量
    int pointSize = goal->trajectory.points.size();
    if (pointSize > 0) {
        for (int i = 0; i < pointSize; i++) {
            // 在循环中逐个处理每个轨迹点(point)
            auto point = goal->trajectory.points.at(i);
        }
    }
    (void) uuid;
    // 接受该目标并执行
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse RobotControl::handle_cancel(const std::shared_ptr<GoalHandleFollowJointTrajectory> /*goal_handle*/)
{
    RCLCPP_INFO(this->get_logger(), "Received request to cancel goal");
    return rclcpp_action::CancelResponse::ACCEPT;
}

// 接受了目标处理句柄，可以用来跟踪和管理执行过程中的目标状态和结果
void RobotControl::handle_accepted(const std::shared_ptr<GoalHandleFollowJointTrajectory> goal_handle)
{
    using std::placeholders::_1;
    // .detach()将新创建的线程与当前线程分离，使得它可以在后台独立运行，不会阻塞当前线程的执行
    std::thread{std::bind(&RobotControl::execute_move, this, _1), goal_handle}.detach();
}

void RobotControl::execute_move(const std::shared_ptr<GoalHandleFollowJointTrajectory> goal_handle)
{
    const auto goal = goal_handle->get_goal();
    auto result = std::make_shared<FollowJointTrajectory::Result>();
    int point_num = goal->trajectory.points.size();
    RCLCPP_INFO(this->get_logger(), "First Move_group give us %d points", point_num);
    point_changed = false;

    //判断当moveit规划的路点数大于3时为有效规划并进行三次样条插值
    if (point_num > 3) {
        /* 各个关节位置 */
        std::vector<double> p_joint1(point_num);
        std::vector<double> p_joint2(point_num);
        std::vector<double> p_joint3(point_num);
        std::vector<double> p_joint4(point_num);
        std::vector<double> p_joint5(point_num);
        std::vector<double> p_joint6(point_num);

        /* 各个关节速度 */
        std::vector<double> v_joint1(point_num);
        std::vector<double> v_joint2(point_num);
        std::vector<double> v_joint3(point_num);
        std::vector<double> v_joint4(point_num);
        std::vector<double> v_joint5(point_num);
        std::vector<double> v_joint6(point_num);

        /* 各个关节加速度 */
        std::vector<double> a_joint1(point_num);
        std::vector<double> a_joint2(point_num);
        std::vector<double> a_joint3(point_num);
        std::vector<double> a_joint4(point_num);
        std::vector<double> a_joint5(point_num);
        std::vector<double> a_joint6(point_num);

        /* 时间数组 */
        std::vector<double> time_from_start(point_num);
        std::vector<double> timens_from_start(point_num);

        for (int i = 0; i < point_num; i++) {
            p_joint1[i] = goal->trajectory.points[i].positions[0];
            p_joint2[i] = goal->trajectory.points[i].positions[1];
            p_joint3[i] = goal->trajectory.points[i].positions[2];
            p_joint4[i] = goal->trajectory.points[i].positions[3];
            p_joint5[i] = goal->trajectory.points[i].positions[4];
            p_joint6[i] = goal->trajectory.points[i].positions[5];

            v_joint1[i] = goal->trajectory.points[i].velocities[0];
            v_joint2[i] = goal->trajectory.points[i].velocities[1];
            v_joint3[i] = goal->trajectory.points[i].velocities[2];
            v_joint4[i] = goal->trajectory.points[i].velocities[3];
            v_joint5[i] = goal->trajectory.points[i].velocities[4];
            v_joint6[i] = goal->trajectory.points[i].velocities[5];

            a_joint1[i] = goal->trajectory.points[i].accelerations[0];
            a_joint2[i] = goal->trajectory.points[i].accelerations[1];
            a_joint3[i] = goal->trajectory.points[i].accelerations[2];
            a_joint4[i] = goal->trajectory.points[i].accelerations[3];
            a_joint5[i] = goal->trajectory.points[i].accelerations[4];
            a_joint6[i] = goal->trajectory.points[i].accelerations[5];

            time_from_start.data()[i] = goal->trajectory.points[i].time_from_start.sec + goal->trajectory.points[i].time_from_start.nanosec / 1e9;
            timens_from_start.data()[i] = goal->trajectory.points[i].time_from_start.nanosec;
        }

        cubicSpline spline;
        double max_time = time_from_start.data()[point_num - 1];
        RCLCPP_INFO(this->get_logger(), "Second Move_group max_time is: %f", max_time);
        time_from_start_.clear();

        // joint1
        if (spline.loadData(time_from_start.data(), p_joint1.data(), point_num, 0, 0, cubicSpline::BoundType_First_Derivative)) {
            p_joint1_.clear();
            v_joint1_.clear();
            a_joint1_.clear();
            x_out = -rate;
            while (x_out < max_time) {
                x_out += rate;
                spline.getYbyX(x_out, y_out);
                time_from_start_.push_back(x_out);
                p_joint1_.push_back(y_out);
                v_joint1_.push_back(vel);
                a_joint1_.push_back(acc);
            }

            // joint2
            if (spline.loadData(time_from_start.data(), p_joint2.data(), point_num, 0, 0, cubicSpline::BoundType_First_Derivative)) {
                p_joint2_.clear();
                v_joint2_.clear();
                a_joint2_.clear();
                x_out = -rate;
                while (x_out < max_time) {
                    x_out += rate;
                    spline.getYbyX(x_out, y_out);
                    p_joint2_.push_back(y_out);
                    v_joint2_.push_back(vel);
                    a_joint2_.push_back(acc);
                }

                // joint3
                if (spline.loadData(time_from_start.data(), p_joint3.data(), point_num, 0, 0, cubicSpline::BoundType_First_Derivative)) {
                    p_joint3_.clear();
                    v_joint3_.clear();
                    a_joint3_.clear();
                    x_out = -rate;
                    while (x_out < max_time) {
                        x_out += rate;
                        spline.getYbyX(x_out, y_out);
                        p_joint3_.push_back(y_out);
                        v_joint3_.push_back(vel);
                        a_joint3_.push_back(acc);
                    }

                    // joint4
                    if (spline.loadData(time_from_start.data(), p_joint4.data(), point_num, 0, 0, cubicSpline::BoundType_First_Derivative)) {
                        p_joint4_.clear();
                        v_joint4_.clear();
                        a_joint4_.clear();
                        x_out = -rate;
                        while (x_out < max_time) {
                            x_out += rate;
                            spline.getYbyX(x_out, y_out);
                            p_joint4_.push_back(y_out);
                            v_joint4_.push_back(vel);
                            a_joint4_.push_back(acc);
                        }

                        // joint5
                        if (spline.loadData(time_from_start.data(), p_joint5.data(), point_num, 0, 0, cubicSpline::BoundType_First_Derivative)) {
                            p_joint5_.clear();
                            v_joint5_.clear();
                            a_joint5_.clear();
                            x_out = -rate;
                            while (x_out < max_time) {
                                x_out += rate;
                                spline.getYbyX(x_out, y_out);
                                p_joint5_.push_back(y_out);
                                v_joint5_.push_back(vel);
                                a_joint5_.push_back(acc);
                            }

                            // joint6
                            if (spline.loadData(time_from_start.data(), p_joint6.data(), point_num, 0, 0, cubicSpline::BoundType_First_Derivative)) {
                                p_joint6_.clear();
                                v_joint6_.clear();
                                a_joint6_.clear();
                                x_out = -rate;
                                while (x_out < max_time) {
                                    x_out += rate;
                                    spline.getYbyX(x_out, y_out);
                                    p_joint6_.push_back(y_out);
                                    v_joint6_.push_back(vel);
                                    a_joint6_.push_back(acc);
                                }

                                p2.vector_len = time_from_start_.size();
                                p2.vector_cnt = 0;

                                point_changed = true;

                                // 等待定时器将数据取出并发送完
                                while (point_changed) {
                                    if (!rclcpp::ok() || goal_handle->is_canceling()) {
                                        result->error_code = -1;
                                        result->error_string = "has cancel";
                                        goal_handle->canceled(result);
                                        RCLCPP_INFO(this->get_logger(), "Goal Canceled");
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (point_num > 0) {
        p_joint1_.clear();  
        p_joint2_.clear();
        p_joint3_.clear();
        p_joint4_.clear();
        p_joint5_.clear();
        p_joint6_.clear();
        v_joint1_.clear();
        v_joint2_.clear();
        v_joint3_.clear();
        v_joint4_.clear();
        v_joint5_.clear();
        v_joint6_.clear();
        a_joint1_.clear();
        a_joint2_.clear();
        a_joint3_.clear();
        a_joint4_.clear();
        a_joint5_.clear();
        a_joint6_.clear();

        for (int i = 0; i < point_num; i++) {
            p_joint1_.push_back(goal->trajectory.points[i].positions[0]);
            p_joint2_.push_back(goal->trajectory.points[i].positions[1]);
            p_joint3_.push_back(goal->trajectory.points[i].positions[2]);
            p_joint4_.push_back(goal->trajectory.points[i].positions[3]);
            p_joint5_.push_back(goal->trajectory.points[i].positions[4]);
            p_joint6_.push_back(goal->trajectory.points[i].positions[5]);

            v_joint1_.push_back(goal->trajectory.points[i].velocities[0]);
            v_joint2_.push_back(goal->trajectory.points[i].velocities[1]);
            v_joint3_.push_back(goal->trajectory.points[i].velocities[2]);
            v_joint4_.push_back(goal->trajectory.points[i].velocities[3]);
            v_joint5_.push_back(goal->trajectory.points[i].velocities[4]);
            v_joint6_.push_back(goal->trajectory.points[i].velocities[5]);

            a_joint1_.push_back(goal->trajectory.points[i].accelerations[0]);
            a_joint2_.push_back(goal->trajectory.points[i].accelerations[1]);
            a_joint3_.push_back(goal->trajectory.points[i].accelerations[2]);
            a_joint4_.push_back(goal->trajectory.points[i].accelerations[3]);
            a_joint5_.push_back(goal->trajectory.points[i].accelerations[4]);
            a_joint6_.push_back(goal->trajectory.points[i].accelerations[5]);
        }

        p2.vector_len = point_num;
        p2.vector_cnt = 0;

        point_changed = true;

        // 等待定时器将数据取出并发送完
        while (point_changed) {
            if (!rclcpp::ok() || goal_handle->is_canceling()) {
                result->error_code = -1;
                result->error_string = "has cancel";
                goal_handle->canceled(result);
                RCLCPP_INFO(this->get_logger(), "Goal Canceled");
                return;
            }
        }
    }

    result->error_code = 0;
    result->error_string = "success";
    goal_handle->succeed(result);
    RCLCPP_INFO(this->get_logger(), "Goal Succeeded");
}


void RobotControl::timer_callback()
{
    // joint_msg.follow = follow_;
    if (point_changed) {
        if (p2.vector_cnt < p2.vector_len) {
            joint_msg.position[0] = p_joint1_.at(p2.vector_cnt);
            joint_msg.position[1] = p_joint2_.at(p2.vector_cnt);
            joint_msg.position[2] = p_joint3_.at(p2.vector_cnt);
            joint_msg.position[3] = p_joint4_.at(p2.vector_cnt);
            joint_msg.position[4] = p_joint5_.at(p2.vector_cnt);
            joint_msg.position[5] = p_joint6_.at(p2.vector_cnt);
            this->joint_pos_publisher->publish(joint_msg);
            p2.vector_cnt++;
            // RCLCPP_INFO(this->get_logger(), "%s: position: %lf", joint_msg.name[0].c_str(), joint_msg.position[0]);
        }
        else {
            if (count_final_joint <= count_keep_send) {
                joint_msg.position[0] = p_joint1_.at(p2.vector_cnt - 1);
                joint_msg.position[1] = p_joint2_.at(p2.vector_cnt - 1);
                joint_msg.position[2] = p_joint3_.at(p2.vector_cnt - 1);
                joint_msg.position[3] = p_joint4_.at(p2.vector_cnt - 1);
                joint_msg.position[4] = p_joint5_.at(p2.vector_cnt - 1);
                joint_msg.position[5] = p_joint6_.at(p2.vector_cnt - 1);
                this->joint_pos_publisher->publish(joint_msg);
                count_final_joint++;
                // RCLCPP_INFO(this->get_logger(), "%s: position: %lf", joint_msg.name[0].c_str(), joint_msg.position[0]);
            }
            else {
                count_final_joint = 0;
                p2.vector_cnt = 0;
                p2.vector_len = 0;
                point_changed = false;
                // RCLCPP_INFO(this->get_logger(), "%s: position: %lf", joint_msg.name[0].c_str(), joint_msg.position[0]);
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    count_keep_send = wait_move_finish_time / min_interval;
    auto node = std::make_shared<RobotControl>("robot_control");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}