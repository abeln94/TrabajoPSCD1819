#include <iostream>
#include <string>
#include <strings.h>
#include <string.h>

#include <sys/socket.h>
#include <ifaddrs.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

#include <arpa/inet.h>

using namespace std;

//Función que devuelve la ip en string

string getIPAddress(){
    string ipAddress="Unable to get IP Address";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    cout << "success on getifaddrs:" << success << endl;
    if (success == 0) {
      // Loop through linked list of interfaces
      temp_addr = interfaces;
      while(temp_addr != NULL) {
        cout << "BBBBBBBBBBBBBBBBBBucle" << endl;
        if(temp_addr->ifa_addr->sa_family == AF_INET) {
          cout << "-------------IPv4" << endl;
          cout << temp_addr->ifa_name << endl;
          cout << inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr) << endl;
          // Check if interface is en0 which is the wifi connection on the iPhone
          if(strcmp(temp_addr->ifa_name, "en0")==0){
            cout << "strcmp" << endl;
            ipAddress=inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
          }
        }
        temp_addr = temp_addr->ifa_next;
      }
    }
    // Free memory
    freeifaddrs(interfaces);
    return ipAddress;
}


main(){
	cout << getIPAddress() << endl;
}
