# oculus_ros

ROS wrapper for the [liboculus][] library, to communicate with oculus sonar!

### Dependencies
- [liboculus][]
- std_msgs

### Building 
- Create catkin workspace
```bash
#create new workspace
mkdir -p ~/oculus_ws/src

# change to the src directory in your Catkin workspace
cd ~/oculus_ws/src
```
- 
- Clone  [liboculus][] 
```bash
git clone https://github.com/apl-ocean-engineering/liboculus.git
```
- Clone [g3log][]
```bash
git clone https://github.com/KjellKod/g3log.git
```
- Clone oculus_ros package
```bash
git clone https://github.com/mahmoud-a-ali/oculus_ros.git
```
- Change to the main Catkin workspace
```bash
 cd ..
```
- Build the workspace (using catkin_tools)
```bash
 catkin build
```
- Source workspace
```bash
source devel/setup.bash
```

### Running
- Run the ros wrapper node using the launch file
```bash
roslaunch oculus_ros oculus_node.launch
```

### Parameters
 - ipAddr : sonar ip address
 - inputFile : Filename to read sonar data from, in case of reading pre-recorded data
 - outputFil : Filename to save sonar data to
 - stopAfter : number of frames to read in case no continous operation required 

  
### Limitation
Check readme file of the original [liboculus_readme][] library

[liboculus]: https://github.com/apl-ocean-engineering/liboculus
[g3log]:https://github.com/KjellKod/g3log
[liboculus_readme]: https://github.com/apl-ocean-engineering/liboculus#liboculus
