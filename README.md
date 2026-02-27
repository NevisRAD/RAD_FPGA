# RAD_FPGA

Convert trained student using hls4ml


On `cleopatra` with Alveo U250C

As user `RAD`:
* Port 8888 gets forwarded to 9901 to open JupyterNotebooks
* Port 5901 gets forwarded to 5901 for VNC. `DISPLAY=:01`

## Requirements
1. Have a trained and saved student model
1. Have input files
```bash
source /scratch/am_vitis/Vitis/2023.1/settings64.sh # For Vitis workflow
source /scratch/xilinx_jsc2268/Vivado/2020.1/settings64.sh # For Vivado workflow

source /opt/xilinx/xrt/setup.sh # For xrt commands

conda activate hls_RAD # conda environment with hls4ml
```


## Convert student using hls4ml
1. Follow the `test_*.ipynb` notebooks
1. `output_dir="../saved_models/hls/64X32"` determines location of converted model
1. Check that resource estimates are below hardware limits. What is shown as available is total, we want to target SLR, which is 1/4 of total for each.

### Some unofficial debuggings
1. `backend="Vitis"`: Either `Vitis` or `VivadoAccelerator`. `VivadoAccelerator` requries sourcing second line from requirements for `vivado_hls`
1. `config["Model"]["ReuseFactor"] = 256`: Reduces resource usage (DSP and BRAM), but increases latency

## Make xilinx object (`.xo`)
1. Launch `vitis_hls` from terminal. GUI IDE will open
1. Open project
    1. Choose `myproject_prj`
    1. Copy over the `myproject_test.cpp`, and add `myproject_axi.*` to the `firmware` folder. Modify `N_IN` in `myproject_axi.h` to match the input.
    1. `Project Settings` --> Set `myproject_axi` as top
    1. C simulation --> C synthesis (choose `Vitis` target)
    1. Export to Vitis: `Export RTL` (IP (`.zip`) or `.xo`)

## Vitis workflow
1. Launch `vitis` from terminal.
1. Make new `Application Project` as `XRT Native API's`
1. In `NAME_kernels`, add the `.xo` file to `src`
1. Choose `myproject_axi` as hardware function, build with hardware target
1. Add host files to `NAME[x86]` src
    1. Modify `host.h` to match input size (2048 = 64 X 32)
    ```C
    static const unsigned N_IN = 2048;
    static const unsigned N_OUT = 1;
    static const unsigned N_ELEM = 1000;
    ```
    1. Modify `host.cpp` to get input data
    ```C
    std::ifstream fin("/mnt/storage1/RAD/RAD_FPGA/saved_models/hls/64X32/tb_data/tb_input_features.dat");
    ```
1. Build again

## Vivado workflow
This enables doing everything inside of Jupyter Notebook
1. Refer to `/mnt/storage1/RAD/akshay_malige_hls4ml-tutorial/part4_quantization-vitis.ipynb`

## Related installations (This is already done, refer if server needs reinstall)
1. Refer to [official documentation](https://www.amd.com/en/products/accelerators/alveo/u250/a-u250-a64g-pq-g.html#get-started)
    1. `Get Started and Downloads`
    1. `Vitis Design Flow`
    1. `CentOS 7`
    1. We use `2023.1` on `cleopatra`

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
