#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <complex>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include "patches_unpack.h"

using namespace goldstein;

TEST_CASE( "patches_unpack", "[cl_kernels]" ) {

        // data setup
        const int height_packed = 72;
        const int width_packed  = 72;
        const int patch_size    = 32;
        const int overlap       = 4;

        const int n_patches_h = height_packed / (patch_size - 2*overlap);
        const int n_patches_w = width_packed  / (patch_size - 2*overlap);

        const int height_unpacked = n_patches_h * patch_size;
        const int width_unpacked  = n_patches_w * patch_size;

        std::vector<float> interf_real_packed   (height_packed*width_packed, 1.0);
        std::vector<float> interf_imag_packed   (height_packed*width_packed, 1.0);

        std::vector<float> interf_real_unpacked (height_unpacked*width_unpacked, -1.0);
        std::vector<float> interf_imag_unpacked (height_unpacked*width_unpacked, -1.0);


        // opencl setup
        cl::Context context = opencl_setup();

        std::vector<cl::Device> devices;
        context.getInfo(CL_CONTEXT_DEVICES, &devices);

        cl::CommandQueue cmd_queue{context, devices[0]};

        // kernel setup
        const int block_size = 16;
        patches_unpack kut_unpack {block_size, context};

        // allocate memory
        cl::Buffer device_interf_real_packed   {context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, height_packed*width_packed*sizeof(float), interf_real_packed.data(), NULL};
        cl::Buffer device_interf_imag_packed   {context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, height_packed*width_packed*sizeof(float), interf_imag_packed.data(), NULL};

        cl::Buffer device_interf_real_unpacked {context, CL_MEM_READ_ONLY, height_unpacked*width_unpacked*sizeof(float), NULL, NULL};
        cl::Buffer device_interf_imag_unpacked {context, CL_MEM_READ_ONLY, height_unpacked*width_unpacked*sizeof(float), NULL, NULL};

        kut_unpack.run(cmd_queue,
                       device_interf_real_packed, 
                       device_interf_imag_packed,
                       device_interf_real_unpacked, 
                       device_interf_imag_unpacked,
                       height_unpacked,
                       width_unpacked,
                       patch_size,
                       overlap);

        cmd_queue.enqueueReadBuffer(device_interf_real_unpacked, CL_TRUE, 0, height_unpacked*width_unpacked*sizeof(float), interf_real_unpacked.data(), NULL, NULL);
        cmd_queue.enqueueReadBuffer(device_interf_imag_unpacked, CL_TRUE, 0, height_unpacked*width_unpacked*sizeof(float), interf_imag_unpacked.data(), NULL, NULL);

        /*
        for(int y = 0; y < height_unpacked; y++) {
            for(int x = 0; x < width_unpacked; x++) {
                std::cout << std::setprecision(5) << (float) interf_real_unpacked[y*width_unpacked + x] << ",";
            }
            std::cout << std::endl;
        }
        */

        bool flag = true;
        for(int i = 0; i < height_packed*width_packed; i++) {
            flag = flag && ( 1.0f == Approx(interf_real_unpacked[i]).epsilon( 0.0001 ));
            flag = flag && ( 1.0f == Approx(interf_imag_unpacked[i]).epsilon( 0.0001 ));
        }
        REQUIRE( flag);
}

TEST_CASE( "patches_unpack_rand_big", "[cl_kernels]" ) {

        // data setup
        const int height_packed = 48;
        const int width_packed  = 72;
        const int patch_size    = 32;
        const int overlap       = 4;

        const int n_patches_h = height_packed / (patch_size - 2*overlap);
        const int n_patches_w = width_packed  / (patch_size - 2*overlap);

        const int height_unpacked = n_patches_h * patch_size;
        const int width_unpacked  = n_patches_w * patch_size;

        std::vector<float> interf_real_packed   (height_packed*width_packed, 1.0);
        std::vector<float> interf_imag_packed   (height_packed*width_packed, 1.0);

        std::vector<float> interf_real_unpacked (height_unpacked*width_unpacked, -1.0);
        std::vector<float> interf_imag_unpacked (height_unpacked*width_unpacked, -1.0);

        std::vector<float> interf_real_unpacked_desired (height_unpacked*width_unpacked, -1.0);
        std::vector<float> interf_imag_unpacked_desired (height_unpacked*width_unpacked, -1.0);

        static std::default_random_engine rand_eng{};
        static std::uniform_int_distribution<int> dist_params(1, 9.0f);

        for(int y = 0; y < height_packed; y++) {
            for(int x = 0; x < width_packed; x++) {
                interf_real_packed[y*width_packed + x] = dist_params(rand_eng);
                interf_imag_packed[y*width_packed + x] = dist_params(rand_eng);
            }
        }
        for(int y = 0; y < height_unpacked; y++) {
            for(int x = 0; x < width_unpacked; x++) {
                const int patch_idx = x/patch_size;
                const int patch_idy = y/patch_size;

                const int rel_tx = x % patch_size;
                const int rel_ty = y % patch_size;

                const int xx = std::min(width_packed  - 1, std::max(0, patch_idx*(patch_size-2*overlap) + (rel_tx - overlap)));
                const int yy = std::min(height_packed - 1, std::max(0, patch_idy*(patch_size-2*overlap) + (rel_ty - overlap)));

                interf_real_unpacked_desired[y*width_unpacked + x] = interf_real_packed[yy*width_packed + xx];
                interf_imag_unpacked_desired[y*width_unpacked + x] = interf_imag_packed[yy*width_packed + xx];
            }
        }

        // opencl setup
        cl::Context context = opencl_setup();

        std::vector<cl::Device> devices;
        context.getInfo(CL_CONTEXT_DEVICES, &devices);

        cl::CommandQueue cmd_queue{context, devices[0]};

        // kernel setup
        const int block_size = 16;
        patches_unpack kut_unpack {block_size, context};

        // allocate memory
        cl::Buffer device_interf_real_packed   {context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, height_packed*width_packed*sizeof(float), interf_real_packed.data(), NULL};
        cl::Buffer device_interf_imag_packed   {context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, height_packed*width_packed*sizeof(float), interf_imag_packed.data(), NULL};

        cl::Buffer device_interf_real_unpacked {context, CL_MEM_READ_ONLY, height_unpacked*width_unpacked*sizeof(float), NULL, NULL};
        cl::Buffer device_interf_imag_unpacked {context, CL_MEM_READ_ONLY, height_unpacked*width_unpacked*sizeof(float), NULL, NULL};

        kut_unpack.run(cmd_queue,
                       device_interf_real_packed, 
                       device_interf_imag_packed,
                       device_interf_real_unpacked, 
                       device_interf_imag_unpacked,
                       height_unpacked,
                       width_unpacked,
                       patch_size,
                       overlap);

        cmd_queue.enqueueReadBuffer(device_interf_real_unpacked, CL_TRUE, 0, height_unpacked*width_unpacked*sizeof(float), interf_real_unpacked.data(), NULL, NULL);
        cmd_queue.enqueueReadBuffer(device_interf_imag_unpacked, CL_TRUE, 0, height_unpacked*width_unpacked*sizeof(float), interf_imag_unpacked.data(), NULL, NULL);

       /*
        for(int y = 0; y < height_unpacked; y++) {
            for(int x = 0; x < width_unpacked; x++) {
                std::cout << interf_real_unpacked_desired[y*width_unpacked + x] << ",";
            }
            for(int x = 0; x < width_unpacked; x++) {
                std::cout << interf_real_unpacked[y*width_unpacked + x] << ",";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        */

        bool flag = true;
        for(int i = 0; i < height_unpacked*width_unpacked; i++) {
            flag = flag && ( interf_real_unpacked_desired[i] == Approx(interf_real_unpacked[i]).epsilon( 0.0001 ));
            flag = flag && ( interf_imag_unpacked_desired[i] == Approx(interf_imag_unpacked[i]).epsilon( 0.0001 ));
        }
        REQUIRE( flag);
}