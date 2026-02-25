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

    1. Implementation on FPGA using Vitis (Part 5)
        * Vivado Accel has feature to interact in Jupyter notebook, Vitis needs own code
        1. `am_vitis/Vitis/2023.1`
        1. `vitis`
        1. Create Application project
        1. Choose device
        1. Vitis IDE example --> Hello World XRT
        1. Kernel (vadd.cpp, FPGA) and host (CPU)
            1. Makes xclbin file --> Load to FPGA, How to Generate?
            1. DATA_SIZE
        1. `/mnt/storage1/RAD/akshay_malige_hls4ml-tutorial/model_3_vitis/hls4ml_prj/vitis/part4/src`
        1. `conda activate /mnt/storage1/akshaymalige/.conda/envs/tutorial/`


# 2026-02-25: Step-by-step
1. Run `hls4ml` notebook to build project.
2. Open project with `vitis_hls`.
    1. Choose `myproject_prj`
    1. `Test Bench/myproject_test.cpp`
        ```C
        int main(int argc, char **argv) {
        // load input data from text file
        std::ifstream fin("tb_data/tb_input_features.dat");
        // load predictions from text file
        std::ifstream fpr("tb_data/tb_output_predictions.dat");
        ```
    1. C simulation --> C synthesis (choose `Vitis` target)
    1. Export to Vitis: `Export RTL` (IP or xo)

1. Device manipulation on terminal
    1. reset device: `xbutil reset -d DEVICE_ID`
        * Device id: `xbutil examine`

1. Setup
    1. `yum remove xrt`
    1. `sudo yum install ./xrt*.rpm`
        1. `/scratch/XilinxDeployment/amalige`

    1. `xbmgmt program --base -d 0000:5e:00.0`
    1. `xbmgmt program --shell /lib/firmware/xilinx/12c8fafb0632499db1c0c6676271b8a6/partition.xsabin -d 0000:5e:00.0`
    1. `xbmgmt examine --report platform --format json --output tmp.json --device 0000:5e:00.0`

    1. `xbutil validate -d 0000:5e:00.1`
    1. `xbutil examine -d 0000:5e:00.1`

1. Vitis workflow
    1. Load exported `.xo` file to kernel `src`
    1. host files for cpu (to the x86 `src`)
        * `/mnt/storage1/RAD/akshay_malige_hls4ml-tutorial/model_3_vitis/hls4ml_prj/vitis/part4/src`
    1. Build with hardware target