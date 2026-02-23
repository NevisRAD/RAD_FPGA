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
        
        Need to launch this before running notebook


# 2026-02-16: Optimization
1. BRAM/DSP: Reuse factor, increases latency
1. 864X64: Too large
1. Precision: can lower as much as dynamic range? use unsigned?
1. Do tests with 64X32 model


# 2026-02-20: Implementation on FPGA
1. Summary of Video tutorials
    1. C sim/synth (Part 1)
        * Convert model using hls (all up to `hls_model.build(csim=False)`)
        * `defines.h`, `myproject.h`, `myproject.cpp`: definitions
        * `/weights`: Weights
        * `myproject_test.cpp`: Testbench file
        Inside Vitis:
            * Open project: `myproject_prj`
            * C simulation
                * Test Bench
                    Modify this part for input and predictions
                    ```C
                    int main(int argc, char **argv) {
                        // load input data from text file
                        std::ifstream fin("tb_data/tb_input_features.dat");
                        // load predictions from text file
                        std::ifstream fpr("tb_data/tb_output_predictions.dat");
                    ```
                    Predict is doing the same thing as CSim
                    * `tb_output_predictions` are those from qkeras?
            * Flow target `Vitis` when C Synth
    1. Hardware deployment (Part 2)
        * reset device: `xbutil reset -d DEVICE_ID`
            * Device id: `xbutil examine`
        * After building, need xclbin: `hls_model.config.backend.make_xclbin(hls_model, platform="xilinx_u250_xdma_201830_2")`
        * Just see the video
    1. Doing it by hand without Jupyter as front-end (Part 3)
    1. 