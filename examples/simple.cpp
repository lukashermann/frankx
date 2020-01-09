#include <frankx/frankx.hpp>


using namespace frankx;


int main(int argc, char *argv[]) {
    // Connect to the robot
    Robot robot(argv[1]);
    robot.automaticErrorRecovery();

    // Reduce the acceleration and velocity dynamic
    robot.setDynamicRel(0.05);

    // Define and move forwards
    auto way = Affine(0.0, 0.2, 0.0);
    auto motion_forward = LinearRelativeMotion(way);
    robot.move(motion_forward);

    // And move backwards using the inverse motion
    auto motion_backward = LinearRelativeMotion(way.inverse());
    robot.move(motion_backward);

    return 0;
}
