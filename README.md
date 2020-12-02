# Description
Development of a basic pipeline based microprocessor design. Following all the design stages, from high level models going down to structural description. Testing all the steps. The uP architecture will be similar to the one analyzed in this [1] Berkeley course's slide.

# Design Stage
Here are described all the design steps that are being followed for the development of this project.
## Requirements
The first step is to define the requirements that the system must have. These will then be used in the testing phase to check that the developed system respects all the initial requests.
### Functional
* RiscV RV32I ISA
* Pipelined
* Forwarding and Stalls Handling (Structural, Data, Control \[see below\])
* Jump Prediction (Predict, Check and Flush the pipeline)
* Debug and Test support (Scan Chain design + BIST)
### Non-Functional

# First Model
The first model wants to be a sort of ISS (Instruction Set Simulator) loosely timed at instruction level. In order to model it SystemC is used. After the model has been developed it is verified.

# Second Model
Starting from the previous result, it is possible to expand the model by inserting more detail, modelling each component for instance, still at a non accurate timing level of simulation.
## CU and DP
### Scheamtic
### Components
#### CU
#### Pipeline
##### FF
##### RF
##### ALU
##### Jump Prediction Unit
##### Instruction Memory
##### Data Memory
##### Instruction Cache
##### Data Cache

## Consideration about SystemC models
### Example of Design Space Exploration - Jump Prediction Unit
#### BTB
#### ...

# Third Model
With SystemC it is possible to model the microprocessor with an accurate clock signals level of simulation.

# Fourth Stage - CU Design for synthesis

# Fifth Stage - DP Design for synthesis

# Sixth Stage - Synthesis and Analysis of Timing, Area and Power

# Seventh Stage - Optimization of the Design


# References
[1] https://inst.eecs.berkeley.edu/~cs61c/sp18/disc/7/disc07_sol.pdf
