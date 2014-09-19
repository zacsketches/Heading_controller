#include <iostream>

/* 
*  test the delta() function to ensure it gives the proper values
*  negative results indicate left of target and positive results
*  indicate right of target.
*/

using namespace std;

int target = 0;

int target_heading() {
	return target;
}

int delta(int current_Z) 
{
	int res = current_Z - target_heading();
	
	if (res < -180){
		res = (360 + res);
	}
	
	if (res > 180){
		res = -(360 - res);
	}
	
	return res;
}

int main (int argc, char  *argv[])
{
	int current_Z;
	int target_Z;
	int dZ;
	
	cout<<"Enter a value for Current Z: ";
	cin>>current_Z;
	cout<<"Enter a value for Target Z: ";
	cin>>target_Z;
	
	target = target_Z;
	
	dZ = delta(current_Z);
	
	cout<<"Delta is: "<<dZ<<endl;
	
	return 0;
}