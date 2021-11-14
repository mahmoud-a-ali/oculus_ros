#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>


#include "liboculus/SonarClient.h"
#include "liboculus/SonarPlayer.h"

// #include <memory>
// #include <thread>
#include <string>
#include <fstream>

using std::string;
using std::ofstream;
using std::ios_base;

using namespace liboculus;



int playbackSonarFile( const std::string &filename, ofstream &output, int stopAfter = -1 );


// Make these global so signal handler can access it
std::unique_ptr< SonarClient > _client;
bool doStop = false;


// Catch signals
void signalHandler( int signo ) {
  if( _client ) _client->stop();
  doStop = true;
}





int main(int argc, char **argv)
{

  ros::init(argc, argv, "oculus_node");
  ros::NodeHandle nh("~");
  ros::Publisher chatter_pub = nh.advertise<std_msgs::String>("chatter", 1000);
  ros::Rate loop_rate(10);


  ofstream output;
  int stopAfter = -1;
  string ipAddr("auto");
  string inputFilename("");
  string outputFilename("");

  nh.getParam("stopAfter", stopAfter);
  nh.getParam("ipAddr", ipAddr);
  nh.getParam("inputFile", inputFilename);
  nh.getParam("outputFile", outputFilename);
 
  ROS_INFO_STREAM("stopAfter:  " << stopAfter);
  ROS_INFO_STREAM("ipAddr:     " << ipAddr);
  ROS_INFO_STREAM("inputFile:  " << inputFilename);
  ROS_INFO_STREAM("outputFil:  " <<  outputFilename);


  if( !outputFilename.empty() ) {
    ROS_INFO_STREAM("Opening output file " << outputFilename);
    output.open( outputFilename, ios_base::binary | ios_base::out );

    if( !output.is_open() ) {
      ROS_INFO_STREAM("Unable to open " << outputFilename << " for output.");
      exit(-1);
    }
  }


  // If playing back an input file, run a different main loop ...
  if( !inputFilename.empty() ) {
     playbackSonarFile( inputFilename, output, stopAfter );
     return 0;
   }




  int count = 0;
  signal(SIGHUP, signalHandler );
  ROS_INFO_STREAM( "Starting loop");

  SonarConfiguration config;
  config.setPingRate( pingRateNormal );

  _client.reset( new SonarClient(config, ipAddr) );
  _client->setDataRxCallback( [&]( const SimplePingResult &ping ) {

    auto valid = ping.valid();
    ROS_INFO_STREAM("Got " << (valid ? "valid" : "invalid") << " ping");

    if( !valid ) {
      ROS_INFO_STREAM( "Got invalid ping");
      return;
    }

    ping.dump();

    if( output.is_open() ) {
      output.write( (const char *)ping.ptr(), ping.size() );
    }

    count++;
    if( (stopAfter>0) && (count >= stopAfter)) _client->stop();

  });

  _client->start();

  // Imprecise statistic for now...
  int lastCount = 0;
  while( !doStop ) {
    auto c = count;

    ROS_INFO_STREAM("Received pings at " << c-lastCount << " Hz");
    ROS_INFO_STREAM("doStop:  " << doStop);
    ROS_INFO_STREAM("lastCount:  " << lastCount);

    lastCount = c;
    // lastCount = lastCount ++ ;
    // if( (stopAfter>0) && (lastCount >= stopAfter)) _client->stop();
    sleep(1);
  }

  _client->join();

  if( output.is_open() ) output.close();

  ROS_INFO_STREAM("At exit");

  return 0;

}




int playbackSonarFile( const std::string &filename, ofstream &output, int stopAfter ) {
  std::shared_ptr<SonarPlayerBase> player( SonarPlayerBase::OpenFile(filename) );

  if( !player ) {
    ROS_INFO_STREAM("Unable to open sonar file");
    return -1;
  }

  if( !player->open(filename) ) {
    ROS_INFO_STREAM("Failed to open " << filename);
    return -1;
  }

  int count = 0;
  SimplePingResult ping;
  while( player->nextPing(ping) && !player->eof() ) {

    if( !ping.valid() ) {
      ROS_INFO_STREAM("Invalid ping");
      continue;
    }

    ping.dump();

    if( output.is_open() ) {
      output.write( (const char *)ping.ptr(), ping.size() );
    }

    count++;
    if( (stopAfter > 0) && (count >= stopAfter) ) break;
  }

  ROS_INFO_STREAM(count << " sonar packets decoded");

  return 0;
}




