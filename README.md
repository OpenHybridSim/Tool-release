# Beta version release
OpenHybridSim is an open source tool for Electromagnetic Transient (EMT)-Transient Stability (TS) hybrid simulation, which is also the first publically available tool for hybrid simulation. It is developed based on another open source simulation software InterPSS (www.interpss.org) and the core engine of InterPSS is called to perform power flow and transient stability simulation. 

A general interface framework is provided to interface with different EMT simulator. At present, an interface with PSCAD/EMTDC has been developed along with this tool. It should be noted that, duo to the use of TCP/IP socket for data exchange in the interface framework,  interfaces with other EMT simulators, such as ATP-EMTP, Matlab/Simulink SimPowerSystems or users' own EMT program, can be developed without much difficulty. If you are interested in this part, please contact us.

The tool is distributed as an executable Jar lib. To use the tool, please first download the jar from https://github.com/OpenHybridSim/OHM-release/blob/master/openHybridSimu_demo_V0.3.jar

The tool is developed based on the Java 8. Before running the tool , please make sure a compatible verison Java lib is installed properly in your PC or laptop.

To run the tool, just double-click the jar file, and the GUI should show up in a second.

The user guide and examples are provided in the wiki page
https://github.com/OpenHybridSim/OHM-release/wiki

The source code of the project is available from: https://github.com/OpenHybridSim/OpenHybridSim-code

## References
If you use or reference this tool for your work, please cite the following papers:
```
@ARTICLE{huang2016hyridsimFIDVR,
  author={Q. {Huang} and V. {Vittal}},
  journal={IEEE Transactions on Power Systems}, 
  title={Application of Electromagnetic Transient-Transient Stability Hybrid Simulation to FIDVR Study}, 
  year={2016},
  volume={31},
  number={4},
  pages={2634-2646},}
 ```
 ```
 @inproceedings{huang2016openhybridsim,
  title={OpenHybridSim: An open source tool for EMT and phasor domain hybrid simulation},
  author={Huang, Qiuhua and Vittal, Vijay},
  booktitle={2016 IEEE Power and Energy Society General Meeting (PESGM)},
  pages={1--5},
  year={2016},
  organization={IEEE}
}
```

## Contact
For any help or feedback, please contact with us via email: qiuhua dot huang AT asu dot edu
