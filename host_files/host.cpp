#include <iostream>
#include <cstring>

#include <algorithm>
#include <fstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <iomanip>
#include <ap_int.h>
#include <map>
#include <bitset>
#include <chrono>
// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"
#include "host.h"
#include <fstream>

template <class src_T, class dst_T, size_t OFFSET, size_t SIZE> void copy_data_axi(std::vector<src_T> src, dst_T dst[SIZE]) {
    for (auto i = 0; i < SIZE; i++)
        if (i == SIZE - 1) {
            dst[i].data = src[i];
            dst[i].last = 1;
        } else {
            dst[i].data = src[i];
            dst[i].last = 0;
        }
}

template <class res_T, size_t SIZE> void print_result(res_T result[SIZE], std::ostream &out, bool keep = false) {
    for (int i = 0; i < SIZE; i++) {
        out << result[i] << " ";
    }
    out << std::endl;
}

int main(int argc, char** argv) {

	int device_index =0;
	auto binaryFile = argv[1];

	// Read settings
	std::cout << "Open the device" << device_index << std::endl;
	auto device = xrt::device(device_index);

	std::cout << "Load the xclbin " << binaryFile << std::endl;
	auto uuid = device.load_xclbin(binaryFile);

	std::cout<<"uuid :"<<uuid<<std::endl;

	input_axi_t in_array[N_LENI];
	output_axi_t out_array[N_LENO];
	size_t size_in_bytes_in  = sizeof(in_array);
	size_t size_in_bytes_out = sizeof(out_array);

    xrt::kernel krnl;
    std::string cu_id;
    std::string krnl_name = "myproject_axi";
	printf("Creating a kernel [%s] for CU(%d)\n", krnl_name.c_str());
	krnl = xrt::kernel(device, uuid, krnl_name.c_str());

	std::cout << "Allocate Buffer in Global Memory\n";
	xrt::bo bo_in;
	xrt::bo bo_out;
	bo_in = xrt::bo(device, size_in_bytes_in, krnl.group_id(0));
	bo_out = xrt::bo(device, size_in_bytes_out, krnl.group_id(1));

	input_axi_t* bo_in_map = {nullptr};
	output_axi_t* bo_out_map = {nullptr};

	// Map the contents of the buffer object into host memory
	bo_in_map = bo_in.map<input_axi_t*>();
	bo_out_map = bo_out.map<output_axi_t*>();

    // Read the data from input file
    std::string iline;
    std::string pline;
    int e = 0;
    std::vector<float> in;
    std::vector<float> in2;
    in.clear();
    in2.clear();
    std::ifstream fin("/mnt/storage1/RAD/RAD_FPGA/saved_models/hls/64X32/tb_data/tb_input_features.dat");
    if (fin.is_open()) {
        while (std::getline(fin, iline)) {
            char *cstr = const_cast<char *>(iline.c_str());
            char *current;
            current = strtok(cstr, " ");
            while (current != NULL) {
                in.push_back(atof(current));
                current = strtok(NULL, " ");
            }
        }
        fin.close();
    }
    for(int i=0; i<N_LENI; i++){
    	in2.push_back(in[i]);
    }

    //copy float data to input_axi_t format
	input_axi_t inputs[N_LENI];
	copy_data_axi<float, input_axi_t, 0, N_LENI>(in2, bo_in_map);

    std::cout << "Write the input data\n";
    std::cout << "synchronize input buffer data to device global memory\n";
	bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto start_t = std::chrono::high_resolution_clock::now();
    auto run = krnl(bo_in, bo_out,  N_LENI, N_LENO);
    run.wait();

	std::cout << "Execution of the kernel complete\n";
    auto end_t = std::chrono::high_resolution_clock::now();
	double elapsed_time_us = std::chrono::duration<double, std::micro>(end_t-start_t).count();

    std::cout << "Synchronize the output buffer data from the device" << std::endl;
	bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    std::cout << "Read the output data\n";
    std::cout << "Quantized predictions "<<e<<"\t"<<in2.size() << std::endl;
    print_result<output_axi_t, N_LENO>(bo_out_map, std::cout, true);

    std::cout <<"\n******Run COMPLETE******\n";
    std::cout<<"Total Kernel Run Time : "<<elapsed_time_us<<"us"<<std::endl;
    std::cout<<"Average Inference Time : "<<elapsed_time_us/N_ELEM<<"us"<<std::endl;

    return 0;
}
