# Beta version release
OpenHybridSim is an open source tool for Electromagnetic Transient (EMT)-(Transient Stability)TS hybrid simulation, which is also the first publically available tool for hybrid simulation. It is developed based on another open source simulation software InterPSS (www.interpss.org) and the core engine of InterPSS is called to perform power flow and transient stability simulation. 

A general interface framework is provided to interface with different EMT simulator. At present, an interface with PSCAD/EMTDC has been developed along with this tool. It should be noted that, duo to the use of TCP/IP socket for data exchange in the interface framework,  interfaces with other EMT simulators, such as ATP-EMTP, Matlab/Simulink SimPowerSystems or users' own EMT program, can be developed without much difficunty. If you are interested in this part, please contact us.

The tool is distributed as an executable Jar lib. To use the tool, please first download the jar from https://github.com/OpenHybridSim/OHM-release/blob/master/openHybridSimu_demo_V0.3.jar

The tool is developed based on the Java 8. Before running the tool , please make sure a compatible verison Java lib is installed properly in your PC or laptop.

To run the tool, just double-click the jar file, and the GUI should show up in a second.

The user guide and examples are provided in the wiki page
https://github.com/OpenHybridSim/OHM-release/wiki

For any help or feedback, please contact with the us via email: OpenHybridSim AT gmail dot com
