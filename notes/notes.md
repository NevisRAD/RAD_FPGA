# 2026-02-06: FPGA Implementation
1. Goal: Have working FPGA implementation by April
1. Conda environment works now
1. cleopatra
    1. VSCode latest version not possible, also difficult to differentiate server and client
    1. Issues with ssh key (annonying)
1. Instructions
    1. Python to C
        1. Look at notebooks with `seokju_*`
    1. C to FPGA
        ```bash
        source /scratch/am_vitis/Vitis/2023.1/settings64.sh
        source /opt/xilinx/xrt/setup.sh
        vitis_hls
        ```
        
        Need to launch this before running notebook?